#pragma once
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>

using namespace std;
using namespace glm;

class Shader {
public:
    unsigned int ID;

    Shader(const string& vertexPath, const string& fragmentPath);
    ~Shader();

    void use() const;

    void setMat4(const string &name, const mat4 &mat) const;
    void setVec3(const string &name, const vec3 &value) const;
    void setFloat(const string &name, float value) const;
    void setInt(const string &name, int value) const;
};