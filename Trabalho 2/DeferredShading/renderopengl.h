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

    int _mode{0};
    //Classe do qt que permite os shaders serem linkados e usados
    QOpenGLShaderProgram* _programGB{nullptr};

    QOpenGLShaderProgram* _programQuad{nullptr};

    std::vector<QVector3D> _points; //Vetor de cada ponto do meu objeto que será renderizado
    std::vector<QVector3D> _normals; //Vetor de normal pra cada vértice do meu cubo
    std::vector<QVector2D> _texCoords; //Vetor de coordenadas de textura
    std::vector<int> _indexPoints; //vetor com os indices que formam os triângulos que serão renderizados
    std::vector<int> _indexNormals; //vetor com os indices que formam os triângulos que serão renderizados
    std::vector<int> _indexTex; //vetor com os indices que formam os triângulos que serão renderizados
    std::vector<QVector3D> _colors;
    std::vector<QVector3D> _tangents; // vetor de tangentes para cada vértice
    std::vector<QVector3D> _bitangents; // vetor de tangentes para cada vértice

    std::vector<QVector3D> _pointsScreen; //Vetor de cada ponto do meu objeto que será renderizado
    std::vector<QVector3D> _lights;

   unsigned int _pointsBuffer = static_cast<unsigned int>(-1);
   unsigned int _normalsBuffer = static_cast<unsigned int>(-1);
   unsigned int _meshBuffer = static_cast<unsigned int>(-1);
   unsigned int _texCoordsBuffer = static_cast<unsigned int>(-1);
   unsigned int _tangentBuffer = static_cast<unsigned int>(-1);
   unsigned int _bitangentBuffer = static_cast<unsigned int>(-1);

   unsigned int _pointsScreenBuffer = static_cast<unsigned int>(-1);

   //Parte de GBuffer
   unsigned int _gBuffer; //Só para fins de curiosidade
   unsigned int _gPosition;
   unsigned int _gNormal;
   unsigned int _gDepth;

   //Buffers adicionais para bump
   unsigned int _gTangente;
   unsigned int _gTex; // Textura já mapeada
   unsigned int _gLight; //Luz no espaço tangente
   unsigned int _gTanViewer; // observador no espaço tangente



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
   QOpenGLVertexArrayObject _vao2;
   unsigned int _textureID;
   unsigned int _normalMap;

private:
    void createVAO(); //Cria VAO
    void createVAO2(); //Cria VAO2
    void createTexture(const std::string &imagePath); //Seta textura difusa
    void createNormalMapTexture(const std::string& imagePath); //Seta Textura de normal
    void quadToTriangleMesh(std::vector<int> &indexPointsQuad, std::vector<int> &indexPointsTriangle, std::vector<int> &indexNormalsTriangles, std::vector<int> &indexTexTriangles, std::vector<int> &indexNormalsQuad, std::vector<int> &indexTexQuad); //Transforma malha de quads em malha de triângulos
    void printThings(); //Printa coisas
    void organizingData(); //Duplica vertices se necessário
    void computeTangents(); //Calcula tangentes e binormais para cada ponto
    void createFrameBuffer();
    void createScreenQuad();
    void updateFrameBuffer();

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
    void createSphere();
};

#endif // RENDER_H
