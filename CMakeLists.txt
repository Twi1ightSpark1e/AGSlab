cmake_minimum_required(VERSION 3.8)
project(cgraphics)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES 
    "src/main.cpp"
    "src/Shader.cpp"
    "src/Extensions.cpp"
    "src/Camera.cpp"
    "src/CameraController.cpp")

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