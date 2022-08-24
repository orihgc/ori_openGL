cmake_minimum_required(VERSION 3.22)
project(ori_openGL)

set(CMAKE_CXX_STANDARD 11)

include_directories(/usr/local/include)
include_directories(depend)

link_directories(/usr/local/Cellar/glfw/3.3.5/lib)

add_executable(
        ori_openGL
        depend/glad.c
        depend/shader_s.h
        depend/stb_image.h
        depend/stb_helper.cpp
        1_base/5_transformations/1_transformation.cpp
)


target_link_libraries(ori_openGL GLFW)


