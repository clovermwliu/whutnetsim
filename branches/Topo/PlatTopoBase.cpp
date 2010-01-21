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
//更改时间：2010-1-4

#include "PlatTopoBase.h"
#include <time.h>

Count_t    CPlatTopoBase::index=0;
CPlatTopoBase::CPlatTopoBase(Count_t  n,
							 IPAddr_t i,
							 const Linkp2p& link1 ,
							 SystemId_t id1)
:CErrorHandler(),nodeCount(n),topoattribute(nil),ip(i),link(link1),sid(id1)
{
	srand((int)time(0));//以后在main中设计
}

CPlatTopoBase::~CPlatTopoBase(void)
{
	delete topoattribute;
}
Node* CPlatTopoBase::GetNode(Count_t c)
{ // Get specified  node
	if (c >= nodeCount) return nil;
	return Node::GetNode(first + c);
}
bool  CPlatTopoBase::AddLinkBetweenTwoNodes(Count_t firstNodeId, 
											Count_t secondNodeId, 
											const Linkp2p& link)  //在两个指定的结点之间添加一条边
{
	if ((firstNodeId<nodeCount)&&(secondNodeId<nodeCount))
	{
		Node* firstNode=Node::nodes[firstNodeId];
		Node* secondNode=Node::nodes[secondNodeId];
		firstNode->AddDuplexLink(secondNode, link);
        
		SetLastError(SUCCESS_PLATTOPO);
		return true;
	}
	
	SetLastError(ERROR_ID_OUT_OF_NODECOUNT_FAIL);
	return false;
}
bool  CPlatTopoBase::SetHook(pHook f)  //设钩子
{
	hook = f;
	SetLastError(SUCCESS_PLATTOPO);
	return true;
}
bool  CPlatTopoBase::CallHook()  //设钩子
{
	//hook=f;
	SetLastError(SUCCESS_PLATTOPO);
	return true;
}
bool  CPlatTopoBase::DeleteHook()  //设钩子
{
	hook = 0;
	return true;
}
Node* CPlatTopoBase::Begin()          //指向第一个结点
{
	if (nodeCount==0)
	{
		
		return NULL;
	}
	else
	{
		index=0;
		return  Node::nodes[index++];
	}
}
Node* CPlatTopoBase::Next()           //指向下一个结点 
{
	if (index==nodeCount)
	{
		return NULL;
	}
	else
	{
		return  Node::nodes[index++];
	}
}
bool  CPlatTopoBase::End()           //是否到达末尾
{
	if (index==nodeCount)
		return false;                 //index指向需要读取数据的下一个位置
	else
		return true;
}
int CPlatTopoBase::random(int MAX)
{
	//
	int i=rand()%MAX;

	//int i=srand((int)time(0))%MAX;
	return i;
}
double CPlatTopoBase::random()//产生0到1之间的小数
{
	//
	double d=((double)rand())/RAND_MAX;  

	//int i=srand((int)time(0))%MAX;
	return d;
}