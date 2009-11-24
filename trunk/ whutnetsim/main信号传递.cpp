//#include <QtGui/QApplication>
//#include "sheet.h"
//
//int main(int argc, char *argv[])
//{
//	/*QApplication a(argc, argv);
//	Sheet w;
//	w.show();
//	a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
//	return a.exec();*/
//}
#include <qapplication.h>
#include "mainwindow.h"
int main( int argc, char **argv )
{
	QApplication app( argc, argv );
	MainWindow *mainwindow = new MainWindow( 0 );
	mainwindow->setGeometry( 50, 20 , 200 ,200 );
	//app.setMainWidget( mainwindow );
	mainwindow->show();
	int result = app.exec();
	return result;
}