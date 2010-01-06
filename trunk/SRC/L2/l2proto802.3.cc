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



// Georgia Tech Network Simulator - Layer 2 802.3 class
// George F. Riley.  Georgia Tech, Spring 2002

// Implementation for 802.3

#include <iostream>

//#define DEBUG_MASK 0x02
#include "G_debug.h"
#include "l2proto802.3.h"
#include "interface.h"
#include "ipv4.h"
#include "trace.h"
#include "node.h"
#include "hex.h"
#include "macaddr.h"
#include "llcsnap.h"
#include "link.h"
#include "packet-callbacks.h"

using namespace std;

L2Proto802_3::L2Proto802_3()
{
}

void L2Proto802_3::BuildDataPDU(MACAddr src, MACAddr dst, Packet* p)
{
  L2802_3Header* l2pdu = new L2802_3Header(src, dst, p->Size());
  p->PushPDU(l2pdu); // Add the 802.3 header 
}

void L2Proto802_3::DataRequest(Packet* p)
{
  // Process callbacks
  if(!iface->GetNode()->CallCallbacks(
         Layer(), 0, PacketCallbacks::TX, p, iface))
    return;
  // Transmit on attached link

  L2802_3Header* l2pdu = (L2802_3Header*)p->PeekPDU();
  if (Trace::Enabled())
    iface->GetNode()->TracePDU(this, l2pdu, p, "-");

  iface->GetLink()->Transmit(p, iface, iface->GetNode());
}

void L2Proto802_3::DataIndication(Packet* p, LinkEvent* le)
{
  bool fbrx = false; // True if first bit rx event
  if (le)
    {
      fbrx = (le->event == LinkEvent::PACKET_FIRSTBIT_RX);
    }
  if (le && !p)
    { // Use packet from link event
      p = le->p;
    }
  if (le) delete le;
  
  // Process callbacks
  if(!iface->GetNode()->CallCallbacks(
         Layer(), 0, PacketCallbacks::RX, p, iface))
    return;
  L2802_3Header* l2pdu = (L2802_3Header*)p->PeekPDU();
  if (Trace::Enabled()) iface->GetNode()->TracePDU(this, l2pdu, p, "+");
  p->PopPDU();
  iface->HandleLLCSNAP(p, fbrx);
}


bool L2Proto802_3::Busy()
{ // For 802.3, the protocol is busy only if link is busy
  Link* l = iface->GetLink();
  return l->Busy();
}

// L2802_3Header methods

// Specifies the size (in bytes) of the 802.3 header when transmitting
// on a link.  Normally, the default 12 is correct.  However, other
// simulators (notably ns2) do not model this, so direct comparisons
// to ns2 are difficult.  Changing this value to 0 (along with the
// corresponding value in the LLCSnap header) will give a more direct
// comparison to ns2.
Size_t L2802_3Header::txSize = 12;


// Construct from serialized buffer
L2802_3Header::L2802_3Header(char* b, Size_t& sz, Packet* p)
{
  DEBUG0((cout << "Constructing l2803-2, initial size " << sz << endl));
  Size_t s = 0;
  b = Serializable::GetSize(b, sz, s);
  DEBUG0((cout << "l2pdu size is " << s << endl));
  s -= sizeof(Size_t);
  sz -= s;
  b = Construct(b, s);
  DEBUG0((cout << "Constructing l2803-2, final  size " << sz << endl));
  p->PushPDUBottom(this);  // Add to packet (bottom of stack)
  new LLCSNAPHeader(b, sz, p);
}

void L2802_3Header::Trace(Tfstream& tos, Bitmap_t b, Packet* p, const char* s)
{  // Trace the contents of this pdu
  tos << " ";
  if (s) 
    tos << s;
  tos << "L2";
  if (Detail(L2Proto802_3::TYPE, b))  tos << "-802.3";
  // Below moved to LLC/SNAP header
  //if (Detail(L2Proto802_3::PROTO, b)) tos << " " << Hex4(l3proto);
  if (Detail(L2Proto802_3::LENGTH, b)) 
    { // Check if packet passed, which we need for length
      if (p)
        tos << " " << p->Size();
    }
  if (Detail(L2Proto802_3::SRC, b))   tos << " " << src;
  if (Detail(L2Proto802_3::DST, b))   tos << " " << dst;
  if (Detail(L2Proto802_3::UID, b))
    {
      if (p) tos << " " << p->uid;
    }
}

Size_t L2802_3Header::SSize()
{
  //return sizeof(src) + sizeof(dst) + sizeof(l3proto);
  return 6 + 6 + 2;
}

char*  L2802_3Header::Serialize(char* b, Size_t& sz)
{ // Serialize to a working buffer
  DEBUG(1,(cout << "Serializing l28023 src " << Hex2(src.macAddr)
           << " dst " << Hex2(dst.macAddr)
           << " lth " << length << endl));
  b = SerializeToBuffer(b, sz, src.macAddr);
  b = SerializeToBuffer(b, sz, dst.macAddr);
  Word_t p = length;
  b = SerializeToBuffer(b, sz, p);
  return b;
}

char*  L2802_3Header::Construct(char* b, Size_t& sz)
{ // Construct from serialized buffer
  b = ConstructFromBuffer(b, sz, src);
  b = ConstructFromBuffer(b, sz, dst);
  Word_t p = 0;
  b = ConstructFromBuffer(b, sz, p);
  length = p;
  DEBUG0((cout << "L2Hdr::Construct, src " << Hex2(src.macAddr)
          << " dst " << Hex2(dst.macAddr)
          << " length " << length << endl));
  return b;
}

