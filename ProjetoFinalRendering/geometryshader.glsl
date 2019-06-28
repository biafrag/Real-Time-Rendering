#version 330 core
layout(lines) in;
layout(line_strip, max_vertices = 2) out;
in vec3 geomPos[];
in vec3 geomNormal[];
in vec3 geomWorldPos[];

out vec3 fragPos;
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

void main(void)
{

    vec2 firstPointOrigin = gl_in[0].gl_Position.xy - gl_in[0].gl_Position.xy;
    vec2 secondPointOrigin = gl_in[1].gl_Position.xy - gl_in[0].gl_Position.xy;

    //Completamente aleatório
    //float f = noise1(128*gl_in[1].gl_Position.xy);
    vec2 UV = (gl_in[0].gl_Position.xy + gl_in[1].gl_Position.xy)/2 ;
    //Noise com 6 oitavas
    float f = (noise1(4*UV)*0.5 + noise1(8*UV)*0.25 + noise1(16*UV)*0.125 + noise1(32*UV)*0.0625 + noise1(64*UV)*0.03125 + noise1(128*UV)*0.015625);
//    float f = 0;
//    vec2 pos = UV;
//    pos *= 8.0;
//    mat2 m = mat2( 1.6,  1.2, -1.2,  1.6 );
//    //f  = 0.5000*noise( pos );
//    pos = m*pos;
//    f += 0.2500*noise1( pos );
//    pos = m*pos;
//    f += 0.1250*noise1( pos );
//    pos = m*pos;
//    f += 0.0625*noise1( pos );


//    f = 0.5 + 0.5*f;

//    f *= smoothstep( 0.0, 0.005, abs(pos.x-0.6) );

    float theta = 2*(3.14/2)*f;
    vec2 firstCol = vec2(cos(theta),sin(theta));
    vec2 secondCol = vec2(sin(theta),cos(theta));
    mat2 rotationMatrix = mat2(firstCol,secondCol);
    vec2 resultRotation = rotationMatrix*secondPointOrigin;

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    vec2 newSecondPoint = resultRotation + gl_in[0].gl_Position.xy;
    gl_Position = vec4(newSecondPoint,0,1);
    EmitVertex();

    EndPrimitive();
}
