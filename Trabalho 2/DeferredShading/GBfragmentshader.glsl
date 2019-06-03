#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gTangente;
layout (location = 3) out vec3 gTex;
layout (location = 4) out vec3 gLight;
layout (location = 5) out vec3 gTanViewer;
out vec3 finalColor;
//Variaveis de entrada
in vec2 fragUV;
in vec3 fragPos;
in vec3 fragNormal;
in vec3 fragTang;

uniform sampler2D normalSampler;

void main()
{
    // store the fragment position vector in the first gbuffer texture
   gPosition = normalize(fragPos);

   gNormal = normalize(fragNormal);

   gTangente = normalize(fragTang);

   gTex = texture(normalSampler,fragUV).rgb;

   finalColor = gTangente;

}
