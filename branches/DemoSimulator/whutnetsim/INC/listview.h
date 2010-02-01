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

    int filesearch(char* , int); //�����ļ�������һΪ�ļ���·����������Ϊ���ҵĲ���
    void mouseDoubleClickEvent(QMouseEvent *event);//�������˫���¼�
	
	QStandardItemModel *model;   //�б�ģ��
	QModelIndex lastindex;       //ǰһ�ε����item������
	int lastindexFlag;           //ǰһ��������ʶ�������ػ���ͼ��
	QStandardItem *currentitem; //��ǰ�����item
    QMap<QStandardItem*,QString> tasklib;//�����
	int filecount;//Ŀ���ļ��ĸ���

signals:
	void currentfile(QString);

};

#endif // LISTVIEW_H
