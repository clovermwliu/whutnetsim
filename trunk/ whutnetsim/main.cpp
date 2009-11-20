#include <QtGui/QApplication>
#include "treeview.h"

int main(int argc, char *argv[])
{
	/*QApplication a(argc, argv);
	treeview w;
	w.show();
	a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
	return a.exec();*/
	QApplication app(argc, argv);
	TreeView view;
	view.setEditTriggers(QAbstractItemView::NoEditTriggers); 
	view.header()->setResizeMode(QHeaderView::ResizeToContents);
	view.resize(800,600);
	view.iterateOverItems();
	view.setWindowTitle(QObject::tr("Magic Linux 服务管理器"));
	view.show();
	return app.exec();

}
