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


mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

float hash1(float n)
{
    return fract(sin(n) * 1e4);
}
float hash1(vec2 p)
{
    return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x))));
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
void main(void)
{  
    //float z = (noise1(4*pos)*0.5 + noise1(8*pos)*0.25 + noise1(16*pos)*0.125 + noise1(32*pos)*0.0625 + noise1(64*pos)*0.03125 + noise1(128*pos)*0.015625);
    fragUV = geoUV[0];
    fragPos = geoPos[0];
    vec3 v1 = geoPos[0] - geoPos[1];
    vec3 v2 = geoPos[2] - geoPos[1];
    vec3 normal = normalize(cross(v1,v2));
    float n1 = noise1(fragPos);
    vec3 axis = vec3(0,-normal.x,normal.y);
    float angle = acos(normal.z);
    mat4 m = rotationMatrix(axis, n1*angle);
    fragNormal = normalize((m * vec4(normal,0)).xyz);

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();


    gl_Position = gl_in[1].gl_Position;
    fragPos = geoPos[1];

    n1 = noise1(fragPos);
    m = rotationMatrix(axis,n1*angle);
    fragNormal = normalize((m * vec4(normal,0)).xyz);

    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    fragPos = geoPos[2];

    n1 = noise1(fragPos);
    m = rotationMatrix(axis,n1*angle);
    fragNormal = normalize((m * vec4(normal,0)).xyz);
    EmitVertex();

    EndPrimitive();
}
