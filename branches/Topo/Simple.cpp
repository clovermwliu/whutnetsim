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
#include "Simple.h"
CSimple ::CSimple(IPAddr_t i,                                           //给所有节点赋的基IP
				 const Linkp2p& link,
				  SystemId_t id)
:CPlatTopoBase(2,i,link,id)
/*
描述：CSimple的构造函数      
参数：[IN] i     ：拓扑节点的基IP
      [IN] link  ：拓扑的节点间的连接方式
      [IN] id    ：分布式系统标识符 
返回：空                                                                                       
备注：为了测试的类，只有两个节点的拓扑
*/
{
}

bool CSimple::GenerateTopo()
/*
描述：生成拓扑         
参数：无
返回：是否生成成功                                                                               
备注：                                       
*/
{
	first = Node::nextId;
	Node* left = new Node(); 
	left->SetIPAddr(ip++);

	Node* right = new Node(); 
	right->SetIPAddr(ip++);

	left->AddDuplexLink(right,link);
	return true;
}
void CSimple::SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
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
	const NodeVec_t& nodes = Node::GetNodes();

	Meters_t x0 = (BoundBoxRightUpper.X()+BoundBoxLeftDown.X())/2;   //第一个大圆的中心位置
	Meters_t y0 = (BoundBoxRightUpper.Y()+BoundBoxLeftDown.Y())/2;
	nodes[first]->SetLocation(Location(BoundBoxLeftDown.X(),y0));
	nodes[first+1]->SetLocation(Location(BoundBoxRightUpper.X(),y0));
}
CSimple::~CSimple(void)
{
}