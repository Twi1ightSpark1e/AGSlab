#include <cgraphics/RenderManager.hpp>
#include <cgraphics/ResourceManager.hpp>

#include <GL/freeglut.h>

#include <algorithm>
#include <cmath>
#include <iostream>

namespace fs = std::filesystem;

long RenderManager::update_count;

void RenderManager::init(std::map<Shader::Type, ShaderPaths> paths)
{
    for (auto &path : paths)
    {
        shaders[path.first].load_vertex_shader(path.second.vertex, false);
        shaders[path.first].load_fragment_shader(path.second.fragment, false);
        shaders[path.first].link(false);

        std::cout << path.second.vertex.filename().replace_extension("") << " compiled and linked" << std::endl;
    }

    create_per_scene_block();

    rectangle.load_rectangle();
    current_msaa = MSAAMode::X8;
}

void RenderManager::start()
{
    fbo_array[current_msaa].bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
    render_objects();
    render_skybox();
    if (aabb_render_mode)
    {
        render_aabb();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    render_to_screen();
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

void RenderManager::set_aabb_mesh_path(const fs::path &path)
{
    aabb_mesh_id = path;
}

void RenderManager::toggle_aabb_render_mode()
{
    aabb_render_mode = !aabb_render_mode;
}

unsigned long RenderManager::get_objects_count() const
{
    return objects.size();
}

void RenderManager::next_multisampling_mode()
{
    current_msaa = (MSAAMode)((current_msaa + 1) % (int)MSAAMode::mAmount);
}

RenderManager::MSAAMode RenderManager::get_multisampling_mode() const
{
    return current_msaa;
}

void RenderManager::next_post_processing()
{
    pp_mode = (PostProcessingMode)((pp_mode + 1) % (int)PostProcessingMode::pAmount);
}

RenderManager::PostProcessingMode RenderManager::get_post_processing() const
{
    return pp_mode;
}

void RenderManager::initialize_framebuffers(int width, int height)
{
    int i = 0;
    for (auto &fbo : fbo_array)
    {
        fbo.init(width, height, std::pow(2, i++));
    }
}

void RenderManager::render_objects()
{
    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);

    shaders[Shader::Type::DirectLight].activate();
    shaders[Shader::Type::DirectLight].set_uniform_int("tex", 1);

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
}

void RenderManager::render_skybox()
{
    glDepthFunc(GL_LEQUAL);
    glCullFace(GL_FRONT);

    shaders[Shader::Type::SkyBox].activate();
    shaders[Shader::Type::SkyBox].set_uniform_int("tex", 0);
    shaders[Shader::Type::SkyBox].set_uniform_mat4("uProjectionMatrix", camera.get_projection_matrix());
    shaders[Shader::Type::SkyBox].set_uniform_mat4("uModelViewMatrix", camera.get_view_matrix() * glm::mat4 {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    });
    skybox.render(GL_TEXTURE0);
}

void RenderManager::render_aabb()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDisable(GL_CULL_FACE);

    shaders[Shader::Type::BoundingBox].activate();
    shaders[Shader::Type::BoundingBox].set_uniform_mat4("ProjectionMatrix", camera.get_projection_matrix());
    shaders[Shader::Type::BoundingBox].set_uniform_vec4("Color", glm::vec4(1, 0, 0, 1));

    for (auto &object : objects)
    {
        auto aabb = object.get_aabb();
        auto scale = glm::mat4 {
            {aabb[0],       0,       0, 0},
            {      0, aabb[1],       0, 0},
            {      0,       0, aabb[2], 0},
            {      0,       0,       0, 1},
        };
        shaders[Shader::Type::BoundingBox].set_uniform_mat4("ModelViewMatrix", camera.get_view_matrix() * object.get_model() * scale);
        ResourceManager::get_instance().get_mesh(aabb_mesh_id).render();
    }
}

void RenderManager::render_to_screen()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    if (current_msaa != MSAAMode::None)
    {
        fbo_array[current_msaa].copy_to_fbo(fbo_array[MSAAMode::None]);
    }

    FramebufferObject::unbind();
    shaders[pp_mode].activate();
    fbo_array[MSAAMode::None].bind_color_texture();
    shaders[pp_mode].set_uniform_int("tex", 0);
    rectangle.render();
}
