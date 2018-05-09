#pragma once

#include <GL/freeglut.h>

class FramebufferObject
{
private:
    GLuint fbo_index = 0, color_tex, depth_tex;
    static int width, height;
    int samples;

    void init_multisample();
public:
    void init(int width, int height, int samples = 1);
    void bind();
    static void unbind();

    void copy_to_fbo(const FramebufferObject &dest);

    void bind_color_texture(GLenum texture_unit = GL_TEXTURE0);
    void bind_depth_texture(GLenum texture_unit = GL_TEXTURE1);
};
