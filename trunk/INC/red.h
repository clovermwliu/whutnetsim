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



// Georgia Tech Network Simulator - RED Queue class
// Tarik Arici (tariq@ece.gatech.edu), Aug. 2002

#ifndef __red_h__
#define __red_h__

#include <list>
#include "droptail.h"

//Doc:ClassXRef
class REDQueue : public DropTail {
 //Doc:Class This class derives from the class DropTail and implements
  //Doc:Class the Random Early Detection Queuing discipline. More references
  //Doc:Class about RED can be founf at Sally Floyd's webpages
  public:
    //Doc:Method
    REDQueue();
    //Doc:Desc Default constructor
    
    //Doc:Method    
    REDQueue( DCount_t in_w_q, Count_t in_min_th, Count_t in_max_th, 
	     Count_t in_limit, DCount_t in_max_p, Count_t in_mean_pktsize);
    //Doc:Desc This constructor the critical RED parameters and builds a
    //Doc:Desc correspoding RED queue
    //Doc:Arg1 weight of the queue
    //Doc:Arg2 minimum threshold
    //Doc:Arg3 maximum threshold
    //Doc:Arg4 Limit/max size for the queue
    //Doc:Arg5 maximum value for mark/drop probability
    //Doc:Arg6 Average packet size
    
    //Doc:Method
    virtual ~REDQueue() { }
    //Doc:Desc Destructor
    
    //Doc:Method
    bool Enque(Packet*);  // Return true if enqued, false if dropped
    //Doc:Desc This method is used to enque a packet in the queue
    //Doc:Arg1 the pointer to the packet to be added to the queue
    //Doc:Return a boolean to indicate if the enque operation succeeded

    //Doc:Method    
    Packet* Deque();     // Return next element (NULL if none)
    //Doc:Desc This method deques a packet from the queue and returns it
    //Doc:Return Pointer to the packet

    
    //Doc:Method    
    Packet* MarkPacketwProb();
    //Doc:Desc This method marks a packet with its mark/drop probablity. This
    //Doc:Desc method is used when the average queue is still between the min
    //Doc:Desc and max thresholds
    //Doc:Return the marked packet
    
    //Doc:Method
    Packet* MarkPacket();
    //Doc:Desc This method marks a packet and returns a pointer to it.
    //Doc:Desc This is method is called when the current queue size is
    //Doc:Desc larger than the max threshold.
    //Doc:Return Pointer to the packet

    //Doc:Method
    Packet* DropPacket();
    //Doc:Desc This method drops the packet from the queue, it is called
    //Doc:Desc when we need an unconditional drop. like when the queue
    //Doc:Desc overflows
    
    //Doc:Method
    Count_t Length() { return size;} // Length in bytes
    //Doc:Desc This method returns the size of the queue in terms of bytes
    //Doc:Return number of bytes

    //Doc:Method
    Queue*  Copy() const;            // Make a copy
    //Doc:Desc This method makes a copy of the queue and returns a pointer
    //Doc:Desc to the copy
    //Doc:Return pointer to a copy of the queue
    
    //Doc:Method
    void    SetInterface(Interface*);// Associated interface
    //Doc:Desc This method attaches the queue to an interface
    //Doc:Arg1 The interface to which it is to be attached

    //Doc:Method
    Rate_t  Bandwidth();             // Get bandwidth of associated link
    //Doc:Desc This method returns the bandwidth of the link associated with
    //Doc:Desc the interface that this queue is attached to.
    //Doc:Return the bandwidth of the link
  private:
    int count;            // packets since last marked packet
    Count_t mean_pktsize; // mean packet size
    Count_t size;         // current bytes in queue
    DCount_t q_avg;       // average queue size
    Count_t min_th;       // minimum threshold
    Count_t max_th;       // maximum threshold
    DCount_t w_q;         // queue weight for the current queue size
    DCount_t max_p;       // maximum value for mark/drop probability
    Time_t idle_since;    // queue is idle since this time
    PktList_t pkts;       // List of packets in the queue
    Interface* iface;     // Associated interface
    // to be modified later
    Count_t bandwidth;    // bandwidth(bits) (we have to get this from the link)
    DCount_t pktspersec;  // pkts per sec is used to update q_avg when it's idle

};
#endif



    
