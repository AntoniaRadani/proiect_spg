#pragma once
#include <glm/glm.hpp>

using namespace glm;

class Particle {
public:
    vec3 position;              // pozitia curenta a punctului
    vec3 prevPosition;          // pozitia din cadrul anterior
    vec3 acceleration;          // pentru a stoca toate fortele care actioneaza asupra unui punct
    vec3 normal;                // directia perpendiculara
    bool isFixed;               // pentru a "agata" panza

    Particle(const glm::vec3& pos, bool fixed = false);
    void applyForce(const glm::vec3& force);
    void integrate(float dt);
    glm::vec3 velocity() const;
};