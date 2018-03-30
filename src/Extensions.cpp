#include <cgraphics/Extensions.hpp>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

namespace fs = std::experimental::filesystem;

std::vector<std::string> Extensions::read_file(const std::experimental::filesystem::path &file)
{
    std::vector<std::string> temp;
    std::ifstream txt(file);
    if (!txt.is_open())
    {
        std::cerr << "File " << file << " not found! Exitting" << std::endl;
        exit(1);
    }
    while (std::getline(txt, temp.emplace_back())) {}
    txt.close();
    return temp;
}

std::tuple<GLchar**,GLint*> Extensions::vector_to_array(const std::vector<std::string> &vector)
{
    auto strs = new GLchar*[vector.size()];
    auto lengths = new GLint[vector.size()];
    for (uint i = 0; i < vector.size(); i++)
    {
        strs[i] = new char[vector[i].length()+2];
        int len = vector[i].copy(strs[i], vector[i].length(), 0);
        strs[i][len] = '\n';
        strs[i][len+1] = 0;
        lengths[i] = vector[i].length() + 1;
    }
    return std::make_tuple(strs,lengths);
}

void Extensions::remove_array(GLchar **array, int length)
{
    for (int i = 0; i < length; i++)
    {
        delete[] array[i];
    }
    delete[] array;
}

fs::path Extensions::resolve_dots(const fs::path &path)
{
    auto tmp = path.string();
    while (tmp.find("/./") != std::string::npos)
    {
        tmp.replace(tmp.find("/./"), 3, "/");
    }
    while (tmp.find("/../") != std::string::npos)
    {
        auto p = fs::path(tmp.substr(0, tmp.find("/../"))).parent_path();
        tmp = p.string() + tmp.substr(tmp.find("/../") + 3);
    }
    return fs::path(tmp);
}

glm::vec4 Extensions::string_as_vec4(const std::string &str, const float &w)
{
    std::istringstream ss(str);
    glm::vec4 v;
    ss >> v.x >> v.y >> v.z;
    v.w = w;
    return v;
}