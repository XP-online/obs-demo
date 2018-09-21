#include "precompile.h"
#include "OBSMainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	OBSMainWindow w;
	w.OBSInit();
	w.show();
	w.PlayVideo("D:\\J.Fla-Let Her Go.mp4");
	return a.exec();
}
