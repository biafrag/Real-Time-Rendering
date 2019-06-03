
#version 410 core

    layout( location = 0 ) in vec3 vertexPos; //Posição do vértice
    layout( location = 1 ) in vec3 vertexNormal; //Normal do vértice

    //Coordenadas de textura
    layout( location = 4 ) in vec2 vertexTexCoord; // Coordenada de textura

    layout( location = 2 ) in vec3 tangent; // Vetor tangente

    //Matrizes
    uniform mat4 mvp; //Matriz model view projection
    uniform mat4 mv; // Matriz model view
    uniform mat4 normalMatrix; //Inversa transposta da MV

    //Variáveis out
    out vec3 fragNormal;
    out vec3 fragPos;
    out vec3 fragTang;

    void main()
    {
        //Posição do vértice no espaço de projeção
        gl_Position = mvp * vec4( vertexPos, 1 );

        //Posição do vértice no espaço de olho
        fragPos =  (mv * vec4( vertexPos, 1 ) ).xyz;

        //Posição da normal no espaço do olho
        fragNormal = ( normalMatrix * vec4( vertexNormal, 0 ) ).xyz;

        fragTang = tangent;

    }
