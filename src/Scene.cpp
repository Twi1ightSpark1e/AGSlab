#include <cgraphics/Scene.hpp>
#include <cgraphics/InputManager.hpp>
#include <cgraphics/Extensions.hpp>
#include <cgraphics/ResourceManager.hpp>
#include <cgraphics/RenderManager.hpp>
#include <cgraphics/Texture.hpp>

#include <iostream>
#include <tuple>

namespace fs = std::experimental::filesystem;

void Scene::init(const fs::path &base_path)
{
    static std::vector<std::tuple<std::string,glm::vec3,float>> scene = 
    {
        std::make_tuple("drug_st",    glm::vec3(     0,    1,    0),   0),
        std::make_tuple("big_tree",   glm::vec3(     5,   .9,    4),   0),
        std::make_tuple("coffee",     glm::vec3(   -10, 1.15,    0),   0),
        std::make_tuple("bus_stop",   glm::vec3(    -3,  0.6,    5),   0),
        std::make_tuple("taxi",       glm::vec3(    -3, -.02,    7),   0),
        std::make_tuple("car_wh",     glm::vec3(     2,  -.1,    7),   0),
        std::make_tuple("light",      glm::vec3(  -5.5,    1,  4.4), -90),
        std::make_tuple("tile",       glm::vec3(     0,  -.7,    0),   0),
        std::make_tuple("tile",       glm::vec3(   -10,  -.7,    0),   0),
        std::make_tuple("tile",       glm::vec3(   -10,  -.7,   10),   0),
        std::make_tuple("tile",       glm::vec3(     0,  -.7,   10),   0),
        std::make_tuple("road_ln_1",  glm::vec3(-20.49,  -.6, -.49),   0),
        std::make_tuple("tile",       glm::vec3(    10,  -.7,    0),   0),
        std::make_tuple("tile",       glm::vec3(    20,  -.7,    0),   0),
        std::make_tuple("tile",       glm::vec3(    20,  -.7,   10),   0),
        std::make_tuple("road_ln_1",  glm::vec3(     0,  -.6,   10),  90),
        std::make_tuple("road_ln_1",  glm::vec3(   -10,  -.6,   10),  90),
        std::make_tuple("road_cor_1", glm::vec3( 10.49,  -.6,   10),  90),
        std::make_tuple("road_cor_1", glm::vec3(-20.49,  -.6,   10), 270),
        std::make_tuple("sky_s_rd",   glm::vec3(    10,  3.8,    0),   0),
        std::make_tuple("sky_s_rd",   glm::vec3(  16.8,  3.8,    0),   0),
    };

    this->base_path = base_path;
    auto result = xml.load_file((base_path / "Resources.xml").c_str());
    if (!result)
    {
        std::cout << "Cannot load Resource.xml" << std::endl;
        return;
    }

    GraphicObject object;
    int id = 0;
    for (auto scene_object : scene)
    {
        object = create_graphic_object(std::get<0>(scene_object));
        object.set_position(std::get<1>(scene_object));
        object.set_rotation(std::get<2>(scene_object));
        object.set_id(id++);
        objects.push_back(object);
    }

    auto xml_resources = xml.child("Resources");

    auto xml_light = xml_resources.child("Light");
    light.set_position(Extensions::string_as_vec4(xml_light.child("Direction").attribute("vector").value(), 0));
    auto xml_light_params = xml_light.child("PhongParameters");
    light.set_diffuse(Extensions::string_as_vec4(xml_light_params.attribute("diffuse").value()));
    light.set_ambient(Extensions::string_as_vec4(xml_light_params.attribute("ambient").value()));
    light.set_specular(Extensions::string_as_vec4(xml_light_params.attribute("specular").value()));

    auto xml_shaders = xml_resources.child("Shaders");
    auto xml_shader_phong = xml_shaders.find_child_by_attribute("id", "phong");
    RenderManager::get_instance().init(
        base_path / xml_shader_phong.attribute("vertex-path").value(),
        base_path / xml_shader_phong.attribute("fragment-path").value()
    );
}

void Scene::simulate(double seconds)
{
    simulate_keyboard(seconds);
    simulate_mouse();
}

void Scene::draw()
{
    RenderManager::get_instance().set_camera(camera);
    RenderManager::get_instance().set_light(light);
    
    for (auto &object : objects)
    {
        RenderManager::get_instance().add_to_queue(object);
    }
}

Camera& Scene::get_camera()
{
    return camera;
}

GraphicObject Scene::create_graphic_object(const std::string &name)
{
    static auto xml_resources = xml.child("Resources");
    static auto xml_models = xml_resources.child("Models");

    GraphicObject graphic_object;
    Material material;
    //Texture texture;

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
    if (InputManager::get_instance().get_arrow_state(GLUT_KEY_LEFT) == GLUT_DOWN)
    {
        camera.move_oxz(0, -delta_s);
    }
    if (InputManager::get_instance().get_arrow_state(GLUT_KEY_UP) == GLUT_DOWN)
    {
        camera.move_oxz(delta_s, 0);
    }
    if (InputManager::get_instance().get_arrow_state(GLUT_KEY_RIGHT) == GLUT_DOWN)
    {
        camera.move_oxz(0, delta_s);
    }
    if (InputManager::get_instance().get_arrow_state(GLUT_KEY_DOWN) == GLUT_DOWN)
    {
        camera.move_oxz(-delta_s, 0);
    }
}
