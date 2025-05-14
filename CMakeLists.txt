cmake_minimum_required(VERSION 3.13)

project(Universe)
set(CMAKE_CXX_STANDARD 17)
add_executable(Universe "vendor/glad.c" "src/main.cpp" "src/renderer/Shader.cpp" "src/renderer/Shader.h" "src/utils/File.h" "src/utils/File.cpp" "src/renderer/gl/VBO.h" "src/renderer/gl/VBO.cpp" "src/renderer/gl/EBO.h" "src/renderer/gl/EBO.cpp" "src/renderer/gl/VAO.h" "src/renderer/gl/VAO.cpp" "src/renderer/Camera.h" "src/renderer/Camera.cpp" "src/utils/Math.h" "src/engine/Body.cpp" "src/engine/Body.h" "src/engine/Skybox.h" "src/engine/Skybox.cpp" "src/renderer/stb_image_impl.cpp" "src/engine/Grid.h" "src/engine/Grid.cpp" "src/renderer/LineRenderer.h" "src/renderer/LineRenderer.cpp")

include_directories("vendor/glad")
include_directories("vendor/KHR")
include_directories("vendor/stb")

add_subdirectory("vendor/glfw")
target_link_libraries(Universe PRIVATE glfw)

add_subdirectory("vendor/glm")
target_link_libraries(Universe PRIVATE glm)

include_directories("vendor/imgui" "vendor/imgui/backends")
target_sources(
    Universe
    PRIVATE
    "vendor/imgui/imgui.cpp"
    "vendor/imgui/imgui_demo.cpp"
    "vendor/imgui/imgui_draw.cpp"
    "vendor/imgui/imgui_tables.cpp"
    "vendor/imgui/imgui_widgets.cpp"
    "vendor/imgui/backends/imgui_impl_glfw.cpp"
    "vendor/imgui/backends/imgui_impl_opengl3.cpp"
)

find_package(OpenGL REQUIRED)
target_link_libraries(Universe PRIVATE OpenGL::GL)

target_include_directories(Universe PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vendor/")

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(Universe copy_assets)