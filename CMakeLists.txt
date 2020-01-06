cmake_minimum_required(VERSION 3.10)

# set the project name
project(RayTracer)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_executable(${PROJECT_NAME}
  ./src/main.cpp
  ./src/core/parallel.cpp
  ./src/bvh_node.cpp
  ./src/box.cpp
  ./src/hitable_list.cpp
  ./src/hitable.cpp
  ./src/medium.cpp
  ./src/perlin.cpp
  ./src/rect.cpp
  ./src/scene.cpp
  ./src/sphere.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC src)
