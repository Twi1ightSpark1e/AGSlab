#include <cgraphics/Extensions.hpp>

std::vector<std::string> Extensions::read_file(const std::string &filename)
{
    std::vector<std::string> temp;
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "File " << filename << " not found! Exitting" << std::endl;
        exit(1);
    }
    while (std::getline(file, temp.emplace_back())) {}
    file.close();
    return temp;
}

std::tuple<GLchar**,GLint*> Extensions::vector_to_array(std::vector<std::string> vector)
{
    auto strs = new GLchar*[vector.size()];
    auto lengths = new GLint[vector.size()];
    for (uint i = 0; i < vector.size(); i++)
    {
        strs[i] = new char[vector[i].length()+1];
        int len = vector[i].copy(strs[i], vector[i].length(), 0);
        strs[i][len] = 0;
        lengths[i] = vector[i].length();
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