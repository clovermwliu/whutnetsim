#include "listview.h"
#include "qtwindow.h"
#include<io.h> 
#include <iostream>
using namespace std;

listview::listview()
: QListView( )
{

	lastindexFlag = 0;	 
	filecount = filesearch(".//", 0);
	model = new QStandardItemModel(filecount,1);
	model->setHeaderData(0, Qt::Horizontal, tr("FileName"));
	QMap<QStandardItem*,QString> ::iterator iter;
	int index = 0;
	for(iter = tasklib.begin(); iter != tasklib.end(); iter++)
	{
		model->setItem(index, 0, iter.key());
		index++;
	}
	setModel(model);
}

int listview::filesearch(char* path,int layer) 
{
	int count = 0;
	struct _finddata_t   filefind;   
	string  curr=path;
	curr+="\\*.*";  
	int   done=0,i,handle;   
	if((handle=_findfirst(curr.c_str(),&filefind))==-1)
		return 0; 
	int index = 0;
	QStandardItem *item;
	while(!(done=_findnext(handle,&filefind)))   
	{   
		if(!strcmp(filefind.name,"..")){

			continue;
		}


		if   ((_A_SUBDIR==filefind.attrib))   
		{            
			curr=path;
			curr+="\\";
			curr+=filefind.name;   
			filesearch((char*)curr.c_str(),layer+1);   
		}   
		else     
		{   	
			char* objstr = strdup(filefind.name);
			char* targetstr = "tad.";
			char* resultstr;
			strrev(objstr);
			resultstr =strstr(objstr,targetstr);
			if (objstr==resultstr)
			{
				string filename=path;
				filename+="\\";
				filename+=filefind.name;

				QString t = QString(filename.c_str());

				item = new QStandardItem(QString(filefind.name));
				item->setIcon(QIcon(".\\Resources\\pause.png"));
				tasklib.insert(item,t);
				count++;
			}	
		}   
	}           
	_findclose(handle);    
	return count;
}

void listview::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) 
	{
		QModelIndex index0 = currentIndex();
		currentitem = model->itemFromIndex(index0);
		if (lastindexFlag==0)
		{
			lastindex = index0;
			lastindexFlag = 1;
		}
		else
		{
			if (lastindex!=index0)
			{	
				QStandardItem *lastitem = model->itemFromIndex(lastindex);
			    lastitem->setIcon(QIcon(".\\Resources\\pause.png"));
			    lastindex = index0;
			}
		}
		//currentitem->setIcon(QIcon(".\\Resources\\play.png"));
		QMap<QStandardItem*,QString>::iterator iter;
		iter = tasklib.find(currentitem);

		emit currentfile(iter.value());		
	
	}
}

void listview::changeModel(QString s)
{
	model->clear();
	tasklib.clear();
	filesearch(s.toLatin1().data(), 0);
	QMap<QStandardItem*,QString> ::iterator iter;
	int index = 0;
	for(iter = tasklib.begin(); iter != tasklib.end(); iter++)
	{
		model->setItem(index, 0, iter.key());
		index++;
	}
}


listview::~listview()
{

}
