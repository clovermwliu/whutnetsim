
#pragma once
//Copyright (c) 2010, Information Security Institute of Wuhan Universtiy(ISIWhu)
//Project Homepage:http://code.google.com/p/whutnetsim/
//corresponding author's email: guochi@mail.whu.edu.cn


//All rights reserved
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
//Construct Date:
//Modify Author:
//Author Organization:
//Modify Date:


//�����ˣ�����
//����ʱ�䣺2010-1-15

#ifndef __TOPOFILE_H__ 
#define __TOPOFILE_H__

#include <fstream> 
#include "G_common_defs.h"
using namespace std;
#include "Waxman.h"
#include "HiberTopoBase.h"
#include "PlatTopoBase.h"
#include "star1.h"

#include "FileConfig.h"
#include "GenericConfigItem.h"
//using namespace WhuTNetSimConfigClass;

class CTopoFileScript :	public CFileConfig
{
	friend class CItemLine;
	template <typename ValueType> friend class CGenericConfigItem;

public:
	CTopoFileScript(const string& _file);
	~CTopoFileScript(void);
public:
	void ReadFile();
	void WriteFile();

	bool ReadAllTopoInfo();
	//����ƺ�Ӧ����main��д?
	bool ReadAllConnectInfo(CHiberTopoBase* Hiber);
	bool ReadOneConnectInfo(CHiberTopoBase* Hiber,string connectType);
	bool ReadOnceWaxmanInfo(const string& PlatType);//����½�һ��ƽ������ʱ����Ϣ
	bool ReadOncePHPInfo(const string& PlatType);
	bool ReadOnceTSInfo(const string& PlatType,CHiberTopoBase*& newPlatTopo);

	bool ReadOnceConnectInfo();//���һ��������Ϣ��Ȼ������

	bool WriteAllTopoInfo();
	bool WriteAllConnectInfo();
	bool WriteOneTopoInfo();//дһ�����˵���Ϣ
	bool WriteOneConnectInfo();//дһ�����˵�������Ϣ


	int  ReadTopoNum();
	bool ReadOneLayerInfo(int _lay,vector<CPlatTopoBase*>&  TopoVec,const string& section);//��ȡһ�����˵���Ϣ

	bool ReadOnceWaxmanInfo(const string& PlatType,CPlatTopoBase*& newPlatTopo);//�½�һ��Waxman����
	bool ReadOncePFPInfo(const string& PlatType,CPlatTopoBase*& newPlatTopo);//�½�һ��PHP������
	bool ReadOnceStarInfo(const string& PlatType,CPlatTopoBase*& newPlatTopo);
	bool ReadOnceSimpleInfo(const string& PlatType,CPlatTopoBase*& newPlatTopo);

	bool CreatePlatTopo(CPlatTopoBase*& newTopo);     //�½�һ��ƽ������
	bool CreateHiberTopo(CHiberTopoBase*& newTopo);    //�½�һ���������
};
#endif
