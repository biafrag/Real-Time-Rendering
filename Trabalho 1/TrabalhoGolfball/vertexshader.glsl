#version 410 core

    layout( location = 0 ) in vec3 vertexPos; //Posição do vértice
    layout( location = 1 ) in vec3 vertexNormal; //Normal do vértice

     //Coordenadas de textura
     layout( location = 4 ) in vec2 vertexTexCoord; // Coordenada de textura

     layout( location = 2 ) in vec3 tangent; // Vetor tangente
     layout( location = 3 ) in vec3 bitangent; // Binormal ou bitangente

    //Matrizes
    uniform mat4 mvp; //Matriz model view projection
    uniform mat4 mv; // Matriz model view
    uniform mat4 normalMatrix; //Inversa transposta da MV
    uniform vec3 lightPos; // Posição da luz em coordenada do olho

    //Variáveis out
    out vec3 fragPos; // Posição do vértice passada pro fragment
    out vec3 fragNormal; // Normal do vértice passada pro fragment
    out vec2 fragUV; // Coordenada de textura passada pro fragment
    out vec3 light; // Posição da luz passada pro fragment
    out vec3 tanViewer;
    out vec3 tang;
    void main()
    {
        //Teste
        //vec3 dyp = dFdx(vertexPos);

        //Posição do vértice no espaço de projeção
        gl_Position = mvp * vec4( vertexPos, 1 );

        //Posição do vétice no espaço do olho
        fragPos = ( mv * vec4( vertexPos, 1 ) ).xyz;

        //Posição da normal no espaço
        fragNormal = ( normalMatrix * vec4( vertexNormal, 0 ) ).xyz;
        vec3 tangentVertexEye = ( normalMatrix * vec4( tangent, 0 ) ).xyz;
        tang = tangent;
        vec3 bitangentVertexEye= normalize(cross(fragNormal,tangentVertexEye));
        //Matriz de rotação tbn para transformar luz para o eapaço tangente
        mat3 rotation = transpose(mat3(tangentVertexEye,bitangentVertexEye,fragNormal));

        //Só passando coordenadas de textura pro fragment
        fragUV = vertexTexCoord ;

        //Colocando luz no espaço tangente
        light = rotation*normalize(lightPos - fragPos);
        tanViewer =rotation*normalize(-fragPos);
    }
