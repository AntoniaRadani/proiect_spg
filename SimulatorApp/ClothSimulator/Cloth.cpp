#include "Cloth.h"

using namespace glm;
using namespace std;

// constructor
Cloth::Cloth(int w, int h, float s) {
    width = w;
    height = h;
    spacing = s;

    // calcul decalaj => centrul panzei sa fie in punctul (0, 0)
    float halfWidth = (width - 1) * spacing / 2.0f;
    float halfDepth = (height - 1) * spacing / 2.0f;

    // generare particule
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // grila orizontala la inaltimea de 1.5 m
            vec3 pos(x * spacing - halfWidth, 1.5f, y * spacing - halfDepth);
            bool fixed = false;
            particles.emplace_back(pos, fixed);
        }
    }

    // generare arcuri
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // arcuri structurale (intre vecini imediati)
            if (x < width - 1)
                springs.emplace_back(&particles[y * width + x], &particles[y * width + (x + 1)], SpringType::STRUCTURAL);
            if (y < height - 1)
                springs.emplace_back(&particles[y * width + x], &particles[(y + 1) * width + x], SpringType::STRUCTURAL);

            // arcuri shear (pe diagonala)
            if (x < width - 1 && y < height - 1) {
                springs.emplace_back(&particles[y * width + x], &particles[(y + 1) * width + (x + 1)], SpringType::SHEAR);
                springs.emplace_back(&particles[y * width + (x + 1)], &particles[(y + 1) * width + x], SpringType::SHEAR);
            }

            // arcuri bend (sarim peste o particula)
            if (x < width - 2)
                springs.emplace_back(&particles[y * width + x], &particles[y * width + (x + 2)], SpringType::BEND);
            if (y < height - 2)
                springs.emplace_back(&particles[y * width + x], &particles[(y + 2) * width + x], SpringType::BEND);
        }
    }
}

// iterations -> cu cat mai multe iteratii, cu atat erorile devin mai mici
void Cloth::simulate(float dt, const vec3& sphereCenter, float sphereRadius, int iterations) {
    // forta de gravitatie
    vec3 gravity(0.0f, -9.81f, 0.0f);

    // aplicam fortele + calcul pozitie noua pt fiecare punct
    for (auto& p : particles) {
        p.applyForce(gravity);
        p.integrate(dt);
    }

    // rezolvarea constrangerilor
    float offset = 0.02f;
    for (int i = 0; i < iterations; i++) {
        // constrangeri de distanta
        // arcurile trag de particule pentru a pastra forma
        for (auto& s : springs)
            s.satisfyConstraint();

        // constrangeri de coliziune
        // verificam daca panza a intrat sau nu in sfera
        for (auto& p : particles) {
            vec3 v = p.position - sphereCenter; // vectorul de la centrul sferei la particula
            float dist = length(v);             // distanta reala

            // distanta < raza => particula e in sfera
            if (dist < sphereRadius + offset) {
                // mutam particula exact pe suprafata sferei
                p.position = sphereCenter + normalize(v) * (sphereRadius + offset);
                // simulam forta de fracare si oprim accelerarea infinita spre interior
                p.prevPosition = p.position + (p.prevPosition - p.position) * 0.5f;
            }
        }
    }
    // recalculam normalele ca lumina sa bata corect
    computeNormals();
}

void Cloth::computeNormals() {
    // reset la normale
    for (auto& p : particles)
        p.normal = vec3(0.0f);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // calcul plan formal din 3 particule vecine
            int i = y * width + x;
            vec3 v1 = particles[i + 1].position - particles[i].position;
            vec3 v2 = particles[i + width].position - particles[i].position;
            // produs vectorial
            vec3 n = cross(v1, v2);

            // distribuim "n" = normala calculata fiecarei particule
            particles[i].normal += n;
            particles[i + 1].normal += n;
            particles[i + width].normal += n;
        }
    }

    // aducem vectorul la lungimea unitara, pastrand doar distanta
    for (auto& p : particles)
        p.normal = normalize(p.normal);
}

void Cloth::getFullMeshData(vector<float>& data) {
    data.clear();
    // parcurgem fiecare "patrat" format de 4 particule vecine
    // (i) ------ (i+1)
    // |            |
    // |            |
    // (i+w)------(i+w+1)
    for (int y = 0; y < height - 1; y++) {
        for (int x = 0; x < width - 1; x++) {
            int indices[4] = {
                y * width + x,          // sus stanga
                (y + 1) * width + x,    // jos stanga
                y * width + x + 1,      // sus dreapta
                (y + 1) * width + x + 1 // jos dreapta
            };

            // impartim patratul in doua triunghiuri 0-1-2 2-1-3
            int order[6] = {0, 1, 2, 2, 1, 3};
            for (int idx : order) {
                Particle& p = particles[indices[idx]];
                // adaugam pozitia
                data.push_back(p.position.x);
                data.push_back(p.position.y);
                data.push_back(p.position.z);
                // adaugam normala
                data.push_back(p.normal.x);
                data.push_back(p.normal.y);
                data.push_back(p.normal.z);
            }
        }
    }
}