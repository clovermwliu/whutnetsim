#ifndef WHUTNETSIM_H
#define WHUTNETSIM_H

#include <QtGui>
#include <Qt3Support>
#include "tab.h"

class Q3Canvas;
class Q3CanvasView;
class QMenu;
class QToolBar;
class QAction;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainWindow();
	void closeEvent(QCloseEvent *);

	void createAction( );
	void createMenuBar( );
	void createToolbar( );
    void createDockWidget( );
	
public:
	QTreeView *tree;
	TabItem   *Info;
	//Q3CanvasView* canvasview;
	//Q3Canvas* canvas;

	QMenu *fileMenu;
	QMenu *editMenu;
	QMenu *viewMenu;
	QMenu *helpMenu;
	QToolBar *fileToolBar;
    QAction *newAct;
	QAction *openAct;
	QAction *editAct;
	QAction *quitAct;
public slots:
    void openfile();
	void newfile();
	void editfile();

signals: 
	void openConfigure(const QString& configureFile);
	void closemainwindow();
};

#endif // WHUTNETSIM_H
