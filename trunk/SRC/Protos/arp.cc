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
// $Id: arp.cc,v 1.10 2006/01/20 17:37:47 riley Exp $



// Georgia Tech Network Simulator - Address Resolution Protocol Class
// Mohamed Abd Elhafez.  Georgia Tech, Spring 2004

// Implements the Address Resolution Protocol, Layer 3


#include <iostream>

#ifdef HAVE_QT
#include <qnamespace.h>
#endif

#include "debug.h"
#include "arp.h"
#include "l2proto802.3.h"
#include "node.h"
#include "droppdu.h"
#include "hex.h"
#include "interface.h"
#include "simulator.h"

using namespace std;


ARPHeader::ARPHeader() : 
  hwmacaddrtype(1),
  protoaddrtype(0x800),
  hwmacaddrsize(6),
  protoaddrsize(4),
  op(0),
  srcmacaddr(MACAddr::NONE),
  srcipaddr(),
  dstmacaddr(MACAddr::NONE),
  dstipaddr()
{
}

ARPHeader::ARPHeader(const ARPHeader& r) : 
  hwmacaddrtype(r.hwmacaddrtype),
  protoaddrtype(r.protoaddrtype),
  hwmacaddrsize(r.hwmacaddrsize),
  protoaddrsize(r.protoaddrsize),
  op(r.op),
  srcmacaddr(r.srcmacaddr),
  srcipaddr(r.srcipaddr),
  dstmacaddr(r.dstmacaddr),
  dstipaddr(r.dstipaddr)
{
}

ARPHeader::~ARPHeader()
{
}

Size_t ARPHeader::Size() const
{
  Count_t sz = 28;
  return sz;
}

// Serialization
Size_t  ARPHeader::SSize()
{ 
  return
    sizeof(hwmacaddrtype) +  // type
    sizeof(protoaddrtype) +
    sizeof(hwmacaddrsize) + 
    sizeof(protoaddrsize) +
    sizeof(op) +
    sizeof(srcmacaddr) + 
    sizeof(srcipaddr) + 
    sizeof(dstmacaddr) + 
    sizeof(dstipaddr);
}

char*   ARPHeader::Serialize(char* b, Size_t& sz)
{
  IPAddr_t src = srcipaddr;
  IPAddr_t dst = dstipaddr;

  b = SerializeToBuffer(b, sz, hwmacaddrtype);
  b = SerializeToBuffer(b, sz, protoaddrtype);
  b = SerializeToBuffer(b, sz, hwmacaddrsize);
  b = SerializeToBuffer(b, sz, protoaddrsize);
  b = SerializeToBuffer(b, sz, op);
  b = SerializeToBuffer(b, sz, srcmacaddr);
  b = SerializeToBuffer(b, sz, src);
  b = SerializeToBuffer(b, sz, dstmacaddr);
  b = SerializeToBuffer(b, sz, dst);
  return b;
}

char*   ARPHeader::Construct(char* b , Size_t& sz)
{
  IPAddr_t src ;
  IPAddr_t dst;

  b = ConstructFromBuffer(b, sz, hwmacaddrtype);
  b = ConstructFromBuffer(b, sz, protoaddrtype);
  b = ConstructFromBuffer(b, sz, hwmacaddrsize);
  b = ConstructFromBuffer(b, sz, protoaddrsize);
  b = ConstructFromBuffer(b, sz, op);
  b = ConstructFromBuffer(b, sz, srcmacaddr);
  b = ConstructFromBuffer(b, sz, src);
  b = ConstructFromBuffer(b, sz, dstmacaddr);
  b = ConstructFromBuffer(b, sz, dst);

  srcipaddr = IPAddr(src);
  dstipaddr = IPAddr(dst);
  return b;
 }



void ARPHeader::Trace(Tfstream& tos, Bitmap_t b, Packet* p, const char* s)
 {
 
  tos << " ";
  if (s)
    tos << s;
  tos << "L3-ARP";
  if (Detail(ARP::HWMACADDRTYPE, b))     tos << " " << hwmacaddrtype;
  if (Detail(ARP::PROTOADDRTYPE, b))     tos << " " << protoaddrtype;
  if (Detail(ARP::HWMACADDRSIZE, b))     tos << " " << hwmacaddrsize;
  if (Detail(ARP::PROTOADDRSIZE, b))     tos << " " << protoaddrsize;
  if (Detail(ARP::OP, b))
    {
      if(op == ARP::REQUEST) tos << " REQ" ;
      else if(op == ARP::REPLY) tos << " RPL";
    }
  if (Detail(ARP::SRCIPADDR, b))     tos << " " << srcipaddr;
  if (Detail(ARP::DSTIPADDR, b))     tos << " " << dstipaddr;
  if (Detail(ARP::SRCMACADDR, b))    tos << " " << srcmacaddr;
  if (Detail(ARP::DSTMACADDR, b))    tos << " " << dstmacaddr;

  if (Detail(ARP::UID, b))
    { // Get uid from packet
      if (p)
        tos << " " << p->uid;
      else
        tos << " " << 0;
    }
}

// IP Protocol Class Methods

ARP::ARP(Node* n) 
{
  rxtimeout = DEFAULT_RX_TIMEOUT;
  // Insert this protocol object in the local protocol graph for this node
  // if the node exists.  If it doesn't exist, it will be added when
  // the node pointer is associted with the interface.
  if (n) n->InsertProto(3, Proto(), this);

  // Set default tracing options
  DetailOff(HWMACADDRTYPE);
  DetailOff(PROTOADDRTYPE);
  DetailOff(HWMACADDRSIZE);
  DetailOff(PROTOADDRSIZE);
}


void ARP::ARPRequest(Interface* i, Packet* p, int type)
{
  IPV4Header* iphdr = (IPV4Header*)p->PeekPDU();
  DEBUG(0, (cout << "Sending arp req for IP " << IPAddr::ToDotted(iphdr->dst) << endl));
  
  // check the arp cache for the destination IP
  ARPCache_t::iterator j = arpcache.find(iphdr->dst);
  if (j != arpcache.end()) // found then just send the packet
    {
      DEBUG(0, (cout << "Entry " << IPAddr::ToDotted((*j).first)  << " expires at " <<(*j).second.lifetime << endl));

      if((*j).second.lifetime > Simulator::Now() ) // not Expired
	{
	  DEBUG(0, (cout << "Entry found mac is "));
	  if ((*j).second.state == ARPEntry::RESOLVED)  // We already know the MAC address
	    {
	      //	      cout << (*j).second.mac << " forwarding pkt to iface" << endl;
	      // Send the packet to the destination mac
	      i->Send(p, (*j).second.mac, type);
	    }
	  else if  ((*j).second.state == ARPEntry::PENDING) // We havent resolved the IP address yet
	    {
	      DEBUG(0, (cout << "Unresolved yet queuing packet" << endl));

	      // Queue the packet for later
	      (*j).second.pQueue.push(PacketPair_t( p, type));
	    }
	  return;
	}
     //  else  // entry is expired
// 	{
// 	  cout << "Entry " << IPAddr::ToDotted((*j).first)  << " Is expired" << endl;
// 	  // send the packet to the old address
// 	  //i->Send(p, (*j).second.mac, type);
// 	}
    }
 
  DEBUG(0,(cout << "Entry not found or expired, creating new" << endl));

  // ARP entry doesnt exist or has expired
  // create cache entry
  ARPEntry e = ARPEntry(ARPEntry::PENDING, MACAddr::NONE, Simulator::Now() + timeout, 1);
  e.pQueue.push(PacketPair_t(p, type));  // Queue the packet for later
  arpcache[iphdr->dst] = e;              // store the new entry
  
  // prepare arp request packet
  Packet* pkt;
#ifdef HAVE_QT
  pkt = new ColoredPacket(Qt::green);  // green packets
#else
  pkt = new Packet();  
#endif
  
  ARPHeader* arphdr = new ARPHeader();
  arphdr->op = ARP::REQUEST;
  arphdr->dstmacaddr = MACAddr::NONE;  // The mac address is gonna be filled in the reply
  arphdr->dstipaddr = iphdr->dst;
  arphdr->srcmacaddr = i->GetMACAddr();
  arphdr->srcipaddr = i->GetIPAddr();
  
  pkt->PushPDU(arphdr);
  // Add to trace file
  if (Trace::Enabled()) i->GetNode()->TracePDU(this, arphdr, pkt, "-");
  
  // schedule retransmition
  Scheduler::Schedule(new ARPEvent(ARPEvent::RETRANSMIT, pkt->Copy(), iphdr->dst, i), rxtimeout, this);

  // give packet to iface to send it
  // ARP request packets are broadcast
  i->Send(pkt,  MACAddr::Broadcast(), ARP_PROTOCOL);
}


void ARP::DataIndication(Interface* i,Packet* p)
{
  ARPHeader* hdr = (ARPHeader*)p->PopPDU();

  DEBUG(0,(cout << "Got ARP Packet " << p->uid << " For IP " << IPAddr::ToDotted(hdr->dstipaddr) << " my IP is " <<  i->GetIPAddr() << endl));

  // Add to trace file
  if (Trace::Enabled()) i->GetNode()->TracePDU(this, hdr, p, "+");
  
  // is this ARP packet for my IP Address ?
  if(hdr->dstipaddr == i->GetIPAddr())
    {
      ARPCache_t::iterator j = arpcache.find(hdr->srcipaddr); // Do we already have the source entry in our cache ?
      if (j != arpcache.end()) // found
	{
	  //cout << "Entry found updating it " << endl;

	  // Uodating the ARP entry
	  (*j).second.lifetime = Simulator::Now() + timeout;
	  (*j).second.mac = hdr->srcmacaddr;  // Updating the mac address in case the IP Addresses are not static

	  if((*j).second.state == ARPEntry::PENDING)
	    {
	      (*j).second.state = ARPEntry::RESOLVED;

	      // Now that we have resolved the MAC address we send all queued packets
	      while ( !(*j).second.pQueue.empty() ) // send the queued pkts
		{
		  PacketPair_t pp = (*j).second.pQueue.front();
		  i->Send(pp.first, hdr->srcmacaddr, pp.second);
		  (*j).second.pQueue.pop(); // Remove from the queue
		}
	    }
	  (*j).second.retries = 0;
	}
      else
	{
	  // create cache entry
	  ARPEntry e = ARPEntry(ARPEntry::RESOLVED, hdr->srcmacaddr, Simulator::Now() + timeout, 0);
	  arpcache[hdr->srcipaddr] = e;
	}
      
      if(hdr->op == ARP::REQUEST)
	{
	  // prepare arp reply packet
	  Packet* pkt;
#ifdef HAVE_QT
	  
	  pkt = new ColoredPacket(Qt::blue);  // blue packets
#else
	  pkt = new Packet(); 
#endif
	  
	  ARPHeader* arphdr = new ARPHeader();
	  arphdr->op = ARP::REPLY;
	  arphdr->dstmacaddr = hdr->srcmacaddr;
	  arphdr->dstipaddr = hdr->srcipaddr;
	  arphdr->srcmacaddr = i->GetMACAddr();
	  arphdr->srcipaddr = i->GetIPAddr();

	  pkt->PushPDU(arphdr);
          // Add to trace file
	  if (Trace::Enabled()) i->GetNode()->TracePDU(this, arphdr, pkt, "+");
	  // give packet to iface to send it
          // ARP reply packets are unicast
	  i->Send(pkt, arphdr->dstmacaddr, ARP_PROTOCOL);
	}
    }
  else
    {
      // just drop the packet as it is not addressed to this host
      DEBUG(0,(cout << "dropping packet " << p->uid << endl));
      DropPDU d("L3-NA", p); // NA= Not Addressed
      i->GetNode()->TracePDU(nil, &d);
      Stats::pktsDropped++;
      delete p;
    }
}
  
void ARP::Handle(Event* e, Time_t t)
{
  ARPEvent* ae = (ARPEvent*)e;
  ARPCache_t::iterator j;
  switch(ae->event) {
  case ARPEvent::RETRANSMIT :
    j = arpcache.find(ae->dst);
    if (j != arpcache.end()) // found
      {
	if((*j).second.state == ARPEntry::PENDING)
	  {
	    if( ((*j).second.retries++ > MAX_RETRIES) || ((*j).second.lifetime < Simulator::Now()) )
	      {
		while ( !(*j).second.pQueue.empty() ) // delete the queued pkts
		  {
		    PacketPair_t pp = (*j).second.pQueue.front();
		    DropPDU d("L3-NE", pp.first);  // NE = Not Exist (cant find mac address for that IP)
		    ae->iface->GetNode()->TracePDU(nil, &d);
		    DEBUG(0,(cout << "Packet Dropped " << endl));
		    Stats::pktsDropped++;
		    delete pp.first;
		    (*j).second.pQueue.pop();
		}
		// delete entry
		arpcache.erase(j);
	      }
	    else
	      {
		//cout << "Resending the arp request packet " << ae->p->uid << " at time " << t << endl;

		// Send the arp request one more time
		Scheduler::Schedule(
                    new ARPEvent(ARPEvent::RETRANSMIT,
                                 ae->p->Copy(), ae->dst, ae->iface),
                    rxtimeout, this);
		ae->iface->Send(ae->p,  MACAddr::Broadcast(), ARP_PROTOCOL);
	      }
	  }
      }
    break;
  default:
    break;
  }
  delete ae;
}


