cmake_minimum_required(VERSION 3.8)
project(cgraphics)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb -DDEBUG -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

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
    "src/Texture.cpp"
    "src/SkyBox.cpp"
    "src/NetProtocol.cpp"
    "src/FramebufferObject.cpp"
)

find_package(OpenGL REQUIRED)
find_package(DevIL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(PugiXML REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED)

include_directories(
    ${OPENGL_INCLUDE_DIRS}
    ${IL_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${PUGIXML_INCLUDE_DIRS}
    ${BOOST_INCLUDE_DIRS}
    "include"
)

add_executable(cgraphics ${SOURCES})

target_link_libraries(cgraphics
    ${OPENGL_LIBRARIES}
    ${IL_LIBRARIES}
    ${ILU_LIBRARIES}
    ${GLUT_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${BOOST_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    "pugixml"
)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_libraries(cgraphics
        "stdc++fs"
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(cgraphics
        "c++fs"
    )
endif()
