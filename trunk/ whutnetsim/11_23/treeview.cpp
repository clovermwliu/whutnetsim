#include "treeview.h"

TreeView::TreeView() : QTreeView()
{      
	QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));
	model = new QStandardItemModel(4,2);
	model->setHeaderData(0, Qt::Horizontal, tr("服务"));
	model->setHeaderData(1, Qt::Horizontal, tr("说明"));
	QStandardItem *item1 = new QStandardItem("avahi-daemon");
	item1->setIcon(QIcon("gparted.png"));
	QStandardItem *item2 = new QStandardItem("bluetooth");
	item2->setIcon(QIcon("gparted.png"));
	QStandardItem *item3 = new QStandardItem("crond");
	QStandardItem *item4 = new QStandardItem("cups");
	model->setItem(0, 0, item1);
	model->setItem(1, 0, item2);
	model->setItem(2, 0, item3);
	model->setItem(3, 0, item4);
	QStandardItem *item5 = new QStandardItem("fifth");
	item4->appendRow(item5);

	QModelIndex parent;
	for (int i = 0; i < 4; ++i) {
		parent = model->index(0, 0, parent);
		model->insertRows(0, 1, parent);
		model->insertColumns(0, 1, parent);
		QModelIndex index = model->index(0, 0, parent);
		model->setData(index, i);
	}
	this->setModel(model);
}

QList<QStandardItem*> TreeView::returnTheItems()
{
	return model->findItems("*", Qt::MatchWildcard | Qt::MatchRecursive);
}

void TreeView::iterateOverItems()
{
	QList<QStandardItem*> list = returnTheItems();

	foreach (QStandardItem* item, list) {
		qDebug() << item->text();
	}
}
void TreeView::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		QModelIndex index0 = currentIndex();
		qDebug() << index0.data().toString();
	}
}

