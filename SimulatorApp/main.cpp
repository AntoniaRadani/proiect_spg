 #define STB_IMAGE_IMPLEMENTATION
 #include "Headers/stb_image.h"
 #include <GL/glew.h>
 #include <GLFW/glfw3.h>
 #include <glm/glm.hpp>
 #include <glm/gtc/matrix_transform.hpp>
 #include <iostream>
 #include <vector>
 #include <algorithm>

 #include "Renderer/Shader.h"
 #include "ClothSimulator/Cloth.h"
 #include "Primitives/Sphere.h"

 const unsigned int SCR_WIDTH = 1000;
 const unsigned int SCR_HEIGHT = 800;

 bool isRunning = false;
 glm::vec3 spherePos(0.0f);
 float sphereRadius = 1.0f;
 float sphereScale = 1.0f;
 float clothScale = 1.0f;
 float generalScale = 1.0f;

 unsigned int loadTexture(const char* path) {
     unsigned int id;
     glGenTextures(1, &id);
     int w, h, c;
     stbi_set_flip_vertically_on_load(true);
     unsigned char* data = stbi_load(path, &w, &h, &c, 0);
     if (data) {
         GLenum f = (c == 4) ? GL_RGBA : GL_RGB;
         glBindTexture(GL_TEXTURE_2D, id);
         glTexImage2D(GL_TEXTURE_2D, 0, f, w, h, 0, f, GL_UNSIGNED_BYTE, data);
         glGenerateMipmap(GL_TEXTURE_2D);
         stbi_image_free(data);
     } else {
         unsigned char white[] = {255, 255, 255};
         glBindTexture(GL_TEXTURE_2D, id);
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, white);
     }
     return id;
 }

 void processInput(GLFWwindow* window, float deltaTime, Cloth& cloth) {
     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
         glfwSetWindowShouldClose(window, true);

     static bool spacePressed = false;
     if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
         if (!spacePressed) isRunning = !isRunning;
         spacePressed = true;
     } else spacePressed = false;

     if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
         cloth.reset();
         isRunning = false;
         spherePos = glm::vec3(0.0f);
         sphereScale = 1.0f;
         clothScale = 1.0f;
         generalScale = 1.0f;
     }

     float moveSpeed = 3.0f * deltaTime;
     if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) spherePos.z -= moveSpeed;
     if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) spherePos.z += moveSpeed;
     if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) spherePos.x -= moveSpeed;
     if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) spherePos.x += moveSpeed;

     float scaleSpeed = 1.5f * deltaTime;
     bool ctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS;

     if (ctrl) {
         if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) sphereScale += scaleSpeed;
         if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) sphereScale = std::max(0.1f, sphereScale - scaleSpeed);
     } else {
         if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) clothScale += scaleSpeed;
         if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) clothScale = std::max(0.1f, clothScale - scaleSpeed);
     }

     if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) generalScale = std::max(0.1f, generalScale - scaleSpeed);
     if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) generalScale += scaleSpeed;
 }

 int main() {
     if (!glfwInit()) return -1;
     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
 #ifdef __APPLE__
     glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
 #endif

     GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Cloth Simulator", NULL, NULL);
     if (!window) { glfwTerminate(); return -1; }
     glfwMakeContextCurrent(window);
     glewExperimental = GL_TRUE;
     glewInit();
     glEnable(GL_DEPTH_TEST);

     Shader ourShader("Shaders/basic.vert", "Shaders/basic.frag");
     unsigned int fabricTex = loadTexture("Textures/brickwall_normal.jpg");
     unsigned int sphereTex = loadTexture("Textures/planet.jpg");

     Cloth cloth(25, 25, 0.12f);
     Sphere sphere(glm::vec3(0.0f), 1.0f, 40, 40);

     GLuint cVAO, cVBO, sVAO, sVBO;
     glGenVertexArrays(1, &cVAO); glGenBuffers(1, &cVBO);
     glGenVertexArrays(1, &sVAO); glGenBuffers(1, &sVBO);

     glBindVertexArray(sVAO);
     glBindBuffer(GL_ARRAY_BUFFER, sVBO);
     glBufferData(GL_ARRAY_BUFFER, sphere.interleavedData.size() * sizeof(float), sphere.interleavedData.data(), GL_STATIC_DRAW);
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); glEnableVertexAttribArray(2);

     float lastFrame = 0.0f;
     //glClearColor(0.244f, 0.188f, 0.217f, 1.0f);
     glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

     while (!glfwWindowShouldClose(window)) {
         float currentFrame = (float)glfwGetTime();
         float deltaTime = currentFrame - lastFrame;
         lastFrame = currentFrame;

         processInput(window, deltaTime, cloth);

         if (isRunning) {
             // 1. Calculăm factorul de scalare combinat pentru sferă
             // Inversăm scalarea pânzei pentru a aduce sfera în "spațiul local" al pânzei
             float relativeScale = (1.0f / clothScale);

             // 2. Ajustăm raza de coliziune
             // Aceasta trebuie să țină cont de cât de mare e bila față de pânză
             float collisionRadius = (1.0f * sphereScale) * relativeScale;

             // 3. Ajustăm poziția de coliziune
             // Mutăm centrul sferei astfel încât să corespundă cu mărirea/micșorarea pânzei
             glm::vec3 collisionPos = spherePos * relativeScale;

             // 4. Trimitem datele ajustate către simulator
             // Folosim deltaTime pentru o mișcare fluidă
             cloth.simulate(deltaTime, collisionPos, collisionRadius, 5);
         }

         int width, height;
         glfwGetFramebufferSize(window, &width, &height);
         glViewport(0, 0, width, height);

         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

         ourShader.use();
         glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
         glm::mat4 view = glm::lookAt(glm::vec3(0, 5, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
         ourShader.setMat4("projection", projection);
         ourShader.setMat4("view", view);

         glm::mat4 generalModel = glm::scale(glm::mat4(1.0f), glm::vec3(generalScale));

         glBindTexture(GL_TEXTURE_2D, fabricTex);
         glm::mat4 modelCloth = glm::scale(generalModel, glm::vec3(clothScale));
         ourShader.setMat4("model", modelCloth);
         std::vector<float> cData;
         cloth.getFullMeshData(cData);
         glBindVertexArray(cVAO);
         glBindBuffer(GL_ARRAY_BUFFER, cVBO);
         glBufferData(GL_ARRAY_BUFFER, cData.size() * sizeof(float), cData.data(), GL_DYNAMIC_DRAW);
         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
         glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); glEnableVertexAttribArray(2);
         glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(cData.size() / 8));

         glBindTexture(GL_TEXTURE_2D, sphereTex);
         glm::mat4 modelSphere = glm::translate(generalModel, spherePos);
         modelSphere = glm::scale(modelSphere, glm::vec3(sphereScale));
         ourShader.setMat4("model", modelSphere);
         glBindVertexArray(sVAO);
         glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(sphere.interleavedData.size() / 8));

         glfwSwapBuffers(window);
         glfwPollEvents();
     }

     glDeleteVertexArrays(1, &cVAO); glDeleteBuffers(1, &cVBO);
     glDeleteVertexArrays(1, &sVAO); glDeleteBuffers(1, &sVBO);
     glfwTerminate();
     return 0;
 }

