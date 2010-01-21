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
//����ʱ�䣺2010-1-11
#include "Waxman.h"
#include <math.h>

CWaxman::CWaxman(Count_t count,double _alpha,double _beta,
					IPAddr_t i,     
					SystemId_t id,const Linkp2p& link)
:CPlatTopoBase(count,i,link,id),alpha(_alpha),beta(_beta)
/*
������          
������                                                 
����ֵ����                                                                                       
��ע�� 
*/
{
}
bool CWaxman::GenerateTopo()
/*
��������������         
��������
���أ��Ƿ����ɳɹ�                                                                               
��ע��                                       
*/
{
	AllocateForNode();
	return true;
}
void CWaxman::AllocateForNode()          //����ڵ�
/*
������Ϊ���˷���ڵ㣬���½����нڵ�     
��������
����ֵ����                                                                                     
��ע��                                       
*/
{
	first = Node::nextId;
	for(int i=0;i<nodeCount;i++) 
	{
		Node* addNode;
		addNode = new Node(sid);
	}
}
void CWaxman::SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
									 const Location& BoundBoxRightUpper,
									 BoxType  type)
/*
������ͨ����λ�������ڵ���������
������[in]BoundBoxLeftDown   �������½ǵ�λ��
      [in]BoundBoxRightUpper �������Ͻǵ�λ��
	  [in]type               ��������λ�õ�����
����ֵ����
*/
{
	//���ڵ�����λ�ã�ǰ�����ڵ�ָ��Ϊ�����ڵ�
	const NodeVec_t& nodes = Node::GetNodes();
	if(nodeCount >= 0) nodes[first+0]->SetLocation(BoundBoxLeftDown);
	if(nodeCount >= 1) nodes[first+1]->SetLocation(BoundBoxRightUpper);


	Meters_t  Xbound = BoundBoxRightUpper.X() - BoundBoxLeftDown.X();
	Meters_t  Ybound = BoundBoxRightUpper.Y() - BoundBoxLeftDown.Y();

	for (NodeId_t nodeNum=2;nodeNum!=nodeCount;)
	{
		int x =  random(Xbound);
		int y =  random(Ybound);
		Location* loc= new Location(BoundBoxLeftDown.X()+x,BoundBoxLeftDown.Y()+y);
		if (loc->X() != BoundBoxLeftDown.X()&&loc->Y() != BoundBoxLeftDown.Y()&&
			loc->X() != BoundBoxRightUpper.X()&&loc->Y() != BoundBoxRightUpper.Y())
		{
			nodes[first+nodeNum]->SetLocation(*loc);
			nodeNum++;
		}
	}
	Meters_t  L = sqrt(Xbound*Xbound + Ybound*Ybound);
    ConnectNode(L);
}
void CWaxman::ConnectNode(const Meters_t  L)
/*
���������ݸ�������       
������[in]L:�����Զ��������ľ���
���أ���                                                                           
��ע��                                       
*/
{
	const NodeVec_t& nodes = Node::GetNodes();
	for (NodeId_t firstNode=0;firstNode!=nodeCount;firstNode++)
	{
		for (NodeId_t secondNode=firstNode + 1;secondNode!=nodeCount;secondNode++)
		{
			Meters_t d = Distance(nodes[first+firstNode],nodes[first+secondNode]);
			double conectP = alpha * exp(-1.0*(d/(beta * L)));
			double ran = random();
			if (ran <= conectP)
			{
				nodes[first+firstNode]->AddDuplexLink(nodes[first+secondNode],link);
			}
		}
	}
}
Meters_t  CWaxman::Distance(Node* first,Node* second)
/*
���������������ڵ�֮��ľ���      
������[in]first  :��һ���ڵ�
      [out]second:�ڶ����ڵ�
���أ������ڵ�֮��ľ���                                                                
��ע��                                       
*/
{
	Meters_t  x1,y1,x2,y2,dx,dy,d;  
	x1 = first->LocationX();
	y1 = first->LocationY();
	x2 = second->LocationX();
	y2 = second->LocationY();
	dx = x1 - x2;
	dy = y1 - y2;
	d  = sqrt(dx*dx + dy*dy);
	return d;
}
CWaxman::~CWaxman(void)
/*
������         
������[in]:
[out]:
����ֵ��                                                                                     
��ע��                                       
*/
{
}
