#version 410 core

layout (location = 5) in vec3 vPos;

out vec2 UV;
    void main()
    {

    //        //Posição do vértice no espaço de projeção
            gl_Position = vec4( vPos, 1 );


            UV = (vPos.xy + vec2(1,1))/2.0;
    }