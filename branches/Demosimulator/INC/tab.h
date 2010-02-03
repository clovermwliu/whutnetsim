#include <QWidget>
#include <QtGui/QTabWidget>
#include <QtGui/QTextEdit>

class TabItem:public QWidget
{
	Q_OBJECT
public:
	TabItem(QWidget *parent=0);
	QSize sizeHint() const;

	QTabWidget *TabWidget;
	QTextEdit *fileinfo;
	QTextEdit *ErrorInfo;
};
class FirstTab:public QWidget
{
	Q_OBJECT
public:
	FirstTab(QWidget *parent=0);
	
};
class SecondTab:public QWidget
{
	Q_OBJECT
public:
	SecondTab(QWidget *parent=0);
};
