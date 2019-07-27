#include <cgraphics/Texture.hpp>

#include <IL/ilu.h>

#include <iostream>

namespace fs = std::filesystem;

void Texture::load(const fs::path &path, bool flip)
{
    auto image_id = ilGenImage();
    ilBindImage(image_id);

    if (!ilLoadImage(path.c_str()))
    {
        std::cout << "Cannot load texture " << path << std::endl;
    }
    else
    {
        if (flip)
        {
            iluFlipImage();
        }
        auto width = ilGetInteger(IL_IMAGE_WIDTH);
        auto height = ilGetInteger(IL_IMAGE_HEIGHT);

        auto *pixels = new ILubyte[width*height*4];
        ilCopyPixels(0, 0, 0,
                    width, height, 1,
                    IL_RGBA, IL_UNSIGNED_BYTE, pixels);

        glGenTextures(1, &texture_id);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                    width, height, 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        delete[] pixels;
    }
    ilDeleteImage(image_id);
}

void Texture::apply(GLenum texture_unit) const
{
    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_2D, texture_id);
}
