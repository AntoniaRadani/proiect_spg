#include "Sphere.h"

using namespace glm;
using namespace std;

// constructor
Sphere::Sphere(vec3 c, float r, int st, int sec) {
    center = c;
    radius = r;
    stacks = st;
    sectors = sec;
    build();
}

// functie care construieste efectiv punctele
void Sphere::build() {
    // golire date vechi
    interleavedData.clear();

    for (int i = 0; i < stacks; i++) {
        float phi1 = pi<float>() * (float)i / stacks;
        float phi2 = pi<float>() * (float)(i + 1) / stacks;

        for (int j = 0; j < sectors; j++) {
            float theta1 = 2.0f * pi<float>() * (float)j / sectors;
            float theta2 = 2.0f * pi<float>() * (float)(j + 1) / sectors;

            // calculam cele 4 puncte ale unui patch
            // p1 ---- p4
            // |       |
            // |       |
            // p2 ---- p3
            vec3 p1 = calculatePoint(phi1, theta1);
            vec3 p2 = calculatePoint(phi2, theta1);
            vec3 p3 = calculatePoint(phi2, theta2);
            vec3 p4 = calculatePoint(phi1, theta2);

            // patrat -> triunghi
            addVertex(p1);
            addVertex(p2);
            addVertex(p3);

            addVertex(p1);
            addVertex(p3);
            addVertex(p4);
        }
    }
}

// coordonate sferice -> coordonate carteziene
vec3 Sphere::calculatePoint(float phi, float theta) {
    // aducem sfera in origine
    return center + vec3(
        radius * sin(phi) * cos(theta),         // pozitie axa x
        radius * cos(phi),                      // pozitie axa y
        radius * sin(phi) * sin(theta));      // pozitie axa z
}

// adaugam punctele in buffer pentru desenare
void Sphere::addVertex(const vec3& p) {
    interleavedData.push_back(p.x);
    interleavedData.push_back(p.y);
    interleavedData.push_back(p.z);

    // calcul normala
    vec3 normal = normalize(p - center);
    interleavedData.push_back(normal.x);
    interleavedData.push_back(normal.y);
    interleavedData.push_back(normal.z);
}

