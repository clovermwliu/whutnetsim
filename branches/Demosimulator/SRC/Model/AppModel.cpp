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
//���������캯������
{
	SetLastError(ERROR_MODEL_SUCCESS);
}

AppModel::AppModel(string StrScriptCache):bprivateCache(true)
//����ConfigCachePath·���������û��棬ָ�븳��pConfigCache 
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
//��������������
{
	if (bprivateCache)//���ڲ����������û������Ӧ����
	{
		//����pConfigCache
		delete pScriptCache;

	}
}


void
AppModel::AttachNode(Node *n)
/*
�������������ģ�Ͷ������ص��Զ��岿�����а󶨣�
��ע�����м̳и÷���������Ӧ�ȵ����������
*/
{
	node = n;
	//����ElementIndex����Ҫ�󶨽����Զ��岿��
	//�󶨵��ͽ����ص��Զ��岿����
	map<int, ElementStruct>::iterator it = ElementIndex.begin();
	while(it != ElementIndex.end())
	{
		if ((it->second).RevelantToNode)//��Ҫ�󶨽�㵽�Զ��岿��
		{
			(it->second).thisElement.AttachNode(n);
		}
		++it;
	}

}


Size_t 
AppModel::AddElement(int id, bool revelantToNode, string strSectionName)
//����������һ���Զ��岿����ElementIndex�����ص�ǰElementIndex��ʣ���Element����
{
	//���ȿ���id�Ƿ��Ѵ���
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
//������ɾ��IDΪid���Զ��岿�������ص�ǰElementIndex��ʣ���Element����
{
	int flag = ElementIndex.erase(id);
	if (!flag)
		SetLastError(ERROR_MODEL_DELETE_NOT_EXIST);

	return ElementIndex.size();
}

double 
AppModel::GetElementValueById(int id)
//�����������Զ��岿�������ط��������IDΪid���Զ��岿����ֵ��������
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


