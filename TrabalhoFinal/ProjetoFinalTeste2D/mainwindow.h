#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_radioButtonClouds_clicked();

    void on_radioOtherThing_clicked();

    void on_radioButtonClouds3D_clicked();

    void on_radioButtonFlames_clicked();

    void on_radioButtonMarmore_clicked();

    void on_radioButtonPontos_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
