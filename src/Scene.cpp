#include <cgraphics/Scene.hpp>
#include <cgraphics/InputManager.hpp>
#include <cgraphics/Extensions.hpp>
#include <cgraphics/ResourceManager.hpp>
#include <cgraphics/RenderManager.hpp>
#include <cgraphics/Texture.hpp>

#include <GLFW/glfw3.h>

#include <array>
#include <iostream>
#include <tuple>

namespace fs = std::filesystem;

namespace Side
{
    constexpr unsigned int 
        None  = 0,
        Left  = 1,
        Up    = 1 << 1,
        Right = 1 << 2,
        Down  = 1 << 3,
        Front = 1 << 4,
        Back  = 1 << 5;

    bool has_flag(unsigned int value, unsigned int flag)
    {
        return (value & flag) == flag;
    }

    bool both_has_flag(unsigned int left, unsigned int right, unsigned int flag)
    {
        return has_flag(left, flag) && has_flag(right, flag);
    }
}

void Scene::init(const fs::path &base_path)
{
    this->base_path = base_path;
    auto result = xml.load_file((base_path / "Resources.xml").c_str());
    if (!result)
    {
        std::cout << "Cannot load Resource.xml" << std::endl;
        return;
    }
    auto xml_resources = xml.child("Resources");
    auto xml_server = xml_resources.child("Server");
    auto xml_server_ip = xml_server.child("IP").attribute("value").value();
    auto xml_server_port = std::stoi(xml_server.child("Port").attribute("value").value());

    protocol.connect(xml_server_ip, xml_server_port);
    std::cout << "Welcome message: " << protocol.get_welcome_message() << std::endl;

    auto xml_camera = xml_resources.child("Camera");
    auto xml_camera_radius = xml_camera.child("Radius");
    camera.set_radius(xml_camera_radius.attribute("current").as_double(),
        xml_camera_radius.attribute("min").as_double(),
        xml_camera_radius.attribute("max").as_double());
    auto xml_camera_oxz = xml_camera.child("AngleTangage");
    camera.set_vertical(xml_camera_oxz.attribute("current").as_double(),
        xml_camera_oxz.attribute("min").as_double(),
        xml_camera_oxz.attribute("max").as_double());
    auto xml_camera_oy = xml_camera.child("AngleYaw");
    camera.set_horizontal(xml_camera_oy.attribute("current").as_double());

    auto xml_light = xml_resources.child("Light");
    light.set_position(Extensions::string_as_vec4(xml_light.child("Direction").attribute("vector").as_string(), 0));
    auto xml_light_params = xml_light.child("PhongParameters");
    light.set_diffuse(Extensions::string_as_vec4(xml_light_params.attribute("diffuse").value()));
    light.set_ambient(Extensions::string_as_vec4(xml_light_params.attribute("ambient").value()));
    light.set_specular(Extensions::string_as_vec4(xml_light_params.attribute("specular").value()));

    auto xml_shaders = xml_resources.child("Shaders");
    auto xml_shader_light = xml_shaders.find_child_by_attribute("id", "light");
    auto xml_shader_skybox = xml_shaders.find_child_by_attribute("id", "skybox");
    auto xml_shader_aabb = xml_shaders.find_child_by_attribute("id", "bounding_box");
    auto xml_shader_simple_pp = xml_shaders.find_child_by_attribute("id", "simple_pp");
    auto xml_shader_sepia_pp = xml_shaders.find_child_by_attribute("id", "sepia_pp");
    auto xml_shader_grey_pp = xml_shaders.find_child_by_attribute("id", "grey_pp");
    ShaderPaths light_shader = {
        base_path / xml_shader_light.attribute("vertex-path").value(),
        base_path / xml_shader_light.attribute("fragment-path").value()
    }, skybox_shader = {
        base_path / xml_shader_skybox.attribute("vertex-path").value(),
        base_path / xml_shader_skybox.attribute("fragment-path").value()
    }, aabb_shader = {
        base_path / xml_shader_aabb.attribute("vertex-path").value(),
        base_path / xml_shader_aabb.attribute("fragment-path").value(),
    }, simple_pp_shader = {
        base_path / xml_shader_simple_pp.attribute("vertex-path").value(),
        base_path / xml_shader_simple_pp.attribute("fragment-path").value(),
    }, sepia_pp_shader = {
        base_path / xml_shader_sepia_pp.attribute("vertex-path").value(),
        base_path / xml_shader_sepia_pp.attribute("fragment-path").value(),
    }, grey_pp_shader = {
        base_path / xml_shader_grey_pp.attribute("vertex-path").value(),
        base_path / xml_shader_grey_pp.attribute("fragment-path").value(),
    };
    RenderManager::get_instance().init({
        { Shader::Type::BoundingBox, aabb_shader },
        { Shader::Type::DirectLight, light_shader },
        { Shader::Type::SkyBox, skybox_shader },
        { Shader::Type::SimplePostProcessing, simple_pp_shader},
        { Shader::Type::SepiaPostProcessing, sepia_pp_shader},
        { Shader::Type::GreyPostProcessing, grey_pp_shader}
    });
    RenderManager::get_instance().set_aabb_mesh_path(base_path / "meshes" / "box.obj");

    auto xml_skybox = xml_resources.child("SkyBox");
    auto xml_skybox_basename = xml_skybox.child("Basename");
    auto xml_skybox_basename_prefix = std::string(xml_skybox_basename.attribute("prefix").value());
    auto xml_skybox_basename_extension = xml_skybox_basename.attribute("extension").value();
    auto xml_skybox_type = std::string(xml_skybox.child("Type").attribute("value").value());
    skybox.load(base_path / (xml_skybox_basename_prefix + '_' + xml_skybox_type), xml_skybox_basename_extension);
}

void Scene::simulate(double seconds)
{
    simulate_keyboard(seconds);
    simulate_mouse();

    auto descriptions = protocol.get_nearby_objects(camera.get_eye() + camera.get_center(), 150);
    for (auto &descr : descriptions)
    {
        auto &object = objects[descr.object_id];
        if (!object.initialized())
        {
            create_graphic_object(std::string(descr.model_name.begin()), object);
        }

        object.set_id(descr.object_id);
        object.set_position(glm::vec3(descr.x, descr.y, descr.z));
        object.set_rotation(-descr.rotation);
        object.set_aabb(descr.aabb);
        object.set_object_type(descr.object_type);
    }
}

void Scene::draw(GLFWwindow *window)
{
    static auto &render_manager = RenderManager::get_instance();

    render_manager.start();

    render_manager.set_camera(camera);
    render_manager.set_light(light);
    render_manager.set_skybox(skybox);

    std::vector<std::reference_wrapper<GraphicObject>> render_objects;
    for (auto &pair : objects)
    {
        render_objects.push_back(pair.second);
    }

    if (lod_enabled)
    {
        level_of_detail(render_objects);
    }
    if (culling_enabled)
    {
        frustum_culling(render_objects);
    }

    for (auto &object : render_objects)
    {
        render_manager.add_to_queue(object.get());
    }

    render_manager.finish(window);
}

void Scene::level_of_detail(std::vector<std::reference_wrapper<GraphicObject>> &render_objects)
{
    static std::vector<float> max_distances {
        600, // object_type = 0
        600, // object_type = 1
        600, // object_type = 2
        300, // object_type = 3
        250, // object_type = 4
        250, // object_type = 5
        150, // object_type = 6
        150, // object_type = 7
    };
    auto camera_position = camera.get_eye() + camera.get_center();
    for (auto it = render_objects.begin(); it < render_objects.end(); it++)
    {
        auto &object = it->get();
        auto distance = glm::distance(camera_position, object.get_position());
        if (distance > max_distances[object.get_object_type()])
        {
            render_objects.erase(it--);
        }
    }
}

void Scene::frustum_culling(std::vector<std::reference_wrapper<GraphicObject>> &render_objects)
{
    std::map<int, GraphicObject> temp;

    for (auto it = render_objects.begin(); it < render_objects.end(); it++)
    {
        auto &object = it->get();

        auto pvm = camera.get_projection_matrix() * camera.get_view_matrix() * object.get_model();
        auto aabb = object.get_aabb();
        std::array<glm::vec4, 8> aabb_vertices = {
            glm::vec4(+aabb[0], +aabb[1], +aabb[2], 1.0),
            glm::vec4(+aabb[0], +aabb[1], -aabb[2], 1.0),
            glm::vec4(+aabb[0], -aabb[1], +aabb[2], 1.0),
            glm::vec4(+aabb[0], -aabb[1], -aabb[2], 1.0),
            glm::vec4(-aabb[0], +aabb[1], +aabb[2], 1.0),
            glm::vec4(-aabb[0], +aabb[1], -aabb[2], 1.0),
            glm::vec4(-aabb[0], -aabb[1], +aabb[2], 1.0),
            glm::vec4(-aabb[0], -aabb[1], -aabb[2], 1.0)
        };
        std::array<int, 8> sides;
        sides.fill(0);
        bool flag = false;
        int side_index = -1;
        for (auto &aabb_vertex : aabb_vertices)
        {
            side_index++;
            aabb_vertex = pvm * aabb_vertex;
            aabb_vertex /= aabb_vertex.w;

            sides[side_index] |= (aabb_vertex.x >  1) ? Side::Right : Side::None;
            sides[side_index] |= (aabb_vertex.x < -1) ? Side::Left  : Side::None;
            sides[side_index] |= (aabb_vertex.y >  1) ? Side::Up    : Side::None;
            sides[side_index] |= (aabb_vertex.y < -1) ? Side::Down  : Side::None;
            sides[side_index] |= (aabb_vertex.z >  1) ? Side::Front : Side::None;
            sides[side_index] |= (aabb_vertex.z < -1) ? Side::Back  : Side::None;
            if (sides[side_index] == Side::None)
            {
                flag = true;
                break;
            }
            for (int i = 0; (i < side_index) && !flag; i++)
            {
                flag = !(Side::both_has_flag(sides[side_index], sides[i], Side::Right)
                    || Side::both_has_flag(sides[side_index], sides[i], Side::Left)
                    || Side::both_has_flag(sides[side_index], sides[i], Side::Up)
                    || Side::both_has_flag(sides[side_index], sides[i], Side::Down)
                    || Side::both_has_flag(sides[side_index], sides[i], Side::Front)
                    || Side::both_has_flag(sides[side_index], sides[i], Side::Back));
            }
            if (flag)
            {
                break;
            }
        }
        if (!flag)
        {
            render_objects.erase(it--);
        }
    }
}

Camera& Scene::get_camera()
{
    return camera;
}

Scene::Optimization Scene::get_enabled_optimizations() const
{
    return (Optimization)((int)culling_enabled + (int)lod_enabled);
}

void Scene::next_optimization()
{
    switch (get_enabled_optimizations())
    {
        case Optimization::None:
            culling_enabled = true;
            return;
        case Optimization::Frustum:
            lod_enabled = true;
            return;
        case Optimization::FrustumLoD:
            culling_enabled = lod_enabled = false;
            return;
        default:
            std::cout << "Wrong optimization level! Current value = " << get_enabled_optimizations() << std::endl;
            return;
    }
}

void Scene::create_graphic_object(const std::string &name, GraphicObject &out)
{
    static auto xml_resources = xml.child("Resources");
    static auto xml_models = xml_resources.child("Models");

    Material material;

    auto xml_model = xml_models.find_child_by_attribute("id", name.c_str());
    fs::path xml_model_mesh = base_path / xml_model.child("Mesh").attribute("path").value();
    auto xml_model_material = xml_model.child("Material");
    auto xml_model_material_texture = base_path / xml_model_material.child("Texture").attribute("path").value();
    auto xml_model_material_phong = xml_model_material.child("PhongParameters");

    material.set_diffuse(Extensions::string_as_vec4(xml_model_material_phong.attribute("diffuse").value()));
    material.set_ambient(Extensions::string_as_vec4(xml_model_material_phong.attribute("ambient").value()));
    material.set_specular(Extensions::string_as_vec4(xml_model_material_phong.attribute("specular").value()));
    material.set_shininess(std::stof(xml_model_material_phong.attribute("shininess").value()));

    ResourceManager::get_instance().get_mesh(xml_model_mesh);
    ResourceManager::get_instance().get_texture(xml_model_material_texture);
    out.set_mesh(xml_model_mesh);
    out.set_material(material);
    out.set_texture(xml_model_material_texture);
}

void Scene::simulate_mouse()
{
    static int mouse_x, mouse_y, prev_button_state = GLFW_RELEASE;

    if ((prev_button_state == GLFW_RELEASE) &&
        (InputManager::get_instance().get_mouse_state(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS))
    {
        mouse_x = InputManager::get_instance().get_mouse_state('x');
        mouse_y = InputManager::get_instance().get_mouse_state('y');
        prev_button_state = GLFW_PRESS;
    }
    else if ((prev_button_state == GLFW_PRESS) &&
        (InputManager::get_instance().get_mouse_state(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS))
    {
        float new_x = InputManager::get_instance().get_mouse_state('x');
        float new_y = InputManager::get_instance().get_mouse_state('y');
        camera.rotate((new_x - mouse_x) / 300, (new_y - mouse_y) / 300);
        mouse_x = new_x;
        mouse_y = new_y;
    }
    else if (InputManager::get_instance().get_mouse_state(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
    {
        prev_button_state = GLFW_RELEASE;
    }
}

void Scene::simulate_keyboard(double delta_s)
{
    static InputManager &input_manager = InputManager::get_instance();

    if (input_manager.get_arrow_state(GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        camera.move_oxz(0, -delta_s);
    }
    if (input_manager.get_arrow_state(GLFW_KEY_UP) == GLFW_PRESS)
    {
        camera.move_oxz(delta_s, 0);
    }
    if (input_manager.get_arrow_state(GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        camera.move_oxz(0, delta_s);
    }
    if (input_manager.get_arrow_state(GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        camera.move_oxz(-delta_s, 0);
    }
}
