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
// $Id: arp.h,v 1.9 2005/11/15 20:48:02 riley Exp $



// Georgia Tech Network Simulator - Address resolution Protocol class
// Mohamed Abd Elhafez.  Georgia Tech, Spring 2004

// Implements the InternetAddress Resolution Protocol Layer 3

#ifndef __arp_h__
#define __arp_h__

#include <queue>


#include "G_common-defs.h"
#include "l3protocol.h"
#include "pdu.h"
#include "ipaddr.h"
#include "ipv4.h"
#include "event.h"
#include "handler.h"

#define ARP_PROTOCOL 0x806           // the protocol number for the arp packet
#define MAX_RETRIES 5                // Maximum number of retries for resolving a certain entry
#define DEFAULT_RX_TIMEOUT 0.1       // default timeout for arp request in seconds

typedef std::pair<Packet*, int> PacketPair_t;
typedef std::queue<PacketPair_t> ARPQueue_t;


class ARPHeader;
class Node;

class ARPEntry {
public:
  typedef enum{FREE, PENDING, RESOLVED} State_t;

  ARPEntry(): state(FREE), mac(MACAddr::NONE), lifetime(nil), retries(0){}
  ARPEntry(State_t s, MACAddr m, Time_t t, Count_t r) : state(s), mac(m), lifetime(t), retries(r) 
    { 
    }
public:
  State_t        state;      // current state, free, rsolved or pending
  MACAddr        mac;        // mac address of the target
  Time_t         lifetime;   // lifetime of this entry
  Count_t        retries;    // number of arp attempts made for this entry
  ARPQueue_t     pQueue;     // Associated Queue
};


typedef std::map<IPAddr_t, ARPEntry> ARPCache_t;


// Define the event subclass for ARP events
//Doc:ClassXRef
class ARPEvent : public Event {
 public:
  typedef enum { RETRANSMIT } ARPEvent_t;
 public:
  ARPEvent() 
    : p(0), dst(0), iface(0)
    { }
  ARPEvent(Event_t ev) 
    : Event(ev), p(0),  dst(0), iface(0)
    { }
#ifndef WIN32
  ARPEvent(Event_t ev, Packet* pkt, IPAddr_t ip, Interface* interface) 
    : Event(ev), p(pkt), dst(ip), iface(interface)
    { }
#else
  ARPEvent(Event_t ev, Packet* pkt, IPAddr_t ip, Interface* interface_) 
    : Event(ev), p(pkt), dst(ip), iface(interface_)
    { }

#endif
  virtual ~ARPEvent() { }
public:
  Packet*     p;     // Associated packet
  IPAddr_t    dst;   // destination IP address
  Interface*  iface; // source interface
};



//Doc:ClassXRef
class ARP : public L3Protocol, public Handler{
//Doc:Class The class {\tt ARP} implements the Address Resolution Protocol
//Doc:Class It derives 	from the class {\tt L3Protocol}. 
	
public:
  typedef enum {NONE, REQUEST, REPLY} ARPPacket_t;
  typedef enum {HWMACADDRTYPE, PROTOADDRTYPE, HWMACADDRSIZE,
                 PROTOADDRSIZE, OP,
                 SRCMACADDR, SRCIPADDR,
                 DSTMACADDR, DSTIPADDR , UID} ARPTrace_t; // Tracing options
  //Doc:Method
  ARP(Node*);
  //Doc:Desc Constructor.
  //Doc:Arg1 Node pointer, used to add this arp protocol to the
  //Doc:Arg1 local protocol graph for this node.

  // Handler
  virtual void Handle(Event*, Time_t);

  //Doc:Method
  void ARPRequest(Interface* , Packet*, int); // prepare an arp request for that target
  //Doc:Desc This method is used to generate an arp request for a certain IP adress
  //Doc:Arg1 A pointer to the interface which needs to resolve the IP address
  //Doc:Arg2 The packet that is queued for the destination
  //Doc:Arg3 The type of the packet


  //Doc:Method
  void DataIndication(Interface*, Packet*);    // From lower layer
  //Doc:Desc This method is used to pass on a packet from any layer 2 protocol
  //Doc:Desc up the stack.
  //Doc:Arg1 A pointer to the interface at which this packet is received
  //Doc:Arg2 A pointer to the packet that was received
  
   
  //Doc:Method
  virtual Proto_t Proto()   { return ARP_PROTOCOL;} // Get the protocol number
  //Doc:Desc This method returns the IANA assigned protocol number
  //Doc:Return the protocol number

  virtual void DataRequest(Node*, Packet*,   void*) {return;}
  virtual Interface* PeekInterface(Node*,   void*) {return nil;}
  virtual Count_t Version() {return 0;}

  //Doc:Method
  void SetARPTimeout(Time_t t){ timeout = t;}
  //Doc:Desc This method sets the timeout for the arp entries
  //Doc:Arg1 the new timeout in seconds

 public:
  ARPCache_t arpcache;    // The arp cache that stores all the entries
  Time_t     timeout;     // Timeout for the arp entries
  Time_t     rxtimeout;   // Timout for retransmitting arp request
private:
  static Trace::TraceStatus traceStatus; // For tracing
 
};

//Doc:ClassXRef
class ARPHeader : public PDU {
//Doc:Class	
public :
  //Doc:Method
  ARPHeader();
  //Doc:Desc This is the default constructor
 
  //Doc:Method 
  ARPHeader(const ARPHeader& r);
  //Doc:Desc This constructor takes the reference to an ARP header

  //Doc:Method
  virtual ~ARPHeader();
  //Doc:Desc Destructor
  
  //Doc:Method
  Size_t  Size() const;
  //Doc:Desc This method returns the size of the PDU
  //Doc:Return the size of the PDU
  
  //Doc:Method
  PDU*    Copy() const { return new ARPHeader(*this);}
  //Doc:Desc This method creates a copy of this PDU
  //Doc:Return a pointer to the copy of this PDU
  
  //Doc:Method
  Layer_t Layer() { return 3;}         // Indicate layer 3 pdu
  //Doc:Desc This method returns the layer number it belongs in the ISO model
  //Doc:Return the layer number

  Proto_t Proto() { return ARP_PROTOCOL;} // Protocol number for ARP
  Size_t  SSize();                     // Size needed for serialization
  char*   Serialize(char*, Size_t&);   // Serialize to a buffer
  char*   Construct(char*, Size_t&);   // Construct from buffer

  //Doc:Method
  void    Trace(Tfstream&, Bitmap_t,  // Trace the contents of this pdu
                     Packet* = nil, const char* = nil);
  //Doc:Desc This method traces the contents of the PDU
  //Doc:Arg1 File output stream descriptor
  //Doc:Arg2 Bitmap that specifies the level of detail of the trace
  //Doc:Arg3 Packet that contains this PDU (optional)
  //Doc:Arg4 Extra text information for trace file (optional)

     
public:
  Word_t    hwmacaddrtype;  // The hardware address type, default is ethernet mac
  Word_t    protoaddrtype;  // The protocol address type, default is IP
  Byte_t    hwmacaddrsize;  // The size for the hardware address, default is 6 for ethernet mac
  Byte_t    protoaddrsize;  // The size for the protocol address, default is 4 for IPV4
  Word_t    op;             // Either Request or Reply
  MACAddr   srcmacaddr;     // The mac address of the source of the packet
  IPAddr    srcipaddr;      // The IP address of the source of the packet
  MACAddr   dstmacaddr;     // The mac address of the destination of the packet
  IPAddr    dstipaddr;      // The IP address of the destination of the packet
  //Count_t   uid;            // Unique id (not part of ARP)
};


#endif

