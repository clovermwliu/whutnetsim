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


// Georgia Tech Network Simulator - Dual Queue Class Definition
// George F. Riley.  Georgia Tech, Spring 2003

// The "dual" queue implements a private queue for TCP traffic
// and another queue for all others.  The two queues are scheduled
// in a work-conserving round-robin method.

#ifndef __dualqueue_h__
#define __dualqueue_h__

#include "queue.h"

class Interface;

//Doc:ClassXRef
class DualQueue : public Queue {
public:
  DualQueue();
  DualQueue(Count_t l);
  DualQueue(const DualQueue& c);
  DualQueue(const Queue& t, const Queue& n); // Specify TCP and Non-TCP queues
  virtual ~DualQueue();
  // Inherited methods from Queue
  virtual bool Enque(Packet*);        // Return true if enqued, false if not
  virtual Packet* Deque();            // Return next element (NULL if none)
  virtual Packet* PeekDeque();        // Peek at next element
  // Most of the following should not be used for DualQueues.
  // Rather, the values should be retrived for the TCP/NonTCP queues directly
  virtual Count_t Length();           // How many bytes are enqued?
  virtual Count_t LengthPkts();       // How many packets are enqued?
  virtual Queue* Copy() const;        // Make a copy of this queue
  virtual void SetLimit(Count_t);     // Set new limit
  virtual Count_t GetLimit() const;   // Get limit (bytes)
  virtual void SetLimitPkts(Count_t); // Set packets limit
  virtual Count_t GetLimitPkts() const;  // Set packets limit
  virtual bool Check(Size_t, Packet* = nil); // Test if buffer space available
  virtual bool Detailed();            // True if detailed model w/ local queing
  virtual void Detailed(bool);        // Set detailed on/off
  virtual Time_t   QueuingDelay();    // Calculate queuing delay at current lth
  virtual DCount_t TotalWorkload();   // Return the byte-seconds (workload)
  virtual void     SetInterface(Interface*);  // Let the q know which interface
  virtual Count_t  DropCount();       // Number of dropped packets
  virtual Count_t  EnqueueCount();    // Number of enqueued packets
  virtual void     CountEnq(Packet* = nil); // Count an enque
  virtual void     ResetStats();      // Reset the statistics to zero
  virtual Packet*  GetPacket(Count_t);// Get the k'th packet
  virtual void     GetPacketColors(ColorVec_t&);

  // Methods specific to DualQueue
  void   TCPQueue(const Queue& q);
  void   NonTCPQueue(const Queue& q);
  Queue* TCPQueue() const { return pTCP;}
  Queue* NonTCPQueue() const { return pNonTCP;}
  bool   LastTCP() const { return lastTCP;}
  Count_t TCPEmpty() const { return tcpEmpty;}
  Count_t UDPEmpty() const { return udpEmpty;}
private:
  bool   IsTCP(Packet* p);            // True if packet has TCP header
private:
  Count_t limit;                      // Limit of bytes in queue
  Count_t pktLimit;                   // Limit on number of pkts (if non-zero)
  bool    lastTCP;                    // True if last deque was from TCP queue
  Count_t tcpEmpty;                   // Times TCP had no packet on it's turn
  Count_t udpEmpty;                   // Times UDP had no packet on it's turn
public:
  Queue*  pTCP;                       // Points to the TCP queue
  Queue*  pNonTCP;                    // Points to non-tcp queue
};

#endif
