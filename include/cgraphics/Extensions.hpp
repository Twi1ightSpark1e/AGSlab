#pragma once

#include <glew.h>

#include <algorithm>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <tuple>

class Extensions
{
public:
    static std::vector<std::string> read_file(std::string filename);
    static std::tuple<GLchar**,GLint*> vector_to_array(std::vector<std::string> vector);
    static std::tuple<std::vector<GLchar*>,std::vector<GLint>> strvector_to_ptrvector(std::vector<std::string> vector);
    static void remove_array(GLchar **array, int length);
};