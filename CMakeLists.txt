cmake_minimum_required(VERSION 4.1)
project(clion_spg)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(clion_spg main.cpp
        ClothSimulator/Particle.cpp
        ClothSimulator/Particle.h
        ClothSimulator/Cloth.cpp
        ClothSimulator/Cloth.h
        ClothSimulator/Spring.cpp
        ClothSimulator/Spring.h)

target_link_libraries(clion_spg OpenGL::GL glfw)

