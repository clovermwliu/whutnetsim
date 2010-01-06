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

// Georgia Tech Network Simulator - PeachQueue class
// George F. Riley.  Georgia Tech, Fall 2005

// Implements the "low-priority" queueing for TCP-Peach dummy packets

#ifndef __peachqueue_h__
#define __peachqueue_h__

#include "droptail.h"

//Doc:ClassXRef
class PeachQueue : public DropTail {
//Doc:Class The class {\tt PeachQueue} implements the low--priority
//Doc:Class queuing for TCP Peach "dummy" packets.  THis simply
//Doc:Class drops any packet being enqued if the "dummy" bit is 
//Doc:Class set (in the TOS IP header field) and if the queue has more than
//Doc:Class one packet presently enqued.
public:
  //Doc:Method
  PeachQueue();

  // Destructor
  virtual ~PeachQueue() { }

  //Doc:Method
  virtual bool Enque(Packet*);    // Return true if enqued, false if not
  //Doc:Desc This method is used to enque a packet in the queue.
  //Doc:Desc Enque will always file if current queue length is 
  //Doc:Desc more than one and the packet is a dummy packet.  See
  //Doc:Desc TCPPeach for info on dummy packets.
  //Doc:Arg1 the pointer to the packet to be added to the queue
  //Doc:Return a boolean to indicate if the enque operation succeeded

};

#endif



