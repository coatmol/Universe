cmake_minimum_required(VERSION 3.13)

project(Universe)
set(CMAKE_CXX_STANDARD 17)
add_executable(Universe "vendor/glad.c" "src/main.cpp")

include_directories("vendor/glad")
include_directories("vendor/KHR")
include_directories("vendor/stb")

add_subdirectory("vendor/glfw")
target_link_libraries(Universe PRIVATE glfw)

add_subdirectory("vendor/glm")
target_link_libraries(Universe PRIVATE glm)

find_package(OpenGL REQUIRED)
target_link_libraries(Universe PRIVATE OpenGL::GL)

target_include_directories(Universe PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vendor/")