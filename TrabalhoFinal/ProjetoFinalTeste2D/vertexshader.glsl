#version 330 core
//Matrizes
uniform mat4 mvp; //Matriz model view projection
uniform mat4 mv; // Matriz model view
    uniform mat4 normalMatrix; //Inversa transposta da MV
layout( location = 0 ) in vec3 vertexPos; //Posição do vértice
layout( location = 1 ) in vec3 vertexNormal; //Normal do vértice
layout (location = 3) in vec3 vVectorPos; //Posição dos vértices do quadrilátero
layout (location = 3) in vec3 vFixedPos; //Posição dos vértices do quadrilátero
layout (location = 5) in vec3 vPos; //Posição dos vértices do quadrilátero
out vec2 UV; //Coordenadas de textura do quad
out vec3 fragPos; //Coordenadas de textura do quad
out vec3 fragNormal;
out vec3 worldPos;
uniform float time;
uniform int mode;
in int gl_VertexID ;

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

    if(mode == 0 || mode == 1)
    {
        //Posição do vértice no espaço de projeção
        gl_Position = vec4( vPos, 1 );
    }
    else
    {
        if(mode == 4 || mode == 5)
        {
            if(gl_VertexID % 2 != 0)
            {
                //Noise com 6 oitavas
                float f = (noise1(4*vVectorPos.xy)*0.5 + noise1(8*vVectorPos.xy)*0.25 + noise1(16*vVectorPos.xy)*0.125 + noise1(32*vVectorPos.xy)*0.0625 + noise1(64*vVectorPos.xy)*0.03125 + noise1(128*vVectorPos.xy)*0.015625);
                //f = noise1(vVectorPos.xy);
                float a,b,c;
                a = 0;
                b = 0;
                c = 3.14/2;
////                //Posição do vértice no espaço de projeção
//                mat3 rotation = mat3( cos(a)*cos(c) - sin(a)*cos(b)*sin(c)  ,    sin(a)*cos(c) + cos(a)*cos(b)*sin(c)  ,   sin(b)* sin(c),
//                                    -cos(a)*sin(c) - sin(a)*cos(b)*cos(c)  ,     -sin(a)*sin(c) + cos(a)*cos(b)*cos(c) ,    sin(b)*cos(c),
//                                    sin(b)*sin(c), -cos(a)*sin(b)  , cos(b)    );
//                mat2 rotation = mat2(cos(c),sin(c), -sin(c),cos(c));
//                mat3 rotation = mat3(cos(a)*cos(c) - sin(a)*cos(b)*sin(c) ,-cos(a)*sin(c) - sin(a)*cos(b)*cos(c),sin(b)*sin(c),
//                                     sin(a)*cos(c) + cos(a)*cos(b)*sin(c),-sin(a)*sin(c) + cos(a)*cos(b)*cos(c) ,-cos(a)*sin(b),
//                                     sin(b)* sin(c),sin(b)*cos(c), cos(b));
                float A = vFixedPos.x;
                float B = vFixedPos.y;
                float C = vFixedPos.z;
                vec3 firstCol = vec3(cos(c) + (1 - cos(c))*A*A, (1 - cos(c))*B*A - sin(c)*C, (1 - cos(c))*C*A + sin(c)*B);
                vec3 secondCol = vec3((1 - cos(c))*B*A - sin(c)*C,cos(c) + (1 - cos(c))*B*B,(1 - cos(c))*C*B - sin(c)*A);
                vec3 thirdCol = vec3((1 - cos(c))*C*A - sin(c)*B, (1 - cos(c))*C*B + sin(c)*A,cos(c) + (1 - cos(c))*C*C);
                mat3 rotation = mat3(firstCol,secondCol,thirdCol);
                vec3 rotated = rotation * 0.5*vVectorPos;

                 gl_Position = vec4(rotated,1);
            }
            else
            {
                gl_Position = vec4(0.5*vVectorPos,1);
            }
        }
        else
        {
            //Posição do vértice no espaço de projeção
            gl_Position = mvp*vec4( vertexPos, 1 );
        }
    }

    float f = (noise1(4*vVectorPos.xy)*0.5 + noise1(8*vVectorPos.xy)*0.25 + noise1(16*vVectorPos.xy)*0.125 + noise1(32*vVectorPos.xy)*0.0625 + noise1(64*vVectorPos.xy)*0.03125 + noise1(128*vVectorPos.xy)*0.015625);
    UV = (vPos.xy + vec2(1,1))/2.0;
    fragPos =  (mv * vec4( vertexPos, 1 ) ).xyz;
    //Posição da normal no espaço do olho
    fragNormal = normalize(( normalMatrix * vec4( vertexNormal, 0 ) ).xyz);
    worldPos = vertexPos;

}
