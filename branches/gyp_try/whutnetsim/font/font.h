#ifndef FONT_H
#define FONT_H

#include <QtGui/QMainWindow>
//#include "ui_font.h"

class font : public QMainWindow
{
	Q_OBJECT

public:
	font(QWidget *parent = 0, Qt::WFlags flags = 0);
	~font();

private:
	Ui::fontClass ui;
};

#endif // FONT_H
