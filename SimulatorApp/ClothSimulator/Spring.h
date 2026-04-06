#pragma once
#include "Particle.h"
#include <glm/glm.hpp>

enum class SpringType { STRUCTURAL, SHEAR, BEND };

class Spring {
public:
    Particle* p1;
    Particle* p2;
    float restLength;
    float stiffness;
    SpringType type;

    Spring(Particle* a, Particle* b, SpringType t = SpringType::STRUCTURAL, float k = -1.0f)
        : p1(a), p2(b), type(t)
    {
        restLength = glm::length(p1->position - p2->position);

        if (k > 0.0f) {
            stiffness = k;
        } else {
            switch (type) {
                case SpringType::STRUCTURAL: stiffness = 0.8f; break;
                case SpringType::SHEAR:      stiffness = 0.5f; break;
                case SpringType::BEND:       stiffness = 0.2f; break;
            }
        }
    }

    void satisfyConstraint() {
        glm::vec3 delta = p2->position - p1->position;
        float currentDist = glm::length(delta);
        if (currentDist < 0.0001f) return;

        float error = (currentDist - restLength) / currentDist;
        glm::vec3 correction = delta * 0.5f * stiffness * error;

        if (!p1->isFixed) p1->position += correction;
        if (!p2->isFixed) p2->position -= correction;
    }
};