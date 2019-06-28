#version 330 core
//Matrizes
uniform mat4 mvp; //Matriz model view projection
uniform mat4 mv; // Matriz model view
    uniform mat4 normalMatrix; //Inversa transposta da MV
layout( location = 0 ) in vec3 vertexPos; //Posição do vértice
layout( location = 1 ) in vec3 vertexNormal; //Normal do vértice
layout (location = 3) in vec3 vVectorPos; //Posição dos vértices do quadrilátero
layout (location = 5) in vec3 vPos; //Posição dos vértices do quadrilátero
out vec2 UV; //Coordenadas de textura do quad
out vec3 fragPos; //Coordenadas de textura do quad
out vec3 fragNormal;
out vec3 worldPos;
float hash1(float n)
{
    return fract(sin(n) * 1e4);
}
float hash1(vec2 p)
{
    return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x))));
}

// 2D Noise based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise1(vec2 x) {
    vec2 i = floor(x);
    vec2 f = fract(x);

        // Four corners in 2D of a tile
    float a = hash1(i);
    float b = hash1(i + vec2(1.0, 0.0));
    float c = hash1(i + vec2(0.0, 1.0));
    float d = hash1(i + vec2(1.0, 1.0));

    // Simple 2D lerp using smoothstep envelope between the values.
        // return vec3(mix(mix(a, b, smoothstep(0.0, 1.0, f.x)),
        //			mix(c, d, smoothstep(0.0, 1.0, f.x)),
        //			smoothstep(0.0, 1.0, f.y)));

        // Same code, with the clamps in smoothstep and common subexpressions
        // optimized away.
    vec2 u = f * f * (3.0 - 2.0 * f);
        return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float noise1(float x) {
    float i = floor(x);
    float f = fract(x);
    float u = f * f * (3.0 - 2.0 * f);
    return mix(hash1(i), hash1(i + 1.0), u);
}
void main()
{

    //Posição do vértice no espaço de projeção
    gl_Position = mvp*vec4( 0.5*vVectorPos, 1 );

    float f = (noise1(4*vertexPos.xy)*0.5 + noise1(8*vertexPos.xy)*0.25 + noise1(16*vertexPos.xy)*0.125 + noise1(32*vertexPos.xy)*0.0625 + noise1(64*vertexPos.xy)*0.03125 + noise1(128*vertexPos.xy)*0.015625);
    //UV = (vPos.xy + vec2(1,1))/2.0;
    fragPos =  (mv * vec4( vVectorPos, 1 ) ).xyz;
    vec3 normal = f * vertexNormal;

    //Matriz de rotação
//    vec3 firstCol = vec3()
//    vec3 secondCol =
//    vec3 thirdCol =
//    mat3 rotationMatrix =
    //Posição da normal no espaço do olho
    fragNormal = normalize(( normalMatrix * vec4( normal, 0 ) ).xyz);
    //worldPos = vertexPos;

}
