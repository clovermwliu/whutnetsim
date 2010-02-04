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
描述：构造函数，根据配置缓存路径创建配置缓存对象
*/
{

}

WormScanModel::WormScanModel(Count_t range, CFileScript &f) 
: initialized(false), scanrange(range), localIP(IPADDR_NONE), AppModel(f)
/*
描述：构造函数，直接指定配置缓存对象
*/
{

}

WormScanModel::WormScanModel(Mask m, CFileScript &f):AppModel(f)
/*
描述：构造函数，直接指定配置缓存对象
*/
{
	scanrange = 1<<(32-m.NBits());
}

WormScanModel::WormScanModel(Mask m,string StrScriptCache):AppModel(StrScriptCache)
/*
描述：构造函数，根据配置缓存路径创建配置缓存对象
*/
{
	scanrange = 1<<(32-m.NBits());
}

AppModel* 
WormScanModel::Copy() const
//描述：返回该对象的拷贝
{
	return new WormScanModel(*this);
}

bool
WormScanModel::GetModelValue(void **p) 
/*
描述：计算模型的值，纯虚函数
参数：[IN][OUT]p：void*指针地址，带出计算得出的模型的值
备注：在该模型中，p为IPAddr_t*的地址，用于带出下一时拍应该扫描的IP地址；
      该类的派生类应先调用这个方法
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
描述：初始化模型相关参数
参数：[IN]p：void*指针，类型由调用该方法的类指定
备注：在该模型中，p为IPAddr_t*类型的
*/
{
	localIP = *(IPAddr_t*)p;
	initialized = true;
}




////////////////////////////////////////////////////////////////////////
///////派生类
//////////////////////////////////////////////////////////////////////////
//构造函数
DynamicVulPrefScan::DynamicVulPrefScan(Count_t range,
									   vector<double> *InDistribute,
									   CFileScript &f
									   ):pVulDistribute(InDistribute),
									   WormScanModel(range, f)
/*
描述：构造函数，直接指定配置缓存对象
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
描述：构造函数，根据配置缓存路径创建配置缓存对象
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
描述：构造函数，直接指定配置缓存对象
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
描述：构造函数，根据配置缓存路径创建配置缓存对象
*/
{
	GroupNum = pVulDistribute->size();
	GroupSize = (pow((double)2, (double)(m.NBits())-1))/GroupNum;
	id = 0;
}


//

AppModel* 
DynamicVulPrefScan::Copy() const
//描述：返回该对象的拷贝
{
	return new DynamicVulPrefScan(*this);
}

bool
DynamicVulPrefScan::IsValidVulDistribute()
//描述：检查脆弱主机分布是否合法，即之和是否为1
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
描述：初始化模型相关参数
参数：[IN]p：void*指针，类型由调用该方法的类指定
备注：在该模型中，p为IPAddr_t*类型的
*/
{
	WormScanModel::Initialize(p);

	////检查数据合法性
	//if (!IsValidVulDistribute())
	//{
	//	//错误处理
	//	cout << "数据不合法!" << endl;
	//	return;
	//}

	//加入自定义部件,当前模型值存在一个自定义部件
	AppModel::AddElement(id, false, CUSTOM_ELEMENT_SECTION);
	if (AppModel::GetLastError() != ERROR_MODEL_SUCCESS)
	{
		//错误处理
		SetLastError(ERROR_MODEEL_ADD_ELEMENT_ID_EXIST);
		return;
	}
	
}

bool 
DynamicVulPrefScan::GetModelValue(void **p)
/*
描述：计算模型的值，纯虚函数
参数：[IN][OUT]p：void*指针地址，带出计算得出的模型的值
备注：在该模型中，p为IPAddr_t*的地址，用于带出下一时拍应该扫描的IP地址
*/
{
	if (!WormScanModel::GetModelValue(p))
	{
		cout<<"TargetVector Not Initialized!!"<<endl;
		return false;
	}
	//
	
	double prefFactor = AppModel::GetElementValueById(id);//取出的值为偏好因子
	//根据偏好因子选择IP
	IPAddr_t *ip = new IPAddr_t(GenerateIPByPrefFactor(prefFactor));

	string str_ip = IPAddr::ToDotted(*ip);

	*p = ip;

	return true;
}

IPAddr_t
DynamicVulPrefScan::GenerateIPByPrefFactor(double prefFactor)
//描述：根据偏好因子随机选择下一时拍应扫描的IP地址
{
	vector<double> tmpProDistribute;
    //计算扫描各组的概率
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

	//在被选择的组里随机选择某一IP，作为返回值，即下一拍扫描的主机的IP
	Random * rngT = new Uniform(GroupIndex*GroupSize, (GroupIndex+1)*GroupSize);

	IPAddr_t SelectIP = rngT->IntValue();

	delete rngT;

	//SelectIP = IPAddr::ToIP("4.16.8.3");


	return SelectIP;
}


int
DynamicVulPrefScan::SpecProRandom(vector<double> & pro)
//描述：按指定概率随机选择各概率所在的下标，返回下标，即组号
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
