#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class Sphere {
public:
    glm::vec3 center;
    float radius;
    int stacks;
    int sectors;

    std::vector<float> interleavedData;

    Sphere(glm::vec3 c = glm::vec3(0.0f), float r = 0.5f, int st = 30, int sec = 30)
        : center(c), radius(r), stacks(st), sectors(sec) {
        build();
    }

    void build() {
        interleavedData.clear();

        for (int i = 0; i < stacks; ++i) {
            float phi1 = glm::pi<float>() * (float)i / stacks;
            float phi2 = glm::pi<float>() * (float)(i + 1) / stacks;

            for (int j = 0; j < sectors; ++j) {
                float theta1 = 2.0f * glm::pi<float>() * (float)j / sectors;
                float theta2 = 2.0f * glm::pi<float>() * (float)(j + 1) / sectors;

                glm::vec3 p1 = calculatePoint(phi1, theta1);
                glm::vec3 p2 = calculatePoint(phi2, theta1);
                glm::vec3 p3 = calculatePoint(phi2, theta2);
                glm::vec3 p4 = calculatePoint(phi1, theta2);

                addVertex(p1);
                addVertex(p2);
                addVertex(p3);

                addVertex(p1);
                addVertex(p3);
                addVertex(p4);
            }
        }
    }

private:
    glm::vec3 calculatePoint(float phi, float theta) {
        return center + glm::vec3(
            radius * sin(phi) * cos(theta),
            radius * cos(phi),
            radius * sin(phi) * sin(theta)
        );
    }

    void addVertex(const glm::vec3& p) {
        interleavedData.push_back(p.x);
        interleavedData.push_back(p.y);
        interleavedData.push_back(p.z);

        glm::vec3 normal = glm::normalize(p - center);
        interleavedData.push_back(normal.x);
        interleavedData.push_back(normal.y);
        interleavedData.push_back(normal.z);
    }
};