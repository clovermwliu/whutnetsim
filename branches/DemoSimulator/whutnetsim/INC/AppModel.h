//Copyright (c) 2010, Information Security Institute of Wuhan Universtiy(ISIWhu)
//All rights reserved.
//
//PLEASE READ THIS DOCUMENT CAREFULLY BEFORE UTILIZING THE PROGRAM
//BY UTILIZING THIS PROGRAM, YOU AGREE TO BECOME BOUND BY THE TERMS OF
//THIS LICENSE.  IF YOU DO NOT AGREE TO THE TERMS OF THIS LICENSE, DO
//NOT USE THIS PROGRAM OR ANY PORTION THEREOF IN ANY FORM OR MANNER.
//
//This License allows you to:
//1. Make copies and distribute copies of the Program's source code provide that any such copy 
//   clearly displays any and all appropriate copyright notices and disclaimer of warranty as set 
//   forth in this License.
//2. Modify the original copy or copies of the Program or any portion thereof ("Modification(s)"). 
//   Modifications may be copied and distributed under the terms and conditions as set forth above. 
//   Any and all modified files must be affixed with prominent notices that you have changed the 
//   files and the date that the changes occurred.

//Termination:
//   If at anytime you are unable to comply with any portion of this License you must immediately 
//   cease use of the Program and all distribution activities involving the Program or any portion 
//   thereof.

//Statement:
//   In this program, part of the code is from the GTNetS project, The Georgia Tech Network 
//   Simulator (GTNetS) is a full-featured network simulation environment that allows researchers in 
//   computer networks to study the behavior of moderate to large scale networks, under a variety of 
//   conditions. Our work have great advance due to this project, Thanks to Dr. George F. Riley from 
//   Georgia Tech Research Corporation. Anyone who wants to study the GTNetS can come to its homepage:
//   http://www.ece.gatech.edu/research/labs/MANIACS/GTNetS/
//


//File Information:
//
//
//File Name:
//File Purpose:
//Original Author:
//Author Organization:
//Construct Data:
//Modify Author:
//Author Organization:
//Modify Data:


#ifndef __appmodel_h__
#define __appmodel_h__


#include "node.h"
#include "ElementCustom.h"
#include "FileScript.h"
#include "ErrorHandler.h"

#include <map>

#define  ERROR_MODEL_SUCCESS                         ERROR_NO_ERROR
#define  ERROR_MODEL_INSERT_EXIST                    0x00050201
#define  ERROR_MODEL_DELETE_NOT_EXIST                0x00050202
#define  ERROR_MODEL_GET_NOT_EXIST                   0x00050203
#define  ERROR_MODEL_WHEN_INIT_CUSTOMELEMENT         0x00050204



class AppModel:public CErrorHandler{

//////////////////////////////////////////////////////////////////////////
//基础方法
//////////////////////////////////////////////////////////////////////////
public:
	//构造函数及析构函数
	AppModel(void);

	~AppModel(void);

	AppModel(CFileScript &f):pScriptCache(&f){ SetLastError(ERROR_MODEL_SUCCESS); }//直接指定配置缓存对象

	AppModel(string StrScriptCache); //根据ConfigCachePath路径构造配置缓存，指针赋给pConfigCache 

	virtual AppModel* Copy() const = 0;//描述：返回当前对象的拷贝，纯虚函数，所有派生类需给出实现

	virtual bool GetModelValue(void **p) = 0;//描述：计算模型的值，纯虚函数，所有派生类需给出实现

	virtual void Initialize(void *p) = 0;//描述：初始化模型相关参数，纯虚函数，所有派生类需给出实现

private:
	CFileScript *pScriptCache;  //模型存取配置文件用的配置缓存对象，经构造函数传入赋值

	bool bprivateCache; //如果是Model类自己构造的配置缓存，则为true；否则为false



//////////////////////////////////////////////////////////////////////////
//和结点绑定
//////////////////////////////////////////////////////////////////////////


protected:
	Node *node;//绑定的结点指针，有的子类不需要

public:
	virtual void AttachNode(Node *n);//所有继承该方法的子类应先实现这个方法




//////////////////////////////////////////////////////////////////////////
//自定义部件相关
//////////////////////////////////////////////////////////////////////////
private:

	struct ElementStruct
	{
		int ID;//子类指定的该自定义部件的id
		bool RevelantToNode;//该自定义部件是否和结点相关
		CElementCustom thisElement;//自定义部件对象
	};//一个自定义部件

	map<int, ElementStruct> ElementIndex;//存储自定义部件及相关信息

protected:

	//Size_t GetElementSize() { return ElementIndex.size(); }

    //描述：增加一个自定义部件到ElementIndex，返回当前ElementIndex中的Element个数
	Size_t AddElement(int id, bool revelantToNode, string strSectionName);
	

	//描述：删除ID为id的自定义部件，返回当前ElementIndex中剩余的Element个数
	Size_t DeleteElement(int id);

	//描述：调用自定义部件类的相关方法，获得ID为id的自定义部件的值，并返回
	double GetElementValueById(int id);



//////////////////////////////////////////////////////////////////////////
///错误处理相关
//////////////////////////////////////////////////////////////////////////

	virtual Error_str GetLastErrorEx();	


};


#endif