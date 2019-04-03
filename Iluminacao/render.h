#ifndef RENDER_H
#define RENDER_H

#include <iostream>
#include <QOpenGLFunctions>
#include<QOpenGLWidget>
#include<QOpenGLShaderProgram>
#include<QOpenGLBuffer>

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
    std::vector<int> _indices; //vetor com os indices que formam os triângulos que serão renderizados
    std::vector<QVector3D> _colors;

    QOpenGLBuffer _pointsBuffer;
    QOpenGLBuffer _colorsBuffer;

private:
    void createVAO();
};

#endif // RENDER_H
