#include "renderopengl.h"
#include <QImage>
#include<QGLWidget>
#include "reader.h"
#include "math.h"
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QMouseEvent>


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
    uniform vec3 lightPos;

    //Posição, normal e luz no espaço da câmera:
    out vec3 fragPos;
    out vec3 fragNormal;

    //Coordenadas de textura do fragmento
    out vec2 fragUV;
    out vec3 light;

    void main()
    {
        gl_Position = mvp * vec4( vertexPos, 1 );
        fragPos = ( mv * vec4( vertexPos, 1 ) ).xyz;

        fragNormal = normalize(( normalMatrix * vec4( vertexNormal, 0 ) ).xyz);

        fragUV = vertexTexCoord ;
        light = normalize(lightPos - fragPos);
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


uniform Material material;
uniform vec3 color;

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragUV;
in vec3 light;

uniform sampler2D sampler;
uniform sampler2D normalsampler;
out vec3 finalColor;

vec3 expand(vec3 v)
{
   return (v - 0.5) * 2;
}

void main()
{
    vec3 ambient = material.ambient * vec3(1,1,1);//texture(sampler, fragUV).rgb; // * light.ambient;
    vec3 diffuse = vec3(0.0,0.0,0.0);
    vec3 specular = vec3(0.0,0.0,0.0);

    vec3 N = /*normalize(fragNormal); /**/expand(texture(normalsampler,fragUV).rgb);
    vec3 L = normalize(light);

    float iDif = dot(L,N);

    if( iDif > 0 )
    {
        diffuse = iDif * material.diffuse * vec3(1,1,1);//texture(sampler, fragUV).rgb; // * light.diffuse;

        vec3 V = normalize(-fragPos);
        vec3 H = normalize(L + V);

        float iSpec = pow(max(dot(N,H),0.0),material.shininess);
        specular = iSpec * material.specular; // * light.specular;
    }

    finalColor = ambient + diffuse + specular;
}
)";

RenderOpengl::RenderOpengl(QWidget* parent)
    :QOpenGLWidget(parent)
{
    cam.at = QVector3D(0.f,1.f,0.f);
    cam.eye =  QVector3D(0.f,200.f,200.f);
    cam.up = QVector3D(0.f,2.f,0.f);
    cam.zNear = 0.1f;
    cam.zFar  = 1000.f;
    cam.fovy  = 60.f;
    cam.width = width();
    cam.height = height();
}

void RenderOpengl::setFile(std::string fileName)
{
    std::vector<int> indexPointsQuad;
    std::vector<int> indexPointsTriangle;
    std::vector<int> indexNormalsTriangle;
    std::vector<int> indexNormalsQuads;
    std::vector<int> indexTexTriangle;
    std::vector<int> indexTexQuads;
    _points.clear();
    _normals.clear();
    _texCoords.clear();
    _indexPoints.clear();
    _indexNormals.clear();
    _indexTex.clear();
    readFile(fileName,_points,_normals,_texCoords,indexPointsTriangle,indexPointsQuad,indexNormalsTriangle,indexTexTriangle,indexNormalsQuads,indexTexQuads);
    //readFile(fileName,_points,_normals,_texCoords,_indexPoints,indexPointsQuad,_indexNormals,_indexTex,indexNormalsQuads,indexTexQuads);
    quadToTriangleMesh(indexPointsQuad, indexPointsTriangle,indexNormalsTriangle,indexTexTriangle,indexNormalsQuads,indexTexQuads);
    organizingData();
    _program->bind();
    createVAO();

}


void RenderOpengl::quadToTriangleMesh(std::vector<int>& indexPointsQuad, std::vector<int>& indexPointsTriangle, std::vector<int>& indexNormalsTriangle, std::vector<int>& indexTexTriangle, std::vector<int>& indexNormalsQuad,std::vector<int>& indexTexQuad)
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

        unsigned int n0 = indexNormalsQuad[4 * i];
        unsigned int n1 = indexNormalsQuad[4 * i + 1];
        unsigned int n2 = indexNormalsQuad[4 * i + 2];
        unsigned int n3 = indexNormalsQuad[4 * i + 3];

        unsigned int t0 = indexTexQuad[4 * i];
        unsigned int t1 = indexTexQuad[4 * i + 1];
        unsigned int t2 = indexTexQuad[4 * i + 2];
        unsigned int t3 = indexTexQuad[4 * i + 3];

//        //First triangle from quadrilateral element
//        _indexPoints.push_back(v0);
//        _indexPoints.push_back(v1);
//        _indexPoints.push_back(v3);

//        //Second triangle from quadrilateral element
//        _indexPoints.push_back(v2);
//        _indexPoints.push_back(v3);
//        _indexPoints.push_back(v1);

        //First triangle from quadrilateral element
        _indexPoints.push_back(v0);
        _indexPoints.push_back(v1);
        _indexPoints.push_back(v3);

        //Second triangle from quadrilateral element
        _indexPoints.push_back(v2);
        _indexPoints.push_back(v3);
        _indexPoints.push_back(v1);

        //First triangle from quadrilateral element
        _indexNormals.push_back(n0);
        _indexNormals.push_back(n1);
        _indexNormals.push_back(n3);

        //Second triangle from quadrilateral element
        _indexNormals.push_back(n2);
        _indexNormals.push_back(n3);
        _indexNormals.push_back(n1);

        //First triangle from quadrilateral element
        _indexTex.push_back(t0);
        _indexTex.push_back(t1);
        _indexTex.push_back(t3);

        //Second triangle from quadrilateral element
        _indexTex.push_back(t2);
        _indexTex.push_back(t3);
        _indexTex.push_back(t1);

    }

    for(int i = 0; i<indexPointsTriangle.size(); i++)
    {
        _indexPoints.push_back(indexPointsTriangle[i]);
        _indexNormals.push_back(indexNormalsTriangle[i]);
        _indexTex.push_back(indexTexTriangle[i]);
    }
}

void RenderOpengl::organizingData()
{
    std::vector<QVector3D> points; //Vetor de cada ponto do meu objeto que será renderizado
    std::vector<QVector3D> normals; //Vetor de normal pra cada vértice do meu cubo
    std::vector<QVector2D> texCoords; //Vetor de coordenadas de textura
    std::vector<int> indexes; //Vetor de cada ponto do meu objeto que será renderizado

    for(int i = 0; i<_indexPoints.size(); i++)
    {
        int idP = _indexPoints[i];
        int idN = _indexNormals[i];
        int idT = _indexTex[i];

        points.push_back(_points[idP]);
        normals.push_back(_normals[idN]);
        texCoords.push_back(_texCoords[idT]);
        indexes.push_back(i);
    }

    _points = points;
    _normals = normals;
    _texCoords = texCoords;
    _indexPoints = indexes;

}

void RenderOpengl::initializeGL()
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
     //setFile("../sphere/sphere.obj");
    setFile("../golfball/golfball.obj");
    //setFile("../earth.obj");
   //setFile("../stones/stones.obj");

    _program->bind();

    createVAO();

    //Criando textura para colocar no meu cubo
    createTexture("../golfball/golfball.png");
    //createTexture("../stones/stones.jpg");

    //createNormalMapTexture("../stones/stones_norm.jpg");

    printThings();
}
void RenderOpengl::printThings()
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


void RenderOpengl::createTexture(const std::string& imagePath)
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




void RenderOpengl::createNormalMapTexture(const std::string& imagePath)
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


void RenderOpengl::paintGL()
{

    //Dando bind no programa e no vao
    _program->bind();
    _vao.bind();

    //Definindo matriz view e projection
     _view.setToIdentity();
     _view.lookAt(cam.eye, cam.at, cam.up);
     _proj.setToIdentity();
     _proj.perspective((cam.fovy*3.14)/180, (float)cam.width/cam.height, cam.zNear, cam.zFar);
     //_model.setToIdentity();

    //Definindo matrizes para passar para os shaders

    QMatrix4x4 m = QMatrix4x4(_model[0][0],_model[0][1],_model[0][2],_model[0][3],
            _model[1][0],_model[1][1],_model[1][2],_model[1][3],
            _model[2][0],_model[2][1],_model[2][2],_model[2][3],
            _model[3][0],_model[3][1],_model[3][2],_model[3][3]);;
    QMatrix4x4 v = _view;
    QMatrix4x4 p = _proj;

    QMatrix4x4 mv = v*m;
    QMatrix4x4 mvp =p*mv;

    //Ativar e linkar a textura
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureID);
    _program->setUniformValue("sampler", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _normalMap);
    _program->setUniformValue("normalsampler", 0);

    //Passando as variáveis uniformes para os shaders
    //model-view : Passa para espaço do olho
    _program->setUniformValue("mv", mv);
    //model-view : Passa para espaço de projeção
    _program->setUniformValue("mvp", mvp);
    //inversa transposta da model-view
    _program->setUniformValue("normalMatrix", mv.inverted().transposed());
    //Variáveis de material e luz
    _program->setUniformValue("lightPos", v*QVector3D(5,5,-5));
    //Imagem 2D
//    _program->setUniformValue("material.ambient", QVector3D(0.1f,0.1f,0.1f));
//    _program->setUniformValue("material.diffuse", QVector3D(1.0f,1.0f,1.0f));
//    _program->setUniformValue("material.specular", QVector3D(0.3f,0.3f,0.3f));
//    _program->setUniformValue("material.shininess", 100.0f);
    //Bola
        _program->setUniformValue("material.ambient", QVector3D(0.2f,0.2f,0.2f));
        _program->setUniformValue("material.diffuse", QVector3D(0.8f,0.8f,0.8f));
        _program->setUniformValue("material.specular", QVector3D(1.0f,1.0f,1.0f));
        _program->setUniformValue("material.shininess", 100.0f);
        _program->setUniformValue("color", QVector3D(1.0,1,1));

    //Desenhando os triângulos que formam o cubo
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indexPoints.size()), GL_UNSIGNED_INT, nullptr);
    update();
}



void RenderOpengl::createVAO()
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
    glBufferData(GL_ARRAY_BUFFER, _texCoords.size()*sizeof(QVector2D), _texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);

    //Criando buffers de indexPoints
    glGenBuffers(1, &_meshBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexPoints.size()*sizeof(int), _indexPoints.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshBuffer);


}

void RenderOpengl::resizeGL(int w, int h)
{
    //Atualizar a viewport
    glViewport(0,0,w,h);

    //Atualizar a câmera
    cam.width = w;
    cam.height = h;
    glm::vec3 zero(0);
    radius=((glm::min(h,w)/2.0)-1);
}

QVector3D RenderOpengl::Points_Sphere(QVector3D pointT)
{
    QVector3D pointf;
    double r,s;
    pointf.setX((pointT.x()-(cam.width/2))/radius);
    pointf.setY((pointT.y()-(cam.height/2))/radius);
    r=pointf.x()*pointf.x()+pointf.y()*pointf.y();

    if(r>1.0)
    {
        s=1.0/sqrt(r);
        pointf.setX(s*pointf.x());
        pointf.setY(s*pointf.y());
        pointf.setZ(0);
    }
    else
    {
        pointf.setZ(sqrt(1.0-r));
    }
    return pointf;

}

void RenderOpengl::mousePressEvent(QMouseEvent *event)
{
    if(mousepress==false && event->button() == Qt::LeftButton)
    {
        mousepress=true;
       QVector3D point( event->x(), height()-event->y(), 0 ); // Pegando o ponto que está na tela
       point.setZ(0.f);
       p0=Points_Sphere(point);


    }
    //Fit
    if(event->button() == Qt::MiddleButton)
    {
          cam.eye = QVector3D(0.f,0.f,20.f);
          _model= glm::mat4x4(1.f);
    }

    update();
}


void RenderOpengl::mouseReleaseEvent(QMouseEvent *event)
{
    mousepress=false;
    update();
}


void RenderOpengl::mouseMoveEvent(QMouseEvent *event)
{
    if(mousepress==true)
    {

            QVector3D point( event->x(), height()-event->y(), 0 ); // Pegando o ponto que está na tela
            point.setZ(0.f);
            p1=Points_Sphere(point);
            glm::vec3 pt0(p0.x(),p0.y(),p0.z());
            glm::vec3 pt1(p1.x(),p1.y(),p1.z());
            glm::dquat Q0(0,pt0);
            glm::dquat Q1(0,pt1);
            glm::dquat Qrot=Q1*glm::conjugate(Q0);
            glm::mat4x4 Matrot;
            Matrot=glm::mat4_cast(Qrot);
           _model=Matrot*_model;

           p0=p1;
    }
    update();
}


void RenderOpengl::wheelEvent(QWheelEvent *event)
{
}

