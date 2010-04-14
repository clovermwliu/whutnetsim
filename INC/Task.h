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

#ifndef __task_h__
#define __task_h__

//#include "G_common_defs.h"
//#include "ErrorHandler.h"
//#include "simulator.h"
//#include "ratetimeparse.h"
#include "FileConfig.h"
//#include "FileScript.h"
//#include "TopoFileScript.h"
//#include "AppFileScript.h"
//#include "PlatTopoBase.h"
//#include "HiberTopoBase.h"

#include <string>
#include <vector>

using namespace std;

typedef std::vector<CFileConfig*> Script_Vec;
//���û�����Ŷ���
#define TOPOCODE 0
#define APPCODE 1

#define  DEFAULT_VALUE                               1
#define  ERROR_TASK_SUCCESS			                 ERROR_NO_ERROR
#define  ERROR_TASK_FILENOTEXSIT                     0x00060101
#define  ERROR_TASK_UNTOPOLOGYSTYLE                  0x00060102


class Simulator;
class CPlatTopoBase;
class CHiberTopoBase;


class CTask : public CErrorHandler
{
public:
	CTask(Time_t, const string&);
public:
	~CTask(void);
	
	virtual Error_str GetLastErrorEx();

	int InitTask();
	int InitTopo();  //��������
	int InitApp();  //�����˽ڵ����Ӧ�õ�����
	int RunSimulation(); //����Simulator
	int Start();  //��ʼ����



	//���ݳ�Ա
public:
	//CTask�Ļ�����Ϣ
	string name;                   //������
	string owner;                  //������
	string createTime;             //����ʱ��
	string modifyTime;             //�޸�ʱ��
	string description;            //��������
	string style;                  //�������ͣ���Ρ�ƽ�棩
	string topoScriptpath;   //�������ýű�·��
	string appScriptpath;    //Ӧ�����ýű�·��

	//CTask����������س�Ա
	const string iniFilepath;      //�����ļ�·��

	Script_Vec scritpVec;   //���ýű��������飨Ŀǰ���õĻ���ű��У����˻��桢Ӧ�û��棩

	Simulator* s;  //�������

	
	CPlatTopoBase* cpt; //ƽ������
	CHiberTopoBase* cht; //�������

	Time_t runtime; //��������ʱ��

	static string currentAppType;

};

#endif

