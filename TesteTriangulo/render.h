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

    std::vector<QVector3D> _points;
    std::vector<QVector3D> _colors;

    QOpenGLBuffer _pointsBuffer;
    QOpenGLBuffer _colorsBuffer;
};

#endif // RENDER_H
