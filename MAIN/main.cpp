#include "qtwindow.h"

int main(int argc, char** argv)
{
	QApplication* app;
	app = new QApplication(argc, argv);
	QTWindow* w = new QTWindow(false,app);
	return app->exec();
}
