#version 330 core
layout(location = 0) in vec3 aPos;          // pozitia fiecarui vertex
layout(location = 1) in vec3 aNormal;       // vectorul normal
layout(location = 2) in vec2 aTexCoords;    // textura

uniform mat4 model;         // pozitia / rotatia / scalarea
uniform mat4 view;          // camera
uniform mat4 projection;    // perspectiva

out vec3 FragPos;       // pozitia pixelului in lume
out vec3 Normal;        // normalul corectat
out vec2 TexCoords;     // iesire pentru textura
void main()
{
    // pozitia finala pe ecran a obiecutului
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}