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

// Defines a strict priority queue.

#include <iostream>


#include <typeinfo>
#include "priqueue.h"
#include "droptail.h"
#include "G_debug.h"

using namespace std;

PriQueue::PriQueue()
{
  for (Count_t i = 0; i < NO_PRIQUEUE; i++)
    que.push_back(new DropTail());
}

PriQueue::PriQueue(Count_t no_queues, const Queue& q)
{
  for (Count_t i = 0; i < no_queues; i++)
    que.push_back(q.Copy());
}

PriQueue::PriQueue(const PriQueue& r)
{
  for (QueVec_t::size_type i = 0; i < r.que.size(); ++i)
    que.push_back(r.que[i]->Copy());
}

PriQueue::~PriQueue()
{
  for (QueVec_t::size_type i = 0; i < que.size(); ++i)
    delete que[i];
}

bool PriQueue::Enque(Packet* p)
{
  Priority_t pri = p->Priority();
  if (que.empty() || pri >= que.size()) return false;

  return que[pri]->Enque(p);
}

Packet* PriQueue::Deque()
{
  for (QueVec_t::reverse_iterator q = que.rbegin(); q != que.rend(); ++q) {
    Packet* p = (*q)->Deque();
    if (p) return p;
  }
  return nil;
}

Packet* PriQueue::PeekDeque()
{
  for (QueVec_t::reverse_iterator q = que.rbegin(); q != que.rend(); ++q) {
    Packet* p = (*q)->PeekDeque();
    if (p) return p;
  }
  return nil;
}

Count_t PriQueue::DequeAllDstMac(MACAddr m)
{
  Count_t count = 0;
  for (QueVec_t::iterator q = que.begin(); q != que.end(); ++q)
    {
      count += (*q)->DequeAllDstMac(m);
    }
  return count;
}

Count_t PriQueue::DequeAllDstIP(IPAddr_t ip)
{
  Count_t count = 0;
  for (QueVec_t::iterator q = que.begin(); q != que.end(); ++q)
    {
      count += (*q)->DequeAllDstIP(ip);
    }
  return count;
}

Packet* PriQueue::DequeOneDstMac(MACAddr m)
{
  for (QueVec_t::iterator q = que.begin(); q != que.end(); ++q)
    {
      Packet* p = (*q)->DequeOneDstMac(m);
      if (p) return p;
    }
  return nil;
}

Packet* PriQueue::DequeOneDstIP(IPAddr_t ip)
{
  for (QueVec_t::iterator q = que.begin(); q != que.end(); ++q)
    {
      Packet* p = (*q)->DequeOneDstIP(ip);
      if (p) return p;
    }
  return nil;
}

Count_t PriQueue::Length()
{
  Count_t len=0;
  for (QueVec_t::iterator q = que.begin(); q != que.end(); ++q)
    len += (*q)->Length();
  return len;
}

Count_t PriQueue::LengthPkts()
{
  Count_t lenPkts=0;
  for (QueVec_t::iterator q = que.begin(); q != que.end(); ++q)
    lenPkts += (*q)->LengthPkts();
  return lenPkts;
}

Queue* PriQueue::Copy() const
{
  return new PriQueue(*this);
}

void PriQueue::SetInterface(Interface* i)
{
#ifndef WIN32
	interface = i;
#else
	interface_ = i;
#endif
  for (QueVec_t::iterator q = que.begin(); q != que.end(); ++q)
    (*q)->SetInterface(i);
}

void PriQueue::SetLimit(Count_t l)
{
  for (QueVec_t::iterator q = que.begin(); q != que.end(); ++q)
    (*q)->SetLimit(l);
}

void PriQueue::SetLimitPkts(Count_t l)
{
  for (QueVec_t::iterator q = que.begin(); q != que.end(); ++q)
    {
      (*q)->SetLimitPkts(l);
      DEBUG0((cout << "PriQueue::SLP " << l << endl));
    }
}

bool PriQueue::Check(Size_t s, Packet* p)
{
  Priority_t pri = 0;
  if (p) pri = p->Priority();
  return que[pri]->Check(s);
}

Packet* PriQueue::GetPacket(Count_t k)
{
  for (QueVec_t::reverse_iterator q = que.rbegin(); q != que.rend(); ++q)
    {
      Packet* p = (*q)->GetPacket(k);
      if (p) return p;

      k -= (*q)->LengthPkts();
    }
  return nil;
}

void PriQueue::GetPacketColors(ColorVec_t& cv)
{
#ifdef HAVE_QT
  for (QueVec_t::reverse_iterator q = que.rbegin(); q != que.rend(); ++q)
    {
      (*q)->GetPacketColors(cv);
    }
#endif
}

Queue* PriQueue::GetQueue(Count_t pri)
{
  if (pri < que.size()) return que[pri];
  return nil; // priority is out of range
}

bool PriQueue::Enque(Packet* p, Count_t pri)
{
  return que[pri]->Enque(p);
}

Packet* PriQueue::Deque(Count_t pri)
{
  return que[pri]->Deque();
}

Packet* PriQueue::PeekDeque(Count_t pri)
{
  return que[pri]->PeekDeque();
}

