#version 410 core
layout (location = 0) out vec3 gTangente; //Buffer de tangentes
layout (location = 1) out vec3 gNormal; //Buffer de normais
layout (location = 2) out vec3 gPosition; // Buffer de posição
layout (location = 3) out vec3 gTex; //Buffer de textura mapeada pra esfera do bump

//Variaveis de entrada
in vec2 fragUV; //Coordenadas de textura
in vec3 fragPos; // Posição no espaço do olho
in vec3 fragNormal; //Normal no espaço do olho
in vec3 fragTang; //Tangente no espaço do olho

uniform sampler2D normalSampler; //Textura de bump

vec3 expand(vec3 v)
{
   return (v - 0.5) * 2;
}

void main()
{
   gPosition = fragPos; //Passando informação de posição para buffer

   gNormal = normalize(fragNormal); //Passando informação de normal para buffer

   gTangente = normalize(fragTang); //Passando informação de tangente para buffer

   gTex = normalize(expand(texture(normalSampler,fragUV).rgb)); //Passando textura de bump já expandida e mapeada para buffer

}
