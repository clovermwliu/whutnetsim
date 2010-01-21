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
//更改时间：2010-1-11
#include "Waxman.h"
#include <math.h>

CWaxman::CWaxman(Count_t count,double _alpha,double _beta,
					IPAddr_t i,     
					SystemId_t id,const Linkp2p& link)
:CPlatTopoBase(count,i,link,id),alpha(_alpha),beta(_beta)
/*
描述：          
参数：                                                 
返回值：无                                                                                       
备注： 
*/
{
}
bool CWaxman::GenerateTopo()
/*
描述：生成拓扑         
参数：无
返回：是否生成成功                                                                               
备注：                                       
*/
{
	AllocateForNode();
	return true;
}
void CWaxman::AllocateForNode()          //分配节点
/*
描述：为拓扑分配节点，即新建所有节点     
参数：无
返回值：无                                                                                     
备注：                                       
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
描述：通过绑定位置来给节点设置坐标
参数：[in]BoundBoxLeftDown   ——左下角的位置
      [in]BoundBoxRightUpper ——右上角的位置
	  [in]type               ——设置位置的类型
返回值：无
*/
{
	//给节点设置位置，前两个节点指定为两个节点
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
描述：依据概率连边       
参数：[in]L:相距最远的两个点的距离
返回：无                                                                           
备注：                                       
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
描述：计算两个节点之间的距离      
参数：[in]first  :第一个节点
      [out]second:第二个节点
返回：两个节点之间的距离                                                                
备注：                                       
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
描述：         
参数：[in]:
[out]:
返回值：                                                                                     
备注：                                       
*/
{
}
