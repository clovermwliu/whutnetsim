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

#include "HiberTopoBase.h"
#include "duplexlink.h "
CHiberTopoBase::CHiberTopoBase(IPAddr_t i,
							 const Linkp2p& link1 ,
							 SystemId_t id1)
:CErrorHandler(),ip(i),link(link1),sid(id1)/*,configFile("")*/
{

}
CHiberTopoBase::~CHiberTopoBase(void)
{

}

Interface* CHiberTopoBase::AddDuplexLink(Node* ln,Node* rn, const Linkp2p& l)
{
	DuplexLink dl(ln, l, rn, l);
	return dl.localif;
}
bool  CHiberTopoBase::ConnectDomain(Count_t layer1,
									Count_t numOfTopo1,
									Count_t topo1NodeId,
									Count_t layer2,
									Count_t numOfTopo2,
									Count_t topo2NodeId)
/*
描述：连接两个拓扑（left，right）
参数：[IN]layer1      ：left拓扑的层数
	  [IN]numOfTopo1  ：left拓扑相对应层上的第几个拓扑
	  [IN]topo1NodeId ：相应拓扑的第几个节点
	  [IN]layer2      ：right拓扑的层数
	  [IN]numOfTopo2  ：right拓扑相对应层上的第几个拓扑
	  [IN]topo2NodeId ：相应拓扑的第几个节点
返回值：是否连接成功
*/
{
	Node* left;
	Node* right;
	switch(layer1)
	{
	case 1:
		if (numOfTopo1 <transitTopoVec.size())
		{
			left = transitTopoVec[numOfTopo1]->GetNode(topo1NodeId);
			SetLastError(SUCCESS_HIBERTOPO);
		}
		else 
		{
			SetLastError(ERROR_ID_OUT_OF_LAY1NODECOUNT_FAIL);		
			return false;
		}
		break;

	case 2:
		if (numOfTopo1 <stubTopoVec.size())
		{
			left = stubTopoVec[numOfTopo1]->GetNode(topo1NodeId);
			SetLastError(SUCCESS_HIBERTOPO);
		}
		else 
		{
			SetLastError(ERROR_ID_OUT_OF_LAY2NODECOUNT_FAIL);
			return false;
		}
		break;

	case 3:
		if (numOfTopo1 <lanTopoVec.size())
		{
			left = lanTopoVec[numOfTopo1]->GetNode(topo1NodeId);
			SetLastError(SUCCESS_HIBERTOPO);
		}
		else 
		{
			SetLastError(ERROR_ID_OUT_OF_LAY3NODECOUNT_FAIL);
			return false;
		}
		break;
	default:
		break;
	}
	switch(layer2)
	{
	case 1:
		if (numOfTopo2 <transitTopoVec.size())
		{
			right = transitTopoVec[numOfTopo2]->GetNode(topo2NodeId);
			SetLastError(SUCCESS_HIBERTOPO);
		}
		else 
		{
			SetLastError(ERROR_ID_OUT_OF_LAY1NODECOUNT_FAIL);
			return false;
		}
		break;

	case 2:
		if (numOfTopo2 <stubTopoVec.size())
		{
			right = stubTopoVec[numOfTopo2]->GetNode(topo2NodeId);
			SetLastError(SUCCESS_HIBERTOPO);
		}
		else 
		{
			SetLastError(ERROR_ID_OUT_OF_LAY2NODECOUNT_FAIL);
			return false;
		}
		break;

	case 3:
		if (numOfTopo2 <lanTopoVec.size())
		{
			right = lanTopoVec[numOfTopo2]->GetNode(topo2NodeId);
			SetLastError(SUCCESS_HIBERTOPO);
		}
		else 
		{
			SetLastError(ERROR_ID_OUT_OF_LAY3NODECOUNT_FAIL);
			return false;
		}
		break;

	default:
		break;
	}

	left->AddDuplexLink(right);
	return true;
}

bool  CHiberTopoBase::ConnectDomain(Count_t layer1,
					Count_t  numOfTopo1,
					Count_t layer2,
					Count_t  numOfTopo2)
/*
描述：连接两个拓扑（left，right），随机的选择对应拓扑的内部节点
参数：[IN]layer1      ：left拓扑的层数
	  [IN]numOfTopo1  ：left拓扑相对应层上的第几个拓扑
	  [IN]layer2      ：right拓扑的层数
	  [IN]numOfTopo2  ：right拓扑相对应层上的第几个拓扑
返回值：是否连接成功
*/
{
	Node* left;
	Node* right;
	switch(layer1)
	{
	case 1:
		if (numOfTopo1 <transitTopoVec.size())
		{
			Count_t topo1NodeId = transitTopoVec[numOfTopo1]->random(transitTopoVec[numOfTopo1]->NodeCount());
			left = transitTopoVec[numOfTopo1]->GetNode(topo1NodeId);
			SetLastError(SUCCESS_HIBERTOPO);
		}
		else 
		{
			SetLastError(ERROR_ID_OUT_OF_LAY1NODECOUNT_FAIL);
			return false;
		}
		break;

	case 2:
		if (numOfTopo1 <stubTopoVec.size())
		{
			Count_t topo1NodeId = stubTopoVec[numOfTopo1]->random(stubTopoVec[numOfTopo1]->NodeCount());
			left = stubTopoVec[numOfTopo1]->GetNode(topo1NodeId);
			SetLastError(SUCCESS_HIBERTOPO);
		}
		else 
		{
			SetLastError(ERROR_ID_OUT_OF_LAY2NODECOUNT_FAIL);
			return false;
		}
		break;

	case 3:
		if (numOfTopo1 <lanTopoVec.size())
		{
			Count_t topo1NodeId = lanTopoVec[numOfTopo1]->random(lanTopoVec[numOfTopo1]->NodeCount());
			left = lanTopoVec[numOfTopo1]->GetNode(topo1NodeId);
			SetLastError(SUCCESS_HIBERTOPO);
		}
		else 
		{
			SetLastError(ERROR_ID_OUT_OF_LAY3NODECOUNT_FAIL);
			return false;
		}
		break;
	default:
		break;
	}
	switch(layer2)
	{
	case 1:
		if (numOfTopo2 <transitTopoVec.size())
		{
			Count_t topo2NodeId = transitTopoVec[numOfTopo1]->random(transitTopoVec[numOfTopo1]->NodeCount());
			right = transitTopoVec[numOfTopo2]->GetNode(topo2NodeId);
			SetLastError(SUCCESS_HIBERTOPO);
		}
		else 
		{
			SetLastError(ERROR_ID_OUT_OF_LAY1NODECOUNT_FAIL);
			return false;
		}
		break;

	case 2:
		if (numOfTopo2 <stubTopoVec.size())
		{
			Count_t topo2NodeId = stubTopoVec[numOfTopo1]->random(stubTopoVec[numOfTopo1]->NodeCount());
			right = stubTopoVec[numOfTopo2]->GetNode(topo2NodeId);
			SetLastError(SUCCESS_HIBERTOPO);
		}
		else 
		{
			SetLastError(ERROR_ID_OUT_OF_LAY2NODECOUNT_FAIL);
			return false;
		}
		break;

	case 3:
		if (numOfTopo2 <lanTopoVec.size())
		{
			Count_t topo2NodeId = lanTopoVec[numOfTopo1]->random(lanTopoVec[numOfTopo1]->NodeCount());
			right = lanTopoVec[numOfTopo2]->GetNode(topo2NodeId);
			SetLastError(SUCCESS_HIBERTOPO);
		}
		else 
		{
			SetLastError(ERROR_ID_OUT_OF_LAY3NODECOUNT_FAIL);
			return false;
		}
		break;

	default:
		break;
	}

	left->AddDuplexLink(right);
	return true;
}