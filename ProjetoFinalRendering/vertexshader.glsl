#version 330 core
//Matrizes
uniform mat4 mvp; //Matriz model view projection
uniform mat4 mv; // Matriz model view
uniform mat4 normalMatrix; //Inversa transposta da MV
uniform vec3 normal;
uniform float time;
layout (location = 3) in vec3 vVectorPos; //Posição dos vértices do quadrilátero
out vec2 geoUV; //Coordenadas de textura do quad
out vec3 geoPos; //Coordenadas de textura do quad
out vec3 geoNormal;
out vec3 pos;
float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}
float noise(vec3 p){
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * (1.0 / 41.0));
    vec4 o2 = fract(k4 * (1.0 / 41.0));

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}

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
float noise1(vec3 x) {
    const vec3 step = vec3(110, 241, 171);

    vec3 i = floor(x);
    vec3 f = fract(x);

    // For performance, compute the base input to a 1D hash from the integer part of the argument and the
    // incremental change to the 1D based on the 3D -> 1D wrapping
    float n = dot(i, step);

    vec3 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(mix( hash1(n + dot(step, vec3(0, 0, 0))), hash1(n + dot(step, vec3(1, 0, 0))), u.x),
                   mix( hash1(n + dot(step, vec3(0, 1, 0))), hash1(n + dot(step, vec3(1, 1, 0))), u.x), u.y),
               mix(mix( hash1(n + dot(step, vec3(0, 0, 1))), hash1(n + dot(step, vec3(1, 0, 1))), u.x),
                   mix( hash1(n + dot(step, vec3(0, 1, 1))), hash1(n + dot(step, vec3(1, 1, 1))), u.x), u.y), u.z);
}
void main()
{
    vec3 pos = vec3(vVectorPos.xy,0);
    //vec2 pos = vec2(vVectorPos.xy);
    float z = (noise1(4*pos)*0.25 + noise1(8*pos)*0.125 /*+ noise(16*pos)*0.125 + noise(32*pos)*0.0625 + noise1(64*pos)*0.03125 + noise(128*pos)*0.015625*/);
    //z = sin(z*3.14/2);

    //float z = (abs(noise1(8*pos)*0.5 - 0.25)  + abs(noise1(16*pos.xy)*0.25 - 0.125) + abs(noise1(32*pos.xy)*0.125 - 0.0625) + abs(noise1(64*pos.xy)*0.0625 - 0.03125)  );
    //z = noise(50*pos)*0.5;

    //float z = noise1(128* pos);
    //z = 0;
    vec3 pos2 = vec3(vVectorPos.x,vVectorPos.y,0.5*z);

    //Posição do vértice no espaço de projeção
    gl_Position = mvp*vec4( pos2, 1 );

    geoPos =  (mv * vec4( pos2, 1 ) ).xyz;

    //Posição da normal no espaço do olho
    geoNormal = normalize(( normalMatrix * vec4( normal, 0 ) ).xyz);

    geoUV = (vVectorPos.xy + vec2(1,1))/2.0;
}
