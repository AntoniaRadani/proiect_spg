cmake_minimum_required(VERSION 4.1)
project(clion_spg)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(clion_spg SimulatorApp/main.cpp
        SimulatorApp/ClothSimulator/Particle.cpp
        SimulatorApp/ClothSimulator/Particle.h
        SimulatorApp/ClothSimulator/Cloth.cpp
        SimulatorApp/ClothSimulator/Cloth.h
        SimulatorApp/ClothSimulator/Spring.cpp
        SimulatorApp/ClothSimulator/Spring.h)

target_link_libraries(clion_spg OpenGL::GL glfw)

