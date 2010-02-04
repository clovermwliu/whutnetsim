#include "gui_frame.h"
#include <QtGui/QDockWidget>
#include <QtGui/QDirModel>
#include <stdio.h>
#include <fstream>
#include <QFile>
#include <QTextStream>
#include "gui_ctrl.h"
using namespace std;

gui_frame::gui_frame(QWidget *parent, Qt::WFlags flags)
: QMainWindow(parent, flags)
{
	//trayIcon = new QSystemTrayIcon(this);
    //trayIcon->setIcon(QIcon("./resources/mainframe.png"));
	//trayIcon->show();

	WorkSpace = ".//";
	createAction();
	createMenuBar();
	createToolbar();
	createDockWidget();
}

void gui_frame::createAction()
{
	newAct = new QAction(QIcon("./resources/new.png"), tr("&New File"),this);
	connect(newAct, SIGNAL(triggered()), this, SLOT(newfile()));

	openAct = new QAction(QIcon("./resources/open.png"), tr("&open File"),this);
	connect(openAct, SIGNAL(triggered()), this, SLOT(openfile()));

	editAct = new QAction(QIcon("./resources/edit.png"), tr("&edit File"),this);
	connect(editAct, SIGNAL(triggered()), this, SLOT(editfile()));

	quitAct = new QAction(QIcon("./resources/quit.png"), tr("&quit"),this);
	connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

	dirsetAct = new QAction(QIcon("./resources/workspace.png"),tr("&WorkSpace"),this);
	connect(dirsetAct,SIGNAL(triggered()),this,SLOT(dirsetting()));

	testentry = new QAction(QIcon("./resources/testentry.png"), tr("&testentry"),this);
	connect(testentry,SIGNAL(triggered()),this,SLOT(testent()));
}
void gui_frame::createMenuBar()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAct);
	fileMenu->addAction(openAct);
	fileMenu->addAction(editAct);
	fileMenu->addSeparator();
	fileMenu->addAction(quitAct);

	settingMenu = menuBar()->addMenu(tr("&Setting"));
	settingMenu->addAction(dirsetAct);

	viewMenu = menuBar()->addMenu(tr("&View"));
	helpMenu = menuBar()->addMenu(tr("&Help"));
}

void gui_frame::createToolbar()
{
	fileToolBar = addToolBar(tr("File"));
	fileToolBar->addAction(newAct);
	fileToolBar->addAction(openAct);
	fileToolBar->addAction(editAct);
	fileToolBar->addAction(quitAct);
	//fileToolBar->addAction(testentry);
}

void gui_frame::createDockWidget()
{
	QDockWidget* dock= new QDockWidget(tr("Dir"),this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	model = new QDirModel;
	tree = new QTreeView(dock);
	tree->setModel(model);
	tree->setRootIndex(model->index(WorkSpace));
	dock->setWidget(tree);
	addDockWidget(Qt::LeftDockWidgetArea, dock);
	viewMenu->addAction(dock->toggleViewAction());

	QDockWidget* dock1 = new QDockWidget(tr("File"),this);
	dock1->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	dirlist = new listview( );
	//connect(list,SIGNAL(doubleClicked(const QModelIndex &) ),this,SLOT(open(const QModelIndex &))) ;
	dock1->setWidget(dirlist);
	tabifyDockWidget(dock,dock1);
	viewMenu->addAction(dock1->toggleViewAction());
	QObject::connect(dirlist,SIGNAL(currentfile(QString)),this,SLOT(fileprocess(QString)));


	dock= new QDockWidget(tr("Info"),this);
	dock->setAllowedAreas(Qt::BottomDockWidgetArea |Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	Info = new TabItem(dock);
	dock->setWidget(Info);
	addDockWidget(Qt::BottomDockWidgetArea, dock);
	viewMenu->addAction(dock->toggleViewAction());

}

void gui_frame::openfile()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), ".");
	if (!fileName.isEmpty())
	{
		emit openConfigure(fileName);
	}
}

void gui_frame::newfile()
{

}

void gui_frame::editfile()
{

}

void gui_frame::testent()
{
	emit globalfun();
}

void gui_frame::fileprocess(QString s)
{
	if (currentfilepath!="")
	{
		gui_ctrl::qtWin->Quit();
	}
	currentfilepath = s;
	// 从文件中读取文本
	QFile file( currentfilepath ); 
	if ( file.open( IO_ReadOnly ) ) 
	{
		QTextStream ts( &file );
		Info->fileinfo->setText( ts.read() );
	}  
	//发送文件路径	
	emit fileready(currentfilepath);

}

void gui_frame::dirsetting()
{
	WorkSpace = QFileDialog::getExistingDirectory(this,
		tr("Find Files"), QDir::currentPath());
	if (!WorkSpace.isEmpty()) 
	{
		if(currentfilepath != "")
		{
			gui_ctrl::qtWin->Quit();
		}
		dirlist->lastindexFlag = 0;
		tree->setRootIndex(model->index(WorkSpace));
		dirlist->changeModel(WorkSpace);
	}
}


void gui_frame::printfError(string s)
{
	QString t;
	t = t + QString(s.c_str());
	QTextStream ts(&t);
	Info->ErrorInfo->setText(ts.read());

}
void gui_frame::closeEvent(QCloseEvent *event)
{
	if (currentfilepath!="")
	{
		gui_ctrl::qtWin->Quit();
	}

}

void gui_frame::changeEvent(QEvent *event)
{
	if(event->WindowStateChange == Qt::WindowMinimized)
	{
		this->hide();
	}
}

gui_frame::~gui_frame()
{
	delete tree;
	delete Info;

}

