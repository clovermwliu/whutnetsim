// GENERAL PUBLIC LICENSE AGREEMENT
// 
// PLEASE READ THIS DOCUMENT CAREFULLY BEFORE UTILIZING THE PROGRAM
// 
// BY UTILIZING THIS PROGRAM, YOU AGREE TO BECOME BOUND BY THE TERMS OF
// THIS LICENSE.  IF YOU DO NOT AGREE TO THE TERMS OF THIS LICENSE, DO
// NOT USE THIS PROGRAM OR ANY PORTION THEREOF IN ANY FORM OR MANNER.
// 
// This Program is licensed, not sold to you by GEORGIA TECH RESEARCH
// CORPORATION ("GTRC"), owner of all code and accompanying documentation
// (hereinafter "Program"), for use only under the terms of this License,
// and GTRC reserves any rights not expressly granted to you.
// 
// 1.  This License allows you to:
// 
// (a) make copies and distribute copies of the Program's source code
// provide that any such copy clearly displays any and all appropriate
// copyright notices and disclaimer of warranty as set forth in Article 5
// and 6 of this License.  All notices that refer to this License and to
// the absence of any warranty must be kept intact at all times.  A copy
// of this License must accompany any and all copies of the Program
// distributed to third parties.
// 
// A fee may be charged to cover the cost associated with the physical
// act of transferring a copy to a third party.  At no time shall the
// program be sold for commercial gain either alone or incorporated with
// other program(s) without entering into a separate agreement with GTRC.
//  
// 
// (b) modify the original copy or copies of the Program or any portion
// thereof ("Modification(s)").  Modifications may be copied and
// distributed under the terms and conditions as set forth above,
// provided the following conditions are met:
// 
//     i) any and all modified files must be affixed with prominent
// notices that you have changed the files and the date that the changes
// occurred.
// 		
//     ii) any work that you distribute, publish, or make available, that
// in whole or in part contains portions of the Program or derivative
// work thereof, must be licensed at no charge to all third parties under
// the terms of this License.
// 
//    iii) if the modified program normally reads commands interactively
// when run, you must cause it, when started running for such interactive
// use in the most ordinary way, to display and/or print an announcement
// with all appropriate copyright notices and disclaimer of warranty as
// set forth in Article 5 and 6 of this License to be clearly displayed.
// In addition, you must provide reasonable access to this License to the
// user.
// 
// Any portion of a Modification that can be reasonably considered
// independent of the Program and separate work in and of itself is not
// subject to the terms and conditions set forth in this License as long
// as it is not distributed with the Program or any portion thereof.
// 
// 
// 2. This License further allows you to copy and distribute the Program
//    or a work based on it, as set forth in Article 1 Section b in
//    object code or executable form under the terms of Article 1 above
//    provided that you also either:
// 
//    i) accompany it with complete corresponding machine-readable source
// code, which must be distributed under the terms of Article 1, on a
// medium customarily used for software interchange; or,
// 
//   ii) accompany it with a written offer, valid for no less than three
// (3) years from the time of distribution, to give any third party, for
// no consideration greater than the cost of physical transfer, a
// complete machine-readable copy of the corresponding source code, to be
// distributed under the terms of Article 1 on a medium customarily used
// for software interchange; or,
// 
// 
// 3.  Export Law Assurance.
// 
// You agree that the Software will not be shipped, transferred or
// exported, directly into any country prohibited by the United States
// Export Administration Act and the regulations thereunder nor will be
// used for any purpose prohibited by the Act.
//  
// 4.  Termination.
// 
// If at anytime you are unable to comply with any portion of this
// License you must immediately cease use of the Program and all
// distribution activities involving the Program or any portion thereof.
// 
// 
// 5.  Disclaimer of Warranties and Limitation on Liability.
// 
// YOU ACCEPT THE PROGRAM ON AN "AS IS" BASIS.  GTRC MAKES NO WARRANTY
// THAT ALL ERRORS CAN BE OR HAVE BEEN ELIMINATED FROM PROGRAM.  GTRC
// SHALL NOT BE RESPONSIBLE FOR LOSSES OF ANY KIND RESULTING FROM THE USE
// OF PROGRAM AND ITS ACCOMPANYING DOCUMENT(S), AND CAN IN NO WAY PROVIDE
// COMPENSATION FOR ANY LOSSES SUSTAINED, INCLUDING BUT NOT LIMITED TO
// ANY OBLIGATION, LIABILITY, RIGHT, CLAIM OR REMEDY FOR TORT, OR FOR ANY
// ACTUAL OR ALLEGED INFRINGEMENT OF PATENTS, COPYRIGHTS, TRADE SECRETS,
// OR SIMILAR RIGHTS OF THIRD PARTIES, NOR ANY BUSINESS EXPENSE, MACHINE
// DOWNTIME OR DAMAGES CAUSED TO YOU BY ANY DEFICIENCY, DEFECT OR ERROR
// IN PROGRAM OR MALFUNCTION THEREOF, NOR ANY INCIDENTAL OR CONSEQUENTIAL
// DAMAGES, HOWEVER CAUSED.  GTRC DISCLAIMS ALL WARRANTIES, BOTH EXPRESS
// AND IMPLIED RESPECTING THE USE AND OPERATION OF PROGRAM AND ITS
// ACCOMPANYING DOCUMENTATION, INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR PARTICULAR PURPOSE AND ANY IMPLIED
// WARRANTY ARISING FROM COURSE OF PERFORMANCE, COURSE OF DEALING OR
// USAGE OF TRADE.  GTRC MAKES NO WARRANTY THAT PROGRAM IS ADEQUATELY OR
// COMPLETELY DESCRIBED IN, OR BEHAVES IN ACCORDANCE WITH ANY
// ACCOMPANYING DOCUMENTATION.  THE USER OF PROGRAM IS EXPECTED TO MAKE
// THE FINAL EVALUATION OF PROGRAM'S USEFULNESS IN USER'S OWN
// ENVIRONMENT.
// 
// GTRC represents that, to the best of its knowledge, the software
// furnished hereunder does not infringe any copyright or patent.
// 
// GTRC shall have no obligation for support or maintenance of Program.
// 
// 6.  Copyright Notice.
// 
// THE SOFTWARE AND ACCOMPANYING DOCUMENTATION ARE COPYRIGHTED WITH ALL
// RIGHTS RESERVED BY GTRC.  UNDER UNITED STATES COPYRIGHT LAWS, THE
// SOFTWARE AND ITS ACCOMPANYING DOCUMENTATION MAY NOT BE COPIED EXCEPT
// AS GRANTED HEREIN.
// 
// You acknowledge that GTRC is the sole owner of Program, including all
// copyrights subsisting therein.  Any and all copies or partial copies
// of Program made by you shall bear the copyright notice set forth below
// and affixed to the original version or such other notice as GTRC shall
// designate.  Such notice shall also be affixed to all improvements or
// enhancements of Program made by you or portions thereof in such a
// manner and location as to give reasonable notice of GTRC's copyright
// as set forth in Article 1.
// 
// Said copyright notice shall read as follows:
// 
// Copyright 2004
// Dr. George F. Riley
// Georgia Tech Research Corporation
// Atlanta, Georgia 30332-0415
// All Rights Reserved
//
// $Id: randomtree.h,v 1.3 2004/10/21 14:51:50 mofta7 Exp $



// Georgia Tech Network Simulator - Random Tree topology
// Monirul I Sharif,  Georgia Tech, Fall 2003

// Specifies a randomtree structure and specifies non-leaf routers with proxy routing
// capability so that packets destined for non existant nodes are routed to the downstream
// routers as far as possible.

// Define the tree topology with random structure using point-to-point links

#ifndef __myrandomtree_h__
#define __myrandomtree_h__

#include "common-defs.h"
#include "node.h"
#include "mask.h"
#include "rng.h"
#include "linkp2p.h"
#include <vector>

class Node;
class Linkp2p;
class Queue;

typedef std::vector<NodeVec_t> NodeVecVec_t;

extern bool TurnOffProxyRouting;

// define the stages in a myrandomtree
struct RandomTreeStage {
  int levels;
  int fanout;
  Linkp2p link;
};

// defined types to keep structure of nodes in the myrandomtree

//Doc:ClassXRef
class myrandomtree {
  //Doc:Class Defines a tree topology using point--to--point links.
public:
 myrandomtree(Count_t stages, struct RandomTreeStage stage[], Mask m,
Random_t = 0.8, SystemId_t = 0 , const Random & = Uniform(0,1));

  // Levels, Max Fanout, IP, Mask
  //Doc:Method
  myrandomtree(Count_t, Count_t,
       Mask m,  Random_t = 0.5, const Random & = Uniform(0,1), 
	     const Location & = Location(0, 0), const Location& =Location(10, 10), SystemId_t = 0);
    //Doc:Desc Construct a {\tt Tree} topology object with the specified
    //Doc:Desc number of levels, fanout, \IPA\ and address mask.  The
    //Doc:Desc default point--to--point link object is used for links.
    //Doc:Arg1 Number of levels in the tree.
    //Doc:Arg2 Fanout of each level (number of child  nodes for each parent).
    //Doc:Arg3 Starting \IPA.  The \IPA\ is incremented by one for each
    //Doc:Arg3 node in the tree.
    //Doc:Arg4 \IPA\ mask.

  // Specify link to use
  //Doc:Method
  myrandomtree(Count_t, Count_t, const Linkp2p&,
	     Mask m, Random_t = 0.5, const Random & = Uniform(0,1), 
	     const Location& = Location(0,0), const Location & = Location(10.0, 10.0), SystemId_t = 0);
    //Doc:Desc Construct a {\tt Tree} topology object with the specified
    //Doc:Desc number of levels, fanout, \IPA\ and address mask, and
    //Doc:Desc the specified point--to--point link.
    //Doc:Arg1 Number of levels in the tree.
    //Doc:Arg2 Fanout of each level (number of child  nodes for each parent).
    //Doc:Arg3 A reference to a point--to--point link object to copy for 
    //Doc:Arg3 each link in the tree.
    //Doc:Arg4 Starting \IPA.  The \IPA\ is incremented by one for each
    //Doc:Arg5 node in the tree.
    //Doc:Arg6 \IPA\ mask.
  myrandomtree(Count_t, Count_t, const Linkp2p&,
	     Count_t, Count_t, const Linkp2p&,
	     Mask m, Random_t = 0.5, const Random & = Uniform(0,1), 
	     const Location& = Location(0,0), const Location & = Location(10.0, 10.0), SystemId_t = 0);

  //Doc:Method
  //Doc:Method
  Node* GetRoot();                    // Get root node
    //Doc:Desc Return a pointer to the root node of the tree.
    //Doc:Return Pointer to root node.

  
  //Doc:Method
  Node* GetNode(Count_t, Count_t);    // Get node at specified level, index
    //Doc:Desc Return a pointer to the node at the specified level, index.
    //Doc:Arg1 Level number (root is level 0).
    //Doc:Arg2 Index, range 0 to number of nodes in the level - 1.
    //Doc:Return Pointer to the specfied node.

  
  //Doc:Method
  Node* GetLeaf(Count_t);             // Get specified leaf
    //Doc:Desc Return a pointer to the specified leaf node.
    //Doc:Arg1 Desired leaf node indexes, counting from zero.
    //Doc:Return Pointer to the specified leaf node.
  /*
  // Get link from specified node to specified child
  //Doc:Method
  Linkp2p* GetChildLink(Count_t, Count_t, Count_t);
    //Doc:Desc Get a pointer to the link from the specified node to its
    //Doc:Desc specified child.
    //Doc:Arg1 Level number (root is level 0).
    //Doc:Arg2 Index, range 0 to number of nodes in the level - 1.
    //Doc:Arg3 Child index, range 0 to fanout - 1.
    //Doc:Return Pointer to the specfied link.

  // Get link from specified node to parent
  //Doc:Method
  Linkp2p* GetParentLink(Count_t, Count_t);
    //Doc:Desc Get a pointer to the link from the specified node to its parent.
    //Doc:Arg1 Level number (root is level 0).
    //Doc:Arg2 Index, range 0 to number of nodes in the level - 1.
    //Doc:Return Pointer to the specfied link.

  // Get queue from specified node to specified child
  //Doc:Method
  Queue*   GetChildQueue(Count_t, Count_t, Count_t);
    //Doc:Desc Get a pointer to the queue from the specified node to its
    //Doc:Desc specified child.
    //Doc:Arg1 Level number (root is level 0).
    //Doc:Arg2 Index, range 0 to number of nodes in the level - 1.
    //Doc:Arg3 Child index, range 0 to fanout - 1.
    //Doc:Return Pointer to the specfied queue.

  // Get queue from specified node to parent
  //Doc:Method
  Queue*   GetParentQueue(Count_t, Count_t);
    //Doc:Desc Get a pointer to the queue from the specified node
    //Doc:Desc to its parent.
    //Doc:Arg1 Level number (root is level 0).
    //Doc:Arg2 Index, range 0 to number of nodes in the level - 1.
    //Doc:Return Pointer to the specfied queue.
    */

  //Doc:Method
  Count_t  LeafCount() { return leafCount;}
    //Doc:Desc Get a count of the number of leaf nodes in this tree.
    //Doc:Return Count of leaf nodes.

  void BoundingBoxOld(const Location&, const Location&);
  //Doc:Method
  void BoundingBox(const Location&, const Location&);
    //Doc:Desc Define a bounding box for this tree object. 
    //Doc:Desc The bounding box
    //Doc:Desc is used to assign node locations, which are used during
    //Doc:Desc the simulation animation.
    //Doc:Arg1 Lower left corner of bounding box.
    //Doc:Arg2 Upper right corner of bounding box.

  void BoundingBox(const Location&, const Location&, double );

public:

  Count_t    levels;     // Number of levels
  Count_t    maxfanout;     // max Children per node

  Count_t    levels_fs;     // number of levels in first stage of tree
  Count_t    maxfanout_fs;  // fanout in first stage of tree

  Count_t    leafCount;  // Number of leaf nodes

// this is for the new version of the myrandomtree 
  Count_t    stages;
  struct RandomTreeStage *stage;

private:
  Random_t   probGenerateChild;  // probability of generating a child from a parent
  Random     *probRandom;         // Random number generator for tree creation

  NodeVecVec_t  nodes;

  void MakeSubtree(Node *parent, Count_t l,  const Linkp2p& link, 
		   const Linkp2p& linkfs, IPAddr_t leafIP, Mask mask, 
		   const Location&, const Location&, const Meters_t, SystemId_t);

  void ConstructorHelper(Count_t, Count_t, const Linkp2p&, 
			 Count_t, Count_t, const Linkp2p&, Mask, Random_t, 
			 const Random &, const Location&, const Location&, SystemId_t);

  IPAddr_t GetIPRandomly(bool IsChanged = true, IPAddr_t ip = -1);
  //从configcache的IP库中随机选择一IP并返回
  //IsChanged为ture时，将取出的IP的原空间替换为ip,否则不变
  //********////


};

#endif



