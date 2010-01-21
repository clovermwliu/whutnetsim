//Copyright (c) 2010, Information Security Institute of Wuhan Universtiy(ISIWhu)
//All rights reserved.
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
//Construct Data:
//Modify Author:
//Author Organization:
//Modify Data:
//更改人：李玉
//更改时间：2010-1-4

#pragma once
#ifndef __CRing_h__ 
#define __CRing_h__ 

#include "G_common_defs.h"
#include "node.h"
#include "PlatTopoBase.h"

class Node;
class Linkp2p;
class Queue;

class CRing : public CPlatTopoBase
{
	//Doc:Class Defines a tree topology using point--to--point links.
public:
	// Size, IP, Mask
	//Doc:Method
	CRing(Count_t,
		  IPAddr_t = IPADDR_NONE,
		  const Linkp2p& = Linkp2p::Default(),
		  SystemId_t = 0);
	bool GenerateTopo();
	//
	//Node* GetNode(Count_t);    // Get node at specified level, index
	
	void SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
								const Location& BoundBoxRightUpper,
								BoxType  type = DEFAULT);

public:
	//NodeId_t   first;      // There is a global node list, we don't need one 
	//NodeId_t   last;       // Just log first/last+1 entries
	//Count_t    nodeCount;  // Number of nodes
	IPAddr_t   firstNodeIP;// Starting IP Address for nodes
	NodeId_t   firstNodeId;// Starting NodeId for nodes
private:
	void ConstructorHelper(const Linkp2p&,IPAddr_t);
};
#endif