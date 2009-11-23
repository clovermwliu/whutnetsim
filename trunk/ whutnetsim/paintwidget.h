#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QtGui>


class PaintWidget : public QMainWindow
{
	Q_OBJECT

public:
	PaintWidget(QWidget *parent = 0, Qt::WFlags flags = 0);
	~PaintWidget();
protected:
	void paintEvent(QPaintEvent*);
};

#endif // PAINTWIDGET_H
