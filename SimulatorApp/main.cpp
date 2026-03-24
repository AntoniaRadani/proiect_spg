#define GL_SILENCE_DEPRECATION      // pentru warning de la macos
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Renderer/Shader.h"

int main() {
    // initializare GLFW
    if (!glfwInit()) return -1;

    // setari / profil
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

// pt macos
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // creare fereastra
    GLFWwindow* window = glfwCreateWindow(800, 600, "Cloth Simulator - Step 0", NULL, NULL);
    if (!window) return -1;

    glfwMakeContextCurrent(window);

    // initializare glew
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW init failed\n";
        return -1;
    }

    // incarca shader
    Shader shader("../SimulatorApp/Shaders/basic.vert",
                  "../SimulatorApp/Shaders/basic.frag");

    // vertex
    float vertices[] = {0.0f, 0.0f, 0.0f}; // punct in centru

    // creare vao + vbo
    // vao - cum se citesc vertexii
    // vbo - stocheaza efectiv coordonatele in gpu
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  // trimit datele catre GPU

    // cum interpretez coordonatele
    // 0 - location 0 (corelat cu vertex shader)
    // 3 - doua componente (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // view + projection -- cum vedem si proiectam obiectele in ecran
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
                                 glm::vec3(0.0f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                            800.0f / 600.0f, 0.1f, 100.0f);

    shader.use();
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    // loop principal
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);       // culoare fundal
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);         // curatare ecran la fiecare frame

        shader.use();                                                   // activare shader pt desenare
        glBindVertexArray(VAO);                                         // conecteaza datele vertex
        glPointSize(10.0f);                                             // dimensiune punct
        glDrawArrays(GL_POINTS, 0, 1);                  // desenam un singur punct

        glfwSwapBuffers(window);                                        // afisare frame
        glfwPollEvents();                                               // procesare input (mouse, tastatura)
    }

    // curatare + inchidere program
    glfwTerminate();
    return 0;
}