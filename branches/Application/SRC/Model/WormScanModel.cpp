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


#include "WormScanModel.h"
#include <math.h>


WormScanModel::WormScanModel(Count_t range, string StrScriptCache) 
: initialized(false), scanrange(range), localIP(IPADDR_NONE), AppModel(StrScriptCache)
/*
���������캯�����������û���·���������û������
*/
{

}

WormScanModel::WormScanModel(Count_t range, CFileScript &f) 
: initialized(false), scanrange(range), localIP(IPADDR_NONE), AppModel(f)
/*
���������캯����ֱ��ָ�����û������
*/
{

}

WormScanModel::WormScanModel(Mask m, CFileScript &f):AppModel(f)
/*
���������캯����ֱ��ָ�����û������
*/
{
	scanrange = 1<<(32-m.NBits());
}

WormScanModel::WormScanModel(Mask m,string StrScriptCache):AppModel(StrScriptCache)
/*
���������캯�����������û���·���������û������
*/
{
	scanrange = 1<<(32-m.NBits());
}

AppModel* 
WormScanModel::Copy() const
//���������ظö���Ŀ���
{
	return new WormScanModel(*this);
}

bool
WormScanModel::GetModelValue(void **p) 
/*
����������ģ�͵�ֵ�����麯��
������[IN][OUT]p��void*ָ���ַ����������ó���ģ�͵�ֵ
��ע���ڸ�ģ���У�pΪIPAddr_t*�ĵ�ַ�����ڴ�����һʱ��Ӧ��ɨ���IP��ַ��
      �����������Ӧ�ȵ����������
*/
{
	
	if (!initialized)
	{
		cout<<"TargetVector Not Initialized!!"<<endl;
		return false;
	}

	return true;
}

void 
WormScanModel::Initialize(void *p)
/*
��������ʼ��ģ����ز���
������[IN]p��void*ָ�룬�����ɵ��ø÷�������ָ��
��ע���ڸ�ģ���У�pΪIPAddr_t*���͵�
*/
{
	localIP = *(IPAddr_t*)p;
	initialized = true;
}




////////////////////////////////////////////////////////////////////////
///////������
//////////////////////////////////////////////////////////////////////////
//���캯��
DynamicVulPrefScan::DynamicVulPrefScan(Count_t range,
									   vector<double> *InDistribute,
									   CFileScript &f
									   ):pVulDistribute(InDistribute),
									   WormScanModel(range, f)
/*
���������캯����ֱ��ָ�����û������
*/

{
	GroupNum = pVulDistribute->size();
	GroupSize = scanrange/GroupNum;
	id = 0;

}

DynamicVulPrefScan::DynamicVulPrefScan(Count_t range,
									   vector<double> *InDistribute,
									   string StrScriptCache
									   ):pVulDistribute(InDistribute),
									   WormScanModel(range, StrScriptCache)
/*
���������캯�����������û���·���������û������
*/
{
	GroupNum = pVulDistribute->size();
	GroupSize = scanrange/GroupNum;
	id = 0;
}


DynamicVulPrefScan::DynamicVulPrefScan(Mask m,
									   vector<double> *InDistribute,
									   CFileScript &f
									   ):pVulDistribute(InDistribute),
									   WormScanModel(m, f)
/*
���������캯����ֱ��ָ�����û������
*/
{
	GroupNum = pVulDistribute->size();

	GroupSize = (pow((double)2, (double)(m.NBits()))-1)/GroupNum;
	id = 0;

}

DynamicVulPrefScan::DynamicVulPrefScan(Mask m,
									   vector<double> *InDistribute,
									   string StrScriptCache
									   ):pVulDistribute(InDistribute),
									   WormScanModel(m, StrScriptCache)
/*
���������캯�����������û���·���������û������
*/
{
	GroupNum = pVulDistribute->size();
	GroupSize = (pow((double)2, (double)(m.NBits())-1))/GroupNum;
	id = 0;
}


//

AppModel* 
DynamicVulPrefScan::Copy() const
//���������ظö���Ŀ���
{
	return new DynamicVulPrefScan(*this);
}

bool
DynamicVulPrefScan::IsValidVulDistribute()
//�����������������ֲ��Ƿ�Ϸ�����֮���Ƿ�Ϊ1
{
	double sum = 0;
	for (int i = 0; i < (int)GroupNum; ++i)
	{
		sum += pVulDistribute->at(i);
	}
	if (sum != 1)
		return false;

	return true;
}

void 
DynamicVulPrefScan::Initialize(void *p)
/*
��������ʼ��ģ����ز���
������[IN]p��void*ָ�룬�����ɵ��ø÷�������ָ��
��ע���ڸ�ģ���У�pΪIPAddr_t*���͵�
*/
{
	WormScanModel::Initialize(p);

	////������ݺϷ���
	//if (!IsValidVulDistribute())
	//{
	//	//������
	//	cout << "���ݲ��Ϸ�!" << endl;
	//	return;
	//}

	//�����Զ��岿��,��ǰģ��ֵ����һ���Զ��岿��
	AppModel::AddElement(id, false, CUSTOM_ELEMENT_SECTION);
	if (AppModel::GetLastError() != ERROR_MODEL_SUCCESS)
	{
		//������
		SetLastError(ERROR_MODEEL_ADD_ELEMENT_ID_EXIST);
		return;
	}
	
}

bool 
DynamicVulPrefScan::GetModelValue(void **p)
/*
����������ģ�͵�ֵ�����麯��
������[IN][OUT]p��void*ָ���ַ����������ó���ģ�͵�ֵ
��ע���ڸ�ģ���У�pΪIPAddr_t*�ĵ�ַ�����ڴ�����һʱ��Ӧ��ɨ���IP��ַ
*/
{
	if (!WormScanModel::GetModelValue(p))
	{
		cout<<"TargetVector Not Initialized!!"<<endl;
		return false;
	}
	//
	
	double prefFactor = AppModel::GetElementValueById(id);//ȡ����ֵΪƫ������
	//����ƫ������ѡ��IP
	IPAddr_t *ip = new IPAddr_t(GenerateIPByPrefFactor(prefFactor));

	string str_ip = IPAddr::ToDotted(*ip);

	*p = ip;

	return true;
}

IPAddr_t
DynamicVulPrefScan::GenerateIPByPrefFactor(double prefFactor)
//����������ƫ���������ѡ����һʱ��Ӧɨ���IP��ַ
{
	vector<double> tmpProDistribute;
    //����ɨ�����ĸ���
	double sum = 0;
	for (int i = 0; i < (int)GroupNum; ++i)
	{
		double tmp = pow(pVulDistribute->at(i), prefFactor);
		sum += tmp;
	}
	for (int i = 0; i < (int)GroupNum; ++i)
	{
		tmpProDistribute.push_back(pow(pVulDistribute->at(i), prefFactor)/sum);
	}
	int GroupIndex = SpecProRandom(tmpProDistribute);

	//GroupIndex = 4;

	//�ڱ�ѡ����������ѡ��ĳһIP����Ϊ����ֵ������һ��ɨ���������IP
	Random * rngT = new Uniform(GroupIndex*GroupSize, (GroupIndex+1)*GroupSize);

	IPAddr_t SelectIP = rngT->IntValue();

	delete rngT;

	//SelectIP = IPAddr::ToIP("4.16.8.3");


	return SelectIP;
}


int
DynamicVulPrefScan::SpecProRandom(vector<double> & pro)
//��������ָ���������ѡ����������ڵ��±꣬�����±꣬�����
{
	//int index = 0;
	Random * rngT = new Uniform();
	double tmp_ran = rngT->Value();
	delete rngT;
	double tmp_sum = 0;
	for (int i = 0; i < (int)pro.size(); ++i)
	{
		tmp_sum += pro[i];
		if (tmp_ran < tmp_sum)
		{
			return i;
		}
	}
	return 0;
}
