#ifndef RENDER_H
#define RENDER_H

#include <iostream>
#include <QOpenGLFunctions>
#include<QOpenGLWidget>
#include<QOpenGLShaderProgram>
#include<QOpenGLBuffer>
#include<QOpenGLVertexArrayObject>
class Render:
        public QOpenGLWidget
        , protected QOpenGLFunctions
{
public:
    Render(QWidget *parent);
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    //Classe do qt que permite os shaders serem linkados e usados
    QOpenGLShaderProgram* _program{nullptr};

    std::vector<QVector3D> _points; //Vetor de cada ponto do meu objeto que será renderizado
    std::vector<QVector3D> _normals; //Vetor de normal pra cada vértice do meu cubo
    std::vector<QVector2D> _texCoords; //Vetor de coordenadas de textura
    std::vector<int> _indexPoints; //vetor com os indices que formam os triângulos que serão renderizados
    std::vector<int> _indexNormals; //vetor com os indices que formam os triângulos que serão renderizados
    std::vector<int> _indexTex; //vetor com os indices que formam os triângulos que serão renderizados
    std::vector<QVector3D> _colors;

   unsigned int _pointsBuffer = static_cast<unsigned int>(-1);
   unsigned int _normalsBuffer = static_cast<unsigned int>(-1);
   unsigned int _meshBuffer = static_cast<unsigned int>(-1);
   unsigned int _texCoordsBuffer = static_cast<unsigned int>(-1);

   struct Camera {
      QVector3D eye;      /* posicao do olho ou centro de projecao conica */
      QVector3D at;       /* ponto para onde a camera aponta              */
      QVector3D up;       /* orientacao da direcao "para cima" da camera  */
      float fovy;         /* angulo de abertura da camera                 */
      float zNear,zFar;   /* distancia do plano proximo e distante        */
      float width,height; /* largura e altura da janela em pixels         */
   };

   Camera cam;
   QMatrix4x4 _model;
   QMatrix4x4 _view;
   QMatrix4x4 _proj;
   QOpenGLVertexArrayObject _vao;
   unsigned int _textureID;

private:
    void createVAO();
    void createTexture(const std::string &imagePath);
};

#endif // RENDER_H
