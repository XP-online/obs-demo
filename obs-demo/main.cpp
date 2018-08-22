#include "OBSMainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	OBSMainWindow w;
	w.show();
	return a.exec();
}
