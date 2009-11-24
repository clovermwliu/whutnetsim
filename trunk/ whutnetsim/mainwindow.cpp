#include "mainwindow.h"
MainWindow::MainWindow( QWidget *parent):QWidget( parent)
{
	QFont f( "Helvetica", 14, QFont::Bold );
	setFont( f );
	AddButton = new QPushButton( "Add", this );
	AddButton->setGeometry( 50, 15, 90, 40 );
	SubButton = new QPushButton( "Sub", this );
	SubButton->setGeometry( 50, 70, 90, 40 );
	counter = new Counter( this );
	counter->setGeometry( 50, 125, 90, 40 );
	QObject::connect( AddButton, SIGNAL( clicked() ),
		counter, SLOT( IncCounter() ) );
	QObject::connect( SubButton, SIGNAL( clicked() ),
		counter, SLOT( DecCounter() ) );
}
