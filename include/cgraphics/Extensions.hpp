#pragma once

#include <GL/glew.h>

#include <experimental/filesystem>
#include <vector>
#include <tuple>

class Extensions
{
public:
    static std::vector<std::string> read_file(const std::experimental::filesystem::path &file);
    static std::tuple<GLchar**,GLint*> vector_to_array(const std::vector<std::string> &vector);
    static void remove_array(GLchar **array, int length);
    static std::experimental::filesystem::path resolve_dots(const std::experimental::filesystem::path &path);
};