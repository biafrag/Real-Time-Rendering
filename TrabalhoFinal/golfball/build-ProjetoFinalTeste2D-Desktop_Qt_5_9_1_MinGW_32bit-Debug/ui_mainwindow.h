/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include <renderopengl.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    RenderOpengl *openGLWidget;
    QHBoxLayout *horizontalLayout;
    QRadioButton *radioButtonClouds;
    QRadioButton *radioOtherThing;
    QRadioButton *radioButtonClouds3D;
    QRadioButton *radioButtonFlames;
    QRadioButton *radioButtonMarmore;
    QRadioButton *radioButtonPontos;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(972, 681);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        openGLWidget = new RenderOpengl(centralWidget);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));

        gridLayout->addWidget(openGLWidget, 1, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        radioButtonClouds = new QRadioButton(centralWidget);
        radioButtonClouds->setObjectName(QStringLiteral("radioButtonClouds"));
        radioButtonClouds->setChecked(true);

        horizontalLayout->addWidget(radioButtonClouds);

        radioOtherThing = new QRadioButton(centralWidget);
        radioOtherThing->setObjectName(QStringLiteral("radioOtherThing"));

        horizontalLayout->addWidget(radioOtherThing);

        radioButtonClouds3D = new QRadioButton(centralWidget);
        radioButtonClouds3D->setObjectName(QStringLiteral("radioButtonClouds3D"));

        horizontalLayout->addWidget(radioButtonClouds3D);

        radioButtonFlames = new QRadioButton(centralWidget);
        radioButtonFlames->setObjectName(QStringLiteral("radioButtonFlames"));

        horizontalLayout->addWidget(radioButtonFlames);

        radioButtonMarmore = new QRadioButton(centralWidget);
        radioButtonMarmore->setObjectName(QStringLiteral("radioButtonMarmore"));

        horizontalLayout->addWidget(radioButtonMarmore);

        radioButtonPontos = new QRadioButton(centralWidget);
        radioButtonPontos->setObjectName(QStringLiteral("radioButtonPontos"));

        horizontalLayout->addWidget(radioButtonPontos);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 972, 17));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        radioButtonClouds->setText(QApplication::translate("MainWindow", "Clouds", Q_NULLPTR));
        radioOtherThing->setText(QApplication::translate("MainWindow", "Clouds2", Q_NULLPTR));
        radioButtonClouds3D->setText(QApplication::translate("MainWindow", "Clouds3D", Q_NULLPTR));
        radioButtonFlames->setText(QApplication::translate("MainWindow", "Flames", Q_NULLPTR));
        radioButtonMarmore->setText(QApplication::translate("MainWindow", "Marmore", Q_NULLPTR));
        radioButtonPontos->setText(QApplication::translate("MainWindow", "Pontos", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
