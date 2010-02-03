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

//更改人：李玉
//更改时间：2010-1-28

#include "PlatTopoBase.h"
#include <time.h>

Count_t    CPlatTopoBase::index=0;
CPlatTopoBase::CPlatTopoBase(Count_t  n,
							 IPAddr_t i,
							 const Linkp2p& link1 ,
							 SystemId_t id1)
:CErrorHandler(),nodeCount(n),topoattribute(nil),ip(i),link(link1),sid(id1)
/*
描述：平面拓扑的基类构造函数      
参数：[IN] n     ：拓扑的节点数目
	  [IN] i     ：拓扑节点的基IP
      [IN] link1 ：拓扑的节点间的连接方式
      [IN] id    ：分布式系统标识符 
返回值：空                                                                                       
备注：派生类有BA、Clique、Inet3、PFP、Ring、Star、Waxman、Ws     
*/
{
	srand((int)time(0));//以后在main中设计
}

CPlatTopoBase::~CPlatTopoBase(void)
/*
描述：平面拓扑的析构函数、将拓扑分析属性分配的空间释放
*/
{
	delete topoattribute;
}

Node* CPlatTopoBase::GetNode(Count_t c)
/*
描述：获得平面拓扑中的第c个节点，第c个节点node的ID不一定是c，而是已经加上了本拓扑的firstid         
参数：[IN] c ：拓扑中的第几个节点
返回值：拓扑的第c个节点的指针                                                                                       
备注： 
*/
{ // Get specified  node
	if (c >= nodeCount) return nil;
	return Node::GetNode(first + c);
}

bool  CPlatTopoBase::AddLinkBetweenTwoNodes(Count_t firstNodeId, 
											Count_t secondNodeId, 
											const Linkp2p& link) 
/*
描述：在两个指定的结点之间添加一条边      
参数：[IN] firstNodeId  ：需要连接的第一个节点
	  [IN] secondNodeId ：需要连接的第二个节点
	  [IN] link         ：两个节点的连接方式
返回值：是否连接成功                                                                                       
备注： 
*/
{
	if ((firstNodeId<nodeCount)&&(secondNodeId<nodeCount))
	{
		Node* firstNode=GetNode(firstNodeId);
		if (firstNode==nil)
		{
			SetLastError(ERROR_ID_OUT_OF_NODECOUNT_FAIL);
			string tmp="Link:Get first node error";
			SetLastErrorStr(tmp);
			return false;
		}

		Node* secondNode=GetNode(secondNodeId);
		if (secondNode==nil)
		{
			SetLastError(ERROR_ID_OUT_OF_NODECOUNT_FAIL);
			string tmp="Link:Get second node error";
			SetLastErrorStr(tmp);
			return false;
		}
		firstNode->AddDuplexLink(secondNode, link);
        
		SetLastError(SUCCESS_PLATTOPO);
		return true;
	}

	SetLastError(ERROR_ID_OUT_OF_NODECOUNT_FAIL);
	string tmp="Link：nodeNum out of vector!";
	SetLastErrorStr(tmp);
	return false;
}

bool  CPlatTopoBase::SetHook(pHook f)  
/*
描述：设置钩子      
参数：[IN] f  ：设置的钩子
返回值：钩子是否设置成功                                                                                       
备注： 
*/
{
	hook = f;
	SetLastError(SUCCESS_PLATTOPO);
	return true;
}

bool  CPlatTopoBase::CallHook()  
/*
描述：调用钩子      
参数：无
返回值：钩子是否调用成功                                                                                       
备注： 
*/
{
	//hook=f;
	SetLastError(SUCCESS_PLATTOPO);
	return true;
}
bool  CPlatTopoBase::DeleteHook()  
/*
描述：删除钩子      
参数：无
返回值：钩子是否删除成功                                                                                       
备注： 
*/
{
	hook = 0;
	return true;
}
Node* CPlatTopoBase::Begin()          
/*
描述：指向第一个结点      
参数：无
返回值：第一个节点的指针                                                                                      
备注： 
*/
{
	if (nodeCount==0)
	{
		return NULL;
	}
	else
	{
		index=0;
		return  GetNode(index++);
	}
}
Node* CPlatTopoBase::Next()           
/*
描述：指向下一个结点      
参数：无
返回值：下一个节点的指针                                                                                      
备注： 
*/
{
	if (index==nodeCount)
	{
		return NULL;
	}
	else
	{
		return  GetNode(index++);
	}
}
bool  CPlatTopoBase::End()           //
/*
描述：是否到达末尾      
参数：无
返回值：是否还有节点                                                                                  
备注：index指向需要读取数据的下一个位置
*/
{
	if (index==nodeCount)
		return false;                 
	else
		return true;
}
int CPlatTopoBase::random(int MAX)
/*
描述：产生最大值是Max的随机数     
参数：[IN] MAX ：最大的值，即指定了范围
返回值：是否执行成功                                                                                  
备注：今后会统一用一套随机数方案
*/
{
	//
	int i=rand()%MAX;

	//int i=srand((int)time(0))%MAX;
	return i;
}
double CPlatTopoBase::random()
/*
描述：产生0到1之间的小数   
参数：无
返回值：产生的0-1之间的小数                                                                                
备注：今后会统一用一套随机数方案
*/
{
	//
	double d=((double)rand())/RAND_MAX;  

	//int i=srand((int)time(0))%MAX;
	return d;
}
