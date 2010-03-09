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
#pragma once
#include "G_common_defs.h"
#include "node.h"
#include "PlatTopoBase.h"
#include "HiberTopoBase.h"

#ifndef __SELECTSETIP_H__ 
#define __SELECTSETIP_H__ 


class CHostTopo
{
public:
	CHostTopo(Node* _route):route(_route),hostNum(0){}
	CHostTopo(Count_t hnum):route(0)
	{
		hostNum = 0;
		AddHosts(hnum);
	}
	bool SetRoute(Node* _route){route = _route;return true;}
	~CHostTopo(void){}
	bool  AddHosts(Count_t hnum)
	{  
		for (Count_t i = 0;i<hnum;i++)
		{
			AddHost();
		}
		return true;
	} 
	void SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
		const Location& BoundBoxRightUpper
		/*BoxType  type = DEFAULT*/)
	{
		Meters_t  Xbound = BoundBoxRightUpper.X() - BoundBoxLeftDown.X();
		Meters_t  Ybound = BoundBoxRightUpper.Y() - BoundBoxLeftDown.Y();
		Meters_t   xstep = Xbound/(hostNum+1);

		Meters_t	x = BoundBoxLeftDown.X();
		Meters_t	y =  BoundBoxRightUpper.Y() - Ybound/2;
		for (size_t i = 0;i<hostNum;i++)
		{
			hosts[i]->SetLocation(x,y);
			x += xstep;
		}
	}
	bool  AddHost()
	{  
		Node* newNode = new Node();
		if (route!=NULL)
		{
			route ->AddDuplexLink(newNode);
			newNode->DefaultRoute(route);
			newNode->IsHostNode(true);

			IPAddr_t routeIp = newNode->GetProxyIP();
			Mask_t  mask = newNode->GetProxyMask().NBits();
			Count_t moveBit = 32 - mask;
			
			IPAddr_t hostIP = routeIp;

			Random * rngT = new Uniform(0, pow((double)2,(double)moveBit)-1);

			bool noIP = true;
			while (noIP)
			{
				IPAddr_t SelectIP = rngT->IntValue();

				vector<IPAddr_t>::iterator   pos;   
				pos = find(ips.begin(),ips.end(),SelectIP);
				if (pos!=ips.end())
				{
				}
				else
				{
					noIP = false;
					ips.push_back(SelectIP);
					hostIP += SelectIP;
					newNode->SetIPAddr(hostIP);
				}
			}
		}
		hostNum++;
		hosts.push_back(newNode);
		return true;
	}                                    //添加一个主机
private:
	Node* route;
	Count_t hostNum;
	vector<Node*> hosts;
	vector<IPAddr_t> ips;
};
class CSelectSetIp : public CHiberTopoBase
{
public:
	CSelectSetIp(NodeId_t IpBits);
	void SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
								const Location& BoundBoxRightUpper,
								BoxType  type = DEFAULT);
    bool GenerateTopo();
	bool AddHostBySelectSetIP(IPAddr_t ip);
	void GetHostLay(vector<CHostTopo*>& _hostLay){_hostLay = hostLay;}

    bool AutoSetDefaultRoute();
	void AutoSetTopoIP();
	//Node* GetNode(NodeId_t i);
public:
	~CSelectSetIp(void);
private:
	NodeId_t IpBits;
	vector<CHostTopo*> hostLay;//主机层
	//NodeId_t first;
};
#endif