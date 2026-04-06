#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

#include "Renderer/Shader.h"
#include "ClothSimulator/Cloth.h"
#include "Primitives/Sphere.h"

const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

glm::vec3 cameraPos   = glm::vec3(0.0f, 1.0f, 4.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, -0.2f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

void setupBuffers(GLuint &VAO, GLuint &VBO) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Cloth Simulation SPG - Sfera", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return -1;

    glEnable(GL_DEPTH_TEST);

    Shader ourShader("Shaders/basic.vert", "Shaders/basic.frag");

    Sphere sphere(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 40, 40);

    Cloth cloth(30, 30, 0.1f);

    GLuint clothVAO, clothVBO;
    setupBuffers(clothVAO, clothVBO);

    GLuint sphereVAO, sphereVBO;
    setupBuffers(sphereVAO, sphereVBO);

    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphere.interleavedData.size() * sizeof(float), sphere.interleavedData.data(), GL_STATIC_DRAW);

    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        cloth.simulate(0.012f, sphere.center, sphere.radius, 10);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)display_w / (float)display_h, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        ourShader.setVec3("objectColor", glm::vec3(1.0f, 0.71f, 0.75f));
        glm::mat4 modelSphere = glm::mat4(1.0f);
        ourShader.setMat4("model", modelSphere);
        glBindVertexArray(sphereVAO);
        glDrawArrays(GL_TRIANGLES, 0, sphere.interleavedData.size() / 6);

        ourShader.setVec3("objectColor", glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 modelCloth = glm::mat4(1.0f);
        ourShader.setMat4("model", modelCloth);

        std::vector<float> clothData;
        cloth.getFullMeshData(clothData);
        glBindVertexArray(clothVAO);
        glBindBuffer(GL_ARRAY_BUFFER, clothVBO);
        glBufferData(GL_ARRAY_BUFFER, clothData.size() * sizeof(float), clothData.data(), GL_DYNAMIC_DRAW);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLES, 0, clothData.size() / 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &clothVAO);
    glDeleteBuffers(1, &clothVBO);
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereVBO);

    glfwTerminate();
    return 0;
}