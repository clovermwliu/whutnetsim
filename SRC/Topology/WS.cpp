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
 
#include "WS.h"
#include <math.h>
#include "gui_ctrl.h"
#include "linkp2p.h"
#include <algorithm>

#define PI 3.1415926
#define s  100 //����������֮��ľ���

CWS::~CWS(void)
{
}
CWS::CWS(Count_t _nodeCount,
		 Count_t _lineForOneNode,
		 double _p,
		 IPAddr_t i,
		 const Linkp2p& link,
		 SystemId_t id)
:CPlatTopoBase(_nodeCount,i,link,id)
/*
������WSС�������������          
������[IN] _nodeCount      �����˵Ľڵ���Ŀ
	  [IN] _lineForOneNode ��һ���ڵ����ӵı���
      [IN] _p              ����������
      [IN] i               �����˽ڵ�Ļ�IP
      [IN] id              ���ֲ�ʽϵͳ��ʶ�� 
      [IN] link            �����˵Ľڵ������ӷ�ʽ
���أ���                                                                                       
��ע�� 
*/
{
	lineForOneNode = _lineForOneNode;
	p=_p;
}

bool CWS::GenerateTopo()
/*
��������������        
��������                                                 
���أ��Ƿ����ɳɹ�                                                                                       
��ע��
*/
{
	//double r=Computer_r();
	//addNodes(r);
	////qtWin->DisplayTopologyAndReturn();
	//addLines();
	ConstructorHelper(link, ip);
	return true;
}
bool CWS::ConstructorHelper( const Linkp2p& link,
							 IPAddr_t leafIP)
/*
�������������˵İ�������        
������[IN] link   �����˵Ľڵ������ӷ�ʽ 
	  [IN] leafIP �����˽ڵ�Ļ�IP 
���أ���                                                                                       
��ע���������Բ�Ҫ����Ϊ�õĶ������еĳ�Ա
*/
{
	AddNodes();
		
	if (!AddLines(link, leafIP))
	{
		SetLastError(ERROR_WS_LINK);
		string tmp="WS Link error";
		SetLastErrorStr(tmp);
		return false;
	}
	return true;
}

void CWS::AddNodes()
/*
��������ӽڵ�       
��������
���أ���                                                                                       
��ע�������еĽڵ��½���
*/
{
	first = Node::nextId;
	Node *n = new Node ();
	n->SetIPAddr(ip++);
	//nodes.push_back(n);
	Node *firstnode = n;
	//nodeCount = count;

	for (Count_t l = 1; l <= nodeCount; ++l)
	{ // Create each subsequent level
		Node *newnode;
		if (l!=nodeCount)
		{
			newnode = new Node();
			newnode->SetIPAddr(ip++);
			//nodes.push_back(newnode);
		}
		else
			newnode = firstnode;
		n = newnode;
	}
	last = Node::nextId;
}
bool CWS::AddLines(const Linkp2p& link,
				   IPAddr_t leafIP)//��Ҫ���ݲ���,���ӵķ�ʽ
/*
�������ڵ�֮������        
������[IN] link   �����˵Ľڵ������ӷ�ʽ 
	  [IN] leafIP �����˽ڵ�Ļ�IP 
���أ���                                                                                       
��ע���������Բ�Ҫ����Ϊ�õĶ������еĳ�Ա
*/
{
	int    remote;
    Node*  node0;
	Node*  node1;
	IPAddr_t  nextIP = leafIP;
	for (int i=0;i<nodeCount;i++)
	{
		node0=Node::nodes[first+i];
		for (int addline=1;addline<=lineForOneNode/2;addline++)
		{
			remote=(i+addline+nodeCount)%nodeCount;
			
			int cp=random(100);
			//cout<<cp<<endl;
			if (cp>p*100)
			{
				//node1=Node::nodes[first+remote];
				node1 = GetNode(remote);
				if (node1==nil)
				{
					SetLastError(ERROR_WSID_OUT_OF_NODECOUNT_FAIL);
					string tmp="Link:WS id out of nodecount";
					SetLastErrorStr(tmp);
					return false;
				}
			}
			else
			{
				bool flag=true;

				NodeWeightVec_t nwv;
				node0->Neighbors(nwv);

				NodeWeightVec_t::iterator iter;
				int nonum;
				while(flag)
				{
					nonum=random(nodeCount);
					
					for (iter=nwv.begin();iter!=nwv.end();iter++)
					{
						if ((*iter).node->Id()==(Count_t)nonum)
						break;
					}
					if (iter==nwv.end())
					{
						flag=false;
					}
				}
				//node1=Node::nodes[first+nonum];
				node1 = GetNode(nonum);
				if (node1==nil)
				{
					SetLastError(ERROR_WSID_OUT_OF_NODECOUNT_FAIL);
					string tmp="Link:WS id out of nodecount";
					SetLastErrorStr(tmp);
					return false;
				}
			}
			if (nextIP == IPADDR_NONE)
			{ // No IP specified
				node0->AddDuplexLink(node1, link);
			}
			else
			{
				node0->AddDuplexLink(node1, link,
					nextIP++, Mask(32), 
					IPADDR_NONE, Mask(32));
			}
		}
	}
	return true;
}
void CWS::SetLocationViaBoundBox(const Location& ll, const Location& ur, BoxType  type)
/*
������ͨ����λ�������ڵ���������
������[in]ll     �����½ǵ�λ��
[in]ur     �����Ͻǵ�λ��
[in]type   ������λ�õ�����
����ֵ����
*/
{
	Meters_t xRadius = fabs(ur.X() - ll.X())/2;
	Meters_t yRadius = fabs(ur.Y() - ll.Y())/2;

	Meters_t xCenter = (ur.X() + ll.X())/2;
	Meters_t yCenter = (ur.Y() + ll.Y())/2;

	double angle = (2 * M_PI)/ nodeCount;

	NodeId_t thisNode = first;
	const NodeVec_t& nodes = Node::GetNodes();

	DEBUG0((cout<<"Putting Ring nodes in place"<<endl));

	// Assign locations for each level
	for (Count_t l = 0; l < nodeCount; ++l)
	{
		Meters_t yLoc = yCenter + yRadius * sin(angle * l);
		Meters_t xLoc = xCenter + xRadius * cos(angle * l);

		nodes[thisNode++]->SetLocation(xLoc,yLoc);

		DEBUG0((cout<<"Putting node "<<l<<" in ("<<xLoc<<","<<yLoc<<")"<<endl));
	}
}