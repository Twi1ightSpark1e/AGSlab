cmake_minimum_required(VERSION 3.8)
project(cgraphics)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic")

if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    set(CMAKE_CXX_FLAGS "-DDEBUG ${CMAKE_CXX_FLAGS}")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES 
    "src/main.cpp"
    "src/Shader.cpp"
    "src/Extensions.cpp"
    "src/Camera.cpp"
    "src/InputManager.cpp"
    "src/Mesh.cpp"
    "src/ResourceManager.cpp"
    "src/GraphicObject.cpp"
    "src/Scene.cpp"
    "src/RenderManager.cpp"
    "src/Texture.cpp")

find_package(OpenGL REQUIRED)
find_package(DevIL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(PugiXML REQUIRED)

include_directories(
    ${OPENGL_INCLUDE_DIRS}
    ${IL_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${PUGIXML_INCLUDE_DIRS}
    "include"
)

add_executable(cgraphics ${SOURCES})

target_link_libraries(cgraphics 
    ${OPENGL_LIBRARIES}
    ${IL_LIBRARIES}
    ${ILU_LIBRARIES}
    ${GLUT_LIBRARIES}
    ${GLEW_LIBRARIES}
    "pugixml"
    "stdc++fs"
)

find_program(clangtidy_path NAMES clang-tidy)
if(clangtidy_path)
    message(STATUS "Found clang-tidy at ${clangtidy_path}")
    add_custom_target(tidy
        COMMAND "${clangtidy_path}" -quiet -p "${PROJECT_BINARY_DIR}" ${SOURCES}
        COMMENT "Running clang-tidy tool"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        SOURCES ${PROJECT_SOURCE_DIR}/.clang-tidy
        )
endif()
