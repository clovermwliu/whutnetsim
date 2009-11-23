#include <QtGui/QApplication>
#include "paintwidget.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	PaintWidget pWidget;  
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));
	QTextCodec *codec = QTextCodec::codecForLocale();
    pWidget.setWindowTitle(codec->toUnicode("»­Í¼"));    
	pWidget.resize(500,400);    
	pWidget.show();
	a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
	return a.exec();
}
