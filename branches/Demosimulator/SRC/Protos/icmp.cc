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
// $Id: icmp.cc,v 1.10 2005/11/15 20:48:03 riley Exp $



// Georgia Tech Network Simulator - ICMP Base Class
// George F. Riley.  Georgia Tech, Summer 2004

#include <iostream>

#include "debug.h"
#include "icmp.h"
#include "protograph.h"
#include "application.h"
#include "tcp.h"

#ifdef HAVE_QT
#include <qcolor.h>
#endif

using namespace std;

// Static variables
bool           ICMP::enabled = false;
bool           ICMP::quench = false;
ICMP*          ICMP::instance = nil;
ICMPCallback_t ICMP::callbacks;

// ICMP Methods
ICMP::ICMP()
{ // ICMP has only static variables, so no initialization needed
}

ICMP::~ICMP()
{ // Destructor, nothing needed
}

void ICMP::DataIndication(Node* n, Packet* p, IPAddr_t ip,
                          L3Protocol& l3, Interface* i)
{ // Called when an ICMP packet is received at an addressed host.
  // ICMP finds the callback(s) for the node specified and calls it.
  // The IPV4 packet has been popped, the ICMP header is still on the stack
  //cout << "Received ICMP packet" << endl;
  IPV4Header* ipv4 = (IPV4Header*)p->PeekPDU(-1);
  ICMPHeader* icmp = (ICMPHeader*)p->PeekPDU();
  // Trace it
  //Trace::Instance()->NewNode(n);
  n->TracePDU(this, icmp, p, "+");
  // First see if any automatic replies are needed
  DEBUG0((cout << "ICMP::DataIndication, packet " << p << endl));
  switch(icmp->type) {
    case ICMP::ECHO :
      DEBUG0((cout << "ICMP::DataInd::ECHO from " << (string)IPAddr(ip)
              << " to " << (string)IPAddr(ipv4->dst)
              << " p " << p << endl));
      EchoReply(n, p, ipv4, icmp);
      break;
    case ICMP::ECHO_REPLY :
    case ICMP::SOURCE_QUENCH :
    case ICMP::TIME_EXCEEDED :
      break; // No action needed for these
    case ICMP::TIMESTAMP :
      TimestampReply(n, p, ipv4, icmp);
      break;
    case ICMP::DESTINATION_UNREACHABLE :
      if (icmp->ipv4Hdr && icmp->l4Hdr)
        { // Both l3 and l4 headers echo'd.  Try to kill any pending connection
          KillPendingConnection(n, icmp->ipv4Hdr, icmp->l4Hdr);
        }
      break;
    case ICMP::TIMESTAMP_REPLY :
    case ICMP::REDIRECT :
    case ICMP::INFORMATION_REQUEST :
    case ICMP::INFORMATION_REPLY :
    case ICMP::PARAMETER_PROBLEM :
      // None of these are implemented
      break;
  }
  
  // Find the callback (if any)
  ICMPCallback_t::iterator it = callbacks.find(n);
  if (it != callbacks.end())
    { // Found it
      ApplVec_t& apps = it->second;
      for (ApplVec_t::size_type k = 0; k < apps.size(); ++k)
        { // Notify each application in the callback list
          // NOTE!  The applications ARE responsible for the
          // packet, as a copy of the packet is forwarded.
          // This subroutine deletes the original packet.
          Application* app = apps[k];
          app->Receive(p->Copy(), nil, 0);
        }
    }
  delete p;
}
#ifdef NOT_USED
// All of the following unused.
NCount_t ICMP::Send(Size_t){return 0;}
NCount_t ICMP::Send(char*, Size_t){return 0;}
NCount_t ICMP::Send(Data&){return 0;}
NCount_t ICMP::SendTo(Size_t, IPAddr_t, PortId_t){return 0;}
NCount_t ICMP::SendTo(char*, Size_t, IPAddr_t, PortId_t){return 0;}
NCount_t ICMP::SendTo(Data&, IPAddr_t, PortId_t){return 0;}
bool     ICMP::Connect(IPAddr_t, PortId_t){return false;}
bool     ICMP::Close(){return false;}

L4Protocol* ICMP::Copy() const
{ // Make a copy of this protocol
  return new ICMP(*this);
}
#endif

// Static methods
void ICMP::Enable(bool e)
{
  if (e)
    { // enabling
      if (enabled) return;
      if (instance) return;
      instance = new ICMP();
      enabled = true;
      ProtocolGraph::CreateCommon();
      // Register ICMP in the protocol graph
      ProtocolGraph::common->Insert(4, (Proto_t)ProtoNumber, instance);
    }
  else
    { // disabling
      if (!enabled) return;
      if (!instance) return;
      ProtocolGraph::common->Remove(4, (Proto_t)ProtoNumber);
      delete instance;
      instance = nil;
      enabled = false;
    }
}

void ICMP::EnableQuench(bool e)
{
  if (!enabled) return;
  quench = e;
}


// Methods for each ICMP message generation, used mostly by IPV4
void ICMP::DestinationUnreachable(Node* n, Packet* p, IPV4Header* ip,
                                  L4PDU* l4, DU_t c)
{ // Prepare a ICMP DestinationUnreachable packet and forward to IPV4
  if (!n->ICMPEnabled()) return; // This node does not support ICMP
  if (!enabled) return;
  //  cout << "sending du at time " << Simulator::Now() << endl;
#ifdef HAVE_QT
  ColoredPacket* p1 = new ColoredPacket(Qt::red);
#else
  Packet* p1 = new Packet();
#endif
  ICMPHeader* icmp = new ICMPHeader(DESTINATION_UNREACHABLE);
  icmp->code = c;
  // Copy the l3 and l4 headers if specified
  if (ip) icmp->ipv4Hdr = (IPV4Header*)ip->Copy();
  if (l4) icmp->l4Hdr   = (L4PDU*)l4->Copy();
  p1->PushPDU(icmp); // Add the ICMP pdu
  // Trace it
  //Trace::Instance()->NewNode(n);
  n->TracePDU(ICMP::Instance(), icmp, p1, "-");
  //cout << "ICMP pckt frm " << IPAddr::ToDotted(ip->src) << " 2 " << IPAddr::ToDotted(ip->dst) << endl;
  IPV4ReqInfo info(ip->src, ip->dst, 64, ProtoNumber);
  IPV4::Instance()->DataRequest(n, p1, &info);
}

void ICMP::TimeExceeded(Node* n, Packet* p, IPV4Header* ip, TE_t c)
{ // Prepare a ICMP TimeExceeded packet and forward to IPV4
  if (!n->ICMPEnabled()) return; // This node does not support ICMP
  if (!enabled) return;
#ifdef HAVE_QT
  ColoredPacket* p1 = new ColoredPacket(Qt::red);
#else
  Packet* p1 = new Packet();
#endif
  ICMPHeader* icmp = new ICMPHeader(TIME_EXCEEDED);
  icmp->code = c;
  p1->PushPDU(icmp); // Add the ICMP pdu
  // Trace it
  //Trace::Instance()->NewNode(n);
  n->TracePDU(ICMP::Instance(), icmp, p1, "-");
  IPV4ReqInfo info(ip->src, IPADDR_NONE, 64, ProtoNumber);
  IPV4::Instance()->DataRequest(n, p1, &info);
}

void ICMP::SourceQuench(Node* n, Packet*, IPV4Header*)
{
  if (!n->ICMPEnabled()) return; // This node does not support ICMP
  if (!enabled) return;
}

void ICMP::EchoReply(Node* n, Packet* p, IPV4Header* ip, ICMPHeader* icmp)
{ // Form an echo reply (PING response)
  if (!n->ICMPEnabled()) return; // This node does not support ICMP
  if (!enabled) return;
#ifdef HAVE_QT
  ColoredPacket* p1 = new ColoredPacket(Qt::red);
#else
  Packet* p1 = new Packet();
#endif
  ICMPHeader* icmp1 = (ICMPHeader*)icmp->Copy();
  icmp1->type = ECHO_REPLY;
  p1->PushPDU(icmp1); // Add the ICMP pdu
  // Trace it
  //Trace::Instance()->NewNode(n);
  n->TracePDU(ICMP::Instance(), icmp1, p1, "-");
  IPV4ReqInfo info(ip->src, IPADDR_NONE, 64, ProtoNumber);
  DEBUG0((cout << "ICMP::EchoReply sending pkt " << p1
          << " to ip " << (string)IPAddr(ip->src) << endl));
  IPV4::Instance()->DataRequest(n, p1, &info);
}

void ICMP::TimestampReply(Node* n, Packet*, IPV4Header*, ICMPHeader*)
{
  if (!n->ICMPEnabled()) return; // This node does not support ICMP
  if (!enabled) return;
}

  
// Static methods used by applications to generate ICMP requests
// Generate an echo message
void ICMP::Echo(Node* n, ICMPHeader* icmp, IPAddr_t dst)
{
  if (!enabled) return;
#ifdef HAVE_QT
  ColoredPacket* p1 = new ColoredPacket(Qt::red);
#else
  Packet* p1 = new Packet();
#endif
  icmp->type = ECHO;
  p1->PushPDU(icmp);
  // Trace it
  //Trace::Instance()->NewNode(n);
  n->TracePDU(ICMP::Instance(), icmp, p1, "-");
  IPV4ReqInfo info(dst, IPADDR_NONE, 64, ProtoNumber);
  DEBUG0((cout << "Creating PING pkt " << p1 
          << " to node " << (string)IPAddr(dst) << endl));
  IPV4::Instance()->DataRequest(n, p1, &info);
}

//Generate timestamp req
void ICMP::Timestamp(Node* n, ICMPHeader* icmp, IPAddr_t dst)
{
  if (!enabled) return;
#ifdef HAVE_QT
  ColoredPacket* p1 = new ColoredPacket(Qt::red);
#else
  Packet* p1 = new Packet();
#endif
  icmp->type = TIMESTAMP;
  p1->PushPDU(icmp);
  // Trace it
  //Trace::Instance()->NewNode(n);
  n->TracePDU(ICMP::Instance(), icmp, p1, "-");
  IPV4ReqInfo info(dst, IPADDR_NONE, 64, ProtoNumber);
  IPV4::Instance()->DataRequest(n, p1, &info);
}


// Request a callback on ICMP message receipt
void ICMP::AddCallback(Node* n, Application* a)
{
  ApplVec_t& apps = callbacks[n];
  // Make sure the app is not already listed
  for (ApplVec_t::size_type i = 0; i < apps.size(); ++i)
    {
      if (apps[i] == a) return;
    }
  // Add it
  apps.push_back(a);
}

// ICMP Private methods
void ICMP::KillPendingConnection(Node* n, IPV4Header* l3, L4PDU* l4)
{
  if (l4->Proto() !=  TCP::ProtoNumber) return; // Not TCP
  TCPHeader* tcpHdr = (TCPHeader*)l4;
  // Find the src/dst srcport/dstport 4-tuple  to lookup bound TCP proto
  IPAddr_t dstIP = l3->dst;
  IPAddr_t srcIP = l3->src;
  PortId_t dstPort = tcpHdr->destPort;
  PortId_t srcPort = tcpHdr->sourcePort;
  TCP* tcp = (TCP*)n->LookupByPort(
      TCP::ProtoNumber, srcPort, srcIP, dstPort, dstIP);
  if (tcp)
    { // Found a tcp, call abort
      tcp->Abort();
    }
}

// ICMPHeader methods
ICMPHeader::ICMPHeader()
    : type(ICMP::ECHO_REPLY), code(0), identifier(0), sequence(0),
      originateTimestamp(0), receiveTimestamp(0), transmitTimestamp(0),
      ipv4Hdr(nil), l4Hdr(nil)
{ // Nothing else needed
}

ICMPHeader::ICMPHeader(ICMP::ICMPMsg_t t)
    : type(t), code(0), identifier(0), sequence(0),
      originateTimestamp(0), receiveTimestamp(0), transmitTimestamp(0),
      ipv4Hdr(nil), l4Hdr(nil)
{ // Nothing else needed
}

  
ICMPHeader::ICMPHeader(const ICMPHeader& h)
    : type(h.type), code(h.code),
      identifier(h.identifier), sequence(h.sequence),
      originateTimestamp(h.originateTimestamp),
      receiveTimestamp(h.receiveTimestamp),
      transmitTimestamp(h.transmitTimestamp),
      ipv4Hdr(nil), l4Hdr(nil)
{
  if (h.ipv4Hdr) ipv4Hdr = (IPV4Header*)h.ipv4Hdr->Copy();
  if (h.l4Hdr)   l4Hdr = (L4PDU*)h.l4Hdr->Copy();
}

Size_t  ICMPHeader::Size() const
{
  Size_t ipSize = 0;
  Size_t s = 0;
  if (ipv4Hdr) ipSize = ipv4Hdr->Size();
  if (l4Hdr)   ipSize += l4Hdr->Size();
  switch(type) {
    case ICMP::ECHO :
    case ICMP::ECHO_REPLY :
      s = 8;
      break;
    case ICMP::DESTINATION_UNREACHABLE :
    case ICMP::SOURCE_QUENCH :
    case ICMP::TIME_EXCEEDED :
      s = 8 + 8;
      break;
    case ICMP::TIMESTAMP :
    case ICMP::TIMESTAMP_REPLY :
      s = 8 + 12;
      break;
    case ICMP::REDIRECT :
    case ICMP::INFORMATION_REQUEST :
    case ICMP::INFORMATION_REPLY :
    case ICMP::PARAMETER_PROBLEM :
      s = 0; // Not implemented
  }
  return ipSize + s;
}

       
PDU*    ICMPHeader::Copy() const
{
  return new ICMPHeader(*this);
}


// Serialization
Size_t  ICMPHeader::SSize()
{ 
  return
      sizeof(unsigned long) +  // type
      sizeof(code) +
      sizeof(identifier) + 
      sizeof(sequence) +
      sizeof(originateTimestamp) +
      sizeof(receiveTimestamp) + 
      sizeof(transmitTimestamp);
}

char*   ICMPHeader::Serialize(char* b, Size_t& sz)
{
  b = SerializeToBuffer(b, sz, (unsigned long&)type);
  b = SerializeToBuffer(b, sz, code);
  b = SerializeToBuffer(b, sz, identifier);
  b = SerializeToBuffer(b, sz, sequence);
  b = SerializeToBuffer(b, sz, originateTimestamp);
  b = SerializeToBuffer(b, sz, receiveTimestamp);
  b = SerializeToBuffer(b, sz, transmitTimestamp);
  return b;
}

char*   ICMPHeader::Construct(char* b , Size_t& sz)
{
  b = ConstructFromBuffer(b, sz, (unsigned long&)type);
  b = ConstructFromBuffer(b, sz, code);
  b = ConstructFromBuffer(b, sz, identifier);
  b = ConstructFromBuffer(b, sz, sequence);
  b = ConstructFromBuffer(b, sz, originateTimestamp);
  b = ConstructFromBuffer(b, sz, receiveTimestamp);
  b = ConstructFromBuffer(b, sz, transmitTimestamp);
  return b;
}

void    ICMPHeader::Trace(Tfstream& tos, Bitmap_t b, Packet* p, const char* s)
{
  tos << " ";
  if (s)
    tos << s;
  tos << "L4-ICMP";
  if (Detail(ICMP::TYPE, b))         tos << " " << type;
  if (Detail(ICMP::CODE, b))         tos << " " << code;
  switch(type){
    case ICMP::ECHO:
    case ICMP::ECHO_REPLY:
      if (Detail(ICMP::IDENTIFIER, b)) tos << " " << identifier;
      if (Detail(ICMP::SEQUENCE,   b)) tos << " " << sequence;
      break;
    case ICMP::TIMESTAMP:
    case ICMP::TIMESTAMP_REPLY:
      if (Detail(ICMP::ORIGIN_TIME,   b))   tos << " " << originateTimestamp;
      if (Detail(ICMP::RECEIVE_TIME,  b))   tos << " " << receiveTimestamp;
      if (Detail(ICMP::TRANSMIT_TIME, b))   tos << " " << transmitTimestamp;
      break;
    default:
      break;
  }
  return;
}


