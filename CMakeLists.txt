cmake_minimum_required(VERSION 3.20)
project(clion_spg)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(GLEW REQUIRED)

add_executable(clion_spg
        SimulatorApp/main.cpp
        SimulatorApp/ClothSimulator/Particle.cpp
        SimulatorApp/ClothSimulator/Particle.h
        SimulatorApp/ClothSimulator/Cloth.cpp
        SimulatorApp/ClothSimulator/Cloth.h
        SimulatorApp/ClothSimulator/Spring.cpp
        SimulatorApp/ClothSimulator/Spring.h
        SimulatorApp/Renderer/Shader.cpp
        SimulatorApp/Renderer/Shader.h
)

target_link_libraries(clion_spg
        OpenGL::GL
        glfw
        glm::glm
)

target_link_libraries(clion_spg
        OpenGL::GL
        glfw
        glm::glm
        GLEW::GLEW
)

target_compile_definitions(clion_spg PRIVATE GL_SILENCE_DEPRECATION)

target_compile_options(clion_spg PRIVATE -Wall -Wextra -Wpedantic)