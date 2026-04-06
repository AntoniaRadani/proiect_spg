#pragma once
#include "Particle.h"
#include "Spring.h"
#include <vector>
#include <glm/glm.hpp>

class Cloth {
public:
    int width, height;
    float spacing;
    std::vector<Particle> particles;
    std::vector<Spring> springs;

    Cloth(int w, int h, float s) : width(w), height(h), spacing(s) {
        float halfWidth = (width - 1) * spacing / 2.0f;
        float halfDepth = (height - 1) * spacing / 2.0f;

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                glm::vec3 pos(
                    x * spacing - halfWidth,
                    1.5f,
                    y * spacing - halfDepth
                );

                bool fixed = false;
                particles.emplace_back(pos, fixed);
            }
        }

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (x < width - 1)
                    springs.emplace_back(&particles[y * width + x], &particles[y * width + (x + 1)], SpringType::STRUCTURAL);
                if (y < height - 1)
                    springs.emplace_back(&particles[y * width + x], &particles[(y + 1) * width + x], SpringType::STRUCTURAL);

                if (x < width - 1 && y < height - 1) {
                    springs.emplace_back(&particles[y * width + x], &particles[(y + 1) * width + (x + 1)], SpringType::SHEAR);
                    springs.emplace_back(&particles[y * width + (x + 1)], &particles[(y + 1) * width + x], SpringType::SHEAR);
                }

                if (x < width - 2)
                    springs.emplace_back(&particles[y * width + x], &particles[y * width + (x + 2)], SpringType::BEND);
                if (y < height - 2)
                    springs.emplace_back(&particles[y * width + x], &particles[(y + 2) * width + x], SpringType::BEND);
            }
        }
    }

    void simulate(float dt, const glm::vec3& sphereCenter, float sphereRadius, int iterations) {
        glm::vec3 gravity(0.0f, -9.81f, 0.0f);

        for (auto& p : particles) {
            p.applyForce(gravity);
            p.integrate(dt);
        }

        float offset = 0.02f;
        for (int i = 0; i < iterations; i++) {
            for (auto& s : springs) s.satisfyConstraint();

            for (auto& p : particles) {
                glm::vec3 v = p.position - sphereCenter;
                float dist = glm::length(v);

                if (dist < sphereRadius + offset) {
                    p.position = sphereCenter + glm::normalize(v) * (sphereRadius + offset);

                    p.prevPosition = p.position + (p.prevPosition - p.position) * 0.5f;
                }
            }
        }
        computeNormals();
    }

    void computeNormals() {
        for (auto& p : particles) p.normal = glm::vec3(0.0f);

        for (int y = 0; y < height - 1; y++) {
            for (int x = 0; x < width - 1; x++) {
                int i = y * width + x;
                glm::vec3 v1 = particles[i + 1].position - particles[i].position;
                glm::vec3 v2 = particles[i + width].position - particles[i].position;
                glm::vec3 n = glm::cross(v1, v2);

                particles[i].normal += n;
                particles[i + 1].normal += n;
                particles[i + width].normal += n;
            }
        }

        for (auto& p : particles) p.normal = glm::normalize(p.normal);
    }

    void getFullMeshData(std::vector<float>& data) {
        data.clear();
        for (int y = 0; y < height - 1; y++) {
            for (int x = 0; x < width - 1; x++) {
                int indices[4] = {
                    y * width + x,
                    (y + 1) * width + x,
                    y * width + x + 1,
                    (y + 1) * width + x + 1
                };
                int order[6] = { 0, 1, 2, 2, 1, 3 };
                for (int idx : order) {
                    Particle& p = particles[indices[idx]];
                    data.push_back(p.position.x); data.push_back(p.position.y); data.push_back(p.position.z);
                    data.push_back(p.normal.x);   data.push_back(p.normal.y);   data.push_back(p.normal.z);
                }
            }
        }
    }
};