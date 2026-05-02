#pragma once
#include "Particle.h"
#include <glm/glm.hpp>

// structural - leaga particulele vecine
// shear - leaga particulele pe diagonala
// bend - leaga particulele peste una
enum class SpringType { STRUCTURAL, SHEAR, BEND };

class Spring {
public:
    // pointeri catre doua particule existente
    Particle* p1;
    Particle* p2;
    float restLength;   // distanta ideala dintre 2 particule
    float stiffness;    // rigiditatea arcului
    SpringType type;    // tipul arcului

    Spring(Particle* a, Particle* b, SpringType t = SpringType::STRUCTURAL, float k = -1.0f);
    void satisfyConstraint();
};