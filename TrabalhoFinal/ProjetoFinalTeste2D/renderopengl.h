//Usar debugger MSVC2017 32 bits
#ifndef RENDER_H
#define RENDER_H

#include <iostream>
#include <QOpenGLFunctions>
#include<QOpenGLWidget>
#include<QOpenGLShaderProgram>
#include<QOpenGLBuffer>
#include<QOpenGLVertexArrayObject>
#include <QOpenGLExtraFunctions>
//#include <QOpenGLFramebufferObject>
//#include <QOpenGLFramebufferObjectFormat>
#include<QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>


#include <glm/glm.hpp>

class RenderOpengl:
        public QOpenGLWidget
        , protected QOpenGLExtraFunctions
{
public:
    RenderOpengl(QWidget *parent);
    ~RenderOpengl();
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void setFile(std::string fileName);
    void setMode(int mode);

private:

    //Classe do qt que permite os shaders serem linkados e usados
    QOpenGLShaderProgram* _program{nullptr};
    QOpenGLShaderProgram* _program3D{nullptr};

    std::vector<QVector3D> _pointsScreen; //Vetor de cada ponto do meu objeto que será renderizado
    std::vector<QVector3D> _pointsTest;
    std::vector<QVector3D> _pointsTestFixed;

    std::vector<QVector3D> _points; //Vetor de cada ponto do meu objeto que será renderizado
    std::vector<QVector3D> _normals; //Vetor de normal pra cada vértice do meu cubo
    std::vector<int> _indexPoints; //vetor com os indices que formam os triângulos que serão renderizados
    std::vector<int> _indexNormals; //vetor com os indices que formam os triângulos que serão renderizados

    unsigned int _pointsBuffer = static_cast<unsigned int>(-1);
    unsigned int _normalsBuffer = static_cast<unsigned int>(-1);
    unsigned int _meshBuffer = static_cast<unsigned int>(-1);
    unsigned int _pointsTestBuffer;
    unsigned int _pointsFixedBuffer;
    unsigned int _pointsScreenBuffer = static_cast<unsigned int>(-1);

   struct Camera {
      QVector3D eye;      /* posicao do olho ou centro de projecao conica */
      QVector3D at;       /* ponto para onde a camera aponta              */
      QVector3D up;       /* orientacao da direcao "para cima" da camera  */
      float fovy;         /* angulo de abertura da camera                 */
      float zNear,zFar;   /* distancia do plano proximo e distante        */
      float width,height; /* largura e altura da janela em pixels         */
   };

   Camera cam;
   glm::mat4x4 _model;
   QMatrix4x4 _view;
   QMatrix4x4 _proj;

   QOpenGLVertexArrayObject _vao;
   QOpenGLVertexArrayObject _vao3D;

private:
    void createVAO(); //Cria VAO
    void createTexture(const std::string &imagePath); //Seta textura difusa
    void printThings(); //Printa coisas
    void createThings();
    void setMode();

    void createVAO3D(); //Cria VAO 3D
    void createSphere();
    void quadToTriangleMesh(std::vector<int> &indexPointsQuad, std::vector<int> &indexPointsTriangle, std::vector<int> &indexNormalsTriangles, std::vector<int> &indexTexTriangles, std::vector<int> &indexNormalsQuad, std::vector<int> &indexTexQuad); //Transforma malha de quads em malha de triângulos
    float time{0};
    //Arcball
    bool mousepress;
    QVector3D p0,p1; //pontos para fazer rotação
    //Parte do arcball
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void keyPressEvent(QKeyEvent* event);
    double radius; //Sphere Radius
    QVector3D Points_Sphere(QVector3D pointT);
    int _mode;
};

#endif // RENDER_H
