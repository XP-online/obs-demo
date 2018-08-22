/********************************************************************************
** Form generated from reading UI file 'OBSMainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OBSMAINWINDOW_H
#define UI_OBSMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OBSMainWindowClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *OBSMainWindowClass)
    {
        if (OBSMainWindowClass->objectName().isEmpty())
            OBSMainWindowClass->setObjectName(QStringLiteral("OBSMainWindowClass"));
        OBSMainWindowClass->resize(600, 400);
        menuBar = new QMenuBar(OBSMainWindowClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        OBSMainWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(OBSMainWindowClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        OBSMainWindowClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(OBSMainWindowClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        OBSMainWindowClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(OBSMainWindowClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        OBSMainWindowClass->setStatusBar(statusBar);

        retranslateUi(OBSMainWindowClass);

        QMetaObject::connectSlotsByName(OBSMainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *OBSMainWindowClass)
    {
        OBSMainWindowClass->setWindowTitle(QApplication::translate("OBSMainWindowClass", "OBSMainWindow", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class OBSMainWindowClass: public Ui_OBSMainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OBSMAINWINDOW_H
