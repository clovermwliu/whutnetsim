#include "tab.h"
#include <QLabel>
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

TabItem::TabItem(QWidget *parent):QWidget(parent)
{
	TabWidget = new QTabWidget;
	fileinfo = new QTextEdit(TabWidget);
	fileinfo->setReadOnly(true);

	ErrorInfo = new QTextEdit(TabWidget);
	ErrorInfo->setReadOnly(true); 

	TabWidget->addTab(fileinfo,QString("File Information"));
	TabWidget->addTab(ErrorInfo,QString("Error Information"));
	//TabWidget->addTab(new FirstTab,QString("ATab"));
	//TabWidget->addTab(new SecondTab,QString("BTab"));

	QVBoxLayout *mainLayout=new QVBoxLayout;
	mainLayout->addWidget(TabWidget);
	setLayout(mainLayout);
	resize(500,500);
}

QSize TabItem::sizeHint() const
{
	return QSize(900,200);    /* 在这里定义dock的初始大小 */
}

FirstTab::FirstTab(QWidget *parent):QWidget(parent)
{
	QLabel *aLabel=new QLabel(tr("I am ATab"),this);
}

SecondTab::SecondTab(QWidget *parent):QWidget(parent)
{
	QLabel *bLabel=new QLabel(tr("I am BTab"),this);
}
