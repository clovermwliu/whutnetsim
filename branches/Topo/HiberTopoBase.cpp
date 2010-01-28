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

#define DomainNum       6
#define NodesInDomain   7
#define NodesInStub     9
#define NodesInLan      10
#include "HiberTopoBase.h"
#include "duplexlink.h "
CHiberTopoBase::CHiberTopoBase(IPAddr_t i,
							   const Linkp2p& link1 ,
							   SystemId_t id1)
:CErrorHandler(),ip(i),link(link1),sid(id1)/*,configFile("")*/
/*
描述：层次拓扑的基类构造函数        
参数：[IN] i     ：拓扑节点的基IP
      [IN] link1 ：拓扑的节点间的连接方式
      [IN] id1    ：分布式系统标识符 
返回值：空                                                                                       
备注：目前的层次拓扑有TS 
*/
{
}
CHiberTopoBase::~CHiberTopoBase(void)
/*
描述：层次拓扑的析构函数、此函数执行完，Node类中的nodes并没有释放
*/
{

}
void CHiberTopoBase::AutoSetTopoIP()
/*
描述：层次拓扑自动设置IP       
参数：无
返回值：无                                                                                 
备注：实现方法，三层。
      第一层 很多个域，Domain1、Domain2、Domain3，域占最高位的DomainNum位
	  对每个域遍历，域中的节点占接下来的NodesInDomain位
	  第二层，先找到管这一层的路由节点，然后根据路由节点的IP，对这一层的节点赋IP，
	  占接下来的NodesInStub位
	  第三层，和第二层很类似，找到管这一层的stub，然后根据上一层的节点的IP，对这一层的节点赋IP，
	  占最后的NodesInLan位
*/
{
	IPAddr_t  newIp;
	IPAddr_t  ip;
	IPAddr_t stubIp,getUpIp;
	//第一层
	for(size_t  domain = 0;domain!=transitTopoVec.size();domain++)
	{
		ip = 0;
		ip |=(domain+1)<<DomainNum;
		CPlatTopoBase* needSetTopo = transitTopoVec[domain];
		IPAddr_t  setIp;
		for (size_t domainNum = 1;domainNum<=needSetTopo->NodeCount();domainNum++)
		{
			setIp = ip|(domainNum<<NodesInDomain);
			needSetTopo->GetNode(domainNum-1)->SetIPAddr(setIp);
			setIp = 0;
		}
	}
	//第二层
	CPlatTopoBase* needSetStubTopo;

	for (size_t stub = 0;stub!=(stubTopoVec.size());stub++)
	{
		needSetStubTopo = stubTopoVec[stub];
		if(needSetStubTopo->routerConnect.size()==0) continue;

		Node* up = Node::GetNode(needSetStubTopo->routerConnect[0].nodeNum);//一个节点可以有多个路由节点吗
		getUpIp  = up->GetIPAddr();
		for (size_t stubNodeNum = 1;stubNodeNum<=needSetStubTopo->NodeCount();stubNodeNum++)
		{
			stubIp = getUpIp|(stubNodeNum<<NodesInStub);
			needSetStubTopo->GetNode(stubNodeNum-1)->SetIPAddr(stubIp);
			stubIp = 0;
		}
	}
	//第三层
	CPlatTopoBase* LanTopo;
	for (size_t lan = 0;lan<lanTopoVec.size();lan++)
	{
		LanTopo = lanTopoVec[lan];
		if(LanTopo->routerConnect.size()==0) continue;
		Node* up = Node::GetNode(LanTopo->routerConnect[0].nodeNum);
		getUpIp  = up->GetIPAddr();
		for (size_t lanNodeNum = 1;lanNodeNum<=LanTopo->NodeCount();lanNodeNum++)
		{
			stubIp = getUpIp|(lanNodeNum<<NodesInLan);
			LanTopo->GetNode(lanNodeNum-1)->SetIPAddr(stubIp);
			stubIp = 0;
		}
	}
}
Interface* CHiberTopoBase::AddDuplexLink(Node* ln,Node* rn, const Linkp2p& l)
/*
描述：在两个指定的结点之间添加一条边      
参数：[IN] ln  ：需要连接的第一个节点的指针
      [IN] rn  ：需要连接的第二个节点的指针
      [IN] l   ：两个节点的连接方式
返回值：连接时的接口                                                                                       
备注： 
*/
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
返回：是否连接成功
备注：连接的过程中，会记录连接的信息
      同一层相连，则，连接信息记录在brotherConnect中
	  下一层和上一层相连，出错
	  上一层和下一层相连，连接信息上一层记录在controlConnect，下一层记录在routerConnect中
*/
{
	Node* left;
	Node* right;
	ConnectInfo connleft;
	connleft.ItselfId = topo2NodeId;
	connleft.lay = layer1;
	connleft.topoNum = numOfTopo1;
	connleft.nodeNum = topo1NodeId;
	ConnectInfo connright;
	connright.ItselfId = topo1NodeId;
	connright.lay = layer2;
	connright.topoNum = numOfTopo2;
	connright.nodeNum = topo2NodeId;
	switch(layer1)
	{
	case 1:
		if (numOfTopo1 <transitTopoVec.size())
		{
			left = transitTopoVec[numOfTopo1]->GetNode(topo1NodeId);
			SetLastError(SUCCESS_HIBERTOPO);
			if (layer1 == layer2)
			{
				transitTopoVec[numOfTopo1]->brotherConnect.push_back(connright);
			}
			else if (layer1 < layer2)
			{
				transitTopoVec[numOfTopo1]->controlConnect.push_back(connright);
				stubTopoVec[numOfTopo2]->routerConnect.push_back(connleft);
			}
			else {return false;}
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
			if (layer1 == layer2)
			{
				stubTopoVec[numOfTopo1]->brotherConnect.push_back(connright);
			}
			else if (layer1 < layer2)
			{
				stubTopoVec[numOfTopo1]->controlConnect.push_back(connright);
				lanTopoVec[numOfTopo2]->routerConnect.push_back(connleft);
			}
			else {return false;}
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

			if (layer1 == layer2)
			{
				lanTopoVec[numOfTopo1]->brotherConnect.push_back(connright);
			}
			else {return false;}
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
			if (layer1 == layer2)
			{
				transitTopoVec[numOfTopo2]->brotherConnect.push_back(connright);
			}
			else if (layer1 < layer2)
			{
				transitTopoVec[numOfTopo2]->controlConnect.push_back(connright);
			}
			else {return false;}
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
			if (layer1 == layer2)
			{
				stubTopoVec[numOfTopo2]->brotherConnect.push_back(connright);
			}
			else if (layer1 < layer2)
			{
				stubTopoVec[numOfTopo2]->controlConnect.push_back(connright);
			}
			else {return false;}
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
			if (layer1 == layer2)
			{
				lanTopoVec[numOfTopo2]->brotherConnect.push_back(connright);
			}
			else {return false;}
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

//关于IP没有考虑
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