cmake_minimum_required(VERSION 3.8)
project(cgraphics)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(CMAKE_CXX_STANDARD 17)

set(SOURCES 
    "src/main.cpp"
    "src/Shader.cpp"
    "src/Extensions.cpp")

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(DevIL REQUIRED)
find_package(GLM REQUIRED)

include_directories(
    ${OPENGL_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${DEVIL_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    "include"
)

add_executable(cgraphics ${SOURCES})

target_link_libraries(cgraphics 
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${DEVIL_LIBRARIES}
)