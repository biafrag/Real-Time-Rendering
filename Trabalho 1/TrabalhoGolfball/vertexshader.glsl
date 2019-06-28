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
    uniform vec3 lightPos; // Posição da luz em coordenada do olho

    //Variáveis out
    out vec2 fragUV; // Coordenada de textura passada pro fragment
    out vec3 light; // Posição da luz passada pro fragment no espaco tangente
    out vec3 tanViewer; //Viewer no espaço tangente
    out vec3 tang;
    out vec3 fragNormal;
    out vec3 fragPos;

    void main()
    {

        //Posição do vértice no espaço de projeção
        gl_Position = mvp * vec4( vertexPos, 1 );

        //Posição do vétice no espaço do olho
         fragPos = ( mv * vec4( vertexPos, 1 ) ).xyz;

        //Posição da normal no espaço do olho
         fragNormal = normalize(( normalMatrix * vec4( vertexNormal, 0 ) ).xyz);

        //Posição da tangente no espaço do olho
        vec3 tangentVertexEye = normalize(( normalMatrix * vec4( tangent, 0 ) ).xyz);

        //Bitangente no espaço do olho
        vec3 bitangentVertexEye= normalize(cross(fragNormal,tangentVertexEye));

        //Matriz de rotação tbn para transformar luz para o eapaço tangente
        mat3 rotation = transpose(mat3(tangentVertexEye,bitangentVertexEye,fragNormal));

        //Só passando coordenadas de textura pro fragment
        fragUV = vertexTexCoord ;

        //Colocando luz no espaco tangente
        light = rotation*normalize(lightPos - fragPos);

        //Viewer no espaco tangente
        tanViewer = rotation*normalize(-fragPos);

        tang = normalize(bitangentVertexEye);

    }
