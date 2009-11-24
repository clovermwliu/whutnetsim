#ifndef COUNTER_H
#define COUNTER_H
#include <qlabel.h>
class Counter: public QWidget
{
	Q_OBJECT
public:
	Counter( QWidget *parent=0);
	public slots:
		void IncCounter();
		void DecCounter();
private:
	int counter;
	QLabel *label;
};
#endif