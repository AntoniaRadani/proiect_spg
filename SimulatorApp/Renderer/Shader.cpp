#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

// constructor
// shader de tip vertex, shades de tip fragment
Shader::Shader(const string& vertexPath, const string& fragmentPath) {
    string vertexCode, fragmentCode; // cod GLSL
    ifstream vShaderFile, fShaderFile;  // pentru citirea fisierelor
    // activare exceptii in caz de eroare la citire
    vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
    fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
    try {
        // deschidem fisierele
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        // citim tot continutul in stream
        stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        // convertim in string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (ifstream::failure& e) {
        cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << endl;
    }
    // convertim la const char
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    // cream si compilam un shader de tip vertex
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    // cream si compilam un shader de tip fragment
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    // cream un container pentru shader
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

// destructor - eliberam programul din gpu
Shader::~Shader() {
    glDeleteProgram(ID);
}

// activare shader
void Shader::use() const {
    glUseProgram(ID);
}

// setarea uniformelor

// matrice 4 x 4 (model, view, projection)
void Shader::setMat4(const string &name, const mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// vector 3D (culoare, pozitie, lumina)
void Shader::setVec3(const string &name, const vec3 &value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

// float
void Shader::setFloat(const string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

// int
void Shader::setInt(const string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}