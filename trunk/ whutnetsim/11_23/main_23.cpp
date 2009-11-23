#include <QtGui/QApplication>
#include "gnetqt.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GnetQT w;
	w.show();
	a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
	return a.exec();
}
