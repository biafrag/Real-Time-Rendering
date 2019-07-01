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



void main(void)
{  
    //float z = (noise1(4*pos)*0.5 + noise1(8*pos)*0.25 + noise1(16*pos)*0.125 + noise1(32*pos)*0.0625 + noise1(64*pos)*0.03125 + noise1(128*pos)*0.015625);
    fragUV = geoUV[0];
    fragPos = geoPos[0];
    vec3 v1 = geoPos[0] - geoPos[1];
    vec3 v2 = geoPos[2] - geoPos[1];
    fragNormal = cross(v1,v2);

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
