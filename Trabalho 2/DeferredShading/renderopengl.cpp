#include "renderopengl.h"
#include <QImage>
#include<QGLWidget>
//#include "reader.h"
#include "reader2.h"
#include "math.h"
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QMouseEvent>

RenderOpengl::RenderOpengl(QWidget* parent)
    :QOpenGLWidget(parent)
{
    cam.at = QVector3D(0.0f,0.f,0.f);
    cam.eye =  QVector3D(0.0f,0.f,6.f);
    cam.up = QVector3D(0.f,2.f,0.f);
    cam.zNear = 0.1f;
    cam.zFar  = 100.f;
    cam.fovy  = 60.f;
    cam.width = 1002;
    cam.height = 701;
    this->setFocus();

    createThings();
}

RenderOpengl::~RenderOpengl()
{
    delete _programGB;
    delete _programQuad;
    glDeleteBuffers(1, &_bitangentBuffer);
    glDeleteBuffers(1, &_meshBuffer);
    glDeleteBuffers(1, &_normalsBuffer);
    glDeleteBuffers(1, &_pointsBuffer);
    glDeleteBuffers(1, &_tangentBuffer);
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
//        readFile(fileName,_points,_normals,_texCoords,indexPointsTriangle,indexPointsQuad,indexNormalsTriangle,indexTexTriangle,indexNormalsQuads,indexTexQuads);
//        quadToTriangleMesh(indexPointsQuad, indexPointsTriangle,indexNormalsTriangle,indexTexTriangle,indexNormalsQuads,indexTexQuads);
//        organizingData();
        //createSphere();
  //      computeTangents();
        //printThings();

        readFile("../bolinha/bolinha.obj",_points, _normals,_tangents, _texCoords, _indexPoints);
}

void RenderOpengl::setMode(int mode)
{
    _mode = mode;
}

int getIndex( int i, int j, int n )
{
    return j + i * ( n + 1 );
}

//Esfera procedural usada pra teste
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
            _texCoords.push_back(QVector2D(s*S,r*R));

            ///Vertex position
            _points.push_back(QVector3D(x*raio,y*raio,z*raio));

            ///Derivada de x,y,z em relação a s - Vetor tangente
            float tangx = -sin(theta)*sin(phi)*2*M_PI*S;
            float tangy =  0;
            float tangz = cos(theta)*sin(phi)*2*M_PI*S;

            _tangents.push_back(QVector3D(tangx,tangy,tangz));
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


//Transforma malha de quads em malha de triângulos
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

//Organiza dados, duplica vértices que tem que duplicar
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
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0,0,0,1);

    createFrameBuffer();
    _programGB = new QOpenGLShaderProgram();

    //Adicionando shaders ao programa

    _programGB->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/GBvertexshader.glsl");
    _programGB->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/GBfragmentshader.glsl");

    //Linka shaders que foram adicionados ao programa
    _programGB->link();

    if (!_programGB->isLinked())
    {
        std::cout<<"Problemas ao linkar shaders de Gbuffer"<<std::endl;
    }
    //Liga o programa ao atual contexto

    setFile("../golfball/golfball.obj");

    _programGB->bind();

    createVAO();

    _programQuad = new QOpenGLShaderProgram();
    //Adicionando shaders ao programa

    _programQuad->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertexshader.glsl");
    _programQuad->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragmentshader.glsl");

    //Linka shaders que foram adicionados ao programa
    _programQuad->link();

    if (!_programQuad->isLinked())
    {
        std::cout<<"Problemas ao linkar shaders de Quad"<<std::endl;
    }
    _programQuad->bind();

    createVAO2();
    createNormalMapTexture("../golfball/golfball.png");

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

    printf("Tangentes: \n");
    for(int i = 0; i< _tangents.size(); i++)
    {
        printf("%f %f %f\n",_tangents[i].x(),_tangents[i].y(),_tangents[i].z());
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
    //PRIMEIRA PASSADA
    _programGB->bind();
    _vao.bind();
    glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);
    glViewport(0, 0, cam.width, cam.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

    QMatrix4x4 v = _view;
    QMatrix4x4 p = _proj;

    QMatrix4x4 mv = v*m;
    QMatrix4x4 mvp =p*mv;

    //Passando as variáveis uniformes para os shaders
    //model-view : Passa para espaço do olho
    _programGB->setUniformValue("mv", mv);
//    //model-view : Passa para espaço de projeção
    _programGB->setUniformValue("mvp", mvp);
//    //inversa transposta da model-view
    _programGB->setUniformValue("normalMatrix", mv.inverted().transposed());

    //Ativar e linkar a textura de normal map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _normalMap);
    GLint normalMap = glGetUniformLocation(_programQuad->programId(), "normalSampler");

    glUniform1i(normalMap,0);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indexPoints.size()), GL_UNSIGNED_INT, nullptr);

//    glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);
//    glReadBuffer(GL_COLOR_ATTACHMENT0);
//    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
//    glBlitFramebuffer(0, 0, cam.width, cam.height,
//                      0, 0, cam.width, cam.height,
//                      GL_COLOR_BUFFER_BIT, GL_NEAREST);

   // return;
    //SEGUNDA PASSADA
    //Dando bind no programa e no vao
    _programQuad->bind();
    _vao2.bind();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
   // glDrawBuffer(GL_BACK);
    glViewport(0, 0, cam.width, cam.height);
    glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);


    //Ativar e linkar a textura de tangente
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _gTangente);
    unsigned int gTangenteLocation = glGetUniformLocation(_programQuad->programId(), "gTangente");
    glUniform1i(gTangenteLocation, 0);

    //Ativar e linkar a textura de normal
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _gNormal);
    GLint gNormalLocation = glGetUniformLocation(_programQuad->programId(), "gNormal");
    glUniform1i(gNormalLocation,  1);

    //Ativar e linkar a textura de posição
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _gPosition);
    GLint gPositionLocation = glGetUniformLocation(_programQuad->programId(), "gPosition");
    glUniform1i(gPositionLocation,  2);

    //Ativar e linkar a textura de textura mapeada
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, _gTex);
    GLint gTexLocation = glGetUniformLocation(_programQuad->programId(), "gTex");
    glUniform1i(gTexLocation, 4);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, _gDepth);
    GLint gDepthLocation = glGetUniformLocation(_programQuad->programId(), "gDepth");

    glUniform1i(gDepthLocation, 5);


    //Bola
    _programQuad->setUniformValue("material.ambient", QVector3D(0.2f,0.2f,0.2f));
    _programQuad->setUniformValue("material.diffuse", QVector3D(0.8f,0.8f,0.8f));
    _programQuad->setUniformValue("material.specular", QVector3D(1.0f,1.0f,1.0f));
    _programQuad->setUniformValue("material.shininess", 100.0f);

    //Colocando luzes

    setUniformArrays(v);
    //Variáveis de material e luz
   _programQuad->setUniformValue("lightPos", v * QVector3D(1.5,1.5,8)/*QVector3D(0,0,300)*/);

   _programQuad->setUniformValue("mode",_mode);
   //printf("COORDS: %f %f %f\n",(v * cam.eye).x(),(v * cam.eye).y(),(v * cam.eye).z());
    //Desenhando os triângulos que formam o cubo
    glDrawArrays(GL_TRIANGLES, 0, (int)_pointsScreen.size());

    updateLights();
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

    //Criando buffer de tangentes
    glGenBuffers(1, &_tangentBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _tangentBuffer);
    glBufferData(GL_ARRAY_BUFFER, _tangents.size()*sizeof(QVector3D), &_tangents[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);

    //Criando buffers de textura
    glGenBuffers(1, &_texCoordsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,_texCoordsBuffer);
    glBufferData(GL_ARRAY_BUFFER, _texCoords.size()*sizeof(QVector2D), &_texCoords[0], GL_STATIC_DRAW);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(3);

    //Criando buffers de indexPoints
    glGenBuffers(1, &_meshBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexPoints.size()*sizeof(int), _indexPoints.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshBuffer);


}


void RenderOpengl::createVAO2()
{
    //Criando e configurando vao
    _vao2.create();
    _vao2.bind();

    //Criando buffer de pontos dos vértices
    glGenBuffers(1, &_pointsScreenBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _pointsScreenBuffer);
    glBufferData(GL_ARRAY_BUFFER, _pointsScreen.size()*sizeof(QVector3D), &_pointsScreen[0], GL_STATIC_DRAW);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(5);

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

    updateFrameBuffer();
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

void RenderOpengl::computeTangents()
{
    //Abordagem em funcionamento
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

            //Calculando dois vetores perpendiculares à normal (tangente e bitangente)
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

void RenderOpengl::createFrameBuffer()
{
    //Gerando GBuffer e dando bind nele
    glGenFramebuffers(1, &_gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);

    //Buffer de Tangente
    glGenTextures(1, &_gTangente);
    glBindTexture(GL_TEXTURE_2D, _gTangente);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cam.width , cam.height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gTangente, 0);

    glGenTextures(1, &_gNormal);
    glBindTexture(GL_TEXTURE_2D, _gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cam.width , cam.height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _gNormal, 0);

    // - Buffer de Posição
    glGenTextures(1, &_gPosition);
    glBindTexture(GL_TEXTURE_2D, _gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, cam.width , cam.height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _gPosition, 0);
    //Texturas adicionais para bump


    //Buffer de coordenadas de textura do bump
    glGenTextures(1, &_gTex);
    glBindTexture(GL_TEXTURE_2D, _gTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cam.width , cam.height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, _gTex, 0);


    unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0,
                                    GL_COLOR_ATTACHMENT1,
                                    GL_COLOR_ATTACHMENT2,
                                    GL_COLOR_ATTACHMENT3
                                  };
    glDrawBuffers(4, attachments);

    //Buffer de profundidade
    glGenTextures(1, &_gDepth);
    glBindTexture(GL_TEXTURE_2D, _gDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, cam.width, cam.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                  0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _gDepth, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Erro no frame buffer\n");
    }
}



void RenderOpengl::updateFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);

    glBindTexture(GL_TEXTURE_2D, _gTangente);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, cam.width , cam.height, 0, GL_RGB, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, _gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cam.width , cam.height, 0, GL_RGB, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, _gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, cam.width , cam.height, 0, GL_RGB, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, _gTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cam.width , cam.height, 0, GL_RGB, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, _gDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, cam.width, cam.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                  0);
}



void RenderOpengl::setUniformArrays(QMatrix4x4 v)
{
    //Colocando luzes
    QVector3D pos;
    pos = v*_lights[0];
    _programQuad->setUniformValue("lights[0].Position", pos);
    _programQuad->setUniformValue("lights[0].Color", QVector3D(0,0,1));
    pos = v*_lights[1];
    _programQuad->setUniformValue("lights[1].Position",pos );
    _programQuad->setUniformValue("lights[1].Color", _colors[0]);
    //pos =  QVector3D(-100,0,20);
    pos = v*_lights[2];
    _programQuad->setUniformValue("lights[2].Position", pos );
    _programQuad->setUniformValue("lights[2].Color", _colors[1]);
    //pos =  QVector3D(0,100,20);
    pos = v*_lights[3];
    _programQuad->setUniformValue("lights[3].Position", pos );
    _programQuad->setUniformValue("lights[3].Color", _colors[2]);
    //pos =  QVector3D(-100,-100,20);
    pos = v*_lights[4];
    _programQuad->setUniformValue("lights[4].Position", pos );
    _programQuad->setUniformValue("lights[4].Color", _colors[3]);
    //pos =  QVector3D(-100,100,20);
    pos = v*_lights[5];
    _programQuad->setUniformValue("lights[5].Position", pos );
    _programQuad->setUniformValue("lights[5].Color", _colors[4]);
    //pos =  QVector3D(100,-100,20);
    pos = v*_lights[6];
    _programQuad->setUniformValue("lights[6].Position", pos );
    _programQuad->setUniformValue("lights[6].Color", _colors[5]);
   // pos =  QVector3D(100,100,20);
    pos = v*_lights[7];
    _programQuad->setUniformValue("lights[7].Position", pos );
    _programQuad->setUniformValue("lights[7].Color", _colors[6]);
    pos = v*_lights[8];
    _programQuad->setUniformValue("lights[8].Position", pos );
    _programQuad->setUniformValue("lights[8].Color", _colors[7]);
    pos = v*_lights[9];
    _programQuad->setUniformValue("lights[9].Position", pos );
    _programQuad->setUniformValue("lights[9].Color", _colors[8]);
    pos = v*_lights[10];
    _programQuad->setUniformValue("lights[10].Position", pos );
    _programQuad->setUniformValue("lights[10].Color", _colors[9]);
    pos = v*_lights[11];
    _programQuad->setUniformValue("lights[11].Position", pos );
    _programQuad->setUniformValue("lights[11].Color", _colors[10]);
    pos = v*_lights[12];
    _programQuad->setUniformValue("lights[12].Position", pos );
    _programQuad->setUniformValue("lights[12].Color", _colors[11]);
    pos = v*_lights[13];
    _programQuad->setUniformValue("lights[13].Position", pos );
    _programQuad->setUniformValue("lights[13].Color", _colors[12]);
    pos = v*_lights[14];
    _programQuad->setUniformValue("lights[14].Position", pos );
    _programQuad->setUniformValue("lights[14].Color", _colors[13]);
    pos = v*_lights[15];
    _programQuad->setUniformValue("lights[15].Position", pos );
    _programQuad->setUniformValue("lights[15].Color", _colors[14]);
    pos = v*_lights[16];
    _programQuad->setUniformValue("lights[16].Position", pos );
    _programQuad->setUniformValue("lights[16].Color", _colors[15]);
    pos = v*_lights[17];
    _programQuad->setUniformValue("lights[17].Position", pos );
    _programQuad->setUniformValue("lights[17].Color", _colors[16]);
    pos = v*_lights[18];
    _programQuad->setUniformValue("lights[18].Position", pos );
    _programQuad->setUniformValue("lights[18].Color", _colors[17]);
    pos = v*_lights[19];
    _programQuad->setUniformValue("lights[19].Position", pos );
    _programQuad->setUniformValue("lights[19].Color", _colors[18]);
}

void RenderOpengl::updateLights()
{

    if(time > 5)
    {
        time = 0;
        int i;
        for(i = 0; i < 5; i++)
        {
            if( _isgoingThere[i])
            {
                _lights[i] += QVector3D(20,0,0);
                if(_lights[i].x() > 100)
                {
                    _isgoingThere[i] = false;
                }
            }
            else
            {
                _lights[i] -= QVector3D(20,0,0);
                if(_lights[i].x() < -100)
                {
                    _isgoingThere[i] = true;
                }
            }
        }
        for(; i < 10; i++)
        {
            if( _isgoingThere[i])
            {
                _lights[i] += QVector3D(10,-10,0);
                if(_lights[i].x() > 100 || _lights[i].y() < -100)
                {
                    _isgoingThere[i] = false;
                }
            }
            else
            {
                _lights[i] -= QVector3D(10,-10,0);
                if(_lights[i].x() < -100 || _lights[i].y() > 100)
                {
                    _isgoingThere[i] = true;
                }
            }
        }
        for(; i < 15; i++)
        {
            if( _isgoingThere[i])
            {
                _lights[i] += QVector3D(0,20,0);
                if(_lights[i].y() > 100)
                {
                    _isgoingThere[i] = false;
                }
            }
            else
            {
                _lights[i] -= QVector3D(0,20,0);
                if(_lights[i].y() < -100)
                {
                    _isgoingThere[i] = true;
                }
            }
        }
        for(; i < 20; i++)
        {
            if( _isgoingThere[i])
            {
                _lights[i] += QVector3D(20,20,0);
                if(_lights[i].x() > 100 || _lights[i].y() > 100)
                {
                    _isgoingThere[i] = false;
                }
            }
            else
            {
                _lights[i] -= QVector3D(20,20,0);
                if(_lights[i].x() <  -100 || _lights[i].y() < -100)
                {
                    _isgoingThere[i] = true;
                }
            }
        }
    }
    time++;
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

   //Cria buffer com cor das luzes
   _colors = {
       QVector3D(0.5f, 0.0f, 0.0f),
       QVector3D(0.0f, 0.5f, 0.0f),
       QVector3D(0.5f,  0.0f, 0.5f),
       QVector3D(1.0f,  0.0f, 0.0f),
       QVector3D(0.0f, 1.0f, 0.0f),
       QVector3D(0.0f,  0.0f, 1.0f),
       QVector3D(0.5f, 0.5f, 0.0f),
       QVector3D(0.0f, 0.5f, 0.5f),
       QVector3D(0.5f,  0.0f, 0.5f),
       QVector3D(1.0f,  1.0f, 0.0f),
       QVector3D(0.0f, 1.0f, 1.0f),
       QVector3D(1.0f,  0.0f, 1.0f),
       QVector3D(0.5f, 1.0f, 0.0f),
       QVector3D(0.5f,  0.0f, 1.0f),
       QVector3D(1.0f, 0.5f, 0.0f),
       QVector3D(1.0f, 1.0f, 0.5f),
       QVector3D(0.5f,  1.0f, 0.5f),
       QVector3D(0.0f,  0.75f, 0.0f),
       QVector3D(0.25f, 0.25f, 0.0f),
       QVector3D(0.0f,  1.0f, 0.75f),
  };

   //Cria vetor com posições iniciais das luzes
   _lights = {
       QVector3D(0.f, 0.0f, 8.0f),
       QVector3D(0.f, -15.0f, 20.0f),
       QVector3D(0.f, -45.0f, 20.0f),
       QVector3D(0.f, -75.0f, 20.0f),
       QVector3D(0.f, -105.0f, 20.0f),

       QVector3D(-30.f, 30.0f, 20.0f),
       QVector3D(-60.f, 60.0f, 20.0f),
       QVector3D(-90.f, 90.0f, 20.0f),
       QVector3D(-120.f, 120.0f, 20.0f),
       QVector3D(-150.f, 150.0f, 20.0f),

       QVector3D(30.f,0.0f, 20.0f),
       QVector3D(60.f,0.0f, 20.0f),
       QVector3D(90.f,0.0f, 20.0f),
       QVector3D(120.f,0.0f, 20.0f),
       QVector3D(150.f,0.0f, 20.0f),

       QVector3D(30.f,30.0f, 20.0f),
       QVector3D(60.f,60.0f, 20.0f),
       QVector3D(90.f,90.0f, 20.0f),
       QVector3D(120.f,120.0f, 20.0f),
       QVector3D(150.f,150.0f, 20.0f),


   };
   std::vector<bool> v(20,true);
   _isgoingThere = v;
}

