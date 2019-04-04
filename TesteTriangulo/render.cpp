#include "render.h"

const char* vertexShaderSource = R"(
    #version 330 core

    layout( location = 0 ) in vec3 vertexPos;
    layout( location = 1 )in vec3 icolor;
    out vec3 ocolor;
    void main()
    {
        gl_Position = vec4( vertexPos, 1 );
        ocolor = icolor;
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core

    in vec3 ocolor;
    //uniform vec3 color;
    out vec3 finalColor;

    void main()
    {
        finalColor = ocolor;
    }
)";

Render::Render(QWidget* parent)
    :QOpenGLWidget(parent)
{
    _points.push_back(QVector3D(-0.5,0,0));
    _points.push_back(QVector3D(0,0.5,0));
    _points.push_back(QVector3D(0.5,0,0));

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
    //glPointSize(8.0f);

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

    _pointsBuffer.create();
    _colorsBuffer.create();
    std::vector<QVector3D> color({QVector3D(1,0,0),QVector3D(0,1,0),QVector3D(0,0,1)});
    _colors = color;

}


void Render::resizeGL(int w, int h)
{

}


void Render::paintGL()
{
    _program->bind();
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    _pointsBuffer.bind();
    _pointsBuffer.allocate( &_points[0], (int)_points.size()*sizeof(QVector3D) );
    //Permite usar locatio que esta no parametro
    _program->enableAttributeArray(0);
    _program->setAttributeBuffer(0,GL_FLOAT,0,3,sizeof(QVector3D));

    _colorsBuffer.bind();
     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    _colorsBuffer.allocate( &_colors[0], (int)_colors.size()*sizeof(QVector3D) );
    _program->enableAttributeArray(1);
    _program->setAttributeBuffer(1,GL_FLOAT,0,3,sizeof(QVector3D));

    //_program->setUniformValue("color", QVector3D(0,1,0));
    //_program->setUniformValue("color",color);

    glDrawArrays(GL_TRIANGLES, 0, (int)_points.size());

    update();
}
