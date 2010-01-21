#include "qtwindow.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	QTWindow w = new QTWindow(false);
	w.DisplayTopologyAndReturn();
	return app.exec();
}
