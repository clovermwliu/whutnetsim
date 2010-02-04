//Copyright (c) 2010, Information Security Institute of Wuhan Universtiy(ISIWhu)
//All rights reserved.
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
//Construct Data:
//Modify Author:
//Author Organization:
//Modify Data:



// Georgia Tech Network Simulator - Layer 2 Protocol base class
// George F. Riley.  Georgia Tech, Spring 2002
// Peng Zhen, Mon, 4, Jan, 2009 

#ifndef __l2proto_h__
#define __l2proto_h__

#include "G_common_defs.h"
#include "protocol.h"
#include "notifier.h"
#include "pdu.h"

class Interface;
class LinkEvent;

//Doc:ClassXRef
class L2Proto : public Protocol, public NotifyHandler {
  //Doc:Class Class {\tt L2Proto} is the base class for all layer two 
  //Doc:Class protocols. these implement specifically the Medium Access 
  //Doc:Class Control and associated functions. This class closely 
  //Doc:Class interacts with the {\tt Link} class to provide the low 
  //Doc:Class level details of a network stack
public:

  //Doc:Method
  L2Proto();
    //Doc:Desc Constructor.

  //Doc:Method
  virtual Layer_t Layer () { return 2; }
    //Doc:Desc This member function returns the layer number of the protocol
    //Doc:Desc stack corresponding to the ISO model.
    //Doc:Return the layer number (in this case 2).

  //Doc:Method
  virtual void BuildDataPDU(MACAddr, MACAddr, Packet*) = 0;
    //Doc:Desc Build and append a layer2 pdu to the specified packet.
    //Doc:Arg1 Source MAC address
    //Doc:Arg2 Destination MAC address
    //Doc:Arg3 Packet pointer to append this pdu to.


  //Doc:Method
  virtual void DataRequest( Packet*) = 0;
  //Doc:Desc This member function is called by the higher layer requesting 
  //Doc:Desc a transfer of  data to a peer.
  //Doc:Arg1 The packet to be sent, with l2 pdu already appended/

  //Doc:Method
  virtual void DataIndication( Packet*, LinkEvent* = nil ) = 0;
  //Doc:Desc This method is called when the direction of protocol stack
  //Doc:Desc traversal is upwards, i.e, a packet has just been received
  //Doc:Desc at the underlying link
  //Doc:Arg1 The LinkEvent with the received packet (may be nil)
  //Doc:Arg1  If non-nil, the DataIndication must delete or re-use the event
  //Doc:Arg2 The received packet

  //Doc:Method
  virtual void SetInterface(Interface* i) { iface = i; }
    //Doc:Desc Specify which interface this protocol is attached to.
    //Doc:Arg1 Interface pointer for interface.

  //Doc:Method
  virtual bool Busy() = 0;                  // True if proto/link is busy
    //Doc:Desc Determine if the protocol is busy processing a packet.
    //Doc:Return True if busy.

  //Doc:Method
  virtual L2Proto* Copy() const = 0;                      // Make a copy
  //Doc:Desc This method is called to make a copy of the layer 2 protocol
  //Doc:Desc object

  //Doc:Method
  virtual bool IsWireless() const = 0;
    //Doc:Desc Query if this protocol needs a wireless interface.
    //Doc:Return True if wireless protocol.

  virtual void Notify(void*) {}

  virtual void Bootstrap() = 0;

   // Over-rides from the tracing methods from protocol.h
   // We do those a bit differently for l2 protocols, since
   // there is not a single "instance" that can globally set the
   // trace on/off status.  Instead we use a static variable
   // common to all instances.
  void    SetTrace(Trace::TraceStatus ts)// Set trace level this proto
    { globalTraceStatus = ts; }
  Trace::TraceStatus GetTrace()          // Get trace level this proto
    { return globalTraceStatus;}
  void DetailOn(Bitmap_t b)
    { globalTraceDetails |= (1L << b);}
  void DetailOff(Bitmap_t b)
    { globalTraceDetails &= (ALLBITS ^ (1L << b));}
  Bitmap_t Details()
    { return globalTraceDetails;}


  // Static methods for managing global trace
  static void GlobalSetTrace(Trace::TraceStatus ts)
    { globalTraceStatus = ts; }
  static Trace::TraceStatus GlobalGetTrace()
    { return globalTraceStatus;}
  static void GlobalDetailOn(Bitmap_t b)
    { globalTraceDetails |= (1L << b);}
  static void GlobalDetailOff(Bitmap_t b)
    { globalTraceDetails &= (ALLBITS ^ (1L << b));}
  static Bitmap_t GlobalDetails()
    { return globalTraceDetails;}
  // Since l2 packets are often (wireless) received at places other
  // than the adressee, this flag allows for reducing trace file size
  // by tracing only if the packet is addressed to the receiver
  // or a broadcast.
  static void TraceDestinationOnly(bool dto = true)
    { destinationOnly = dto; }
  
 public:
  Interface* iface;  // Interface for this protocol
 private:
  static Trace::TraceStatus globalTraceStatus;
  static Bitmap_t           globalTraceDetails;  // Bitmap of trace info
public:
  static bool               destinationOnly;     // True if trace dest only
};

//Doc:ClassXRef
class L2PDU : public PDU {
  //Doc:Class Class {\tt L2PDU} is the base class for all layer two 
  //Doc:Class protocol data units.  All L2PDU's must implement
  //Doc:Class the GetSrcMac and GetDstMac methods.
public:
  virtual Layer_t Layer()  {return 2;}
  //Doc:Method
  virtual MACAddr GetSrcMac() = 0;
    //Doc:Desc Return the source MAC address.
    //Doc:Return Source MAC Address

  //Doc:Method
  virtual MACAddr GetDstMac() = 0;
    //Doc:Desc Return the destination MAC address.
    //Doc:Return destination MAC Address
};

#endif

