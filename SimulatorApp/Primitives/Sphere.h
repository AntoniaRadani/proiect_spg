#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class Sphere {
public:
    glm::vec3 center;   // centrul sferei
    float radius;       // raza sferei
    int stacks;         // nr diviziuni verticala
    int sectors;        // nr diviziuni orizontala

    std::vector<float> interleavedData;

    Sphere(glm::vec3 c = glm::vec3(0.0f), float r = 0.5f, int st = 30, int sec = 30);
    void build();

private:
    glm::vec3 calculatePoint(float phi, float theta);
    void addVertex(const glm::vec3& p);
};