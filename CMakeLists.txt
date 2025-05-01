cmake_minimum_required(VERSION 3.8)
project(render-toy)

file(COPY data DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
file(COPY shaders DESTINATION "${CMAKE_CURRENT_BINARY_DIR}") # Copy shaders and data to the build folder for debugging

file (GLOB render-toy-src
  "src/*.cc"
  "src/platform/*.cc"

  # Imgui
  "include/imgui/*.cpp"
  "include/imgui/backends/imgui_impl_glfw.cpp"
  "include/imgui/backends/imgui_impl_opengl3.cpp"
) # Src compilation

add_executable(render-toy ${render-toy-src})
target_sources(render-toy PRIVATE "include/tinygltf/tiny_gltf.cc")

find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(nlohmann_json REQUIRED)

target_include_directories(render-toy PRIVATE "include/tinygltf/")
target_include_directories(render-toy PRIVATE "include/tinyobjloader/")
target_include_directories(render-toy PRIVATE "include/stb/")
target_include_directories(render-toy PRIVATE "include/imgui/")
target_include_directories(render-toy PRIVATE "include/imgui/backends")
target_include_directories(render-toy PUBLIC "src/")
target_include_directories(render-toy PUBLIC "src/platform")

add_subdirectory(include/tinygltf/)
add_subdirectory(include/tinyobjloader/)

target_link_libraries(render-toy glm::glm vulkan glfw nlohmann_json::nlohmann_json)

