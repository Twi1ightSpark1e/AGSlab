#pragma once

#include <cgraphics/Shader.hpp>
#include <cgraphics/Camera.hpp>
#include <cgraphics/Light.hpp>
#include <cgraphics/GraphicObject.hpp>
#include <cgraphics/SkyBox.hpp>
#include <cgraphics/FramebufferObject.hpp>

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
public:
    enum MSAAMode
    {
        None,
        X2,
        X4,
        X8,
        mAmount
    };

    enum PostProcessingMode
    {
        Simple,
        Sepia,
        Grey,
        pAmount
    };
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

    std::array<Shader, Shader::Type::Amount> shaders;
    Camera camera;
    Light  light;
    SkyBox skybox;

    std::array<FramebufferObject, MSAAMode::mAmount> fbo_array;
    Mesh rectangle;
    MSAAMode current_msaa;
    PostProcessingMode pp_mode;

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

    void init(std::map<Shader::Type, ShaderPaths> paths);
    void start();

    void set_camera(const Camera &camera);
    void set_light(const Light &light);
    void set_skybox(const SkyBox &skybox);
    void add_to_queue(GraphicObject object);

    void finish();

    void set_aabb_mesh_path(const std::experimental::filesystem::path &path);
    void toggle_aabb_render_mode();

    void next_multisampling_mode();
    MSAAMode get_multisampling_mode() const;
    void next_post_processing();
    PostProcessingMode get_post_processing() const;
    void initialize_framebuffers(int width, int height);
private:
    void render_skybox();
    void render_objects();
    void render_aabb();
    void render_to_screen();
};
