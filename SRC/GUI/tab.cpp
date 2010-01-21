#include "tab.h"
#include <QLabel>
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

TabItem::TabItem(QWidget *parent):QWidget(parent)
{
	TabWidget=new QTabWidget;
	TabWidget->addTab(new FirstTab,QString("ATab"));
	TabWidget->addTab(new SecondTab,QString("BTab"));
	QPushButton *ok=new QPushButton(tr("OK"));
	QPushButton *cancel=new QPushButton(tr("Cancel"));
	//connect(ok,SIGNAL(clicked()),this,SLOT(accept()));
	//connect(cancel,SIGNAL(clicked()),this,SLOT(reject()));
	QHBoxLayout *btnLayout=new QHBoxLayout;
	btnLayout->addStretch(1);
	btnLayout->addWidget(ok);
	btnLayout->addWidget(cancel);
	QVBoxLayout *mainLayout=new QVBoxLayout;
	mainLayout->addWidget(TabWidget);
	mainLayout->addLayout(btnLayout);
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
