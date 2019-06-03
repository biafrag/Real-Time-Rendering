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
layout (location = 1) out vec3 gAmbiente;
layout (location = 2) out vec3 gDifusa;
//layout (location = 3) out vec3 gNormal;
//out vec3 finalColor;
//Variaveis de entrada
in vec2 fragUV;
in vec3 fragPos;
in vec3 fragNormal;

void main()
{
    // store the fragment position vector in the first gbuffer texture
   gPosition = normalize(fragPos);

   gAmbiente = normalize(material.ambient);

   gDifusa = normalize(material.diffuse);

  // gNormal = normalize(fragNormal);

}
