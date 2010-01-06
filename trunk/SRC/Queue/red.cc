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

///////////////////////////////////////////////////////////
// These paramater values are taken from S.Floyd's simulator; tcpsim.
#define q_weight 0.002
#define min_threshold 5
#define max_threshold 15
#define queue_size 60
#define max_prob 0.1
#define mean_pkt_size 500
////////////////////////////////////////////////////////////
#include "red.h"
#include <math.h>
#include "simulator.h"
#include "rng.h"
#include "interface.h"
#include "link.h"

//#define DEBUG_MASK 0x1
#include "G_debug.h"

using namespace std;

REDQueue::REDQueue() : iface(nil)
{
  w_q = q_weight;
  mean_pktsize = mean_pkt_size;
  size = 0;
  min_th = min_threshold * mean_pktsize;
  max_th = max_threshold * mean_pktsize;
  SetLimit(queue_size * mean_pktsize);
  max_p = max_prob;
  bandwidth = 800000;
  count = -1;
  q_avg = 0;
  pktspersec = bandwidth / mean_pktsize;  // [(bits/sec) / (bits/pkt)]
  if (Simulator::instance != NULL)
    {
    idle_since = Simulator::instance->Now();
    }
  else
    {
    idle_since = 0.0;                  // scheduler not instantiated yet
    }
}

REDQueue::REDQueue(
   DCount_t in_w_q, Count_t in_min_th, Count_t in_max_th, 
   Count_t in_limit, DCount_t in_max_p, Count_t in_mean_pktsize) : iface(nil)
{
  DEBUG(0,(cout << "REDQueue::RedQueue" << endl));
  // set by the user
  w_q = in_w_q;
  min_th = in_min_th;
  max_th = in_max_th;
  SetLimit(in_limit);
  max_p = in_max_p;
  mean_pktsize = in_mean_pktsize;
  size = 0;
  bandwidth = 800000;                // !!!GET THIS FROM THE LINK
  count = -1;
  q_avg = 0;
  if (Simulator::instance != NULL)
    {
    idle_since = Simulator::instance->Now();
    }
  else
    {
    idle_since = 0.0;                  // scheduler not instantiated yet
    }
  // pkts per sec. is used to update average queue size when queue is idle
  pktspersec = bandwidth / mean_pktsize;  // [(bits/sec) / (bits/pkt)]


}

bool REDQueue::Enque(Packet* p)
{
  //Time_t now = Simulator::instance->Now();
  DEBUG(0,(cout << "Red::Enque, this " << this
           << " length " << Length() << endl));
  DEBUG(0,(cout << "REDQueue::Enqueue" << endl));
  DEBUG(1,(cout << "w_q: "<< w_q <<"min_th: "<<min_th<<"max_th: "<< max_th
       <<"size: "<<size<<endl));
  DEBUG(1,(cout<<"max_p: "<<max_p<<"count: "<<count<<"q_avg: "<<q_avg<<"limit: "<< GetLimit()<<endl));

  UpdateAverage();
  // update average queue size
  if (size == 0 ) { // pkt arriving to an idle queue
    DEBUG(1,(cout << "packet finds queue idle" << endl));
    int num_pkts_idle;    // number of packets that would have been arrived
                          // when the queue is idle
    Time_t now = Simulator::instance->Now();
    num_pkts_idle = int((now - idle_since) * pktspersec);
    DEBUG(1,(cout << "num_pkts_idle" << num_pkts_idle << endl));
    q_avg = pow((1 - w_q),num_pkts_idle) * q_avg; 
    // pkt arriving to an idle queue will never be dropped
    pkts.push_back(p);    
    size += p->Size();  
    DEBUG(1,(cout << "size: "<<size<< "q_avg: " << q_avg
	     << "now: "<< now << endl));
    return true;
  } 

  pkts.push_back(p);    // put the packet into the queue
  size += p->Size();    // update size of the queue
  q_avg = (1 - w_q) * q_avg + w_q * size;
  // actually here always last packet is dropped !!!
  // if the queue is full drop a packet
  if (Length() > Limit()) {
    Packet* pkt_to_drop = DropPacket();
    count = 0;          // after dropping, counter from last marked 
                        //pkt must be reset
    if (pkt_to_drop == p) 
      {
	DEBUG(1,(cout << "queue is full, pkt dropped" << endl));
        dropCount++;
	return false;
      }
    else return true;
  }

  // mark/drop packet with probablity p_a
  if ((min_th <= q_avg) && (q_avg <= max_th)){
    count++;
    DEBUG(1,(cout << "count: " << count));
    Packet* pkt_to_drop = MarkPacketwProb();
    if (pkt_to_drop == p) 
      {
	DEBUG(1,(cout << "pkt marked, min < avg < max" << endl));
        dropCount++;
	return false;
      }
    else
      {
	DEBUG(1,(cout << "pkt not marked, min < avg < max" << endl));
	return true;
      }
  } else if (q_avg >= max_th){
    count++;
    Packet* pkt_to_drop = MarkPacket();
    if (pkt_to_drop == p) 
      {
	DEBUG(1,(cout << "pkt marked, max < avg" << endl));
        dropCount++;
	return false;
      }
    else 
      {
	DEBUG(1,(cout << "pkt not marked, max < avg" << endl));
	return true;
      }
  } else {
    DEBUG(1,(cout << "pkt not marked, avg < min" << endl));
    DEBUG(1,(cout << "'count' before initialisation:"<< count << endl));
    count = -1;     // when avg smaller than min_th count must be reset
    return true;
  }

}

// Mark a pkt with probability  
// This implementation drops the arriving packet 
// with probability p_a as in Floyd's 93 paper.
// Different queings may be implemented by modifying this function
Packet* REDQueue::MarkPacketwProb()
{
  DEBUG(0,(cout << "REDQueue::MarkPacketwProb" << endl));
  DCount_t p_b;        

  DCount_t p_a;        
  p_b = (max_p * (q_avg - min_th) / (max_th - min_th));
  p_a = p_b / (1 - count * p_b); 
  DEBUG(1,(cout << "mark_prob: " << p_a <<endl));
  Uniform U_rng = Uniform();          
  if (U_rng.Value() <= p_a){
    count = 0;          
    Packet* pkt = pkts.back();  
    pkts.pop_back();          
    size -= pkt->Size();   
    return pkt;
  }
  return NULL;
}

// Drops the arriving packet because queue is larger then max_th
// Different queings may be implemented by modifying this function
Packet* REDQueue::MarkPacket()
{
  DEBUG(0,(cout << "REDQueue::MarkPacket" << endl));
  count = 0;          
  Packet* pkt = pkts.back(); 
  pkts.pop_back();          
  size -= pkt->Size();      
  return pkt;
}

// Drops the arriving packet because queue is full
// Different queings may be implemented by modifying this function
Packet* REDQueue::DropPacket()
{
  DEBUG(0,(cout << "REDQueue::DropPacket" << endl));
  count = 0;         
  Packet* pkt = pkts.back(); 
  pkts.pop_back();          
  size -= pkt->Size();      
  return pkt;
}

Queue*  REDQueue::Copy() const
{
  return new REDQueue(*this);
}

Packet* REDQueue::Deque()
{
  DEBUG(0,(cout << "Red::Deque, this " << this
           << " length " << Length() << endl));
  DEBUG(0,(cout << "REDQueue::Deque" << endl));
  Time_t now = Simulator::instance->Now();
  UpdateAverage();
  if (Length() == 0){
    idle_since = now;                // if queue is empty, set idle_since
    return NULL;                         // Nothing enqued
  }
  Packet* p = pkts.front();              // Get head of queue
  pkts.pop_front();                      // Remove it
  size -= p->Size();                     // Note byte count removed
  return p;                              // Return the packet
}

void REDQueue::SetInterface(Interface* i)
{ // Note the associated interface
  iface = i;
}

Rate_t REDQueue::Bandwidth()
{
  if (iface)
    { // Interface is known
      Link* l = iface->GetLink();
      if (l)
        { // Link exists
          return l->Bandwidth();
        }
    }
  return 0.0; // Not known
}
