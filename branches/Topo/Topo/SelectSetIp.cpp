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
//更改时间：2010-3-9
#include "SelectSetIp.h"
#include "Waxman.h"
#include "PlatTopoBase.h"
#include <math.h>


//#define _alpha 0.2
//#define _beta  0.2

CSelectSetIp::CSelectSetIp(NodeId_t _IpBits):CHiberTopoBase(),IpBits(_IpBits)
{
}
CSelectSetIp::~CSelectSetIp(void)
{
}
 bool CSelectSetIp::GenerateTopo()
{
	Count_t lay1Num = pow((double)2,(double)IpBits);

	CPlatTopoBase* platTopo;
	//第一层
    platTopo = new CWaxman(lay1Num);
	platTopo->GenerateTopo();
	transitTopoVec.push_back(platTopo);

	//主机层
	CHostTopo* hostTopo;
	for (size_t i3 = 0;i3<transitTopoVec.size();i3++)
	{
		for (size_t i4 = 0;i4<transitTopoVec[i3]->NodeCount();i4++)
		{
			hostTopo = new CHostTopo((Count_t)0);
			Node *route = transitTopoVec[i3]->GetNode(i4);
			hostTopo->SetRoute(route);
			hostLay.push_back(hostTopo);
		}
	}
	return true;
}
 void CSelectSetIp::SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
										   const Location& BoundBoxRightUpper,
										   BoxType  type)
 {
	 Meters_t  Xbound = BoundBoxRightUpper.X() - BoundBoxLeftDown.X();
	 Meters_t  Ybound = BoundBoxRightUpper.Y() - BoundBoxLeftDown.Y();

	 Meters_t   ystep = Ybound/3;

	 Meters_t   setX,setY;
	 //NodeId_t   setLocationId = 0;//设置的第setLocationId个节点的位置

	 //第一层节点的位置设置

	 Meters_t   xstep = Xbound/transitTopoVec.size();
	 Meters_t	x = BoundBoxLeftDown.X();
	 Meters_t	y = BoundBoxRightUpper.Y();
	 for (size_t lay1 = 0;lay1!=transitTopoVec.size();lay1++)
	 {
		 transitTopoVec[lay1]->SetLocationViaBoundBox(Location(x,y-ystep),Location(x+xstep,y));
		 x += xstep;
	 }
	 
	 //第四层节点的位置设置
	 xstep = Xbound/hostLay.size();
	 x = BoundBoxLeftDown.X();
	 y -= 2*ystep;
	 for (size_t lay4 = 0;lay4!=hostLay.size();lay4++)
	 {
		 hostLay[lay4]->SetLocationViaBoundBox(Location(x,y-ystep),Location(x+xstep,y));
		 x += xstep;
	 }
 }


 bool CSelectSetIp::AutoSetDefaultRoute()
 {
	 //最上面一层
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
			 int re = BFSForDefaultRoute(N, nodenum + needSetTopo->GetFirst(), routeId);
			 if (re==-1)
			 {
				 needSetTopo->GetNode(nodenum)->SetDomainRoute(true);
				 needSetTopo->GetNode(nodenum)->IsRouteNode(true);
				 continue;
			 }
			 Node* nextRoute = Node::GetNode(re);
			 if (nextRoute==NULL)
			 {
				 return false;
			 }
			 needSetTopo->GetNode(nodenum)->DefaultRoute(nextRoute);
			 needSetTopo->GetNode(nodenum)->IsRouteNode(true);
		 }
	 }

	 return true;
 }
 void CSelectSetIp::AutoSetTopoIP()
 {
	 IPAddr_t  newIp;
	 IPAddr_t  ip;
	 IPAddr_t stubIp,getUpIp;
	 //第一层
	 for(size_t  domain = 0;domain!=transitTopoVec.size();domain++)
	 {
		 CPlatTopoBase* needSetTopo = transitTopoVec[domain];
		 IPAddr_t  ip = 0;
		 Mask mask(IpBits);

		 Count_t moveBit = 32 - IpBits;

		 IPAddr_t setIp;
		 for (size_t domainNum = 1;domainNum<=needSetTopo->NodeCount();domainNum++)
		 {
			 setIp = ip << moveBit;
			 needSetTopo->GetNode(domainNum-1)->SetProxyRoutingConfig(setIp,mask);
			 ip ++;
		 }
	 }
 }