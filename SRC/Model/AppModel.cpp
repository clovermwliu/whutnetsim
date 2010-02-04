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


#include "AppModel.h"


AppModel::AppModel(void)
//描述：构造函数，空
{
	SetLastError(ERROR_MODEL_SUCCESS);
}

AppModel::AppModel(string StrScriptCache):bprivateCache(true)
//根据ConfigCachePath路径构造配置缓存，指针赋给pConfigCache 
{
	CFileScript *f = new CFileScript(StrScriptCache);

	if (!f->LoadFile())
	{
		cout << f->GetLastError()<<":"<<f->GetLastErrorIdentify()<<":"<< f->GetLastErrorEx()<<"\n"<<endl;
	}
	else
	{
		pScriptCache = f;
	}
	SetLastError(ERROR_MODEL_SUCCESS);
}


AppModel::~AppModel(void)
//描述：析构函数
{
	if (bprivateCache)//类内部创建的配置缓存对象，应销毁
	{
		//销毁pConfigCache
		delete pScriptCache;

	}
}


void
AppModel::AttachNode(Node *n)
/*
描述：将结点与模型对象和相关的自定义部件进行绑定，
备注：所有继承该方法的子类应先调用这个方法
*/
{
	node = n;
	//遍历ElementIndex中需要绑定结点的自定义部件
	//绑定到和结点相关的自定义部件上
	map<int, ElementStruct>::iterator it = ElementIndex.begin();
	while(it != ElementIndex.end())
	{
		if ((it->second).RevelantToNode)//需要绑定结点到自定义部件
		{
			(it->second).thisElement.AttachNode(n);
		}
		++it;
	}

}


Size_t 
AppModel::AddElement(int id, bool revelantToNode, string strSectionName)
//描述：增加一个自定义部件到ElementIndex，返回当前ElementIndex中剩余的Element个数
{
	//首先看该id是否已存在
	if (ElementIndex.count(id))
	{
		SetLastError(ERROR_MODEL_INSERT_EXIST);
		return ElementIndex.size();
	}

	CElementCustom ce;

	if(!pScriptCache->InitCustomElementBySectionName(strSectionName, ce))
	{
		SetLastError(ERROR_MODEL_WHEN_INIT_CUSTOMELEMENT);
		return ElementIndex.size();
	}

	ElementStruct eStruct;
	eStruct.ID = id;
	eStruct.RevelantToNode = revelantToNode;
	eStruct.thisElement = ce;

	ElementIndex.insert(make_pair(id, eStruct));

	return ElementIndex.size();
}

Size_t 
AppModel::DeleteElement(int id)
//描述：删除ID为id的自定义部件，返回当前ElementIndex中剩余的Element个数
{
	int flag = ElementIndex.erase(id);
	if (!flag)
		SetLastError(ERROR_MODEL_DELETE_NOT_EXIST);

	return ElementIndex.size();
}

double 
AppModel::GetElementValueById(int id)
//描述：调用自定义部件类的相关方法，获得ID为id的自定义部件的值，并返回
{
	map<int, ElementStruct>::iterator it = ElementIndex.find(id);
	if (it != ElementIndex.end())
	{
		return it->second.thisElement.GetValue();
	}
	else
	{
		SetLastError(ERROR_MODEL_GET_NOT_EXIST);
		return -1;
	}
}

string 
AppModel::GetLastErrorEx()
/*
#define  ERROR_MODEL_SUCCESS                         ERROR_NO_ERROR
#define  ERROR_MODEL_INSERT_EXIST                    0x00050201
#define  ERROR_MODEL_DELETE_NOT_EXIST                0x00050202
#define  ERROR_MODEL_GET_NOT_EXIST                   0x00050203
*/
{
	switch (GetLastError())
	{
	case ERROR_MODEL_SUCCESS:
		return "Model successfully";
	case ERROR_MODEL_INSERT_EXIST:
		return "When insert by id, this element_id already exist";
	case ERROR_MODEL_DELETE_NOT_EXIST:
		return "When delete by id, this element_id does not exist";
	case ERROR_MODEL_GET_NOT_EXIST:
		return "When get by id, this element_id does not exist";
	case ERROR_MODEL_WHEN_INIT_CUSTOMELEMENT:
		return "Error when init custom element";

	default:
		return "UNKNOWN";
	}

}


