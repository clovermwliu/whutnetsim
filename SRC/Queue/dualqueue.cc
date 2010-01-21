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
// $Id: dualqueue.cc,v 1.5 2004/09/17 16:11:02 riley Exp $



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
