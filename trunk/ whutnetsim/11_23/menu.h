#pragma once

//#include <QMenu>
//
//#include <QAction>
//
//#include <QMainWindow>
//
//#include <QToolBar>
//
////#include "MainBoard.h"
//
//
//
//class MyMainWindow:public QMainWindow //ºÃ≥–¡ÀQMainWindow
//
//{
//
//	Q_OBJECT
//
//public:
//
//	MyMainWindow();
//
//	void creatAction();
//
//	void creatMenu();
//
//	void creatToolbar();
//
//private:
//
//	QMenu *Game, *Options, *Help;
//
//	QAction *newgame, *quitgame, *fight, *start, *pause, *jumpover;
//
//	QAction *gamehelp, *aboutgame, *setting;
//
//	QtoolBar *toolbar;
//
//	QWidget *mainboard;
//
//	public slots:
//
//};


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class QAction;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	QAction *openAction;
};

#endif // MAINWINDOW_H