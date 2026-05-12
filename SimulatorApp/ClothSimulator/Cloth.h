#pragma once
#include "Particle.h"
#include "Spring.h"
#include <vector>
#include <glm/glm.hpp>

class Cloth {
public:
    int width, height;                  // nr de particule pe orizontala / verticala
    float spacing;                      // distanta dintre 2 particule vecine
    vector<Particle> particles;
    vector<Spring> springs;

    Cloth(int w, int h, float s);
    void simulate(float dt, const vec3& sphereCenter, float sphereRadius, int iterations);
    void computeNormals();
    void getFullMeshData(vector<float>& data);
    void reset();
};