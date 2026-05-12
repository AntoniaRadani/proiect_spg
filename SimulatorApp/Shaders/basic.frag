#version 330 core
out vec4 FragColor; // (R, G, B, A) A = transparenta

in vec3 FragPos;    // pozitia exacta a pixelului
in vec3 Normal;     // directia perpendiculara pe suprafata (normala)
in vec2 TexCoords;  // textura

uniform sampler2D texture_diffuse;
//uniform vec3 objectColor;
uniform vec3 lightPos = vec3(10.0, 5.0, 10.0);      // pozitia luminii in scena
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);      // culoarea luminii (alb)
uniform vec3 uColor;                                // culoarea din gui

void main() {
    vec3 objectColor = texture(texture_diffuse, TexCoords).rgb; // luam culoarea din textura
    vec4 texColor = texture(texture_diffuse, TexCoords);

    // lumina ambientala -- lumineaza tot uniform
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);                  // transforma normalul la lungime 1
    vec3 lightDir = normalize(lightPos - FragPos);  // vect dintre pixel si sursa de lumina => directia luminii
    float diff = max(dot(norm, lightDir), 0.0);     // calcul unghi dintre vectori
    vec3 diffuse = diff * lightColor;               // cat de mult bate lumina "direct"


    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
    //FragColor = vec4(1.0, 0.5, 0.2, 1.0);
    //FragColor = vec4(TexCoords.x, TexCoords.y, 0.0, 1.0);
    //vec3 lighting = ambient + diffuse;
    //vec3 finalRGB = texColor.rgb * uColor * lighting;

    //FragColor = vec4(finalRGB, texColor.a);
}