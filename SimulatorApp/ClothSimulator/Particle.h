#pragma once
#include <glm/glm.hpp>

class Particle {
public:
    glm::vec3 position;
    glm::vec3 prevPosition;
    glm::vec3 acceleration;
    glm::vec3 normal;
    bool isFixed = false;

    Particle(const glm::vec3& pos, bool fixed = false)
        : position(pos), prevPosition(pos), acceleration(0.0f), normal(0.0f, 1.0f, 0.0f), isFixed(fixed) {}

    void applyForce(const glm::vec3& force) {
        acceleration += force;
    }

    void integrate(float dt) {
        if (isFixed) return;

        glm::vec3 temp = position;
        float damping = 0.99f;
        position = position + (position - prevPosition) * damping + acceleration * dt * dt;
        prevPosition = temp;

        acceleration = glm::vec3(0.0f);
    }

    glm::vec3 velocity() const {
        return position - prevPosition;
    }
};