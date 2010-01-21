#include "mainwindow.h"
#include <QtGui/QDockWidget>
#include <QtGui/QDirModel>

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
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
}
void MainWindow::createMenuBar()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAct);
	fileMenu->addAction(openAct);
	fileMenu->addAction(editAct);
	fileMenu->addSeparator();
	fileMenu->addAction(quitAct);

	editMenu = menuBar()->addMenu(tr("&Edit"));
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
}

void MainWindow::createDockWidget()
{
	QDockWidget* dock= new QDockWidget(tr("Dir"),this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	QDirModel *model = new QDirModel;
    tree = new QTreeView(dock);
	tree->setModel(model);
	tree->setRootIndex(model->index("./"));
	dock->setWidget(tree);
	addDockWidget(Qt::LeftDockWidgetArea, dock);
	viewMenu->addAction(dock->toggleViewAction());

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

void MainWindow::closeEvent(QCloseEvent *)
{
	
	emit closemainwindow();
}

MainWindow::~MainWindow()
{
   delete tree;
   delete Info;

}

