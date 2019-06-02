#version 330 core

struct Material //Propriedades do material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAmbiente;
layout (location = 3) out vec3 gDifusa;
layout (location = 4) out vec4 gSpecShi;
//layout (location = 5) out vec4 gTexCoords;

//Variaveis de entrada
in vec2 fragUV;
in vec3 fragPos;
in vec3 fragNormal;

void main()
{
    // store the fragment position vector in the first gbuffer texture
    gPosition = fragPos;

    gNormal = fragNormal;

    gAmbiente = material.ambient;

    gDifusa = material.diffuse;

    gSpecShi = vec4(material.specular,material.shininess);

//    gTexCoords = vec3(fragUV,0);
}
