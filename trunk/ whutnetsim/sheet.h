#ifndef SHEET_H
#define SHEET_H

#include <QtGui/QMainWindow>
#include "ui_sheet.h"

class Sheet : public QMainWindow
{
	Q_OBJECT

public:
	Sheet(QWidget *parent = 0, Qt::WFlags flags = 0);
	~Sheet();

private:
	Ui::SheetClass ui;

private slots:
	void on_pushButton_clicked();
};

#endif // SHEET_H
