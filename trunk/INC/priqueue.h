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

// Implements a strict priority queue.

/* -*- mode:c++ -*- */

#ifndef __PRIQUEUE_H
#define __PRIQUEUE_H

#include <vector>
#include "queue.h"

#define NO_PRIQUEUE	3

typedef std::vector<Queue*> QueVec_t;

class PriQueue : public Queue {
public:
  PriQueue();
  PriQueue(Count_t, const Queue& = Queue::Default());
  PriQueue(const PriQueue&);
  virtual ~PriQueue();

  bool      Enque(Packet*);
  Packet*   Deque();
  Packet*   PeekDeque();

  //Doc:Method
  virtual Count_t DequeAllDstMac(MACAddr);
  //Doc:Desc   Remove all packets in the queue with the specified
  //Doc:Desc destination MAC address.
  //Doc:Arg1 MAC Address to remove
  //Doc:Return Count of removed packets

  //Doc:Method
  virtual Count_t DequeAllDstIP(IPAddr_t);
  //Doc:Desc   Remove all packets in the queue with the specified
  //Doc:Desc destination IP address.
  //Doc:Arg1 IP Address to remove
  //Doc:Return Count of removed packets

  //Doc:Method
  virtual Packet* DequeOneDstMac(MACAddr);
  //Doc:Desc   Remove a packet in the queue with the specified
  //Doc:Desc destination MAC address.
  //Doc:Arg1 MAC Address to remove
  //Doc:Return Pointer to the removed packet

  //Doc:Method
  virtual Packet* DequeOneDstIP(IPAddr_t);
  //Doc:Desc   Remove a packet in the queue with the specified
  //Doc:Desc destination IP address.
  //Doc:Arg1 IP Address to remove
  //Doc:Return Pointer to the removed packet

  Count_t   Length();
  Count_t   LengthPkts();
  Queue*    Copy() const;
  void      SetInterface(Interface*);
  void      SetLimit(Count_t);
  Count_t   GetLimit() const { return que[0]->GetLimit(); }
  void      SetLimitPkts(Count_t);
  Count_t   GetLimitPkts() const { return que[0]->GetLimitPkts(); }
  bool      Check(Size_t, Packet* = nil);
  Packet*   GetPacket(Count_t);
  void      GetPacketColors(ColorVec_t&);
  Queue*    GetQueue(Count_t);       // Get queue for a given priority
  // Helpers to enque/deque  individual queues
protected:
  bool      Enque(Packet*, Count_t); // Enque a packet in a particular queue
  Packet*   Deque(Count_t);          // Deque a packet from a particular queue
  Packet*   PeekDeque(Count_t);      // Peek at head of queue without removal
private:
  QueVec_t  que;
};

#endif
