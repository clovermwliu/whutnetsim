
//Copyright (c) 2010, Information Security Institute of Wuhan Universtiy(ISIWhu)
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
// Georgia Tech Network Simulator - Layer 4 Protocol base class
// George F. Riley.  Georgia Tech, Spring 2002

// Define the layer 4 protocol interface

#ifndef __l4protocol_h__
#define __l4protocol_h__

#include "protocol.h"
#include "l3protocol.h"
#include "node.h"
#include "ipv4.h"
#include "datapdu.h"
#include "event.h"
#include "handler.h"

class Application;
class Interface;
class Notifier;
class Interface;

class QColor;

//Doc:ClassXRef
class L4PDU : public PDU {
public:
  L4PDU() : PDU() { }
  virtual Layer_t Layer() { return 4; }
};

//Doc:ClassXRef
class L4Protocol : public Protocol, public Handler {
  //Doc:Class Class {\tt L4Protocol} is a virtual base class defines
  //Doc:Class the interface
  //Doc:Class for all of the layer 4 protocols in \GTNS.

public:
  typedef enum { DELAYED_RX, DELAYED_TX } L4DelayEv_t;

public:
  //Doc:Method
  L4Protocol();
    //Doc:Desc Default constructor, no arguments.

  //Doc:Method
  L4Protocol(Node* n);
    //Doc:Desc Construct a layer 4 protocol and assign it to the specified
    //Doc:Desc node.
    //Doc:Arg1 Pointer to node to which to assign this protocol.

  //Doc:Method
  L4Protocol(const L4Protocol&); // Copy constructor
    //Doc:Desc Copy constructor.
    //Doc:Arg1 L4Protocol object to copy.

  virtual ~L4Protocol();

  //Doc:Method
  virtual void Handle(Event*, Time_t);
    //Doc:Desc The layer 4 protocol class is a subclass of {\tt Handler}, 
    //Doc:Desc since it must handle events, specificaly
    //Doc:Desc the delayed transmit and receive events for processing
    //Doc:Desc "ExtraDelay".
    //Doc:Arg1 A pointer to the event being handled.
    //Doc:Arg2 The current simulation time.


  //Doc:Method
  virtual Layer_t Layer() { return 4;}   // Protocol layer number
    //Doc:Desc All protocol objects in \GTNS\ must define the {\tt Layer}
    //Doc:Desc method, returning the protocol layer for this protocol.
    //Doc:Return Protocol layer number for this L4 protocol (always 4).

public:
  // Data indication From lower layer
  //Doc:Method
  virtual void DataIndication(Node*, Packet*, IPAddr_t, Interface*) = 0;
    //Doc:Desc Indicates that a packet has been received by the layer 3
    //Doc:Desc protocol addressed to this node, and the protocol number in 
    //Doc:Desc the L3 header matches the registered protocol number for 
    //Doc:Desc this protocol.
    //Doc:Arg1 Node pointer to associated node.
    //Doc:Arg2 Pointer to the received packet.  The L3 PDU has been removed,
    //Doc:Arg3 so the L4 PDU is the top of the PDU stack.
    //Doc:Arg3 \IPA\ of destination, since nodes can have multiple \IPA{s}.
    //Doc:Arg4 Interface from which this packet was received.
public:
  // Sending data
  //Doc:Method
  virtual NCount_t Send(Size_t) = 0;         // Send empty data
    //Doc:Desc Send the specified amount of data.  This version of {\tt Send}
    //Doc:Desc does not actually use data contents, just an indication
    //Doc:Desc of the amount of data.  This is useful for applications
    //Doc:Desc where data contents is not needed for the simulation.
    //Doc:Arg1 Number of bytes to send.
    //Doc:Return Number of bytes actually sent.

  //Doc:Method
  virtual NCount_t Send(char*, Size_t) = 0;  // Send with data
    //Doc:Desc Send the specified data.  This version uses both data size
    //Doc:Desc and data contents.
    //Doc:Arg1 Pointer to data to send.
    //Doc:Arg2 Number of bytes to send.
    //Doc:Return Number of bytes actually sent.

  //Doc:Method
  virtual NCount_t Send(Data&) = 0;          // Send with data
    //Doc:Desc Send the specified {\tt Data} PDU.
    //Doc:Arg1 Reference to the {\tt Data} PDU to send.  {\tt Data} PDUs
    //Doc:Arg1 have size and an optional contents field.
    //Doc:Return Number of bytes actually sent.

  //Doc:Method
  virtual NCount_t SendTo(Size_t, IPAddr_t, PortId_t) = 0; // Send to target
    //Doc:Desc Send  the specifed number of bytes to the specified \IPA\ and
    //Doc:Desc port.  Does not include data contents.
    //Doc:Arg1 Number of bytes to send.
    //Doc:Arg2 Destination \IPA.
    //Doc:Arg3 Destination port.
    //Doc:Return Number of bytes actually sent.

  //Doc:Method
  virtual NCount_t SendTo(char*, Size_t, IPAddr_t, PortId_t) = 0;// Send w/data
    //Doc:Desc Send  the specifed number of bytes with associated
    //Doc:Desc data contents to the specified \IPA\ and
    //Doc:Desc port.
    //Doc:Arg1 Number of bytes to send.
    //Doc:Arg2 Pointer to data to send.
    //Doc:Arg3 Destination \IPA.
    //Doc:Arg4 Destination port.
    //Doc:Return Number of bytes actually sent.

  //Doc:Method
  virtual NCount_t SendTo(Data&, IPAddr_t, PortId_t) = 0;        // Send w/data
    //Doc:Desc Send a {\tt Data} PDU to the specified destination.
    //Doc:Arg1 Reference to the {\tt Data} PDU to send.  {\tt Data} PDUs
    //Doc:Arg1 have size and an optional contents field.
    //Doc:Arg2 Destination \IPA.
    //Doc:Arg3 Destination port.
    //Doc:Return Number of bytes actually sent.

  // Binding
  //Doc:Method
  virtual bool Bind(PortId_t);       // Bind to specific port
    //Doc:Desc Bind this L4 protocol to the specifed port on the the attached
    //Doc:Desc node.
    //Doc:Arg1 Port number to bind.
    //Doc:Return True if successful.

  //Doc:Method
  virtual bool Bind();               // Bind to available port
    //Doc:Desc Bind this L4 protocol to any available port on the
    //Doc:Desc attached node.
    //Doc:Return True if successful.

  //Doc:Method
  virtual bool Unbind(Proto_t proto, PortId_t port); // Remove port binding
    //Doc:Desc Remove a port binding.
    //Doc:Arg1 Protocol number for this L4 object.
    //Doc:Arg2 Port number to remove binding for.
    //Doc:Return True  if successful.

  // Connection management
  //Doc:Method
  void Attach(Node* n) { localNode = n;}
    //Doc:Desc Attach this L4 object to the specified node.
    //Doc:Arg1 Pointer to node to attach.

  //Doc:Method
  PortId_t Port() { return localPort;}
    //Doc:Desc Return the currently bound port number.
    //Doc:Return Currently bound port, or PORT_NONE if not bound.

  //Doc:Method
  virtual Proto_t  Proto() const = 0;
    //Doc:Desc Return the protocol number for this layer  4 protocol
    //Doc:Return Protocol number

  //Doc:Method
  virtual bool Connect(IPAddr_t, PortId_t) = 0; // Connect to remote host
    //Doc:Desc Initiate a connection to a remote host.
    //Doc:Arg1 \IPA\ of remote host.
    //Doc:Arg2 Port number of remote L4 protocol.
    //Doc:Return True if connection initiated ok.  This does not mean
    //Doc:Return the connection was successful if a {\tt TCP} L4 object.

  //Doc:Method
  virtual bool Close() = 0;                     // Close connection to remote
    //Doc:Desc Close an open connection.
    //Doc:Return True if close  is successful.  This does not mean that a
    //Doc:Return {\tt TCP} connection has completely closed, since the
    //Doc:Return return is immediate.  {\tt TCP} will timeout and retry
    //Doc:Return the close action if the {\tt FIN} packet is lost.

  // Miscellaneous
  //Doc:Method
  void    TTL(Count_t t) { ttl = t;}     // Set TTL
    //Doc:Desc Set the time--to--live value to set in all packet created
    //Doc:Desc by this protocol endpoing.
    //Doc:Arg1 {\tt TTL} value to set.

  //Doc:Method
  Count_t TTl()          { return ttl;}  // Get TTL
    //Doc:Desc Return the current {\tt TTL} value for this connection.
    //Doc:Return Current {\tt TTL} value for this connection.

  //Doc:Method
  void    TOS(Byte_t t) { tos = t;}
    //Doc:Desc Set the "Type of Service" required for all packets created
    //Doc:Desc by this protocol.  It will be passed to the L3 processor
    //Doc:Desc which will set the appropriate field in the L3 header.
    //Doc:Arg1 TOS desired.

  //Doc:Method
  Byte_t  TOS() { return tos;}
    //Doc:Desc   Return the current {\tt Tos} value for this protocol.
    //Doc:Return TOS Value.

  //Doc:Method
  void    FlowId(FId_t f){ fid = f;}     // Set flow id
    //Doc:Desc Assign a {\em Flow Identifier} for this connection.
    //Doc:Desc The flow id is a simulation artifact with no basis in
    //Doc:Desc real networks, but is useful for simulations to keep
    //Doc:Desc track of all packets associated with a given flow.
    //Doc:Desc The flow identifer is logged in the trace file for
    //Doc:Desc each packet.
    //Doc:Arg1 Flow identifier to assign.

  //Doc:Method
  FId_t   FlowId()       { return fid;}  // Get flow id
    //Doc:Desc Return the current flow identifier for this connection.
    //Doc:Return Flow identifier for this connection.

  //Doc:Method
  virtual L4Protocol* Copy() const = 0;  // Create a copy of this protocol
    //Doc:Desc Make a copy of this protocol endpoint.
    //Doc:Return Pointer to copy of this protocol.

  //Doc:Method
  virtual Proto_t ProtocolNumber() = 0;  // Return the l4 protocol number
    //Doc:Desc Return the protocol number registered for this layer 4 protocol.
    //Doc:Return Protocol number.

  //Doc:Method
  virtual bool    IsTCP() = 0;           // True if TCP
    //Doc:Desc Determine if this endpoint is {\tt TCP}.
    //Doc:Return True if {\tt TCP}

  //Doc:Method
  void    AttachApplication(Application* a) { localApplication = a;}
    //Doc:Desc Attach an application to this L4 protocol.
    //Doc:Arg1 Pointer to application to attach.

  //Doc:Method
  Application* GetApplication() { return localApplication;}
    //Doc:Desc Get the currently attached application.
    //Doc:Return Pointer to the currently attached application,  or {\tt nil}
    //Doc:Return if none.

  //Doc:Method
  void    AddNotification(NotifyHandler* n);
    //Doc:Desc Adds a "Packet Transmission" notification for all packets
    //Doc:Desc sent by this protocol.
    //Doc:Arg1 Notifier object to receive the notification.

#ifdef HAVE_QT
  //Doc:Method
  bool IsColored() {return color;}
    //Doc:Desc Determine if this application has a color assigned for
    //Doc:Desc created packets
    //Doc:Return True if a color assigned.

  //Doc:Method
   void SetColor(const QColor&);
    //Doc:Desc Specifiy a color for all packets generated by this protocol.
    //Doc:Arg1 Desired color (see qcolor.h in qt). 
#endif
   
  //Doc:Method
   Packet* NewPacket();
    //Doc:Desc Allocate a new packet.
    //Doc:Desc Uses either a normal "uncolored" packet or
    //Doc:Desc a colored packet, depending on whether the protocol is colored
    //Doc:Desc or not.

  //Doc:Method
   void AddExtraRxDelay(Time_t);
    //Doc:Desc The "Extra Delay" mechanism allows the simulator to
    //Doc:Desc vary the round--trip time on a protocol by protocol basis,
    //Doc:Desc rather than using link delays.
    //Doc:Desc The Rx delay is extra delay added to received packets.
    //Doc:Arg1 Extra propagation delay on the last--hop link.

  //Doc:Method
   void AddExtraTxDelay(Time_t);
    //Doc:Desc The "Extra Delay" mechanism allows the simulator to
    //Doc:Desc vary the round--trip time on a protocol by protocol basis,
    //Doc:Desc rather than using link delays.
    //Doc:Desc The Rx delay is extra delay added to received packets.
    //Doc:Arg1 Extra propagation delay on the last--hop link.


protected:
  void    RequestNotification(NotifyHandler*, void*); // Request notification
  void    CancelNotification(NotifyHandler*);    // Cancel pending notif.
  bool    BufferAvailable(Size_t,Packet* = nil); // True if output buff avail.
private:
  void    SetInterface();                // Set the interface for tx packets
public:
  Node*    localNode;                    // Local node 
  IPAddr_t peerIP;                       // Remote peer IP Address
  PortId_t localPort;                    // Local port number
  PortId_t peerPort;                     // Remote peer port
  Application*   localApplication;       // Local application object
  NotifyHandler* notification;           // Object to notify on packet tx
  L3Protocol*  l3Proto;                  // Layer 3 protocol to use
  Count_t      ttl;                      // Layer 3 ttl
  FId_t        fid;                      // Flow id
  Byte_t       tos;                      // Layer 3 "type of service"
  Interface*   iFace;                    // Interface used for this flow
  Time_t       extraRxDelay;             // Extra Rx Delay info (if any)
  Time_t       extraTxDelay;             // Extra Tx Delay info (if any)
	bool				 deleteAppOnComplete;
#ifdef HAVE_QT
  QColor*      color;                    // Color for all packets (if non-nil)
#endif
};

// Define the event for a delayed receive.  This allows for variable
// round trip times for flows.
//Doc:ClassXRef
class L4DelayedRxEvent : public Event {
public:
  L4DelayedRxEvent() 
      : Event(L4Protocol::DELAYED_RX), l4Proto(nil), node(nil), packet(nil),
                               ipAddr(IPADDR_NONE), fromIF(nil) {}
  L4DelayedRxEvent(L4Protocol* l4,Node* n,Packet* p,IPAddr_t ip,Interface* i)
      : Event(L4Protocol::DELAYED_RX), l4Proto(l4), node(n), packet(p),
                               ipAddr(ip), fromIF(i) {}
public:
  L4Protocol* l4Proto;
  Node*       node;
  Packet*     packet;
  IPAddr_t    ipAddr;
  Interface*  fromIF;
};

    
// Define the event for a delayed transmit.  This allows for variable
// round trip times for flows.
//Doc:ClassXRef
class L4DelayedTxEvent : public Event {
public:
  L4DelayedTxEvent() 
      : Event(L4Protocol::DELAYED_TX), l3Proto(nil), node(nil), packet(nil),
        msgLength(0), notifySent(false) {}
  L4DelayedTxEvent(L3Protocol* l3, Node* n, Packet* p, const IPV4ReqInfo& v4,
                   Count_t m, bool notif)
      : Event(L4Protocol::DELAYED_TX), l3Proto(l3), node(n), packet(p),
        reqInfo(v4), msgLength(m), notifySent(notif) {}
public:
  L3Protocol* l3Proto;
  Node*       node;
  Packet*     packet;
  IPV4ReqInfo reqInfo;
  Count_t     msgLength;
  bool        notifySent;
};

    
#endif
