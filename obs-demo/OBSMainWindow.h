#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_OBSMainWindow.h"

class OBSMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	OBSMainWindow(QWidget *parent = Q_NULLPTR);

private:
	Ui::OBSMainWindowClass ui;
};
