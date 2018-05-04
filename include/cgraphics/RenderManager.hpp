#pragma once

#include <cgraphics/Shader.hpp>
#include <cgraphics/Camera.hpp>
#include <cgraphics/Light.hpp>
#include <cgraphics/GraphicObject.hpp>
#include <cgraphics/SkyBox.hpp>

#include <map>
#include <vector>
#include <experimental/filesystem>

#include <glm/glm.hpp>

struct ShaderPaths
{
    std::experimental::filesystem::path vertex, fragment;
};

class RenderManager
{
private:
    struct ObjectState
    {
        GraphicObject object;
        bool updated = false;
        GLuint ubo_index;
    };

    struct PerSceneBlock
    {
        glm::mat4 ProjectionMatrix;
        glm::vec4 lAmbient;
        glm::vec4 lDiffuse;
        glm::vec4 lSpecular;
        glm::vec4 lPosition;
    };

    struct PerObjectBlock
    {
        glm::mat4 ModelViewMatrix;
        glm::vec4 mAmbient;
        glm::vec4 mDiffuse;
        glm::vec4 mSpecular;
    };

    Shader light_shader, skybox_shader, aabb_shader;
    Camera camera;
    Light  light;
    SkyBox skybox;
    std::vector<GraphicObject> objects;
    std::map<int, ObjectState> object_states;
    GLuint per_scene_ubo_index;
    static long update_count;

    std::experimental::filesystem::path aabb_mesh_id;
    bool aabb_render_mode = false;

    RenderManager() = default;
    ~RenderManager() = default;

    GLuint create_per_scene_block();
    GLuint create_per_object_block(const GraphicObject &object);
    void   update_per_scene_block(int ubo_index);
    void   update_per_object_block(int ubo_index, const GraphicObject &object);
public:
    RenderManager(const RenderManager&) = delete;
    RenderManager(RenderManager&&) = delete;
    RenderManager& operator=(const RenderManager&) = delete;
    RenderManager& operator=(RenderManager&&) = delete;

    static RenderManager& get_instance()
    {
        static RenderManager manager;
        return manager;
    }
    static long get_update_count()
    {
        return update_count;
    }
    unsigned long get_objects_count() const;

    void init(const ShaderPaths& light, const ShaderPaths& skybox, const ShaderPaths& aabb);
    void start();

    void set_camera(const Camera &camera);
    void set_light(const Light &light);
    void set_skybox(const SkyBox &skybox);
    void add_to_queue(GraphicObject object);

    void finish();

    void set_aabb_mesh_path(const std::experimental::filesystem::path &path);
    void toggle_aabb_render_mode();
private:
    void render_skybox();
    void render_objects();
    void render_aabb();
};
