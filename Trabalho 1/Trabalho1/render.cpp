
#include "render.h"
#include <QImage>
#include<QGLWidget>
#include "reader.h"


const char* vertexShaderSource = R"(
    #version 330 core

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

    void main()
    {
        //Posição do vértice no espaço de projeção
        gl_Position = mvp * vec4( vertexPos, 1 );

        //Posição do vétice no espaço do olho
        fragPos = ( mv * vec4( vertexPos, 1 ) ).xyz;

        //Posição da normal no espaço
        fragNormal = ( normalMatrix * vec4( vertexNormal, 0 ) ).xyz;

        //Matriz de rotação tbn para transformar luz para o eapaço tangente
        mat3 rotation = transpose(mat3(tangent,bitangent,fragNormal));

        //Só passando coordenadas de textura pro fragment
        fragUV = vertexTexCoord ;

        //Colocando luz no espaço tangente
        light = rotation*normalize(lightPos - fragPos);
    }
)";


const char* fragmentShaderSource = R"(
#version 330 core

struct Material //Propriedades do material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};


uniform Material material;
uniform vec3 color;

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragUV;
in vec3 light;

uniform sampler2D sampler; //Textura difusa
uniform sampler2D normalsampler; // Textura de mapa de normal
out vec3 finalColor; // Cor final do objeto

vec3 expand(vec3 v)
{
   return (v - 0.5) * 2;
}

void main()
{
    vec3 ambient = material.ambient * texture(sampler, fragUV).rgb; //Componente da luz ambiente
    vec3 diffuse = vec3(0.0,0.0,0.0);
    vec3 specular = vec3(0.0,0.0,0.0);

    //Normal usada é a de textura de mapa de normal
    vec3 N = /*fragNormal;*/expand(texture(normalsampler,fragUV).rgb);

    //Normalizando novamente a luz no espaço do olho
    vec3 L = normalize(light);

    //Calcula produto interno entre luz e normal no espaço do olho
    float iDif = dot(L,N);

    //Se certifica que a luz e a normal não são perpendiculares
    if( iDif > 0 )
    {
        diffuse = iDif * material.diffuse * texture(sampler, fragUV).rgb; // Calcula componente difusa da luz

        vec3 V = normalize(-fragPos); // Viewer
        vec3 H = normalize(L + V);

        float iSpec = pow(max(dot(N,H),0.0),material.shininess);
        specular = iSpec * material.specular; //Calcula componente especular
    }

    finalColor = ambient + diffuse + specular;
}
)";

Render::Render(QWidget* parent)
    :QOpenGLWidget(parent)
{
    cam.at = QVector3D(0.5f,0.5f,0.f);
    cam.eye =  QVector3D(0.f,100.f,200.f);
    cam.up = QVector3D(0.f,2.f,0.f);
    cam.zNear = 0.1f;
    cam.zFar  = 1000.f;
    cam.fovy  = 20.f;
    cam.width = width();
    cam.height = height();
}

void Render::setFile(std::string fileName)
{
    std::vector<int> indexPointsQuad;
    std::vector<int> indexPointsTriangle;
    _points.clear();
    _normals.clear();
    _texCoords.clear();
    _indexPoints.clear();
    _indexNormals.clear();
    _indexTex.clear();
    readFile(fileName,_points,_normals,_texCoords,indexPointsTriangle,indexPointsQuad,_indexNormals,_indexTex);
    quadToTriangleMesh(indexPointsQuad, indexPointsTriangle);
    //computeTangent();
    _program->bind();
}


void Render::quadToTriangleMesh(std::vector<int>& indexPointsQuad, std::vector<int>& indexPointsTriangle)
{
    //Checar
    std::vector<unsigned int> triangleMesh;

    unsigned int numberofQuads = static_cast<unsigned int>(indexPointsQuad.size() / 4);

    //Every four elements it's a quadrilateral element
    for(unsigned int i = 0; i < numberofQuads; i++)
    {
        unsigned int v0 = indexPointsQuad[4 * i];
        unsigned int v1 = indexPointsQuad[4 * i + 1];
        unsigned int v2 = indexPointsQuad[4 * i + 2];
        unsigned int v3 = indexPointsQuad[4 * i + 3];

        //First triangle from quadrilateral element
        _indexPoints.push_back(v0);
        _indexPoints.push_back(v1);
        _indexPoints.push_back(v3);

        //Second triangle from quadrilateral element
        _indexPoints.push_back(v2);
        _indexPoints.push_back(v3);
        _indexPoints.push_back(v1);
    }

    for(int i = 0; i<indexPointsTriangle.size(); i++)
    {
        _indexPoints.push_back(indexPointsTriangle[i]);
    }
}
void Render::initializeGL()
{
    initializeOpenGLFunctions();

    makeCurrent();

    glViewport(0,0,width(),height());

    //Layout de ponto e linha:
    glEnable(GL_POINT_SMOOTH);
    glEnable( GL_LINE_SMOOTH );
    glLineWidth(1.0f);

    _program = new QOpenGLShaderProgram();

    //Adicionando shaders ao programa
    _program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    _program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);

    //Linka shaders que foram adicionados ao programa
    _program->link();

    if (!_program->isLinked())
    {
        std::cout<<"Problemas ao linkar shaders"<<std::endl;
    }
    //Liga o programa ao atual contexto
   // setFile("../golfball/golfball.obj");
    //setFile("../sphere/sphere.obj");
   setFile("../stones/stones.obj");

    _program->bind();

    createVAO();

    //Criando textura para colocar no meu cubo
   // createTexture("../golfball/golfball.png");
     createTexture("../stones/stones.jpg");

    createNormalMapTexture("../stones/stones_norm.jpg");

    GLint textureLocation = glGetUniformLocation(_program->programId(), "sampler");
    glUniform1i(textureLocation, 0);

    GLint normalMap = glGetUniformLocation(_program->programId(), "normalsampler");
    glUniform1i(normalMap,  1);

    printf("Tamanho do vetor de pontos: %d  Tamanho do vetor de tangente: %d Tamanho de normal: %d",_points.size(),_tangents.size(),_normals.size());

    printThings();
}
void Render::printThings()
{
    printf("Points: \n");
    for(int i = 0; i< _points.size(); i ++)
    {
        printf( "%f %f %f\n",_points[i].x(),_points[i].y(),_points[i].z());
    }


    printf("Normals: \n");
    for(int i = 0; i< _normals.size(); i ++)
    {
        printf( "%f %f %f\n",_normals[i].x(),_normals[i].y(),_normals[i].z());
    }


    printf("Textura: \n");
    for(int i = 0; i< _texCoords.size(); i ++)
    {
        printf( "%f %f \n",_texCoords[i].x(),_texCoords[i].y());
    }


    printf("Indices: \n");
    for(int i = 0; i< _indexPoints.size(); i ++)
    {
        printf( "%d ",_indexPoints[i]);
    }
}


void Render::createTexture(const std::string& imagePath)
{
    //Gerando textura e recebendo ID dessa textura
    glGenTextures(1, &_textureID);

    //Linkar (bind) a textura criada
    glBindTexture(GL_TEXTURE_2D, _textureID);

    //Abrir arquivo de imagem com o Qt
    QImage texImage = QGLWidget::convertToGLFormat(QImage(imagePath.c_str()));
    //QImage texImage(imagePath.c_str());

    //Enviar a imagem para o OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA,
                 texImage.width(), texImage.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage.bits());


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);
}




void Render::createNormalMapTexture(const std::string& imagePath)
{
    //Gerando textura e recebendo ID dessa textura
    glGenTextures(1, &_normalMap);

    //Linkar (bind) a textura criada
    glBindTexture(GL_TEXTURE_2D, _normalMap);

    //Abrir arquivo de imagem com o Qt
    QImage texImage = QGLWidget::convertToGLFormat(QImage(imagePath.c_str()));
    //QImage texImage(imagePath.c_str());

    //Enviar a imagem para o OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA,
                 texImage.width(), texImage.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);
}



void Render::resizeGL(int w, int h)
{
    cam.width = w;
    cam.height = h;
}



void Render::paintGL()
{

    //Dando bind no programa e no vao
    _program->bind();
    _vao.bind();

    //Definindo matriz view e projection
     _view.setToIdentity();
     _view.lookAt(cam.eye, cam.at, cam.up);
     _proj.setToIdentity();
     _proj.perspective((cam.fovy*3.14)/180, (float)cam.width/cam.height, cam.zNear, cam.zFar);
     _model.setToIdentity();

    //Definindo matrizes para passar para os shaders
    QMatrix4x4 m = _model;
    QMatrix4x4 v = _view;
    QMatrix4x4 p = _proj;

    QMatrix4x4 mv = v*m;
    QMatrix4x4 mvp =p*mv;

    //Ativar e linkar a textura
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureID);
//    _program->setUniformValue("sampler", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _normalMap);
//    _program->setUniformValue("normalsampler", 0);

    //Passando as variáveis uniformes para os shaders
    //model-view : Passa para espaço do olho
    _program->setUniformValue("mv", mv);
    //model-view : Passa para espaço de projeção
    _program->setUniformValue("mvp", mvp);
    //inversa transposta da model-view
    _program->setUniformValue("normalMatrix", mv.inverted().transposed());
    //Variáveis de material e luz
    _program->setUniformValue("lightPos", cam.eye);
    //Imagem 2D
//    _program->setUniformValue("material.ambient", QVector3D(0.1f,0.1f,0.1f));
//    _program->setUniformValue("material.diffuse", QVector3D(1.0f,1.0f,1.0f));
//    _program->setUniformValue("material.specular", QVector3D(0.f,0.3f,0.3f));
//    _program->setUniformValue("material.shininess", 100.0f);
    //Bola
        _program->setUniformValue("material.ambient", QVector3D(0.2f,0.2f,0.2f));
        _program->setUniformValue("material.diffuse", QVector3D(0.8f,0.8f,0.8f));
        _program->setUniformValue("material.specular", QVector3D(0.0f,0.0f,0.0f));
        _program->setUniformValue("material.shininess", 100.0f);
        _program->setUniformValue("color", QVector3D(1.0,1.0,1.0));

    //Desenhando os triângulos que formam o cubo
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indexPoints.size()), GL_UNSIGNED_INT, nullptr);
    update();
}

void Render::computeTangent()
{
    std::vector<QVector3D> auxTang;
    for ( int i=0; i<_indexPoints.size(); i++)
    {

            int id0 = _indexPoints[3*i];
            int id1 = _indexPoints[3*i + 1];
            int id2 = _indexPoints[3*i + 2];

            int idv0 = _indexTex[3*i];
            int idv1 = _indexTex[3*i + 1];
            int idv2 = _indexTex[3*i + 2];

            // Shortcuts for vertices
            QVector3D v0 = _points[id0];
            QVector3D v1 = _points[id1];
            QVector3D v2 = _points[id2];

            // Shortcuts for UVs
            QVector2D uv0 = _texCoords[idv0];
            QVector2D uv1 = _texCoords[idv1];
            QVector2D uv2 = _texCoords[idv2];

            // Edges of the triangle : position delta
            QVector3D deltaPos1 = v1-v0;
            QVector3D deltaPos2 = v2-v0;

            // UV delta
            QVector2D deltaUV1 = uv1-uv0;
            QVector2D deltaUV2 = uv2-uv0;
    //We can now use our formula to compute the tangent and the bitangent :

            float r = 1.0f / (deltaUV1.x() * deltaUV2.y() - deltaUV1.y() * deltaUV2.x());
            QVector3D tangent = (deltaPos1 * deltaUV2.y()   - deltaPos2 * deltaUV1.y())*r;
            QVector3D bitangent = (deltaPos2 * deltaUV1.x()   - deltaPos1 * deltaUV2.x())*r;
   // Finally, we fill the *tangents *and *bitangents *buffers. Remember, these buffers are not indexed yet, so each vertex has its own copy.

            // Set the same tangent for all three vertices of the triangle.
            // They will be merged later, in vboindexer.cpp
            _tangents.push_back(tangent);
            _tangents.push_back(tangent);
            _tangents.push_back(tangent);

            // Same thing for bitangents
            _bitangents.push_back(bitangent);
            _bitangents.push_back(bitangent);
            _bitangents.push_back(bitangent);
    }

}

void Render::createVAO()
{
    //Criando e configurando vao
    _vao.create();
    _vao.bind();

    //Criando buffer de pontos dos vértices
    glGenBuffers(1, &_pointsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _pointsBuffer);
    glBufferData(GL_ARRAY_BUFFER, _points.size()*sizeof(QVector3D), &_points[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    //Criando buffer de normais
    glGenBuffers(1, &_normalsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _normalsBuffer);
    glBufferData(GL_ARRAY_BUFFER, _normals.size()*sizeof(QVector3D), &_normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    //Criando buffer de tangentes
    glGenBuffers(1, &_tangentBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _tangentBuffer);
    glBufferData(GL_ARRAY_BUFFER, _tangents.size()*sizeof(QVector3D), &_tangents[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);

    //Criando buffer de tangentes
    glGenBuffers(1, &_bitangentBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _bitangentBuffer);
    glBufferData(GL_ARRAY_BUFFER, _bitangents.size()*sizeof(QVector3D), &_bitangents[0], GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(3);

    //Criando buffers de textura
    glGenBuffers(1, &_texCoordsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,_texCoordsBuffer);
    glBufferData(GL_ARRAY_BUFFER, _texCoords.size()*sizeof(QVector2D), _texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(4);

    //Criando buffers de indexPoints
    glGenBuffers(1, &_meshBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexPoints.size()*sizeof(int), _indexPoints.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshBuffer);


}

