#version 410 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gTangente;
layout (location = 3) out vec3 gTex;
//out vec3 finalColor;
//Variaveis de entrada
in vec2 fragUV;
in vec3 fragPos;
in vec3 fragNormal;
in vec3 fragTang;

uniform sampler2D normalSampler;

void main()
{
    // store the fragment position vector in the first gbuffer texture
   gPosition = fragPos;

   gNormal = normalize(fragNormal);

   gTangente = normalize(fragTang);
   //gTangente = length(gTangente) == 0 ? vec3(1, 0, 0) : vec3(0, 1, 0);

   gTex = normalize(texture(normalSampler,fragUV).rgb);

   //finalColor = length(gTangente) == 0 ? vec3(1, 0, 0) : vec3(0, 1, 0);

}
