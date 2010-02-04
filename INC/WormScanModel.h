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


#ifndef __wormscanmodel_h__
#define __wormscanmodel_h__


#include "AppModel.h"
#include "ipaddr.h"


class WormScanModel : public AppModel
{
public:
	//构造函数及析构函数
	WormScanModel(Count_t range, string StrScriptCache);

	WormScanModel(Count_t range, CFileScript &f);

	WormScanModel(Mask m, string StrScriptCache);

	WormScanModel(Mask m, CFileScript &f);

public:

	// The following methods are to be overriden for changing the 
	// functionality
	virtual AppModel* Copy() const;

	virtual bool GetModelValue(void **p);//子类应先执行父类的这个函数

	virtual void Initialize(void *p);//子类应先执行父类的这个函数

private:
	bool initialized;//是否已初始化扫描策略模型

protected:
	Count_t scanrange;//扫描范围
	IPAddr_t localIP;//本地IP

};



//////////////////////////////////////////////////////////////////////////
////偏好扫描策略
//////////////////////////////////////////////////////////////////////////

#define CUSTOM_ELEMENT_SECTION "WormDynamicPrefScan"

class DynamicVulPrefScan : public WormScanModel 
{
public:
	////构造函数
	//动态
	DynamicVulPrefScan(Count_t range,
		               vector<double> *InDistribute,
					   CFileScript &f
					   );

	DynamicVulPrefScan(Count_t range,
		               vector<double> *InDistribute,
					   string StrScriptCache
					   );


	DynamicVulPrefScan(Mask m,
		               vector<double> *InDistribute,
					   CFileScript &f
					   );

	DynamicVulPrefScan(Mask m,
		               vector<double> *InDistribute,
					   string StrScriptCache
					   );


	virtual AppModel* Copy() const;

	virtual bool GetModelValue(void **p);

	virtual void Initialize(void *p);

private:

	vector<double> * pVulDistribute;//脆弱主机分布
//
	Size_t GroupNum;//网络被分成的组数

	int GroupSize;//每组的大小，各组大小相同

	int id;//当前自定义部件id

	int SpecProRandom(vector<double> & pro);

	bool IsValidVulDistribute();

	IPAddr_t GenerateIPByPrefFactor(double prefFactor);

};



#endif