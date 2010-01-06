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

//#define DEBUG_MASK 0x08
#include "G_debug.h"
#include "droptail.h"
#include "interface.h"
#include "link.h"

#ifdef HAVE_QT
#include <qcolor.h>
#endif

using namespace std;

bool DropTail::Enque(Packet* p)
{
  if (!Detailed()) UpdateSize(); // Maintain virtual size
  UpdateAverage();
  totEnq++;
  if (verbose && 0)
    {
      cout << "Enquing pkt time " << Simulator::Now()
           << " size " << size
           << " pktsize " << p->Size()
           << " pktCount " << pktCount
           << " pktLimit " << pktLimit << endl;
    }
  bool full = false;
  if (pktLimit)
    { // Queuing in packets
      full = (pktCount == pktLimit);
    }
  else
    { // Queuing in bytes
      full = (size + p->Size()) > Limit();
    }
  
  DEBUG(0,(cout << "Enquing pkt time " << Simulator::Now()
           << " size " << size
           << " pktsize " << p->Size()
           << " pktCount " << pktCount
           << " pktLimit " << pktLimit << endl));
  pktCount++; // Count number of packets in queue
  if (CheckForcedLoss() || full)
    {
      DEBUG(2,(cout << "Dropping pkt, size " << size
               << " pSize " << p->Size()
               << " limit " << Limit()
               << " time " << Simulator::Now()
               << endl));
      dropCount++;
      pktCount--;
      globalQueueDrop++;
      Stats::pktsDropped++;
      if (verbose)
        {
          cout << "Dropping pkt, size " << size
               << " pSize " << p->Size()
               << " limit " << Limit()
               << " time " << Simulator::Now()
               << endl;
        }
      return false; // Full
    }
  DBG((Stats::pktsEnqued++));
  size += p->Size();                     // Note byte count added
  if (Detailed())
    {
      pkts.push_back(p);      // Save the packet if detailed
    }
  else
    { // Non-Detailed
#ifndef WIN32
      if (!interface) return false;      // Can't handle w/o interface
#else
		if (!interface_) return false;      // Can't handle w/o interface
#endif
      if (!ndDeq) ndDeq = new NDDeq_t(); // Allocate the Deque
      Time_t startTx = Simulator::Now(); // Time that this pkt starts transmit
      if (!ndDeq->empty())
        { // Not empty, starts after last one finishes
          NDInfo& nd = ndDeq->back();    // Last entry in queue
          startTx = nd.stopTx;           // Time that last one finishes
        }
#ifndef WIN32
      Link* link = interface->GetLink();
#else
	  Link* link = interface_->GetLink();
#endif
      if (!link) return false;           // Link must exist
      Time_t txTime =  ((Rate_t)p->Size() * 8) / link->Bandwidth();
      NDInfo  newND(startTx, startTx + txTime, p->Size());
      ndDeq->push_back(newND);           // Add to the deque
    }
  DEBUG(1,(cout << " newsize " << size << endl));
  return true;                           // Not full, enqued
}

Packet* DropTail::Deque()
{ // Deque is never called for non-detailed queues
  if (!Detailed()) return nil;            // Always nil if not detailed
  UpdateAverage();
  if (Length() == 0) return nil;         // Nothing enqued
  DBG((Stats::pktsDequed++));
  Packet* p = pkts.front();              // Get head of queue
  pkts.pop_front();                      // Remove it
  size -= p->Size();                     // Note byte count removed
  pktCount--;                            // Note pkt removed
  return p;                              // Return the packet
}

Packet* DropTail::PeekDeque()
{
  if (Length() == 0) return nil;         // Nothing enqued
  return pkts.front();                   // Return head of queue without remove
}

Count_t DropTail::DequeAllDstMac(MACAddr m)
{
  Count_t count = 0;
  for (PktList_t::iterator i = pkts.begin(); i != pkts.end(); )
    {
      PktList_t::iterator next = i;
      next++;
      Packet* p = *i;
      if (p->GetDstMac() == m)
        {
          count++;
          pkts.erase(i);
          size -= p->Size();
          pktCount--;
          delete p;
        }
      i = next;
    }
  return count;
}

Count_t DropTail::DequeAllDstIP(IPAddr_t ip)
{
  Count_t count = 0;
  for (PktList_t::iterator i = pkts.begin(); i != pkts.end(); )
    {
      PktList_t::iterator next = i;
      next++;
      Packet* p = *i;
      if (p->GetDstIP() == ip)
        {
          count++;
          pkts.erase(i);
          size -= p->Size();
          pktCount--;
          delete p;
        }
      i = next;
    }
  return count;
}

Packet* DropTail::DequeOneDstMac(MACAddr m)
{
  for (PktList_t::iterator i = pkts.begin(); i != pkts.end(); ++i)
    {
      Packet* p = *i;
      if (p->GetDstMac() == m)
        {
          pkts.erase(i);
          size -= p->Size();
          pktCount--;
          return p;
        }
    }
  return nil;
}

Packet* DropTail::DequeOneDstIP(IPAddr_t ip)
{
  for (PktList_t::iterator i = pkts.begin(); i != pkts.end(); ++i)
    {
      Packet* p = *i;
      if (p->GetDstIP() == ip)
        {
          pkts.erase(i);
          size -= p->Size();
          pktCount--;
          return p;
        }
    }
  return nil;
}

Count_t DropTail::Length()
{
  if (!Detailed())UpdateSize();
  return (Count_t)size;
}

Count_t DropTail::LengthPkts()
{
  if (!Detailed())UpdateSize();
  return pktCount;
}

void   DropTail::SetInterface(Interface* i)
{
#ifndef WIN32
  interface = i;
#else
  interface_ = i;
#endif
}

Queue* DropTail::Copy() const
{ // Make a copy of this queue
  return new DropTail(*this);
}

void DropTail::SetLimit(Count_t l)
{
  limit = l;
}

Time_t   DropTail::QueuingDelay()
{
  if (Detailed()) return Queue::QueuingDelay();
  UpdateSize();
  // Not detailed, use ndDeq list
  Time_t now = Simulator::Now();
  // Find when last packet will finish
  if (!ndDeq) return 0;  // No pending pkts
  if (ndDeq->empty()) return 0; // No pending pkts
  NDInfo& nd = ndDeq->back();   // Newest entry
  return (nd.stopTx - now);     // Time till this one finishes
}


bool DropTail::Check(Size_t s, Packet*)
{ // Check if sufficient buffer space available
  if(!Detailed())UpdateSize();
  if (Length() == 0) return true; // Always true if empty
  if (pktLimit)
    { // Using packet limit, not bytes
      return pktCount < pktLimit;
    }
  // Queue limit in bytes
  return ((Length() + s) < Limit());
}

Packet* DropTail::GetPacket(Count_t k)
{
  if (pkts.empty()) return nil;
  // Get the k'th packet
  PktList_t::iterator i = pkts.begin();

  for (Count_t j = 0; j < k; ++j)
    {
      ++i;
      if (i == pkts.end()) return nil; // Does not exist
    }
  return *i;
}

void    DropTail::GetPacketColors(ColorVec_t& cv)
{ // Build a list of packet colors from head to tail
#ifdef HAVE_QT
  for (PktList_t::iterator i = pkts.begin(); i != pkts.end(); ++i)
    {
      Packet* p = *i;
      QColor c;
      if (p->IsColored())
        c = QColor(p->R(), p->G(), p->B());
      else
        c = Qt::blue;
      cv.push_back(c.rgb());
    }
#endif
}

// Private methods
void DropTail::UpdateSize()
{ // Adjust size to account for packets transmitted since last update
  if (!ndDeq) return; // Nothing to do if no non-detailed deque
  Time_t now = Simulator::Now();
  while(!ndDeq->empty())
    {
      NDInfo& nd = ndDeq->front(); // Oldest entry
      if (nd.startTx > now) return;      // Starts in future
      if (nd.stopTx  > now)
        { // Stops in future.  This means the packet would have
          // been dequeued at this point, but the link is still
          // busy transmitting it.  We should reduce the size
          // of the queue by the size of the packet, but leave
          // this entry in the ndDeq so we know when the link is
          // free.
          if (!nd.updated)
            {
              size -= nd.size;
              pktCount--;
              nd.updated = true;
            }
          return; // done for now
        }
      // This packet has finished, remove it
      if (!nd.updated)
        {
          size -= nd.size;
          pktCount--;
        }
      ndDeq->pop_front(); // Remove from queue and continue searching
    }
}



