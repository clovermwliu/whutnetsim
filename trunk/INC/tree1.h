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

#ifndef __tree1_h__ 
#define __tree1_h__ 

#include "G_common-defs.h"
#include "node.h"
#include "mask.h"
#include "PlatTopoBase.h"

class Node;
class Linkp2p;
class Queue;

#define N_FANOUT 4
#define N_DEPTH 3
#define N_TREES 5

//Doc:ClassXRef
class Tree1 : public CPlatTopoBase
{
  //Doc:Class Defines a tree topology using point--to--point links.
public:
  // Levels, Fanout, IP
  //Doc:Method
  Tree1(Count_t, Count_t,
       IPAddr_t = IPADDR_NONE, SystemId_t = 0);
   
 
  Tree1(Count_t, Count_t, const Linkp2p&,
       IPAddr_t = IPADDR_NONE, SystemId_t = 0);
  
  bool GenerateTopo();
   
  Node* GetRoot();                    // Get root node
   
  Node* GetNode(Count_t, Count_t);    // Get node at specified level, index
  
  Node* GetLeaf(Count_t);             // Get specified leaf
   
  Linkp2p* GetChildLink(Count_t, Count_t, Count_t);
    
  Linkp2p* GetParentLink(Count_t, Count_t);
   
  Queue*   GetChildQueue(Count_t, Count_t, Count_t);
   
  Queue*   GetParentQueue(Count_t, Count_t);
   
  Count_t  LeafCount() { return leafCount;}
  
  void SetLocationViaBoundBox(const Location&, const Location&,BoxType  type = DEFAULT);
  

  void BoundingBox(const Location&, const Location&, double angle);

public:
  NodeId_t   first;      // There is a global node list, we don't need one 
  NodeId_t   last;       // Just log first/last+1 entries
  Count_t    levels;     // Number of levels
  Count_t    fanout;     // Children per node
  Count_t    leafCount;  // Number of leaf nodes
  IPAddr_t   firstLeafIP;// Starting IP Address for leaf
  NodeId_t   firstLeafId;// Starting NodeId for leaf
private:
  void ConstructorHelper(Count_t, Count_t,
                         const Linkp2p&,
                         IPAddr_t,
                         SystemId_t);
  void LocateRow(Count_t, Count_t, NodeId_t,
                  const Location&, const Location&, 
                  std::vector<Meters_t>&);
};

#endif


