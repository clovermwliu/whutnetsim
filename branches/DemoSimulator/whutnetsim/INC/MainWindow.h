#ifndef WHUTNETSIM_H
#define WHUTNETSIM_H

#include <QtGui>
#include <Qt3Support>
#include <QString>
#include "tab.h"
#include "listview.h"

class Q3Canvas;
class Q3CanvasView;
class QMenu;
class QToolBar;
class QAction;
//int   globalfun();

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainWindow();

	void createAction( );
	void createMenuBar( );
	void createToolbar( );
    void createDockWidget( );

    void printfError(string s);


	void changeEvent(QEvent *event);
	void closeEvent(QCloseEvent *event);

    QString WorkSpace;
	QString currentfilepath;   //��ǰ�ļ�·��
    listview *dirlist;         //list��ʾ
	QTreeView *tree;           //tree��ʾ
	QDirModel *model;           //tree model
	TabItem   *Info;           //������Ϣ��ʾ
private:
	QMenu *fileMenu;
	QMenu *settingMenu;
	QMenu *viewMenu;
	QMenu *helpMenu;
	QToolBar *fileToolBar;

    QAction *newAct;
	QAction *openAct;
	QAction *editAct;
	QAction *quitAct;
	QAction *dirsetAct;
	QAction *testentry;
public slots:
    void openfile();
	void newfile();
	void editfile();
	void testent();
	void dirsetting();

	void fileprocess(QString);

signals: 
	void openConfigure(const QString& configureFile);
	void globalfun();
	void fileready(QString);
};

#endif // WHUTNETSIM_H
