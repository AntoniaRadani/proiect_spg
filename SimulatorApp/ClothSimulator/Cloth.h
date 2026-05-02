#pragma once
#include "Particle.h"
#include "Spring.h"
#include <vector>
#include <glm/glm.hpp>

class Cloth {
public:
    int width, height;                  // nr de particule pe orizontala / verticala
    float spacing;                      // distanta dintre 2 particule vecine
    std::vector<Particle> particles;
    std::vector<Spring> springs;

    Cloth(int w, int h, float s);
    void simulate(float dt, const glm::vec3& sphereCenter, float sphereRadius, int iterations);
    void computeNormals();
    void getFullMeshData(std::vector<float>& data);
};