//#include "menu.h"
//
////menu::menu(void)
////{
////	/*QPopupMenu* file = new QPopupMenu;
////	file->insertItem( "&Open", this, SLOT( openFile() ) );
////	file->insertItem( "&Save", this, SLOT( saveFile() ) );
////	file->insertSeparator();
////	file->insertItem( "&Quit", this, SLOT( exitMain() ) );
////
////	QMenuBar* menu;
////	menu = new QMenuBar( this );
////	menu->insertItem( "&File", file );*/
////}
////
////menu::~menu(void)
////{
////}
//MyMainWindow::MyMainWindow()
//{
//	mainboard = new QWidget; //这个是窗口中的主体部件
//
//	setCentralWidget(mainboard); //加入这个部件
//
//	creatAction(); //创建Action
//
//	creatMenu(); //创建菜单
//
//	creatToolbar();   //创建工具栏
//}
//
//
//void MyMainWindow::creatAction()
//{
//	newgame = new QAction("&New Game",this);
//
//	newgame -> setIcon(QIcon("newgame.png"));
//
//	newgame -> setShortcut(QKeySequence::New);
//
//	connect(newgame,SIGNAL(triggered()),mainboard,SIGNAL(restartSIG()));
//}
//
//void MyMainWindow::creatMenu()
//
//{
//	Game = menuBar()->addMenu("&Game");
//
//	Game -> addAction(newgame);
//
//}
//
//
//Void MyMainWindow::creatToolbar()
//
//{
//
//	toolbar = addToolBar(“&Game”);
//
//	toolbar -> addAction(newgame);
//
//	toolbar -> addSeparator();
//}
//
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QKeySequence>
#include <QtGui/QToolBar>
#include "menu.h"

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
{
	openAction = new QAction(tr("&Open"), this);
	openAction->setShortcut(QKeySequence::Open);
	openAction->setStatusTip(tr("Open a file."));

	QMenu *file = menuBar()->addMenu(tr("&File"));
	file->addAction(openAction);

	QToolBar *toolBar = addToolBar(tr("&File"));
	toolBar->addAction(openAction);
}

MainWindow::~MainWindow()
{

}