#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <QtGui>
#include <qstring.h>
#include <qmap.h>
#include <map>
#include <string>
using namespace std;

class QComboBox;
class QDir;
class QLabel;
class QPushButton;
class QFileDialog;

class listview : public QListView
{
	Q_OBJECT
public:
	listview();
	~listview();

    int filesearch(char* , int); //查找文件，参数一为文件夹路径，参数二为查找的层数
    void mouseDoubleClickEvent(QMouseEvent *event);//处理鼠标双击事件
	
	QStandardItemModel *model;   //列表模型
	QModelIndex lastindex;       //前一次点击的item的索引
	int lastindexFlag;           //前一次索引标识，用于重绘其图标
	QStandardItem *currentitem; //当前点击的item
    QMap<QStandardItem*,QString> tasklib;//任务库
	int filecount;//目标文件的个数

signals:
	void currentfile(QString);

};

#endif // LISTVIEW_H
