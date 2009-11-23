#ifndef GNETQT_H
#define GNETQT_H

#include <QtGui/QMainWindow>
#include "ui_gnetqt.h"

#include <QApplication>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QDir>

class QAction;
class GnetQT : public QMainWindow
{
	Q_OBJECT

public:
	GnetQT(QWidget *parent = 0, Qt::WFlags flags = 0);
	~GnetQT();

private:
	void listFile(QTreeWidgetItem *, QFileInfo &);

private:
	Ui::GnetQTClass ui;
	QAction *openAction;

private slots:
	void on_cancelButton_clicked();
};

//class ATab:public QWidget
//{
//	Q_OBJECT
//public:
//	ATab(QTreeWidget *parent=0);
//};
//class BTab:public QWidget
//{
//	Q_OBJECT
//public:
//	BTab(QTreeWidget *parent=0);
//};
#endif // GNETQT_H
