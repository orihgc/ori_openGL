cmake_minimum_required(VERSION 3.22)
project(ori_openGL)

set(CMAKE_CXX_STANDARD 11)

include_directories(/usr/local/include)

link_directories(/usr/local/Cellar/glfw/3.3.5/lib)

add_executable(ori_openGL 1_base/2_triangle.cpp glad.c)

target_link_libraries(ori_openGL GLFW)


