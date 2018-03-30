#include <cgraphics/Scene.hpp>
#include <cgraphics/CameraController.hpp>
#include <cgraphics/Extensions.hpp>
#include <cgraphics/ResourceManager.hpp>

#include <GL/freeglut.h>

#include <iostream>

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

    auto object = create_graphic_object("house_1_bl");
    object.set_position(glm::vec3(0, 0, 0));
    object.set_rotation(0);
    objects.push_back(object);

    object = create_graphic_object("light");
    object.set_position(glm::vec3(-6.5, -0.55,   3));
    object.set_rotation(-90);
    objects.push_back(object);

    object = create_graphic_object("light");
    object.set_position(glm::vec3( 6.5, -0.55,   3));
    object.set_rotation(-89);
    objects.push_back(object);

    object = create_graphic_object("ambul");
    object.set_position(glm::vec3( 2.5,  -1.7, 5.2));
    object.set_rotation(0);
    objects.push_back(object);

    auto xml_resources = xml.child("Resources");

    auto xml_light = xml_resources.child("Light");
    light.set_position(Extensions::string_as_vec4(xml_light.child("Direction").attribute("vector").value(), 0));
    auto xml_light_params = xml_light.child("PhongParameters");
    light.set_diffuse(Extensions::string_as_vec4(xml_light_params.attribute("diffuse").value()));
    light.set_ambient(Extensions::string_as_vec4(xml_light_params.attribute("ambient").value()));
    light.set_specular(Extensions::string_as_vec4(xml_light_params.attribute("specular").value()));

    auto xml_shaders = xml_resources.child("Shaders");
    auto xml_shader_phong = xml_shaders.find_child_by_attribute("id", "phong");
    shader.load_vertex_shader(base_path / xml_shader_phong.attribute("vertex-path").value(), false);
    shader.load_fragment_shader(base_path / xml_shader_phong.attribute("fragment-path").value(), false);
    shader.link(false);
}

void Scene::simulate(double seconds)
{
    simulate_keyboard(seconds);
    simulate_mouse();
}

void Scene::draw()
{
    shader.activate();

    const auto &projection = camera.get_projection_matrix();
    const auto &view = camera.get_view_matrix();

    shader.set_uniform_mat4("ProjectionMatrix", projection);
    shader.set_uniform_vec4("lAmbient", light.get_ambient());
    shader.set_uniform_vec4("lDiffuse", light.get_diffuse());
    shader.set_uniform_vec4("lSpecular", light.get_specular());
    shader.set_uniform_vec4("lPosition", view * light.get_position());
    shader.set_uniform_float("SpecularPow", 64);

    for (auto &object : objects)
    {
        auto model = object.get_model();
        shader.set_uniform_mat4("ModelViewMatrix", view * model);
        auto material = object.get_material();
        shader.set_uniform_vec4("mAmbient", material.get_ambient());
        shader.set_uniform_vec4("mDiffuse", material.get_diffuse());
        shader.set_uniform_vec4("mSpecular", material.get_specular());
        ResourceManager::get_instance().get_mesh(object.get_mesh()).render();
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
