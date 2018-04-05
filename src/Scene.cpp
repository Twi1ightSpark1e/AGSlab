#include <cgraphics/Scene.hpp>
#include <cgraphics/CameraController.hpp>
#include <cgraphics/Extensions.hpp>
#include <cgraphics/ResourceManager.hpp>
#include <cgraphics/RenderManager.hpp>

#include <iostream>
#include <tuple>

namespace fs = std::experimental::filesystem;

void Scene::init(const fs::path &base_path)
{
    static std::vector<std::tuple<std::string,glm::vec3,float>> scene = 
    {
        std::make_tuple("drug_st",  glm::vec3(   0, 1,   0),   0),
        std::make_tuple("big_tree", glm::vec3(   1, 1,   5),   0),
        std::make_tuple("coffee",   glm::vec3( -10, 1,   0),   0),
        std::make_tuple("bus_stop", glm::vec3(  -3, 0.9, 5),   0),
        std::make_tuple("car_wh",   glm::vec3(  -3, 0,   7),   0),
        std::make_tuple("car_wh",   glm::vec3(   2, 0,   7),   0),
        std::make_tuple("light",    glm::vec3(-5.5, 1, 4.4), -90)
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
    for (auto &object : objects)
    {
        RenderManager::get_instance().add_to_queue(object);
    }

    RenderManager::get_instance().set_camera(camera);
    RenderManager::get_instance().set_light(light);
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

    auto xml_model = xml_models.find_child_by_attribute("id", name.c_str());
    fs::path xml_model_mesh = base_path / xml_model.child("Mesh").attribute("path").value();
    auto xml_model_material = xml_model.child("Material");
    //auto xml_model_material_texture = xml_model_material.child("Texture").attribute("path").value();
    auto xml_model_material_phong = xml_model_material.child("PhongParameters");

    material.set_diffuse(Extensions::string_as_vec4(xml_model_material_phong.attribute("diffuse").value()));
    material.set_ambient(Extensions::string_as_vec4(xml_model_material_phong.attribute("ambient").value()));
    material.set_specular(Extensions::string_as_vec4(xml_model_material_phong.attribute("specular").value()));
    material.set_shininess(std::stof(xml_model_material_phong.attribute("shininess").value()));

    ResourceManager::get_instance().get_mesh(xml_model_mesh);
    graphic_object.set_mesh(xml_model_mesh);
    graphic_object.set_material(material);

    return graphic_object;
}

void Scene::simulate_mouse()
{
    static int mouse_x, mouse_y, prev_button_state = GLUT_UP;

    if ((prev_button_state == GLUT_UP) && 
        (CameraController::get_instance().get_mouse_state(GLUT_RIGHT_BUTTON) == GLUT_DOWN))
    {
        mouse_x = CameraController::get_instance().get_mouse_state('x');
        mouse_y = CameraController::get_instance().get_mouse_state('y');
        prev_button_state = GLUT_DOWN;
    }
    else if ((prev_button_state == GLUT_DOWN) && 
        (CameraController::get_instance().get_mouse_state(GLUT_RIGHT_BUTTON) == GLUT_DOWN))
    {
        float new_x = CameraController::get_instance().get_mouse_state('x');
        float new_y = CameraController::get_instance().get_mouse_state('y');
        camera.rotate((new_x - mouse_x) / 300, (new_y - mouse_y) / 300);
        mouse_x = new_x;
        mouse_y = new_y;
    }
    else if (CameraController::get_instance().get_mouse_state(GLUT_RIGHT_BUTTON) == GLUT_UP)
    {
        prev_button_state = GLUT_UP;
    }
}

void Scene::simulate_keyboard(double delta_s)
{
    if (CameraController::get_instance().get_arrow_state(GLUT_KEY_LEFT) == GLUT_DOWN)
    {
        camera.move_oxz(0, -delta_s);
    }
    if (CameraController::get_instance().get_arrow_state(GLUT_KEY_UP) == GLUT_DOWN)
    {
        camera.move_oxz(delta_s, 0);
    }
    if (CameraController::get_instance().get_arrow_state(GLUT_KEY_RIGHT) == GLUT_DOWN)
    {
        camera.move_oxz(0, delta_s);
    }
    if (CameraController::get_instance().get_arrow_state(GLUT_KEY_DOWN) == GLUT_DOWN)
    {
        camera.move_oxz(-delta_s, 0);
    }
}
