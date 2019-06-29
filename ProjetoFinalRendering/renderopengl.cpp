#include "renderopengl.h"
#include <QImage>
#include<QGLWidget>
#include "reader.h"
#include "math.h"
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QMouseEvent>

RenderOpengl::RenderOpengl(QWidget* parent)
    :QOpenGLWidget(parent)
{
    cam.at = QVector3D(0.0f,0.f,0.f);
    cam.eye =  QVector3D(0.0f,0.f,2.5);
    cam.up = QVector3D(0.f,2.f,0.f);
    cam.zNear = 0.1f;
    cam.zFar  = 100.f;
    cam.fovy  = 30.f;
    cam.width = 1002;
    cam.height = 701;
    this->setFocus();

    createThings();
    _mode = 0;
    createSphere();
}

RenderOpengl::~RenderOpengl()
{
    delete _program;
}


void RenderOpengl::initializeGL()
{
    initializeOpenGLFunctions();

    makeCurrent();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0,0,0,1);
    glPointSize(5);
    glLineWidth(3);
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

    _program->bind();

    createVAO();
    createTexture("../ProjetoFinalRendering/ocean.jpg");

}
void RenderOpengl::printThings()
{
//    printf("Points: \n");
//    for(int i = 0; i< _points.size(); i ++)
//    {
//        printf( "%f %f %f\n",_points[i].x(),_points[i].y(),_points[i].z());
//    }


//    printf("Normals: \n");
//    for(int i = 0; i< _normals.size(); i ++)
//    {
//        printf( "%f %f %f\n",_normals[i].x(),_normals[i].y(),_normals[i].z());
//    }


//    printf("Textura: \n");
//    for(int i = 0; i< _texCoords.size(); i ++)
//    {
//        printf( "%f %f \n",_texCoords[i].x(),_texCoords[i].y());
//    }


//    printf("Indices: \n");
//    for(int i = 0; i< _indexPoints.size(); i ++)
//    {
//        printf( "%d ",_indexPoints[i]);
//    }

//    printf("Tangentes: \n");
//    for(int i = 0; i< _tangents.size(); i++)
//    {
//        printf("%f %f %f\n",_tangents[i].x(),_tangents[i].y(),_tangents[i].z());
//    }
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
     _proj.perspective(cam.fovy, (float)cam.width/cam.height, cam.zNear, cam.zFar);

    //Definindo matrizes para passar para os shaders

    QMatrix4x4 m = QMatrix4x4(_model[0][0],_model[0][1],_model[0][2],_model[0][3],
            _model[1][0],_model[1][1],_model[1][2],_model[1][3],
            _model[2][0],_model[2][1],_model[2][2],_model[2][3],
            _model[3][0],_model[3][1],_model[3][2],_model[3][3]);

    QMatrix4x4 rot;
    rot.rotate(angle,QVector3D(1,0,0));
    m = rot*m;
    QMatrix4x4 trans;
    trans.translate(0,0.25,0);
    m = trans*m;
    QMatrix4x4 v = _view;
    QMatrix4x4 p = _proj;

    QMatrix4x4 mv = v*m;
    QMatrix4x4 mvp = p*mv;

    //Bola
    _program->setUniformValue("material.ambient", QVector3D(0.2f,0.2f,0.2f));
    _program->setUniformValue("material.diffuse", QVector3D(0.8f,0.8f,0.8f));
    _program->setUniformValue("material.specular", QVector3D(1.0f,1.0f,1.0f));
    _program->setUniformValue("material.shininess", 100.0f);

    QVector3D posLight = rot * QVector3D(cam.eye.x(),cam.eye.y(),cam.eye.z() + 1);
    //Passando as variáveis uniformes para os shaders
    _program->setUniformValue("mv", mv);
    _program->setUniformValue("mvp", mvp);
    _program->setUniformValue("normalMatrix", mv.inverted().transposed());
    //Variáveis de material e luz
    _program->setUniformValue("light",posLight /*v * cam.eye /*v*QVector3D(10,10,2)*/);
    _program->setUniformValue("mode",_mode);
    _program->setUniformValue("time",time);

    //Ativar e linkar a textura de tangente
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureID);
    unsigned int oceanTextureLocation = glGetUniformLocation(_program->programId(), "oceanTex");
    glUniform1i(oceanTextureLocation, 0);

    _program->setUniformValue("normal",QVector3D(0,0,1));
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    //Desenhando os triângulos que formam o cubo
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indexGrid.size()), GL_UNSIGNED_INT, nullptr);

    update();
    time += 0.01;
    if(time == 150)
    {
        time = 0;
    }
}
void RenderOpengl::createNormalsGrid()
{
    _normalsTest.resize(_pointsTest.size(),QVector3D(0,0,1));
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


    glGenBuffers(1, &_normalsBuffer);
    //Criando buffer de normais
    glBindBuffer(GL_ARRAY_BUFFER, _normalsBuffer);
    glBufferData(GL_ARRAY_BUFFER, _normalsTest.size()*sizeof(QVector3D), &_normalsTest[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    //Criando buffers de indexPoints
//    glGenBuffers(1, &_meshBuffer);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshBuffer);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexPoints.size()*sizeof(int), _indexPoints.data(), GL_STATIC_DRAW);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshBuffer);

    glGenBuffers(1, &_meshBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexGrid.size()*sizeof(int), _indexGrid.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshBuffer);

    //Criando buffer de pontos dos vértices
    glGenBuffers(1, &_pointsScreenBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _pointsScreenBuffer);
    glBufferData(GL_ARRAY_BUFFER, _pointsScreen.size()*sizeof(QVector3D), &_pointsScreen[0], GL_STATIC_DRAW);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(5);

    //Criando buffer de pontos dos vértices
    glGenBuffers(1, &_pointsTestBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _pointsTestBuffer);
    glBufferData(GL_ARRAY_BUFFER, _pointsTest.size()*sizeof(QVector3D), &_pointsTest[0], GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(3);

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

void RenderOpengl::keyPressEvent(QKeyEvent* event)
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
        cam.eye = QVector3D(0.f,0.f,300.f);
    }
    update();
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
          cam.eye = QVector3D(0.f,20.f,20.f);
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

void RenderOpengl::createThings()
{
    //Criando pontos do quad da tela
   _pointsScreen = {
        QVector3D(1.0f, -1.0f, 0.0f),
         QVector3D(-1.0f, -1.0f, 0.0f),
         QVector3D(-1.0f,  1.0f, 0.0f),
         QVector3D(-1.0f,  1.0f, 0.0f),
         QVector3D(1.0f, -1.0f, 0.0f),
         QVector3D(1.0f,  1.0f, 0.0f),
   };
    float x = -1;
    float y = 1;
    int cont = 0;
    while(y >= -1)
    {
        while(x <= 1)
        {
            QVector3D point(x,y,0);
            //QMatrix4x4 rotationMatrix;
            //rotationMatrix.rotate(120,QVector3D(1,0,0));
            //point = rotationMatrix*point;
            _pointsTest.push_back(point);
            x += 0.1;
            cont++;
        }

        x = -1;
        y -= 0.1;
    }
    makeTriangleMesh();

}

unsigned int GetSubgridIndex(int i, int j, int width)
{
    return (unsigned int) i*width + j;
}

void RenderOpengl::makeTriangleMesh()
{
    int numCols = 20;
    int numRows = 20;
    _indexGrid.clear();
    for(unsigned int i = 0; i < numRows - 1 ; i++)
    {
        for(unsigned int j = 0; j < numCols - 1; j++)
        {

            _indexGrid.push_back(GetSubgridIndex(i,j,numRows));
            _indexGrid.push_back(GetSubgridIndex(i + 1,j,numRows));
            _indexGrid.push_back(GetSubgridIndex(i + 1,j + 1,numRows));

            _indexGrid.push_back(GetSubgridIndex(i + 1,j + 1,numRows));
            _indexGrid.push_back(GetSubgridIndex(i,j + 1,numRows));
            _indexGrid.push_back(GetSubgridIndex(i,j,numRows));
        }

    }
    createNormalsGrid();

}

void RenderOpengl::quadToTriangleMesh(std::vector<int> &indexPointsQuad, std::vector<int> &indexPointsTriangle, std::vector<int> &indexNormalsTriangles, std::vector<int> &indexTexTriangles, std::vector<int> &indexNormalsQuad, std::vector<int> &indexTexQuad)
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
//        _indexTex.push_back(t0);
//        _indexTex.push_back(t1);
//        _indexTex.push_back(t3);

        //Second triangle from quadrilateral element
//        _indexTex.push_back(t2);
//        _indexTex.push_back(t3);
//        _indexTex.push_back(t1);

    }

    for(int i = 0; i<indexPointsTriangle.size(); i++)
    {
        _indexPoints.push_back(indexPointsTriangle[i]);
        //_indexNormals.push_back(indexNormalsTriangle[i]);
        //_indexTex.push_back(indexTexTriangle[i]);
    }
}

void RenderOpengl::createSphere()
{
    //Outra esfera

        std::vector<int> indices;

        float raio = 0.5;
        unsigned int rings = 50;
        unsigned int sectors = 50;

        float const R = 1/(float)(rings-1); // 0 a rings
        float const S = 1/(float)(sectors-1); // 0 a sectors
        unsigned r, s;

        for(r = 0; r < rings; r++)
            for(s = 0; s < sectors; s++) {

                float phi = M_PI * r * R;
                float theta = 2 * M_PI * s * S;

                float const x = cos( theta ) * sin( phi );
                float const y = cos( phi );
                float const z = sin( theta ) * sin ( phi );

                //Textura s,t
                //_texCoords.push_back(QVector2D(s*S,r*R));

                ///Vertex position
                _points.push_back(QVector3D(x*raio,y*raio,z*raio));

                ///Derivada de x,y,z em relação a s - Vetor tangente
                float tangx = -sin(theta)*sin(phi)*2*M_PI*S;
                float tangy =  0;
                float tangz = cos(theta)*sin(phi)*2*M_PI*S;

                //_tangents.push_back(QVector3D(tangx,tangy,tangz));
                ///Vetor Normal
            }
        _normals = _points;

        unsigned int k = 0;
        indices.resize(rings * sectors * 4);
        int i = 0;
        for(r = 0; r < rings-1; r++) for(s = 0; s < sectors-1; s++)
        {
            indices[i] = (r * sectors + s);
            indices[i+1] =  r * sectors + (s+1);
            indices[i+2] = (r+1) * sectors + (s+1);
            indices[i+3] = (r+1) * sectors + s;
            i = i+4;
        }
        std::vector<int> indexPointsTriangle;
        std::vector<int> indexNormalsTriangle;
        std::vector<int> indexTexTriangle;

        quadToTriangleMesh(indices, indexPointsTriangle,indexNormalsTriangle,indexTexTriangle,indices,indices);
}

void RenderOpengl::setMode(int mode)
{
    _mode = mode;
}
