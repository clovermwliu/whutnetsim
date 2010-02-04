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
//����ʱ�䣺2010-1-28

#define DomainNum       6
#define NodesInDomain   7
#define NodesInStub     9
#define NodesInLan      10
#include "HiberTopoBase.h"
#include "duplexlink.h "
#include "node.h"
#include "bfs.h"
CHiberTopoBase::CHiberTopoBase(IPAddr_t i,
							   const Linkp2p& link1 ,
							   SystemId_t id1)
:CErrorHandler(),ip(i),link(link1),sid(id1)/*,configFile("")*/
/*
������������˵Ļ��๹�캯��        
������[IN] i     �����˽ڵ�Ļ�IP
      [IN] link1 �����˵Ľڵ������ӷ�ʽ
      [IN] id1    ���ֲ�ʽϵͳ��ʶ�� 
����ֵ����                                                                                       
��ע��Ŀǰ�Ĳ��������TS 
*/
{
}
CHiberTopoBase::~CHiberTopoBase(void)
/*
������������˵������������˺���ִ���꣬Node���е�nodes��û���ͷ�
*/
{

}
bool CHiberTopoBase::AutoSetDefaultRoute()
{
	//��һ��
	for(size_t  domain = 0;domain!=transitTopoVec.size();domain++)
	{
		CPlatTopoBase* needSetTopo = transitTopoVec[domain];
		map<NodeId_t, int> N;
		NodeId_t routeId=needSetTopo->GetDefaultRouteId();
		for (NodeId_t nodenum=0;nodenum<needSetTopo->NodeCount();nodenum++)
		{
			N.insert(make_pair(needSetTopo->GetNode(nodenum)->Id(), -1));
		}
		for (NodeId_t nodenum=0;nodenum<needSetTopo->NodeCount();nodenum++)
		{
			int re = BFSForDefaultRoute(N, nodenum, routeId);
			if (re==-1)
			{
				needSetTopo->GetNode(nodenum)->SetDomainRoute(true);
				continue;
			}
			Node* nextRoute = Node::GetNode(re);
			if (nextRoute==NULL)
			{
				return false;
			}
			needSetTopo->GetNode(nodenum)->DefaultRoute(nextRoute);
		}
		
	}
	//�ڶ���
	CPlatTopoBase* needSetStubTopo;
	for (size_t stub = 0;stub!=(stubTopoVec.size());stub++)
	{
		needSetStubTopo = stubTopoVec[stub];
		map<NodeId_t, int> N;
		NodeId_t routeId=needSetStubTopo->GetDefaultRouteId();
		for (NodeId_t nodenum=0;nodenum<needSetStubTopo->NodeCount();nodenum++)
		{
			N.insert(make_pair(needSetStubTopo->GetNode(nodenum)->Id(), -1));
		}
		for (NodeId_t nodenum=0;nodenum<needSetStubTopo->NodeCount();nodenum++)
		{
			int re = BFSForDefaultRoute(N, nodenum, routeId);
			
			if (re==-1)
			{
				Node* upRoute;
				//needSetStubTopo->GetNode(nodenum)->SetDomainRoute(true);
				GetNodeByConnectInfo(needSetStubTopo->routerConnect[0],upRoute);
				if (upRoute==NULL)
				{
					needSetStubTopo->GetNode(nodenum)->SetDomainRoute(true);
					continue;
				}
				needSetStubTopo->GetNode(nodenum)->DefaultRoute(upRoute);
				continue;
			}
			Node* nextRoute = Node::GetNode(re);
			if (nextRoute==NULL)
			{
				return false;
			}
			needSetStubTopo->GetNode(nodenum)->DefaultRoute(nextRoute);
		}
	}
	//������
	CPlatTopoBase* LanTopo;
	for (size_t lan = 0;lan<lanTopoVec.size();lan++)
	{
		LanTopo = lanTopoVec[lan];
		NodeId_t routeId=LanTopo->GetDefaultRouteId();
		Node* Route = Node::GetNode(routeId);

		Node* upRoute;
		GetNodeByConnectInfo(needSetStubTopo->routerConnect[0],upRoute);
		if (upRoute==NULL)
		{
			Route->SetDomainRoute(true);
			continue;
		}
        Route->DefaultRoute(upRoute);
		
		for (NodeId_t nodenum=1;nodenum<=LanTopo->NodeCount();nodenum++)
		{
			needSetStubTopo->GetNode(nodenum)->DefaultRoute(Route);
		}
	}
	return true;
}
void CHiberTopoBase::AutoSetTopoIP()
/*
��������������Զ�����IP       
��������
����ֵ����                                                                                 
��ע��ʵ�ַ��������㡣
      ��һ�� �ܶ����Domain1��Domain2��Domain3����ռ���λ��DomainNumλ
	  ��ÿ������������еĽڵ�ռ��������NodesInDomainλ
	  �ڶ��㣬���ҵ�����һ���·�ɽڵ㣬Ȼ�����·�ɽڵ��IP������һ��Ľڵ㸳IP��
	  ռ��������NodesInStubλ
	  �����㣬�͵ڶ�������ƣ��ҵ�����һ���stub��Ȼ�������һ��Ľڵ��IP������һ��Ľڵ㸳IP��
	  ռ����NodesInLanλ
*/
{
	IPAddr_t  newIp;
	IPAddr_t  ip;
	IPAddr_t stubIp,getUpIp;
	//��һ��
	for(size_t  domain = 0;domain!=transitTopoVec.size();domain++)
	{
		ip = 0;
		ip = domain+1;
		CPlatTopoBase* needSetTopo = transitTopoVec[domain];
		IPAddr_t  setIp;
		Mask mask(DomainNum+NodesInDomain);
		for (size_t domainNum = 1;domainNum<=needSetTopo->NodeCount();domainNum++)
		{
			setIp = ip<<NodesInDomain|(domainNum);
			//needSetTopo->GetNode(domainNum-1)->SetIPAddr(setIp);
			setIp = setIp<<(NodesInStub+NodesInLan);
			needSetTopo->GetNode(domainNum-1)->SetProxyRoutingConfig(setIp,mask);
			setIp = 0;
		}
	}
	//�ڶ���
	CPlatTopoBase* needSetStubTopo;

	for (size_t stub = 0;stub!=(stubTopoVec.size());stub++)
	{
		needSetStubTopo = stubTopoVec[stub];
		if(needSetStubTopo->routerConnect.size()==0) continue;
       
		Node* up;
		//Node* up = Node::GetNode(needSetStubTopo->routerConnect[0].nodeNum);//һ���ڵ�����ж��·�ɽڵ���
		GetNodeByConnectInfo(needSetStubTopo->routerConnect[0],up);
		
		if (up==nil)
		{
			SetLastError(ERROR_ID_OUT_OF_NODECOUNT_FAIL);
			string tmp="SetIP:Get routeId error!";
			SetLastErrorStr(tmp);
			return;
		}
		getUpIp  = up->GetProxyIP();
		Mask mask(DomainNum+NodesInDomain+NodesInStub);
		for (size_t stubNodeNum = 1;stubNodeNum<=needSetStubTopo->NodeCount();stubNodeNum++)
		{
			stubIp = getUpIp|stubNodeNum<<NodesInLan;
			//needSetStubTopo->GetNode(stubNodeNum-1)->SetIPAddr(stubIp);
			needSetStubTopo->GetNode(stubNodeNum-1)->SetProxyRoutingConfig(stubIp,mask);
			stubIp = 0;
		}
	}
	//������
	CPlatTopoBase* LanTopo;
	for (size_t lan = 0;lan<lanTopoVec.size();lan++)
	{
		LanTopo = lanTopoVec[lan];
		if(LanTopo->routerConnect.size()==0) continue;
		Node* up;
		//Node* up = Node::GetNode(LanTopo->routerConnect[0].nodeNum);
		GetNodeByConnectInfo(LanTopo->routerConnect[0],up);
		if (up==nil)
		{
			SetLastError(ERROR_ID_OUT_OF_NODECOUNT_FAIL);
			string tmp="SetIP:Get routeId error!";
			SetLastErrorStr(tmp);
			return;
		}
		getUpIp  = up->GetProxyIP();
		for (size_t lanNodeNum = 1;lanNodeNum<=LanTopo->NodeCount();lanNodeNum++)
		{
			stubIp = getUpIp|lanNodeNum;
			LanTopo->GetNode(lanNodeNum-1)->SetIPAddr(stubIp);

			IFVec_t interfaces = LanTopo->GetNode(lanNodeNum-1)->Interfaces();
			if (interfaces.size()>0)
			{
				interfaces[0]->SetIPAddr(stubIp);
			}
			stubIp = 0;
		}
	}
}
bool  CHiberTopoBase::GetNodeByConnectInfo(ConnectInfo& connectinfo,Node*& connectNode)
{
	CPlatTopoBase* connectTopo;
	switch(connectinfo.lay)
	{
	case 1:
		connectTopo = transitTopoVec[connectinfo.topoNum];
		break;
	case 2:
		connectTopo = stubTopoVec[connectinfo.topoNum];
		break;
	case 3:
		connectTopo = lanTopoVec[connectinfo.topoNum];
		break;
	default:
		SetLastError(ERROR_LAY_OUT_OF_TOPOVEC_WHEN_CONNECT);
		string tmp="Lay out of topovec when connect";
		SetLastErrorStr(tmp);
		return false;
	}
	connectNode = connectTopo->GetNode(connectinfo.nodeNum);

	if (connectNode!=NULL)
	{
		return true;
	}
	else  
	{
		SetLastError(ERROR_NODE_OUT_OF_TOPO_WHEN_CONNECT);
		string tmp="node out of topo when connect";
		SetLastErrorStr(tmp);
		return false;
	}
}

Interface* CHiberTopoBase::AddDuplexLink(Node* ln,Node* rn, const Linkp2p& l)
/*
������������ָ���Ľ��֮�����һ����      
������[IN] ln  ����Ҫ���ӵĵ�һ���ڵ��ָ��
      [IN] rn  ����Ҫ���ӵĵڶ����ڵ��ָ��
      [IN] l   �������ڵ�����ӷ�ʽ
����ֵ������ʱ�Ľӿ�                                                                                       
��ע�� 
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
�����������������ˣ�left��right��
������[IN]layer1      ��left���˵Ĳ���
	  [IN]numOfTopo1  ��left�������Ӧ���ϵĵڼ�������
	  [IN]topo1NodeId ����Ӧ���˵ĵڼ����ڵ�
	  [IN]layer2      ��right���˵Ĳ���
	  [IN]numOfTopo2  ��right�������Ӧ���ϵĵڼ�������
	  [IN]topo2NodeId ����Ӧ���˵ĵڼ����ڵ�
���أ��Ƿ����ӳɹ�
��ע�����ӵĹ����У����¼���ӵ���Ϣ
      ͬһ����������������Ϣ��¼��brotherConnect��
	  ��һ�����һ������������
	  ��һ�����һ��������������Ϣ��һ���¼��controlConnect����һ���¼��routerConnect��
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
			if (left==nil)
			{
				SetLastError(ERROR_ID_OUT_OF_NODECOUNT_FAIL);
				string tmp="Get topo1NodwId error!";
				SetLastErrorStr(tmp);
				return false;
			}
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
			string tmp="The first ID out of lay1 nodeNum";
			SetLastErrorStr(tmp);
			return false;
		}
		break;

	case 2:
		if (numOfTopo1 <stubTopoVec.size())
		{
			left = stubTopoVec[numOfTopo1]->GetNode(topo1NodeId);
			if (left==nil)
			{
				SetLastError(ERROR_ID_OUT_OF_NODECOUNT_FAIL);
				string tmp="Get topo1NodwId error!";
				SetLastErrorStr(tmp);
				return false;
			}
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
			string tmp="The first ID out of lay2 nodeNum";
			SetLastErrorStr(tmp);
			return false;
		}
		break;

	case 3:
		if (numOfTopo1 <lanTopoVec.size())
		{
		   left = lanTopoVec[numOfTopo1]->GetNode(topo1NodeId);

			if (left==nil)
			{
				SetLastError(ERROR_ID_OUT_OF_NODECOUNT_FAIL);
				string tmp="Get topo1NodwId error!";
				SetLastErrorStr(tmp);
				return false;
			}
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
			string tmp="The first ID out of lay3 nodeNum";
			SetLastErrorStr(tmp);
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
			if (right==nil)
			{
				SetLastError(ERROR_ID_OUT_OF_NODECOUNT_FAIL);
				string tmp="Get topo2NodeId error!";
				SetLastErrorStr(tmp);
				return false;
			}
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
			string tmp="The second ID out of lay1 nodeNum";
			SetLastErrorStr(tmp);
			return false;
		}
		break;

	case 2:
		if (numOfTopo2 <stubTopoVec.size())
		{
			right = stubTopoVec[numOfTopo2]->GetNode(topo2NodeId);
			if (right==nil)
			{
				SetLastError(ERROR_ID_OUT_OF_NODECOUNT_FAIL);
				string tmp="Get topo2NodeId error!";
				SetLastErrorStr(tmp);
				return false;
			}
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
			string tmp="The second ID out of lay2 nodeNum";
			SetLastErrorStr(tmp);
			return false;
		}
		break;

	case 3:
		if (numOfTopo2 <lanTopoVec.size())
		{
			//right = lanTopoVec[numOfTopo2]->GetNode(topo2NodeId);
			right = lanTopoVec[numOfTopo2]->GetHub();
			if (right==nil)
			{
				SetLastError(ERROR_ID_OUT_OF_NODECOUNT_FAIL);
				string tmp="Get topo2NodeId error!";
				SetLastErrorStr(tmp);
				return false;
			}
			SetLastError(SUCCESS_HIBERTOPO);
			if (layer1 == layer2)
			{
				lanTopoVec[numOfTopo2]->brotherConnect.push_back(connright);
			}else
				if (layer1<layer2)
				{
				}
			else {return false;}
		}
		else 
		{
			SetLastError(ERROR_ID_OUT_OF_LAY3NODECOUNT_FAIL);
			string tmp="The second ID out of lay3 nodeNum";
			SetLastErrorStr(tmp);
			return false;
		}
		break;

	default:
		break;
	}

	left->AddDuplexLink(right);
	return true;
}

//����IPû�п���
bool  CHiberTopoBase::ConnectDomain(Count_t layer1,
					                Count_t  numOfTopo1,
					                Count_t layer2,
					                Count_t  numOfTopo2)
/*
�����������������ˣ�left��right���������ѡ���Ӧ���˵��ڲ��ڵ�
������[IN]layer1      ��left���˵Ĳ���
	  [IN]numOfTopo1  ��left�������Ӧ���ϵĵڼ�������
	  [IN]layer2      ��right���˵Ĳ���
	  [IN]numOfTopo2  ��right�������Ӧ���ϵĵڼ�������
����ֵ���Ƿ����ӳɹ�
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