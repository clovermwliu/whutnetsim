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

//#define DEBUG_MASK 0x02
#include "G_debug.h"
#include "dualqueue.h"
#include "ipv4.h"
#include "tcp.h"
#include "simulator.h"

using namespace std;

// DualQueue constructors

DualQueue::DualQueue() 
  : Queue(true), limit(Queue::DefaultLength()), pktLimit(0),
    lastTCP(true), tcpEmpty(0), udpEmpty(0)
{
  pTCP = Queue::Default().Copy();
  pNonTCP = Queue::Default().Copy();
}

DualQueue::DualQueue(Count_t l) 
  : Queue(true), limit(l), pktLimit(0),
    lastTCP(true), tcpEmpty(0), udpEmpty(0)
{
  pTCP = Queue::Default().Copy();
  pTCP->SetLimit(l);
  pNonTCP = Queue::Default().Copy();
  pNonTCP->SetLimit(l);
}

// Specify the queues for tcp and non-tcp
DualQueue::DualQueue(const Queue& t, const Queue& n) : Queue(true)
{
  pTCP = t.Copy();
  pNonTCP = n.Copy();
  limit = t.GetLimit();
  pktLimit = t.GetLimitPkts();
  lastTCP = true;
}

// Copy constructor
DualQueue::DualQueue(const DualQueue& c)
    : Queue(c), limit(c.limit), pktLimit(c.pktLimit)
{
  pTCP = c.TCPQueue()->Copy();
  pNonTCP = c.NonTCPQueue()->Copy();
  lastTCP = c.LastTCP();
}

// Destructor
DualQueue::~DualQueue()
{
  if (pTCP) delete pTCP;
  if (pNonTCP) delete pNonTCP;
}

  // Inherited methods from Queue
bool DualQueue::Enque(Packet* p)
{ // Return true if enqued, false if not
  DEBUG0((cout << "DQ " << this << " enquing size " << p->Size() << endl));
  bool b;
  if (IsTCP(p))
    {
      b = pTCP->Enque(p);
      DEBUG(1,(cout << "DQ " << this 
               << " enqTCP, size " << pTCP->LengthPkts() 
               << " time " << Simulator::Now() << endl));
      if (!b) DEBUG0((cout << "DQ " << this << " dropping TCP pkt" << endl));
      return b;
    }
  // Not TCP
  b = pNonTCP->Enque(p);
  DEBUG(1,(cout << "DQ " << this 
           << " enqNonTCP, size " << pNonTCP->LengthPkts() 
           << " time " << Simulator::Now() << endl));
  if (!b) DEBUG(1,(cout << "DQ " << this << " dropping NonTCP pkt" << endl));
  return b;
}

Packet* DualQueue::Deque()
{ // Return next element (NULL if none)
  bool useTCP;
  if (lastTCP)
    { // Try to use Non-TCP next, if non-empty
      useTCP = (pNonTCP->LengthPkts() == 0); // UDP is empty
      if (useTCP) udpEmpty++;                // Count times UDP gave up slot
    }
  else
    {
      useTCP = (pTCP->LengthPkts());         // TCP is not empty
      if (!useTCP) tcpEmpty++;               // Count times TCP gave up slot
    }
  if (useTCP)
    {
      DEBUG(1,(cout << "DQ " << this << " deque TCP" 
              << " size " << pTCP->LengthPkts() 
               << " time " << Simulator::Now() << endl));
      lastTCP = true;
      return pTCP->Deque();
    }
  else
    {
      DEBUG(1,(cout << "DQ " << this << " deque NonTCP" 
              << " size " << pNonTCP->LengthPkts() 
               << " time " << Simulator::Now() << endl));
      lastTCP = false;
      return pNonTCP->Deque();
    }
}

Packet* DualQueue::PeekDeque()
{ // Peek at next element (NULL if none)
  bool useTCP;
  if (lastTCP)
    { // Try to use Non-TCP next, if non-empty
      useTCP = (pNonTCP->LengthPkts() == 0); // UDP is empty
    }
  else
    {
      useTCP = (pTCP->LengthPkts());         // TCP is not empty
    }
  if (useTCP)
    {
      return pTCP->PeekDeque();
    }
  else
    {
      return pNonTCP->PeekDeque();
    }
}

// Most of the following should not be used for DualQueues.
// Rather, the values should be retrived for the TCP/NonTCP queues directly
Count_t DualQueue::Length()
{ // How many bytes are enqued?
  return pTCP->Length() + pNonTCP->Length();
}

Count_t DualQueue::LengthPkts()
{ // How many packets are enqued?
  return pTCP->LengthPkts() + pNonTCP->LengthPkts();
}

Queue* DualQueue::Copy() const
{ // Make a copy of this queue
  return new DualQueue(*this);
}

void DualQueue::SetLimit(Count_t l)
{ // Set new limit
  pTCP->SetLimit(l);
  pNonTCP->SetLimit(l);
}

Count_t DualQueue::GetLimit() const
{  // Get limit (bytes)
  return pTCP->GetLimit() + pNonTCP->GetLimit(); // ?? Is this right?
}

void DualQueue::SetLimitPkts(Count_t l)
{// Set packets limit
  pTCP->SetLimitPkts(l);
  pNonTCP->SetLimitPkts(l);
  return;
}

Count_t DualQueue::GetLimitPkts() const
{ // Set packets limit
  return pTCP->GetLimitPkts() + pNonTCP->GetLimitPkts(); // ?? Is this right?
}

bool DualQueue::Check(Size_t s, Packet* p)
{ // Test if buffer space available
  if (IsTCP(p)) return pTCP->Check(s, p);
  return pNonTCP->Check(s, p);
}

bool DualQueue::Detailed()
{ // True if detailed model w/ local queing
  return true;
}

void DualQueue::Detailed(bool d)
{ // Set detailed on/off
  pTCP->Detailed(d);
  pNonTCP->Detailed(d);
}

Time_t DualQueue::QueuingDelay()
{ // Calculate queuing delay at current lth
  return 0.0; // Not sure here
}

DCount_t DualQueue::TotalWorkload()
{ // Return the byte-seconds (workload)
  return pTCP->TotalWorkload() + pNonTCP->TotalWorkload();
}

void DualQueue::SetInterface(Interface* i)
{ // Let the q know which interface
  pTCP->SetInterface(i);
  pNonTCP->SetInterface(i);
}

Count_t DualQueue::DropCount()
{ // Number of dropped packets
  return pTCP->DropCount() + pNonTCP->DropCount();
}

Count_t DualQueue:: EnqueueCount()
{ // Number of enqueued packets
  return pTCP->EnqueueCount() + pNonTCP->EnqueueCount();
}

void DualQueue::CountEnq(Packet* p)
{ // Count an enque
  if (IsTCP(p)) return pTCP->CountEnq(p);
  return pNonTCP->CountEnq(p);
}

void DualQueue::ResetStats()
{ // Reset the statistics to zero
  pTCP->ResetStats();
  pNonTCP->ResetStats();
  tcpEmpty = 0;
  udpEmpty = 0;
}

Packet* DualQueue::GetPacket(Count_t k)
{
  return nil; // Code this later
}

void    DualQueue::GetPacketColors(ColorVec_t&)
{ // Code  later
}

// Methods specific to DualQueue
void DualQueue::TCPQueue(const Queue& q)
{
  if (pTCP) delete pTCP;
  pTCP = q.Copy();
}

void DualQueue::NonTCPQueue(const Queue& q)
{
  if (pNonTCP) delete pNonTCP;
  pNonTCP = q.Copy();
}

// Private Methods
bool   DualQueue::IsTCP(Packet* p)
{
  if (!p) return false; // Can't tell if no packet
  // We need to inspect the PDU's to see this is TCP packet or not
  Count_t offset = 0;
  PDU* pdu;
  while((pdu = p->PeekPDU(offset++)))
    {
      Layer_t l = pdu->Layer();
      switch(l) {
        case 0 : // Unknown
          break;
        case 3 : // IP
          {
            Count_t v = pdu->Version(); // Get version
            switch(v) {
              case 4 :
                {
                  // Process IPV4
                  IPV4Header* h = (IPV4Header*)pdu;
                  return h->protocol == 6;
                }
                break;
              case 6 :
                // Process IPV6, Not implemented yet
                return false; // Need to code this
                break;
            }
          }
          break;
        case 4 : // TCP or UDP
          {
            L4PDU* l4 = (L4PDU*)pdu;
            return l4->Proto() == 6;
          }
          break;
        }
    }
  // Can't find, assume not TCP
  return false;
}
