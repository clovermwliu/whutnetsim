#include "mainwindow.h"
#include <QtGui/QDockWidget>
#include <QtGui/QDirModel>
#include <stdio.h>
#include <fstream>
#include <QFile>
#include <QTextStream>
#include "qtwindow.h"
using namespace std;

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	WorkSpace = ".//";
	createAction();
	createMenuBar();
	createToolbar();
	createDockWidget();
}

void MainWindow::createAction()
{
	newAct = new QAction(QIcon("./resources/new.png"), tr("&New File"),this);
    connect(newAct, SIGNAL(triggered()), this, SLOT(newfile()));
	
	openAct = new QAction(QIcon("./resources/open.png"), tr("&open File"),this);
	connect(openAct, SIGNAL(triggered()), this, SLOT(openfile()));
	
	editAct = new QAction(QIcon("./resources/edit.png"), tr("&edit File"),this);
	connect(editAct, SIGNAL(triggered()), this, SLOT(editfile()));
	
	quitAct = new QAction(QIcon("./resources/quit.png"), tr("&quit"),this);
	connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    dirsetAct = new QAction(QIcon("./resources/dirsetting.png"),tr("&WorkSpace"),this);
	connect(dirsetAct,SIGNAL(triggered()),this,SLOT(dirsetting()));

	testentry = new QAction(QIcon("./resources/testentry.png"), tr("&testentry"),this);
	connect(testentry,SIGNAL(triggered()),this,SLOT(testent()));
}
void MainWindow::createMenuBar()
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

void MainWindow::createToolbar()
{
	fileToolBar = addToolBar(tr("File"));
	fileToolBar->addAction(newAct);
	fileToolBar->addAction(openAct);
	fileToolBar->addAction(editAct);
	fileToolBar->addAction(quitAct);
	//fileToolBar->addAction(testentry);
}

void MainWindow::createDockWidget()
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

void MainWindow::openfile()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), ".");
	if (!fileName.isEmpty())
	{
		emit openConfigure(fileName);
	}
}

void MainWindow::newfile()
{

}

void MainWindow::editfile()
{

}

void MainWindow::testent()
{
     emit globalfun();
}

void MainWindow::fileprocess(QString s)
{
    if (currentfilepath!="")
    {
		QTWindow::qtWin->Quit();
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

void MainWindow::dirsetting()
{
	QString directory = QFileDialog::getExistingDirectory(this,
		tr("Find Files"), QDir::currentPath());
    
	tree = nil;
	model = nil;
	model = new QDirModel;
	tree->setModel(model);
	tree->setRootIndex(model->index(WorkSpace));
}


void MainWindow::printfError(string s)
{
   QString t;
   t = t + QString(s.c_str());
   QTextStream ts(&t);
   Info->ErrorInfo->setText(ts.read());

}
void MainWindow::closeEvent(QCloseEvent *event)
{
	if (currentfilepath!="")
	{
		QTWindow::qtWin->Quit();
	}
	
}

void MainWindow::changeEvent(QEvent *event)
{
	if(event->WindowStateChange == Qt::WindowMinimized)
	{
       this->hide();
	}
}

MainWindow::~MainWindow()
{
   delete tree;
   delete Info;

}

