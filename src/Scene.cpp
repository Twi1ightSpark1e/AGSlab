#include <cgraphics/Scene.hpp>
#include <cgraphics/InputManager.hpp>
#include <cgraphics/Extensions.hpp>
#include <cgraphics/ResourceManager.hpp>
#include <cgraphics/RenderManager.hpp>
#include <cgraphics/Texture.hpp>

#include <iostream>
#include <tuple>

#include <ohf/Exception.hpp>
#include <ohf/tcp/Socket.hpp>

namespace fs = std::experimental::filesystem;

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
    ShaderPaths light_shader = {
        base_path / xml_shader_light.attribute("vertex-path").value(),
        base_path / xml_shader_light.attribute("fragment-path").value()
    }, skybox_shader = {
        base_path / xml_shader_skybox.attribute("vertex-path").value(),
        base_path / xml_shader_skybox.attribute("fragment-path").value()
    }, aabb_shader = {
        base_path / xml_shader_aabb.attribute("vertex-path").value(),
        base_path / xml_shader_aabb.attribute("fragment-path").value(),
    };
    RenderManager::get_instance().init(light_shader, skybox_shader, aabb_shader);
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

    objects.clear();
    auto descriptions = protocol.get_nearby_objects(camera.get_eye() + camera.get_center(), 100);
    for (auto &descr : descriptions)
    {
        auto object = create_graphic_object(std::string(descr.model_name.begin()));

        object.set_id(descr.object_id);
        object.set_position(glm::vec3(descr.x, descr.y, descr.z));
        object.set_rotation(-descr.rotation);
        object.set_aabb(descr.aabb);

        objects[descr.object_id] = object;
    }
}

void Scene::draw()
{
    static auto &render_manager = RenderManager::get_instance();

    render_manager.start();

    render_manager.set_camera(camera);
    render_manager.set_light(light);
    render_manager.set_skybox(skybox);

    if (culling_enabled)
    {
        frustum_culling();
    }
    else
    {
        for (auto &object : objects)
        {
            render_manager.add_to_queue(object.second);
        }
    }

    render_manager.finish();
}

void Scene::frustum_culling()
{
    static auto &render_manager = RenderManager::get_instance();
    std::map<int, GraphicObject> temp;

    for (auto &object : objects)
    {
        auto pvm = camera.get_projection_matrix() * camera.get_view_matrix() * object.second.get_model();
        auto aabb = object.second.get_aabb();
        std::vector<glm::vec4> aabb_vertices = {
            glm::vec4(+aabb[0], +aabb[1], +aabb[2], 1.0),
            glm::vec4(+aabb[0], +aabb[1], -aabb[2], 1.0),
            glm::vec4(+aabb[0], -aabb[1], +aabb[2], 1.0),
            glm::vec4(+aabb[0], -aabb[1], -aabb[2], 1.0),
            glm::vec4(-aabb[0], +aabb[1], +aabb[2], 1.0),
            glm::vec4(-aabb[0], +aabb[1], -aabb[2], 1.0),
            glm::vec4(-aabb[0], -aabb[1], +aabb[2], 1.0),
            glm::vec4(-aabb[0], -aabb[1], -aabb[2], 1.0)
        };
        for (auto &aabb_vertex : aabb_vertices)
        {
            aabb_vertex = pvm * aabb_vertex;
            aabb_vertex /= aabb_vertex.w;

            if (((aabb_vertex.x <= 1) && (aabb_vertex.x >= -1)) &&
                ((aabb_vertex.y <= 1) && (aabb_vertex.y >= -1)) &&
                ((aabb_vertex.z <= 1) && (aabb_vertex.z >= -1)))
            {
                render_manager.add_to_queue(object.second);
            }
        }
    }
}

Camera& Scene::get_camera()
{
    return camera;
}

bool Scene::get_culling_enabled() const
{
    return culling_enabled;
}

void Scene::toggle_culling()
{
    culling_enabled = !culling_enabled;
}

GraphicObject Scene::create_graphic_object(const std::string &name)
{
    static auto xml_resources = xml.child("Resources");
    static auto xml_models = xml_resources.child("Models");

    GraphicObject graphic_object;
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
    graphic_object.set_mesh(xml_model_mesh);
    graphic_object.set_material(material);
    graphic_object.set_texture(xml_model_material_texture);

    return graphic_object;
}

void Scene::simulate_mouse()
{
    static int mouse_x, mouse_y, prev_button_state = GLUT_UP;

    if ((prev_button_state == GLUT_UP) && 
        (InputManager::get_instance().get_mouse_state(GLUT_RIGHT_BUTTON) == GLUT_DOWN))
    {
        mouse_x = InputManager::get_instance().get_mouse_state('x');
        mouse_y = InputManager::get_instance().get_mouse_state('y');
        prev_button_state = GLUT_DOWN;
    }
    else if ((prev_button_state == GLUT_DOWN) && 
        (InputManager::get_instance().get_mouse_state(GLUT_RIGHT_BUTTON) == GLUT_DOWN))
    {
        float new_x = InputManager::get_instance().get_mouse_state('x');
        float new_y = InputManager::get_instance().get_mouse_state('y');
        camera.rotate((new_x - mouse_x) / 300, (new_y - mouse_y) / 300);
        mouse_x = new_x;
        mouse_y = new_y;
    }
    else if (InputManager::get_instance().get_mouse_state(GLUT_RIGHT_BUTTON) == GLUT_UP)
    {
        prev_button_state = GLUT_UP;
    }
}

void Scene::simulate_keyboard(double delta_s)
{
    static InputManager &input_manager = InputManager::get_instance();

    if (input_manager.get_arrow_state(GLUT_KEY_LEFT) == GLUT_DOWN)
    {
        camera.move_oxz(0, -delta_s);
    }
    if (input_manager.get_arrow_state(GLUT_KEY_UP) == GLUT_DOWN)
    {
        camera.move_oxz(delta_s, 0);
    }
    if (input_manager.get_arrow_state(GLUT_KEY_RIGHT) == GLUT_DOWN)
    {
        camera.move_oxz(0, delta_s);
    }
    if (input_manager.get_arrow_state(GLUT_KEY_DOWN) == GLUT_DOWN)
    {
        camera.move_oxz(-delta_s, 0);
    }
}