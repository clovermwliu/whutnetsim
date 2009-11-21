#include <QtGui/QApplication>
#include <QtGui>
//#include "treeview.h"

int main(int argc, char *argv[])
{
	/*QApplication app(argc, argv);
	TreeView view;
	view.setEditTriggers(QAbstractItemView::NoEditTriggers); 
	view.header()->setResizeMode(QHeaderView::ResizeToContents);
	view.resize(800,600);
	view.iterateOverItems();
	view.setWindowTitle(QObject::tr("Magic Linux ���������"));
	view.show();
	return app.exec();*/

	//////////////////////////////////////////////////////////////////
	QApplication app(argc, argv);
	QSplitter *splitter = new QSplitter;

	QDirModel *model = new QDirModel;
	//��ȱʡĿ¼��������
	QTreeView *tree = new QTreeView(splitter);
	tree->setModel(model);
	tree->setRootIndex(model->index(QDir::currentPath()));

	QListView *list = new QListView(splitter);
	list->setModel(model);
	list->setRootIndex(model->index(QDir::currentPath()));
	//����һ��viewȥ��ʾmodel�е����ݣ�ֻ��Ҫ�򵥵ص���setModel(),����Ŀ¼model��Ϊ��������
	//setRootIndex()����views��ʾ�ĸ�Ŀ¼����Ϣ������Ҫ�ṩһ��model index,Ȼ�������
	//model indexȥmodel��ȥ��ȡ����
	//index()���������QDirModel���еģ�ͨ����һ��Ŀ¼��Ϊ�������õ�����Ҫ��model index
	//�����Ĵ���ֻ�Ǵ���show����,���������¼�ѭ���ͺ���
	splitter->setWindowTitle("Two views onto the same directory model");
	splitter->show();
	return app.exec();


}
