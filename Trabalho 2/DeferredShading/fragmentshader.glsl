#version 410 core

////Variaveis de entrada

in vec2 UV;
out vec3 finalColor; // Cor final do objeto

uniform sampler2D gAmbiente;
uniform sampler2D gDepth;
uniform sampler2D gDifusa;
uniform sampler2D gPosition;

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
    finalColor = texture(gAmbiente,UV).rgb;

}
