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
// Peng Zhen, Mon, 4, Jan, 2009 

#ifndef __l2proto_802_3_h
#define __l2proto_802_3_h

#include "l2proto.h"
#include "tfstream.h"
#include "macaddr.h"

class LinkEvent;

//Doc:ClassXRef
class L2Proto802_3 : public L2Proto {
//Doc:Class Class {\tt L2Proto802\_3} defines the 802.3 layer 2 protocol. 
//Doc:Class This is derived from class {\tt L2Proto}. It besically implements
//Doc:Class the Medium Access Control portion of layer 2. 
public:
  typedef enum { TYPE, PROTO, LENGTH, SRC, DST, UID } L2Trace_t;  // Tracing options
  //Doc:Method
  L2Proto802_3(); // Constructor
  //Doc:Desc The Default Constructor

  //Doc:Method
  virtual   void BuildDataPDU(MACAddr, MACAddr, Packet*);
    //Doc:Desc Builds the layer2 pdu and appends it to the specified packet.
    //Doc:Arg1 Source MAC address
    //Doc:Arg2 Destination MAC address
    //Doc:Arg3 Packet pointer to append this pdu to.

  //Doc:Method
  void DataRequest(Packet*);
  //Doc:Desc This is the real member function is called by the higher layer 
  //Doc:Desc requesting a transfer of  data to a peer for 802.3
  //Doc:Arg1 The packet to be sent
  
  //Doc:Method
  void DataIndication(Packet*, LinkEvent* = 0);    // from lower layer
  //Doc:Desc This method is called when the direction of protocol stack
  //Doc:Desc traversal is upwards, i.e, a packet has just been received
  //Doc:Desc at the underlying link
  //Doc:Arg1 The LinkEvent with the received packet (may be nil)
  //Doc:Arg1  If non-nil, the DataIndication must delete or re-use the event
  //Doc:Arg2 The received packet

  //Doc:Method
  virtual   bool Busy();                     // True if proto/link is busy
    //Doc:Desc Test if the protocol (or link) is busy processing a packet.
    //Doc:Return True if currently busy.

  //Doc:Method
   L2Proto802_3*  Copy() const { return new L2Proto802_3 (*this); }
  //Doc:Desc This method makes a copy of the protocol object and returns the
  //Doc:Desc pointer to the copy
  //Doc:Return pointer to the copy

  //Doc:Method
   bool IsWireless() const { return false;}
    //Doc:Desc Query if this protocol needs a wireless interface.
    //Doc:Return Always returns false

   void Bootstrap() {}
   
public:
  Word_t       length;

};

//Doc:ClassXRef
class L2802_3Header : public L2PDU {
//Doc:Class Class {\ttL2802_3Header } defines a PDU (Protocol Data Unit) 
//Doc:Class structure that defines the 802.3 header for the packet
public :
  //Doc:Method
  L2802_3Header() : src(MACAddr::NONE), dst(MACAddr::NONE), length(0) { };
  //Doc:Desc This method is the default constructor for the header. It fills 
  //Doc:Desc all the fields with zeros.
  //Doc:Method
  L2802_3Header(MACAddr s, MACAddr d, Size_t len)
    : src(s), dst(d), length(len) { };
  //Doc:Desc This method takes in three arguements namely, source MAC address
  //Doc:Desc destination MAC address and the layer 3 protocol type and creates
  //Doc:Desc a header PDU object from the same.
  //Doc:Arg1 Source MAC address
  //Doc:Arg2 Destination MAC Address
  //Doc:Arg3 Layer 3 protocol type

  //Doc:Method
  L2802_3Header(char*, Size_t&, Packet*);
  // Construct from serialized buffer
  //Doc:Desc This method creates an L2 802.3 Header from a serialized character
  //Doc:Desc buffer and pushes it into a packet. This is useful only while
  //Doc:Desc using distributed simulations over a network of workstations.
  //Doc:Arg1 the character buffer pointer
  //Doc:Arg2 the size of the buffer
  //Doc:Arg3 the pointer to the packet 

  //Doc:Method
  Size_t  Size() const { return txSize;}
  //Doc:Desc This method returns the size of the layer 2 header
  //Doc:Return the size of the header

  //Doc:Method
  PDU*    Copy() const { return new L2802_3Header(*this);}
  //Doc:Desc This method makes a copy of itself and returns the copy
  //Doc:Return A PDU pointer to a copy of self

  //Doc:Method
  void    Trace(Tfstream& tos, Bitmap_t b,   // Trace the contents of this pdu 
                Packet* = nil, const char* = nil); 
  //Doc:Desc This method is used to trace the protocol data to a file (whose
  //Doc:Desc stream descriptor is tos) according to the level of detail 
  //Doc:Desc specified byt the bitmap
  //Doc:Arg1 the file I/O stream descriptor
  //Doc:Arg2 the bitmap
  //Doc:Arg3 Packet the contains this PDU (optional)
  //Doc:Arg4 Extra text information for trace file (optional)

  // Serialization
  //Doc:Method
  Size_t SSize();                   // Size needed for serialization
  //Doc:Method
  char*  Serialize(char*, Size_t&); // Serialize to a buffer
  //Doc:Desc This method serializes the header into a character buffer
  //Doc:Arg1 the pointer to the character buffer
  //Doc:Arg2 the current size of the buffer

  //Doc:Method
  char*  Construct(char*, Size_t&); // Construct from buffer
  //Doc:Desc This method is a helper method for the constructor. It actually 
  //Doc:Desc does the job of constructing a L2 802.3 header from the serialized
  //Doc:Desc character buffer obtained.
  //Doc:Arg1 the pointer to the character buffer
  //Doc:Arg2 the current size of the buffer.

  // Inherited from L2PDU
  //Doc:Method
  MACAddr GetSrcMac() { return src;}
    //Doc:Desc Return the source MAC address.
    //Doc:Return source MAC Address
  
  //Doc:Method
  MACAddr GetDstMac() { return dst;}
    //Doc:Desc Return the destination MAC address.
    //Doc:Return destination MAC Address
  
public:
  MACAddr   src;
  MACAddr   dst;
  Word_t    length;
public:
  static Size_t txSize;  // Sie of header when transmitting on link
  static void TxSize(Size_t txs) { txSize = txs; }
  
};

#endif
