#include "gui_ctrl.h"

int main(int argc, char** argv)
{
	QApplication* app;
	app = new QApplication(argc, argv);
	gui_ctrl* w = new gui_ctrl(false,app);
	return app->exec();
}
