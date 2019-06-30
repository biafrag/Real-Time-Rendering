#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;
uniform mat4 normalMatrix; //Inversa transposta da MV
in vec2 geoUV[]; //Coordenadas de textura do quad
in vec3 geoPos[]; //Coordenadas de textura do quad
in vec3 geoNormal[];

out vec2 fragUV; //Coordenadas de textura do quad
out vec3 fragPos; //Coordenadas de textura do quad
out vec3 fragNormal;

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

void main(void)
{  
    //float z = (noise1(4*pos)*0.5 + noise1(8*pos)*0.25 + noise1(16*pos)*0.125 + noise1(32*pos)*0.0625 + noise1(64*pos)*0.03125 + noise1(128*pos)*0.015625);
    fragUV = geoUV[0];
    fragPos = geoPos[0];
    vec3 v1 = geoPos[0] - geoPos[1];
    vec3 v2 = geoPos[2] - geoPos[1];
    fragNormal = cross(v2,v1);

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    fragPos = geoPos[1];
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    fragPos = geoPos[2];
    EmitVertex();

    EndPrimitive();
}
