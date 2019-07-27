#include <cgraphics/SkyBox.hpp>

#include <iostream>

namespace fs = std::filesystem;

void SkyBox::load(const fs::path &base_path, const std::string &extension) noexcept
{
    std::vector<ILbyte> vec;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    auto base_filename = base_path.filename();
    load_texture(GL_TEXTURE_CUBE_MAP_POSITIVE_X, (base_path / (base_filename.string() + "_Left")).replace_extension(extension));
    load_texture(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, (base_path / (base_filename.string() + "_Right")).replace_extension(extension));
    load_texture(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, (base_path / (base_filename.string() + "_Up")).replace_extension(extension));
    load_texture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, (base_path / (base_filename.string() + "_Down")).replace_extension(extension));
    load_texture(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, (base_path / (base_filename.string() + "_Front")).replace_extension(extension));
    load_texture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, (base_path / (base_filename.string() + "_Back")).replace_extension(extension));

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    mesh = base_path.parent_path().parent_path().parent_path() / "meshes" / "box.obj";
}

void SkyBox::load_texture(const int &target, const fs::path &path)
{
    auto info = load_image(path);
    glTexImage2D(target, 0, GL_RGBA,
        info.width, info.height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE,
        &info.data[0]);
}

SkyBox::TextureInfo SkyBox::load_image(const std::filesystem::path &path)
{
    std::vector<ILbyte> vec;
    TextureInfo info;
    auto image_id = ilGenImage();
    ilBindImage(image_id);

    if (!ilLoadImage(path.c_str()))
    {
        std::cout << "Cannot load texture " << path << std::endl;
        vec.clear();
        info = {
            0, 0, vec
        };
    }
    else
    {
        auto width = ilGetInteger(IL_IMAGE_WIDTH);
        auto height = ilGetInteger(IL_IMAGE_HEIGHT);

        auto *pixels = new ILubyte[width*height*4];
        ilCopyPixels(0, 0, 0,
                    width, height, 1,
                    IL_RGBA, IL_UNSIGNED_BYTE, pixels);
        vec.reserve(width * height * 4);
        vec.assign(pixels, pixels + width*height*4);

        info = {
            width, height,
            vec
        };

        delete[] pixels;
    }
    ilDeleteImage(image_id);
    return info;
}

void SkyBox::render(GLenum texture_unit) noexcept
{
    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    ResourceManager::get_instance().get_mesh(mesh).render();
}
