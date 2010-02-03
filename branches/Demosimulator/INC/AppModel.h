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
//��������
//////////////////////////////////////////////////////////////////////////
public:
	//���캯������������
	AppModel(void);

	~AppModel(void);

	AppModel(CFileScript &f):pScriptCache(&f){ SetLastError(ERROR_MODEL_SUCCESS); }//ֱ��ָ�����û������

	AppModel(string StrScriptCache); //����ConfigCachePath·���������û��棬ָ�븳��pConfigCache 

	virtual AppModel* Copy() const = 0;//���������ص�ǰ����Ŀ��������麯�������������������ʵ��

	virtual bool GetModelValue(void **p) = 0;//����������ģ�͵�ֵ�����麯�������������������ʵ��

	virtual void Initialize(void *p) = 0;//��������ʼ��ģ����ز��������麯�������������������ʵ��

private:
	CFileScript *pScriptCache;  //ģ�ʹ�ȡ�����ļ��õ����û�����󣬾����캯�����븳ֵ

	bool bprivateCache; //�����Model���Լ���������û��棬��Ϊtrue������Ϊfalse



//////////////////////////////////////////////////////////////////////////
//�ͽ���
//////////////////////////////////////////////////////////////////////////


protected:
	Node *node;//�󶨵Ľ��ָ�룬�е����಻��Ҫ

public:
	virtual void AttachNode(Node *n);//���м̳и÷���������Ӧ��ʵ���������




//////////////////////////////////////////////////////////////////////////
//�Զ��岿�����
//////////////////////////////////////////////////////////////////////////
private:

	struct ElementStruct
	{
		int ID;//����ָ���ĸ��Զ��岿����id
		bool RevelantToNode;//���Զ��岿���Ƿ�ͽ�����
		CElementCustom thisElement;//�Զ��岿������
	};//һ���Զ��岿��

	map<int, ElementStruct> ElementIndex;//�洢�Զ��岿���������Ϣ

protected:

	//Size_t GetElementSize() { return ElementIndex.size(); }

    //����������һ���Զ��岿����ElementIndex�����ص�ǰElementIndex�е�Element����
	Size_t AddElement(int id, bool revelantToNode, string strSectionName);
	

	//������ɾ��IDΪid���Զ��岿�������ص�ǰElementIndex��ʣ���Element����
	Size_t DeleteElement(int id);

	//�����������Զ��岿�������ط��������IDΪid���Զ��岿����ֵ��������
	double GetElementValueById(int id);



//////////////////////////////////////////////////////////////////////////
///���������
//////////////////////////////////////////////////////////////////////////

	virtual Error_str GetLastErrorEx();	


};


#endif