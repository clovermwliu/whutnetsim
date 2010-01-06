//Copyright (c) 2010, Information Security Institute of Wuhan Universtiy(ISIWhu)
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
//更改时间：2010-1-4

#ifndef __dumbbell1_h__ 
#define __dumbbell1_h__ 

#include "G_common-defs.h"
#include "node.h"
#include "HiberTopoBase.h"
class Node;
class Linkp2p;
class Queue;

//Doc:ClassXRef
class Dumbbell1 : public CHiberTopoBase
{
public:

	Dumbbell1(Count_t l, Count_t r,Mult_t m,IPAddr_t lip, IPAddr_t rip,
			 const Linkp2p& link = Linkp2p::Default(), SystemId_t id = 0);
  ////Doc:Method
  //Dumbbell1(Count_t, Count_t, Mult_t, SystemId_t = 0);
  //  
  //Dumbbell1(Count_t, Count_t, Mult_t, const Linkp2p&, SystemId_t = 0);
  // 
  //Dumbbell1(Count_t, Count_t, Mult_t, 
  //         IPAddr_t leftIP, IPAddr_t rightIP, SystemId_t = 0);
  //  
  //Dumbbell1(Count_t, Count_t, Mult_t, 
  //         IPAddr_t leftIP, IPAddr_t rightIP,         // Specify ip addresses
  //         const Linkp2p&, SystemId_t = 0); // Specify link for leafs
 
	bool GenerateTopo();
	void SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
										const Location& BoundBoxRightUpper,
										BoxType  type);
private:
  //NodeId_t   first;      // There is a global node list, we don't need one 
  //NodeId_t   last;       // Just log first/last+1 entries
  Count_t    leftCount;  // Number of left side nodes
  Count_t    rightCount; // Number of right side nodes
  IPAddr_t   leftip;
  IPAddr_t   rightip;
  CPlatTopoBase *left;
  CPlatTopoBase *right;

  //Linkp2p    routerLink;
  Mult_t     bound;          //带宽
  
private:
  //void ConstructorHelper(Count_t, Count_t, Mult_t, 
  //                       IPAddr_t, IPAddr_t,
  //                       const Linkp2p&, SystemId_t);
};

#endif


