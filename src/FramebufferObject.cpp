#include <GL/glew.h>

#include <cgraphics/FramebufferObject.hpp>

#include <iostream>

int FramebufferObject::FramebufferObject::width, FramebufferObject::FramebufferObject::height;

void FramebufferObject::init(int width, int height, int samples)
{
    std::cout << "Initializing FBO with w=" << width << "; h=" << height << std::endl;
    if (fbo_index != 0)
    {
        std::cout << "Deleting previous FBO with w=" << FramebufferObject::width << "; h=" << FramebufferObject::height << std::endl;

        glDeleteTextures(1, &color_tex);
        glDeleteTextures(1, &depth_tex);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &fbo_index);
    }
    FramebufferObject::width = width;
    FramebufferObject::height = height;
    this->samples = samples;
    
    if (samples != 1)
    {
        init_multisample();
        return;
    }

    glGenTextures(1, &color_tex);
    glBindTexture(GL_TEXTURE_2D, color_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        FramebufferObject::width, FramebufferObject::height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glGenTextures(1, &depth_tex);
    glBindTexture(GL_TEXTURE_2D, depth_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24,
        FramebufferObject::width, FramebufferObject::height, 0,
        GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);

    glGenFramebuffers(1, &fbo_index);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_index);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_tex, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_tex, 0);
    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FramebufferObject::init_multisample()
{
    glGenTextures(1, &color_tex);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, color_tex);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples,
        GL_COLOR_RENDERABLE, FramebufferObject::width, FramebufferObject::height, false);

    glGenTextures(1, &depth_tex);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depth_tex);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples,
        GL_DEPTH_RENDERABLE, FramebufferObject::width, FramebufferObject::height, false);

    glGenFramebuffers(1, &fbo_index);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_index);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_tex, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_tex, 0);
    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FramebufferObject::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_index);
    glViewport(0, 0, FramebufferObject::width, FramebufferObject::height);
    //glDrawBuffer(GL_BACK);
}

void FramebufferObject::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, FramebufferObject::width, FramebufferObject::height);
    glDrawBuffer(GL_BACK);
}

void FramebufferObject::copy_to_fbo(const FramebufferObject &dest)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_index);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest.fbo_index);
    glBlitFramebuffer(
        0, 0, FramebufferObject::width, FramebufferObject::height,
        0, 0, FramebufferObject::width, FramebufferObject::height,
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
        GL_LINEAR
    );
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FramebufferObject::copy_to_fbo(const GLuint &dest_id)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_index);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest_id);
    if (dest_id == 0)
    {
        glDrawBuffer(GL_BACK);
    }
    glBlitFramebuffer(
        0, 0, FramebufferObject::width, FramebufferObject::height,
        0, 0, FramebufferObject::width, FramebufferObject::height,
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
        GL_LINEAR
    );
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FramebufferObject::bind_color_texture(GLenum texture_unit)
{
    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_2D, color_tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void FramebufferObject::bind_depth_texture(GLenum texture_unit)
{
    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_2D, depth_tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
