#version 330 core

in vec2 UV; //Coordenadas de textura do quad
in vec3 fragPos;
in vec3 fragNormal;
in vec3 worldPos;
out vec4 finalColor; // Cor final do objeto
uniform vec3 light; // Posição da luz em coordenada do olho

struct Material //Propriedades do material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;
//
// Author @patriciogv - 2015 - patriciogonzalezvivo.com

#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

uniform sampler2D oceanTex; //Textura de profundidade

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


vec2 hash( vec2 x )  // replace this by something better
{
    const vec2 k = vec2( 0.3183099, 0.3678794 );
    x = x*k + k.yx;
    return -1.0 + 2.0*fract( 16.0 * k*fract( x.x*x.y*(x.x+x.y)) );
}

float noise( in vec2 p )
{
    vec2 i = floor( p );
    vec2 f = fract( p );

        vec2 u = f*f*(3.0-2.0*f);

    return mix( mix( dot( hash( i + vec2(0.0,0.0) ), f - vec2(0.0,0.0) ),
                     dot( hash( i + vec2(1.0,0.0) ), f - vec2(1.0,0.0) ), u.x),
                mix( dot( hash( i + vec2(0.0,1.0) ), f - vec2(0.0,1.0) ),
                     dot( hash( i + vec2(1.0,1.0) ), f - vec2(1.0,1.0) ), u.x), u.y);
}

float noise3D(vec3 x) {
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

        //vec3 colorNoise;
        vec3 ambient = material.ambient * vec3(0.3,0.7,0.8);/**texture(oceanTex,UV).rgb*/;//texture(sampler, fragUV).rgb; // * light.ambient;
        vec3 diffuse = vec3(0.0,0.0,0.0)*vec3(0,0,1);
        vec3 specular = vec3(0.0,0.0,0.0);

        vec3 N = normalize(fragNormal);
        vec3 L = normalize(light - fragPos);

        float iDif = dot(L,N);

        if( iDif > 0 )
        {
            diffuse = iDif * material.diffuse * vec3(0.3,0.7,0.8)/**texture(oceanTex,UV).rgb*/;//texture(sampler, fragUV).rgb; // * light.diffuse;

            vec3 V = normalize(-fragPos);
            vec3 H = normalize(L + V);

            float iSpec = pow(max(dot(N,H),0.0),material.shininess);
            specular = iSpec * material.specular; // * light.specular;
        }

//        //Noise com 6 oitavas
//        float f = (noise3D(4*worldPos)*0.5 + noise3D(8*worldPos)*0.25 + noise3D(16*worldPos)*0.125 + noise3D(32*worldPos)*0.0625 + noise3D(64*worldPos)*0.03125 + noise3D(128*worldPos)*0.015625);
//        vec3 skyColor = vec3(0,0,0.8);
//        vec3 cloudColor = vec3(0.8,0.8,0.8);
//        colorNoise = mix(skyColor,cloudColor,f);
        finalColor = vec4(diffuse + ambient + specular,1);
        //finalColor = vec4(1,0,0,1);
}
