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
    ui->radioButtonGolfball->clicked(true);
}


void MainWindow::on_radioButtonGolfball_clicked()
{
    ui->openGLWidget->setFile("../golfball/golfball");
}

void MainWindow::on_radioButtonStones_clicked()
{
    ui->openGLWidget->setFile("../stones/stones");
}

