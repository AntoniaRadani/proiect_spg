#pragma once
#include <glm/glm.hpp>

class Particle {
public:
    glm::vec3 position;      // poziția curenta
    glm::vec3 prevPosition;  // poziția anterioara
    glm::vec3 acceleration;  // acumularea forțelor
    bool isFixed = false;    // daca punctul este fixat în spatiu

    // constructor
    Particle(const glm::vec3& pos, bool fixed = false)
        : position(pos), prevPosition(pos), acceleration(0.0f), isFixed(fixed) {}

    // aplica o forța asupra particulei
    void applyForce(const glm::vec3& force) {
        acceleration += force;  // m = 1 => F = a
    }

    // integrare Verlet
    void integrate(float dt) {
        if (isFixed)
            return; // nu se mișca dacă e fix

        glm::vec3 temp = position;
        position = position + (position - prevPosition) + acceleration * dt * dt;
        prevPosition = temp;

        // resetam acceleratia pentru urmatorul frame
        acceleration = glm::vec3(0.0f);
    }

    // returneaza viteza aproximativa
    glm::vec3 velocity() const {
        return position - prevPosition;
    }
};