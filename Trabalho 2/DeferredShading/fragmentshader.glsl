#version 410 core

////Variaveis de entrada

in vec2 UV;
out vec3 finalColor; // Cor final do objeto

uniform sampler2D gDepth;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gTangente;

float linearizeDepth(vec2 uv)
{
    float zNear = 0.1;
    float zFar  = 1000.0;
    float depth = texture2D(gDepth, uv).x;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

void main()
{
//    float d;
//    d = linearizeDepth(UV);
//    finalColor = vec3(d,d,d);
    vec3 fragPos = texture(gPosition,UV).rgb;
    vec3 fragNormal = texture(gNormal,UV).rgb;
    vec3 fragTang = texture(gTangente,UV).rgb;
    finalColor = texture(gTangente,UV).rgb;

}
