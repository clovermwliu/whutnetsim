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
//����ʱ�䣺2010-3-9
#ifndef __WAXMAN_H__ 
#define __WANMAN_H__

#pragma once
#include "PlatTopoBase.h"

class CWaxman: public CPlatTopoBase
{
public:
	CWaxman(Count_t count,double _alpha = 1,double _beta = 0.2,
			IPAddr_t i = IPADDR_NONE,     
		    SystemId_t id = 0,const Linkp2p& link = Linkp2p::Default());
	~CWaxman(void);
public:
	bool GenerateTopo();
	void SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
								const Location& BoundBoxRightUpper,
								BoxType  type = DEFAULT);
private:
	void AllocateForNode();
	void ConnectNode(const Meters_t  L);
	Meters_t  Distance(Node* first,Node* second);//���������ڵ�֮��ľ���
private:
	double alpha;
	double beta;
};
#endif