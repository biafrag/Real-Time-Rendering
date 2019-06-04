#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QWidget::setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    ui->radioButton_Result->setChecked(true);
}

MainWindow::~MainWindow()
{
    delete ui;

}

void MainWindow::on_radioButton_Result_clicked()
{
    ui->openGLWidget->setMode(0);
}

void MainWindow::on_radioButton_PositionBuffer_clicked()
{
    ui->openGLWidget->setMode(1);
}

void MainWindow::on_radioButton_NormalBuffer_clicked()
{
     ui->openGLWidget->setMode(2);
}

void MainWindow::on_radioButton_ZBuffer_clicked()
{
    ui->openGLWidget->setMode(3);
}

void MainWindow::on_radioButton_TengentBuffer_clicked()
{
    ui->openGLWidget->setMode(4);
}
