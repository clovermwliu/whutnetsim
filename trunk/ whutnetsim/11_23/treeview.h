#include <QtGui>
class TreeView :public QTreeView
{
public:
	TreeView();
	void iterateOverItems();
	QList<QStandardItem*> returnTheItems();
	void mouseDoubleClickEvent(QMouseEvent *event);
private:
	QStandardItemModel *model;
};

