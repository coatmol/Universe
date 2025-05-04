cmake_minimum_required(VERSION 3.13)

project(Universe)
set(CMAKE_CXX_STANDARD 17)
add_executable(Universe "vendor/glad.c" "src/main.cpp" "src/renderer/Shader.cpp" "src/renderer/Shader.h" "src/utils/File.h" "src/utils/File.cpp" "src/renderer/gl/VBO.h" "src/renderer/gl/VBO.cpp" "src/renderer/gl/EBO.h" "src/renderer/gl/EBO.cpp" "src/renderer/gl/VAO.h" "src/renderer/gl/VAO.cpp" "src/renderer/Camera.h" "src/renderer/Camera.cpp")

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

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(Universe copy_assets)