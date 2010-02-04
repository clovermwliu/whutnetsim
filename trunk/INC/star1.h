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
//更改时间：2010-1-4

#ifndef __star_h__
#define __star_h__

#include <math.h>
#include "PlatTopoBase.h"
#include "G_common_defs.h"

class Node;
class Linkp2p;
class Queue;
class Location;

//Doc:ClassXRef
class Star1 : public CPlatTopoBase
{
  //Doc:Class Define a star topology using point--to--point links
public:
  Star1(Count_t, IPAddr_t = IPADDR_NONE, SystemId_t = 0);// Number of nodes
 
  Star1(Count_t, const Linkp2p&, IPAddr_t = IPADDR_NONE, SystemId_t = 0);
   
  Star1(Count_t, Node*, IPAddr_t = IPADDR_NONE, SystemId_t = 0);
   
  Star1(Count_t, Node*, const Linkp2p&, IPAddr_t = IPADDR_NONE, SystemId_t = 0);
  bool GenerateTopo();  

  Node* GetHub();                     // Get the hub node
   
  Node* GetLeaf(Count_t);             // Get specified leaf
   
  Linkp2p* HubLink(Count_t);          // Get link from hub to leaf router
   
  Linkp2p* LeafLink(Count_t);         // Get link from leaf to hub
   
  Queue*   HubQueue(Count_t);         // Get queue from hub to leaf
   
  Queue*   LeafQueue(Count_t);        // Get queue from leaf to hub
   
  Count_t  LeafCount() { return leafCount;}
   
  void    StarSetLocationViaBoundBox(const Location&, const Location&,
      Angle_t = 0, Angle_t = 2*M_PI);

public:
  NodeId_t   first;      // First leaf node id
  NodeId_t   last;       // Just log first/last+1 entries
  Node*      hub;        // Points to hub node
  Count_t    leafCount;  // Number of leaf nodes
  Node* n;
private:
  void ConstructorHelper(Count_t, Node*, 
                         IPAddr_t, const Linkp2p&, SystemId_t);
};

#endif


