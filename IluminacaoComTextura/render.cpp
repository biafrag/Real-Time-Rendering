#include "render.h"
#include <QImage>
#include<QGLWidget>

const char* vertexShaderSource = R"(
    #version 330 core

    layout( location = 0 ) in vec3 vertexPos;
    layout( location = 1 ) in vec3 vertexNormal;

    //Coordenadas de textura
    layout( location = 2 ) in vec2 vertexTexCoord;

    //Matrizes
    uniform mat4 mvp;
    uniform mat4 mv;
    uniform mat4 normalMatrix; //inversa transposta da MV

    //Posição e normal no espaço da câmera:
    out vec3 fragPos;
    out vec3 fragNormal;

    //Coordenadas de textura do fragmento
    out vec2 fragUV;
    void main()
    {
        gl_Position = mvp * vec4( vertexPos, 1 );
        fragPos = ( mv * vec4( vertexPos, 1 ) ).xyz;

        fragNormal = ( normalMatrix * vec4( vertexNormal, 0 ) ).xyz;

        fragUV = vertexTexCoord;
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core

    struct Material
    {
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float shininess;
    };

    struct Light
    {
        vec3 position; //No espaço da câmera
    };

    uniform Light light;
    uniform Material material;

    in vec3 fragPos;
    in vec3 fragNormal;
    in vec2 fragUV;

    uniform sampler2D sampler;
    out vec3 finalColor;

    void main()
    {
       vec3 ambient = material.ambient * texture(sampler,fragUV).rgb; // * light.ambient;
       vec3 diffuse = vec3(0.0,0.0,0.0);
       vec3 specular = vec3(0.0,0.0,0.0);

       vec3 N = normalize(fragNormal);
       vec3 L = normalize(light.position - fragPos);

       float iDif = dot(L,N);

       if( iDif > 0 )
       {
           diffuse = iDif * material.diffuse *  texture(sampler,fragUV).rgb; // * light.diffuse;

           vec3 V = normalize(-fragPos);
           vec3 H = normalize(L + V);

           float iSpec = pow(max(dot(N,H),0.0),material.shininess);
           specular = iSpec * material.specular; // * light.specular;
       }

       finalColor = ambient + diffuse + specular;
    }
)";

Render::Render(QWidget* parent)
    :QOpenGLWidget(parent)
{
    //São 8 pontos em um cubo
    _points = {
                {-0.5,0.5,0.5}, //1
                {0.5,0.5,0.5}, //2
                {-0.5,-0.5,0.5}, //3
                {0.5,-0.5,0.5}, //4
                {-0.5,0.5,-0.5}, //5
                {0.5,0.5,-0.5},  //6
                {-0.5,-0.5,-0.5},//7
                {0.5,-0.5,-0.5} //8
                };

//    //São 8 pontos em um cubo
//    _points = {
//                {-1,1,1}, //1
//                {1,1,1}, //2
//                {-1,-1,1}, //3
//                {1,-1,1}, //4
//                {-1,1,-1}, //5
//                {1,1,-1},  //6
//                {-1,-1,-1},//7
//                {1,-1,-1} //8
//                };

    // 2 triângulos por face, são 6 faces, então 12 triêngulos
    _indices = {

                1, 0, 2,
                1, 2, 3,
                5, 1, 3,
                5, 3, 7,
                4, 5, 6,
                6, 5, 7,
                0, 4, 6,
                0, 6, 2,
                2, 6, 3,
                3, 6, 7,
                5, 4, 1,
                4, 0, 1

                };

    //Coordenadas de textura
    _texCoords = {
        {0.50, 0.25}, {0.75, 0.25}, {0.5, 0.0},
        {0.75, 0.00}, {0.50, 0.50}, {0.75, 0.50},
        {0.50, 0.75}, {0.75, 0.75}
    };
    _normals.resize(_points.size(), QVector3D(0, 0, 0));
    for (unsigned int t = 0; t < _indices.size() / 3; t++)
    {
       //Pega vértices do triângulo
       unsigned int v0 = _indices[3 * t + 0];
       unsigned int v1 = _indices[3 * t + 1];
       unsigned int v2 = _indices[3 * t + 2];

       //Faz produto externo para achar normal
       QVector3D n = QVector3D::crossProduct(_points[v1] - _points[v0], _points[v2] - _points[v0]);

       //Os pontos são afetados por 3 normais, por isso o +=
       _normals[v0] += n;
       _normals[v1] += n;
       _normals[v2] += n;
    }
    //Normaliza todas as normais
    for (auto& normal : _normals)
    {
       normal = normal.normalized();
    }
    cam.at = QVector3D(0.f,0.f,0.f);
    cam.eye =  QVector3D(0.f,30.f,90.f);
    cam.up = QVector3D(0.f,2.f,0.f);
    cam.zNear = 0.1f;
    cam.zFar  = 100.f;
    cam.fovy  = 60.f;
    cam.width = width();
    cam.height = height();
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
    _program->bind();

    createVAO();

    //Criando textura para colocar no meu cubo
    createTexture(":/textures/cube_texture.png");
}



void Render::createTexture(const std::string& imagePath)
{
    //Gerando textura e recebendo ID dessa textura
    glGenTextures(1, &_textureID);

    //Linkar (bind) a textura criada
    glBindTexture(GL_TEXTURE_2D, _textureID);

    //Abrir arquivo de imagem com o Qt
    QImage texImage = QGLWidget::convertToGLFormat(QImage(imagePath.c_str()));


    //Enviar a imagem para o OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA,
                 texImage.width(), texImage.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage.bits());

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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureID);
    _program->setUniformValue("sampler", 0);

    //Passando as variáveis uniformes para os shaders
    //model-view : Passa para espaço do olho
    _program->setUniformValue("mv", mv);
    //model-view : Passa para espaço de projeção
    _program->setUniformValue("mvp", mvp);
    //inversa transposta da model-view
    _program->setUniformValue("normalMatrix", mv.inverted().transposed());
    //Variáveis de material e luz
    _program->setUniformValue("light.position", v*QVector3D(5,5,-5) );
    _program->setUniformValue("material.ambient", QVector3D(0.7f,0.7f,0.7f));
    _program->setUniformValue("material.diffuse", QVector3D(1.0f,1.0f,1.0f));
    _program->setUniformValue("material.specular", QVector3D(1.0f,1.0f,1.0f));
    _program->setUniformValue("material.shininess", 24.0f);
    _program->setUniformValue("color", QVector3D(1,0,0));

    //Desenhando os triângulos que formam o cubo
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_INT, nullptr);

    update();
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

    //Criando buffers de textura
    glGenBuffers(1, &_texCoordsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,_texCoordsBuffer);
    glBufferData(GL_ARRAY_BUFFER, _texCoords.size()*sizeof(int), _texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);

    //Criando buffers de indices
    glGenBuffers(1, &_meshBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size()*sizeof(int), _indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshBuffer);


}

