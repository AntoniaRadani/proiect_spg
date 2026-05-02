#include "Spring.h"

using namespace glm;

// constructor
Spring::Spring(Particle* a, Particle*b, SpringType t, float k) {
    p1 = a;
    p2 = b;
    type = t;

    // calcul distanta ideala dintre 2 particule
    restLength = length(p1->position - p2->position);

    if (k > 0.0f) {
        stiffness = k;  // daca k a fost trimis ca parametru
    } else {
        switch (type) {
            // valori default
            case SpringType::STRUCTURAL: stiffness = 0.8f; break;
            case SpringType::SHEAR: stiffness = 0.5f; break;
            case SpringType::BEND: stiffness = 0.2f; break;
        }
    }
}

// pastrarea formei panzei
void Spring::satisfyConstraint() {
    // delta = vectorul dintre cele 2 particule
    vec3 delta = p2->position - p1->position;
    float currentDist = length(delta);

    // daca particulele sunt in acelasi loc
    // evitam eroarea "division by zero"
    if (currentDist < 0.0001f)
        return;

    // eroarea = cat de mult s-a deformat arcul fata de poz init
    float error = (currentDist - restLength) / currentDist;
    // vectorul de corectie
    // delta - directia pe care trebuie sa mutam
    // 0.5f - impartim corectia la ambele particule (50%)
    // stiffness - cat de rigida e arcul
    // error - cat de mare este abaterea
    vec3 correction = delta * 0.5f * stiffness * error;

    // aplicam mutarea daca nu sunt fixed
    // p1 este impinsa spre p1
    // p2 este impinsa spre p1
    if (!p1->isFixed)
        p1->position += correction;
    if (!p2->isFixed)
        p2->position -= correction;
}