#include "gnetqt.h"

GnetQT::GnetQT(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	openAction = new QAction(tr("&Open"), this);
	openAction->setShortcut(QKeySequence::Open);
	openAction->setStatusTip(tr("Open a file."));

	QMenu *file = menuBar()->addMenu(tr("&File"));
	file->addAction(openAction);

	QToolBar *toolBar = addToolBar(tr("&File"));
	toolBar->addAction(openAction);


	
	//QTreeWidget的设置

	// 查询的目录    
	QFileInfo fileInfo("D:\\Inetpub");
	QStringList fileColumn;
	fileColumn.append(fileInfo.fileName());

	QTreeWidgetItem *dir = new QTreeWidgetItem(fileColumn);
	dir->setIcon(0, QIcon("caterpillar_head.jpg"));
	dir->setCheckState(0, Qt::Checked); // 设定可核取的方块 
	ui.treeWidget->addTopLevelItem(dir);

	// 查询目录 
	listFile(dir, fileInfo);
}

GnetQT::~GnetQT()
{

}


void GnetQT::on_cancelButton_clicked()
{

}


//ATab::ATab(QTreeWidget *parent):QWidget(parent)
//{
//	QLabel *aLabel=new QLabel(tr("I am ATab"),this);
//}
//BTab::BTab(QTreeWidget *parent):QWidget(parent)
//{
//	QLabel *bLabel=new QLabel(tr("I am BTab"),this);
//}

void GnetQT::listFile(QTreeWidgetItem *parentWidgetItem, QFileInfo &parent) {
	QDir dir;
	dir.setPath(parent.filePath());
	dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks);
	dir.setSorting(QDir::DirsFirst | QDir::Name);    

	const QFileInfoList fileList = dir.entryInfoList();

	for (int i = 0; i < fileList.size(); i++) {
		QFileInfo fileInfo = fileList.at(i);
		QStringList fileColumn;
		fileColumn.append(fileInfo.fileName());
		if (fileInfo.fileName() == "." || fileInfo.fileName() == ".." ); // nothing
		else if(fileInfo.isDir()) {
			QTreeWidgetItem *child = new QTreeWidgetItem(fileColumn);
			child->setIcon(0, QIcon("caterpillar_head.jpg"));
			child->setCheckState(0, Qt::Checked);
			parentWidgetItem->addChild(child);
			// 查询子目录
			listFile(child, fileInfo);
		}
		else {
			fileColumn.append(QString::number(fileInfo.size()));
			QTreeWidgetItem *child = new QTreeWidgetItem(fileColumn);
			child->setIcon(0, QIcon("momor_head.jpg"));
			child->setCheckState(0, Qt::Checked);            
			parentWidgetItem->addChild(child);
		}
	}    
}