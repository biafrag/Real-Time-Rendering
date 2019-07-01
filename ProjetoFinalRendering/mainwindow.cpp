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

void MainWindow::on_spinBoxOctaves_valueChanged(int arg1)
{
    ui->openGLWidget->setOctaves(arg1);
}

void MainWindow::on_pushButton_clicked()
{
    ui->openGLWidget->changeAnimationStatus();
}
