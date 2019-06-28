#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_radioButtonClouds_clicked()
{
    ui->openGLWidget->setMode(0);
}

void MainWindow::on_radioOtherThing_clicked()
{
    ui->openGLWidget->setMode(1);
}

void MainWindow::on_radioButtonClouds3D_clicked()
{
    ui->openGLWidget->setMode(2);
}

void MainWindow::on_radioButtonFlames_clicked()
{
    ui->openGLWidget->setMode(3);
}

void MainWindow::on_radioButtonMarmore_clicked()
{
    ui->openGLWidget->setMode(4);
}

void MainWindow::on_radioButtonPontos_clicked()
{
    ui->openGLWidget->setMode(5);
}
