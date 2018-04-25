#include <cgraphics/RenderManager.hpp>
#include <cgraphics/ResourceManager.hpp>

#include <GL/freeglut.h>

#include <algorithm>
#include <iostream>

namespace fs = std::experimental::filesystem;

long RenderManager::update_count;

void RenderManager::init(const ShaderPaths& light, const ShaderPaths& skybox)
{
    light_shader.load_vertex_shader(light.vertex, false);
    light_shader.load_fragment_shader(light.fragment, false);
    light_shader.link(false);

    skybox_shader.load_vertex_shader(skybox.vertex, false);
    skybox_shader.load_fragment_shader(skybox.fragment, false);
    skybox_shader.link(false);

    create_per_scene_block();
}

void RenderManager::start()
{
    glClear(GL_DEPTH_BUFFER_BIT);

    objects.clear();
}

void RenderManager::set_camera(const Camera &camera)
{
    if (this->camera != camera)
    {
        this->camera = camera;
        update_per_scene_block(per_scene_ubo_index);
        std::for_each(object_states.begin(), object_states.end(), [&](auto &object) {
            object.second.updated = false;
        });
    }
}

void RenderManager::set_light(const Light &light)
{
    if (this->light != light)
    {
        this->light = light;
        update_per_scene_block(per_scene_ubo_index);
        std::for_each(object_states.begin(), object_states.end(), [&](auto &object) {
            object.second.updated = false;
        });
    }
}

void RenderManager::set_skybox(const SkyBox &skybox)
{
    this->skybox = skybox;
}

void RenderManager::add_to_queue(GraphicObject object)
{
    objects.push_back(object);

    auto it = object_states.find(object.get_id());
    if (it != object_states.end())
    {
        // такой объект есть в мапе
        // изменился ли объект?
        if ((it->second.object != object) || (!it->second.updated))
        {
            object_states[object.get_id()].object = object;
            update_per_object_block(object_states[object.get_id()].ubo_index, object);
            object_states[object.get_id()].updated = true;
        }
    }
    else
    {
        // объекта нет в мапе
        auto ubo_index = create_per_object_block(object);
        ObjectState state = {
            object,
            true,
            ubo_index
        };
        object_states[object.get_id()] = state;
    }
}

void RenderManager::finish()
{
    glEnable (GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);

    light_shader.activate();
    light_shader.set_uniform_int("tex", 1);
    
    glBindBuffer(GL_UNIFORM_BUFFER, per_scene_ubo_index);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, per_scene_ubo_index);
    
    for (auto &object : objects)
    {
        if (!object_states[object.get_id()].updated)
        {
            update_per_object_block(object_states[object.get_id()].ubo_index, object);
            object_states[object.get_id()].updated = true;
        }
        ResourceManager::get_instance().get_texture(object.get_texture()).apply(GL_TEXTURE1);
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, object_states[object.get_id()].ubo_index);
        ResourceManager::get_instance().get_mesh(object.get_mesh()).render();
    }

    glDepthFunc(GL_LEQUAL);
    glCullFace(GL_FRONT);

    skybox_shader.activate();
    skybox_shader.set_uniform_int("tex", 0);
    skybox_shader.set_uniform_mat4("uProjectionMatrix", camera.get_projection_matrix());
    skybox_shader.set_uniform_mat4("uModelViewMatrix", camera.get_view_matrix() * glm::mat4 {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    });
    skybox.render(GL_TEXTURE0);

    Shader::deactivate();
    glutSwapBuffers();
}

GLuint RenderManager::create_per_scene_block()
{
    glCreateBuffers(1, &per_scene_ubo_index);
    glBindBuffer(GL_UNIFORM_BUFFER, per_scene_ubo_index);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PerSceneBlock), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    return per_scene_ubo_index;
}

GLuint RenderManager::create_per_object_block(const GraphicObject &object)
{
    PerObjectBlock block = {
        camera.get_view_matrix() * object.get_model(),
        object.get_material().get_ambient(),
        object.get_material().get_diffuse(),
        object.get_material().get_specular()
    };

    uint index;
    glCreateBuffers(1, &index);
    glBindBuffer(GL_UNIFORM_BUFFER, index);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(block), &block, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    return index;
}

void   RenderManager::update_per_scene_block(int ubo_index)
{
    PerSceneBlock block = {
        camera.get_projection_matrix(),
        light.get_ambient(),
        light.get_diffuse(),
        light.get_specular(),
        camera.get_view_matrix() * light.get_position()
    };

    glBindBuffer(GL_UNIFORM_BUFFER, ubo_index);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(block), &block, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    RenderManager::update_count++;
}

void   RenderManager::update_per_object_block(int ubo_index, const GraphicObject &object)
{
    PerObjectBlock block = {
        camera.get_view_matrix() * object.get_model(),
        object.get_material().get_ambient(),
        object.get_material().get_diffuse(),
        object.get_material().get_specular()
    };

    glBindBuffer(GL_UNIFORM_BUFFER, ubo_index);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(block), &block, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    RenderManager::update_count++;
}
