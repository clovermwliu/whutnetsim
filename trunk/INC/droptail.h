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


// Georgia Tech Network Simulator - DropTail Queue class
// George F. Riley.  Georgia Tech, Spring 2002

#ifndef __droptail_h__
#define __droptail_h__

#include "queue.h"

class Interface;

#define DEFAULT_DROPTAIL_LIMIT 50000

//Doc:ClassXRef
class DropTail : public Queue {
//Doc:Class This method implements the simple droptail or FIFO queue.
public:

  //Doc:Method
  DropTail()
      : Queue(true), limit(Queue::DefaultLength()),
        pktLimit(Queue::DefaultLimitPkts()),
        size(0), pktCount(0), lastTime(0) { }
  //Doc:Desc the default constructor

  //Doc:Method
  DropTail(Count_t l)
    : Queue(true), limit(l), pktLimit(0),
      size(0), pktCount(0), lastTime(0) { }
  //Doc:Desc This constructor creates a queue of specified size
  //Doc:Arg1 the size of the queue in bytes

  //Doc:Method
  DropTail(const DropTail& c)
    : Queue(c), limit(c.limit), pktLimit(c.pktLimit),
      size(c.size), pktCount(c.pktCount), lastTime(c.lastTime) { }
  //Doc:Desc This constructor creates a queue identical to another queue
  //Doc:Arg1 Reference to an existing queue

  virtual ~DropTail() { }

  //Doc:Method
  bool Enque(Packet*); // Return true if enqued, false if dropped
  //Doc:Desc This method is used to enque a packet in the queue
  //Doc:Arg1 the pointer to the packet to be added to the queue
  //Doc:Return a boolean to indicate if the enque operation succeeded or
  //Doc:Return if the packet was dropped.

  //Doc:Method
  Packet* Deque();     // Return next element (NULL if none)
  //Doc:Desc This method deques a packet from the queue and returns it
  //Doc:Return Pointer to the packet

  //Doc:Method
  Packet* PeekDeque();
    //Doc:Desc Return a pointer to the next packet in the queue, without
    //Doc:Desc removing it.  This is used by some subclasses of
    //Doc:Desc   queue (DiffServQueue for example) to find out if the packet
    //Doc:Desc has enough tokens to be transmitted.
    //Doc:Return Pointer to the packet, or nil if none available.

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

  //Doc:Method
  Count_t Length();    // Length in bytes
  //Doc:Desc This method returns the size of the queue in terms of bytes
  //Doc:Return number of bytes

  //Doc:Method
  Count_t LengthPkts();// Length in packets
  //Doc:Desc This method returns the size of the queue in terms of the
  //Doc:Desc number of packets
  //Doc:Return number of packets enqued

  //Doc:Method
  void    SetInterface(Interface*);// Associated interface
  //Doc:Desc This method attaches the queue to an interface
  //Doc:Arg1 The interface to which it is to be attached


  //Doc:Method
  Queue*  Copy() const;       // Make a copy
  //Doc:Desc This method makes a copy of the queue and returns a pointer
  //Doc:Desc to the copy
  //Doc:Return pointer to a copy of the queue

  //Doc:Method
  virtual void SetLimit(Count_t l);
  //Doc:Desc This method sets a limit to the size of the queue. this limit
  //Doc:Desc is in terms of bytes
  //Doc:Arg1 size of the queue in bytes


  //Doc:Method
  Count_t GetLimit() const { return limit;}
  //Doc:Desc This method returns the size of the queue in terms of the bytes
  //Doc:Return Max limit of the queue size in bytes

  //Doc:Method
  virtual void SetLimitPkts(Count_t l) { pktLimit = l;} // Set packets limit
  //Doc:Desc This method sets a limit to the size of the queue. this limit
  //Doc:Desc is in terms of number of packets that can be enqued
  //Doc:Arg1 size of the queue in packets

  //Doc:Method
  virtual Count_t GetLimitPkts() const { return pktLimit;} // Get pkt limit
  //Doc:Desc This method returns the size of the queue in terms of the packets
  //Doc:Return Max limit of the queue size in number of packets


  //Doc:Method
  virtual Time_t  QueuingDelay();    // Calculate queuing delay at current lth
  //Doc:Desc This method calculates the current queuing delay
  //Doc:Return the queuing delay


  //Doc:Method
  bool    Check(Size_t, Packet* = nil); // Test if buffer space available
  //Doc:Desc This method checks if buffer space (of a given size) is available
  //Doc:Arg1 The size to be checked
  //Doc:Arg2 The pointer to a packet to be queued.
  //Doc:Return boolean to indicate true or false

  //Doc:Method
  Count_t Limit() { return limit;}
  //Doc:Desc This method returns the maximum limit of the queue size.
  //Doc:Return the limit (in bytes)

  //Doc:Method
  Packet* GetPacket(Count_t k);
    //Doc:Desc Used by the animation.  Gets the k'th packet from the head.
    //Doc:Arg1 Index of desired packet.
    //Doc:Return Packet pointer to k'th packet.  Nil if does not exist.

  //Doc:Method
  void     GetPacketColors(ColorVec_t&);
    //Doc:Desc Fill in the vector of colors for each pending packet.
    //Doc:Desc This is more efficient than the GetPacket method above,
    //Doc:Desc and the above is deprecated.
    //Doc:Arg1 Vector of colors to fill in.
    //Doc:Arg1 zero'th entry is next packet to deque.

private:
  void    UpdateSize();       // Adjust size since last update
public:
  Count_t    limit;           // Limit of bytes in queue
  Count_t    pktLimit;        // Limit on number of pkts (if non-zero)
  Count_t    size;            // Current bytes in queue
  Count_t    pktCount;        // Current packets in queue
  Time_t     lastTime;        // Time size was calculated
public:
  PktList_t  pkts;            // List of packets in the queue
};

#endif
