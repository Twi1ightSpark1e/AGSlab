#include <cgraphics/RenderManager.hpp>
#include <cgraphics/ResourceManager.hpp>

#include <GL/freeglut.h>

namespace fs = std::experimental::filesystem;

void RenderManager::init(const fs::path& vsh, const fs::path& fsh)
{
    shader.load_vertex_shader(vsh, false);
    shader.load_fragment_shader(fsh, false);
    shader.link(false);
}

void RenderManager::start()
{
    glClearColor (0.0, 0.0, 0.0, 1.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable (GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    objects.clear();
    shader.activate();
}

void RenderManager::set_camera(const Camera &camera)
{
    this->camera = camera;
}

void RenderManager::set_light(const Light &light)
{
    this->light = light;
}

void RenderManager::add_to_queue(const GraphicObject &object)
{
    objects.push_back(object);
}

void RenderManager::finish()
{
    const auto &view = camera.get_view_matrix();

    shader.set_uniform_mat4("ProjectionMatrix", camera.get_projection_matrix());
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

    Shader::deactivate();

    glutSwapBuffers();
}
