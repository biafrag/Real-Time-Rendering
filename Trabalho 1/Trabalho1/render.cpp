
#include "render.h"
#include <QImage>
#include<QGLWidget>
#include "reader.h"
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <QMouseEvent>

Render::Render(QWidget* parent)
    :QOpenGLWidget(parent)
{
    cam.at = QVector3D(0.5f,0.5f,0.f);
    cam.eye =  QVector3D(0.f,0.0f,90.f);
    cam.up = QVector3D(0.f,1.f,0.f);
    cam.zNear = 0.1f;
    cam.zFar  = 1000.f;
    cam.fovy  = 45.f;
    cam.width = width();
    cam.height = height();
    this->setFocus();
}

void Render::setFile(std::string fileName)
{
    std::vector<int> indexPointsQuad;
    std::vector<int> indexPointsTriangle;
    readFile(fileName,_points,_normals,_texCoords,indexPointsTriangle,indexPointsQuad,_indexNormals,_indexTex);
    quadToTriangleMesh(indexPointsQuad, indexPointsTriangle);
    computeTangent();
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
    _program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertexshader.glsl");
    _program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragmentshader.glsl");

    //Linka shaders que foram adicionados ao programa
    _program->link();

    if (!_program->isLinked())
    {
        std::cout<<"Problemas ao linkar shaders"<<std::endl;
    }
    //Liga o programa ao atual contexto
   setFile("../stones/stones.obj");

    _program->bind();

    createVAO();

    //Criando textura difusa e normal map
     createTexture("../stones/stones.jpg");
    createNormalMapTexture("../stones/stones_norm.jpg");

    GLint textureLocation = glGetUniformLocation(_program->programId(), "sampler");
    glUniform1i(textureLocation, 0);

    GLint normalMap = glGetUniformLocation(_program->programId(), "normalsampler");
    glUniform1i(normalMap,  1);

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
    //Atualizar a viewport
    glViewport(0,0,w,h);

    //Atualizar a câmera
    cam.width = w;
    cam.height = h;
    glm::vec3 zero(0);
    radius=((glm::min(h,w)/2.0)-1);
}



void Render::paintGL()
{
        glEnable(GL_DEPTH_TEST);
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
             _model[3][0],_model[3][1],_model[3][2],_model[3][3]);
     //QMatrix4x4 m = _model;
     QMatrix4x4 v = _view;
     QMatrix4x4 p = _proj;

     QMatrix4x4 mv = v*m;
     QMatrix4x4 mvp =p*mv;

    //Ativar e linkar a textura
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureID);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _normalMap);

    //Passando as variáveis uniformes para os shaders
    //model-view : Passa para espaço do olho
    _program->setUniformValue("mv", mv);
    //model-view : Passa para espaço de projeção
    _program->setUniformValue("mvp", mvp);
    //inversa transposta da model-view
    _program->setUniformValue("normalMatrix", mv.inverted().transposed());
    //Variáveis de material e luz
    _program->setUniformValue("lightPos", v * cam.eye);

    //2DImage
        _program->setUniformValue("material.ambient", QVector3D(0.1f,0.1f,0.1f));
        _program->setUniformValue("material.diffuse", QVector3D(0.9f,0.9f,0.9f));
        _program->setUniformValue("material.specular", QVector3D(0.3f,0.3f,0.3f));
        _program->setUniformValue("material.shininess", 100.0f);
        _program->setUniformValue("hasDT", true);

    //Desenhando os triângulos que formam o cubo
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indexPoints.size()), GL_UNSIGNED_INT, nullptr);
    update();
}

void Render::computeTangent()
{
    std::vector<QVector3D> tanA;
    std::vector<QVector3D> tanB;
    tanA.resize(_points.size());
    tanB.resize(_points.size());
    for(int i = 0; i< tanA.size();i++)
    {
        tanA[i] = QVector3D(0,0,0);
        tanB[i] = QVector3D(0,0,0);
    }

    _tangents.resize(_points.size());
    for (size_t i = 0; i < _indexPoints.size()/3; i++)
    {
            size_t i0 = _indexPoints[3*i];
            size_t i1 = _indexPoints[3*i + 1];
            size_t i2 = _indexPoints[3*i + 2];

            QVector3D pos0 = _points[i0];
            QVector3D pos1 = _points[i1];
            QVector3D pos2 = _points[i2];

            QVector2D tex0 = _texCoords[i0];
            QVector2D tex1 = _texCoords[i1];
            QVector2D tex2 = _texCoords[i2];

            QVector3D edge1 = pos1 - pos0;
            QVector3D edge2 = pos2 - pos0;

            QVector2D uv1 = tex1 - tex0;
            QVector2D uv2 = tex2 - tex0;

            float r = 1.0f / (uv1.x() * uv2.y() - uv1.y() * uv2.x());

            QVector3D tangent(
                ((edge1.x() * uv2.y()) - (edge2.x() * uv1.y())) * r,
                ((edge1.y() * uv2.y()) - (edge2.y() * uv1.y())) * r,
                ((edge1.z() * uv2.y()) - (edge2.z() * uv1.y())) * r
            );

            QVector3D bitangent(
                ((edge1.x() * uv2.x()) - (edge2.x() * uv1.x())) * r,
                ((edge1.y() * uv2.x()) - (edge2.y() * uv1.x())) * r,
                ((edge1.z() * uv2.x()) - (edge2.z() * uv1.x())) * r
            );

            tanA[i0] += tangent;
            tanA[i1] += tangent;
            tanA[i2] += tangent;

            tanB[i0] += bitangent;
            tanB[i1] += bitangent;
            tanB[i2] += bitangent;
        }

        // (2)
        for (size_t i = 0; i < _points.size(); i++)
        {
            QVector3D n = _normals[i];
            QVector3D t0 = tanA[i];
            QVector3D t1 = tanB[i];

            QVector3D t = t0 - (n * QVector3D::dotProduct(n, t0));
            t.normalize();
            _tangents[i] = QVector3D(t.x(), t.y(), t.z());
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

//ArcBall
void Render::mousePressEvent(QMouseEvent *event)
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
          cam.eye = QVector3D(0.f,0.f,90.f);
          _model= glm::mat4x4(1.f);
    }

    update();
}


void Render::mouseReleaseEvent(QMouseEvent *event)
{
    mousepress=false;
    update();
}

QVector3D Render::Points_Sphere(QVector3D pointT)
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

void Render::mouseMoveEvent(QMouseEvent *event)
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

//Outra forma que mexe a camera
void Render::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_A)
    {
        cam.eye.setZ(cam.eye.z() - 20);
    }
    else if (event->key() == Qt::Key_Z)
    {
         cam.eye.setZ(cam.eye.z() + 20);
    }
    else if (event->key() == Qt::Key_S)
    {
        cam.eye.setX(cam.eye.x() + 20);
    }
    else if(event->key() == Qt::Key_X)
    {
        cam.eye.setX(cam.eye.x() - 20);
    }
    else if(event->key() == Qt::Key_Y)
    {
        cam.eye.setY(cam.eye.y() + 20);
    }
    else if (event->key() == Qt::Key_H)
    {
        cam.eye.setY(cam.eye.y() - 20);
    }
    else if (event->key() == Qt::Key_F)
    {
        cam.eye = QVector3D(0.f,0.f,90.f);
    }
    update();
}


