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
    void on_radioButton_Result_clicked();

    void on_radioButton_PositionBuffer_clicked();

    void on_radioButton_NormalBuffer_clicked();

    void on_radioButton_ZBuffer_clicked();

    void on_radioButton_TengentBuffer_clicked();

    void on_radioButton_Phong_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
