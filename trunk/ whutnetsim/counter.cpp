#include <stdio.h>
#include "counter.h"
Counter::Counter( QWidget *parent):
QWidget( parent)
{
	counter = 0;
	label = new QLabel( "0", this );
	//label->setAlignment( AlignVCenter | AlignHCenter );
}
void Counter::IncCounter()
{
	char str[30];
	sprintf( str, "%d", ++counter );
	label->setText( str );
}
void Counter::DecCounter()
{
	char str[30];
	sprintf( str, "%d", --counter );
	label->setText( str );
}