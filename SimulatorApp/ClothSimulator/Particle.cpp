#include "Particle.h"

// constructor
Particle::Particle(const vec3& pos, bool fixed) {
    // v = position - prevPosition => viteza_init = 0
    position = pos;
    prevPosition = pos;
    acceleration = vec3(0.0f);       // nu actioneaza nicio forta la inceput
    normal = vec3(0.0f, 1.0f, 0.0f); // (stanga/dreapta, sus/jos, inainte/inapoi) => initial normala e in sus
    isFixed = fixed;                 // presupunem initial ca nu e fixed
}

// functie pentru a aplica forta asupra unei particule
void Particle::applyForce(const vec3& force) {
    acceleration += force;
}

void Particle::integrate(float dt) {
    // daca punctul este fix iesim din functie
    // nu se schimba pozitia lui
    if (isFixed)
        return;

    vec3 temp = position;
    float damping = 0.99f; // rezistenta la miscare

    // formula de integrare Verlet
    // viteza = poz - prevPoz
    // * damping -> reducem energia ca sa prevenim miscarea panzei la infinit
    // distanta parcursa sub acceleratie : d = a * t ^ 2 / 2 (in Verlet /2 e absorbit de restul alg)
    // => se aplica un fragment din acceleratia gravitationala
    position = position + (position - prevPosition) * damping + acceleration * dt * dt;

    prevPosition = temp;

    acceleration = vec3(0.0f);
}

// calcul viteza fara sa "strice" particula
vec3 Particle::velocity() const {
    return position - prevPosition;
}