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


#include "Task.h"
#include "simulator.h"
//#include "G_common_defs.h"
#include "ErrorHandler.h"
#include "ratetimeparse.h"
#include "FileConfig.h"
#include "FileScript.h"
#include "TopoFileScript.h"
#include "AppFileScript.h"
#include "PlatTopoBase.h"
#include "HiberTopoBase.h"
////////////
#include "application-tcpsend.h"
#include "application-tcpserver.h"



using namespace std;

CTask::CTask(Time_t t, const string& inipath):iniFilepath(inipath),runtime(t)
/*
���캯����
����һ�����ÿ�ʼ��ʾ��ͼ��ʱ�䣨ͨ��Ϊ0��
�������������ָ��
�����������������ļ���·��
�����ģ�Ӧ�������ļ���·��
*/
{

	s = new Simulator();

	cpt = nil;
	cht = nil;
}




CTask::~CTask()
{
	//�ű����������
	Script_Vec::iterator iter;
	for ( iter=scritpVec.begin(); iter!=scritpVec.end(); iter++ )
	{
		delete *iter;
	}
	//������������
	s->~Simulator();
	delete s;
}	


int CTask::Start()
{

	////��ȡTask�����Ϣ
	//if ( InitTask() != ERROR_TASK_SUCCESS )
	//	return GetLastError();

	//��ȡ���������ļ������ڵ㡢�ߡ��Լ��ڵ�IP�����ã����������ˣ����������ļ���Ϣ��������Ӧ���ˣ�
	if ( InitTopo() != ERROR_TASK_SUCCESS )
		return GetLastError();


	//��ȡӦ�������ļ�����Ӧ�����͡�Ӧ�����ԡ���λ�õȣ��������˽���Ӧ�õ�����
	if ( InitApp() != ERROR_TASK_SUCCESS )
		return GetLastError();


	//����simulator����ʼ����
	if ( RunSimulation() != ERROR_TASK_SUCCESS )
		return GetLastError();


/*
	Simulator os;
	Node* n1 = new Node();
	Node* n2 = new Node();
	n1->SetLocation(0,0);
	n2->SetLocation(1,0);
#ifdef HAVE_QT
	n1->Color(Qt::blue);
	n2->Color(Qt::red);
#endif

	Linkp2p link(Rate("1Mb"), Time("10ms"));

	n1->AddDuplexLink(n2, link, 
		IPAddr("192.168.1.1"), Mask(32),
		IPAddr("192.168.1.2"), Mask(32));
	TCPServer* server = (TCPServer*)n1->AddApplication(TCPServer());
	server->BindAndListen(80); // Bind to port 80
	TCPSend*   client = (TCPSend*)n2->AddApplication(
		TCPSend(n1->GetIPAddr(), 80, Constant(10000)));
	client->Start(0);  // Start the client at time 0
	s.StopAt(10.0);

	s.AnimationUpdateInterval(Time("10us")); // 10us initial update rate
	s.StartAnimation(0, true);               // Start the animation at time 0

	s.Run();                                 // Run the simulation
	std::cout << "Simulation Complete" << std::endl;
	s.PrintStats();
	*/



	return GetLastError();
}

int CTask::InitTask()
{
	list<string> v;
	int num;

	CFileConfig iniFile(iniFilepath);
	if ( iniFile.LoadFile() != SUCCESS_NO_ERROR )
	{
		//�����ļ���ȡ������
		SetLastError(ERROR_TASK_FILENOTEXSIT);
		return 0;
	}

	CGenericConfigItem<string> _name(iniFile,"Total","name");
	name = _name.MyValue();
	CGenericConfigItem<string> _owner(iniFile,"Total","owner");
	owner = _owner.MyValue();
	CGenericConfigItem<string> _createTime(iniFile,"Total","createTime");
	createTime = _createTime.MyValue();
	CGenericConfigItem<string> _modifyTime(iniFile,"Total","modifyTime");
	modifyTime = _modifyTime.MyValue();
	CGenericConfigItem<string> _description(iniFile,"Total","description");
	description = _description.MyValue();
	CGenericConfigItem<string> _style(iniFile,"Total","style");
	style = _style.MyValue();
	CGenericConfigItem<string> _topoScriptpath(iniFile,"Total","topoScriptpath");
	topoScriptpath = _topoScriptpath.MyValue();
	CGenericConfigItem<string> _appScriptpath(iniFile,"Total","appScriptpath");
	appScriptpath = _appScriptpath.MyValue();

	return GetLastError();
}

int CTask::InitTopo()
//��ȡ���������ļ������ڵ㡢�ߡ��Լ��ڵ�IP�����ã����������ˣ����������ļ���Ϣ��������Ӧ���ˣ�
{
	if ( style == "Platopo" )
	{
		CTopoFileScript* topofile = new CTopoFileScript(topoScriptpath);
		topofile->LoadFile();
		topofile->CreatePlatTopo(cpt);
		cpt->SetLocationViaBoundBox(Location(0,0),Location(300,300));
		scritpVec.push_back(topofile);

	} 
	else if ( style == "Hibertopo" )
	{
		CTopoFileScript* topofile = new CTopoFileScript(topoScriptpath);
		topofile->LoadFile();
		topofile->CreateHiberTopo(cht);
		scritpVec.push_back(topofile);
	}
	
	return GetLastError();
}


int CTask::InitApp()
{
	CAppFileScript* appfile = new CAppFileScript(appScriptpath);
	appfile->LoadFile();

	appfile->ReadAppbyInifile();
	scritpVec.push_back(appfile);

	return GetLastError();
}

int CTask::RunSimulation()
{
	s->AnimationUpdateInterval(Time("10us")); //���ó�ʼ����ˢ��ʱ��
	s->StartAnimation(runtime);  //������ʾ��ʼʱ��
	s->Run(); //��ʼ����

	return GetLastError();
}


Error_str CTask::GetLastErrorEx()
//������
{
	switch ( GetLastError() )
	{
	case ERROR_TASK_FILENOTEXSIT:
		return "INIFILE NOT EXIST!";
	case ERROR_TASK_UNTOPOLOGYSTYLE:
		return "TOPOLOGY TYPE UNKNOWN!";

	}





}