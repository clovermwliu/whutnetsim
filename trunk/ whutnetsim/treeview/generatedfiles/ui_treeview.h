/********************************************************************************
** Form generated from reading ui file 'treeview.ui'
**
** Created: Fri Nov 20 16:44:09 2009
**      by: Qt User Interface Compiler version 4.3.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_TREEVIEW_H
#define UI_TREEVIEW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QTreeView>
#include <QtGui/QWidget>

class Ui_treeviewClass
{
public:
    QWidget *centralWidget;
    QTreeView *treeView;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *treeviewClass)
    {
    if (treeviewClass->objectName().isEmpty())
        treeviewClass->setObjectName(QString::fromUtf8("treeviewClass"));
    treeviewClass->resize(600, 400);
    centralWidget = new QWidget(treeviewClass);
    centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
    treeView = new QTreeView(centralWidget);
    treeView->setObjectName(QString::fromUtf8("treeView"));
    treeView->setGeometry(QRect(290, 110, 120, 80));
    treeviewClass->setCentralWidget(centralWidget);
    menuBar = new QMenuBar(treeviewClass);
    menuBar->setObjectName(QString::fromUtf8("menuBar"));
    menuBar->setGeometry(QRect(0, 0, 600, 20));
    treeviewClass->setMenuBar(menuBar);
    mainToolBar = new QToolBar(treeviewClass);
    mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
    mainToolBar->setOrientation(Qt::Horizontal);
    treeviewClass->addToolBar(static_cast<Qt::ToolBarArea>(4), mainToolBar);
    statusBar = new QStatusBar(treeviewClass);
    statusBar->setObjectName(QString::fromUtf8("statusBar"));
    treeviewClass->setStatusBar(statusBar);

    retranslateUi(treeviewClass);

    QMetaObject::connectSlotsByName(treeviewClass);
    } // setupUi

    void retranslateUi(QMainWindow *treeviewClass)
    {
    treeviewClass->setWindowTitle(QApplication::translate("treeviewClass", "treeview", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(treeviewClass);
    } // retranslateUi

};

namespace Ui {
    class treeviewClass: public Ui_treeviewClass {};
} // namespace Ui

#endif // UI_TREEVIEW_H
