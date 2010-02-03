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



// Georgia Tech Network Simulator - InterfaceEthernet class
// Mohamed Abd Elhafez.  Georgia Tech, Spring 2004


// Define class to model network link interfaces
// Implements the Layer 2 protocols

#ifndef __interface_ethernet_h__
#define __interface_ethernet_h__

#include "G_common_defs.h"
#include "simulator.h"
#include "protocol.h"
#include "packet.h"
#include "handler.h"
#include "ipaddr.h"
#include "notifier.h"
#include "macaddr.h"
#include "node.h"
#include "link.h"
#include "link-real.h"
#include "eventcq.h"
#include "interface.h"
#include "interface-real.h"
#include "ethernet.h"

#define INITIAL_BACKOFF 1           // Initial value for the max contention window
#define SLOT_TIME 512               // Slot time in bit times
#define BACKOFF_LIMIT 1024          // maximum allowable contention window
#define ATTEMPT_LIMIT 16            // number of retransmit before giving up
#define JAM_TIME 32                 // Jam time in bit times
#define INTER_FRAME_GAP 96          // Slot time in bit times

// Define the event subclass for link events
//Doc:ClassXRef
class EthernetEvent : public LinkEvent {
public:
typedef enum { 
  PACKET_RX,     // Packet receive event
  PACKET_TX,     // Packet transmit completed succesfuly 
  NOTIFY,        // Notify clients of available buffer space
  FBR,           // First Bit Received   
  CLR,           // Packet transmit aborted, link free
  RETRANSMIT,      // Start packet transmit
  UPDATE_NODES,  // Updae Node List
  CHAN_ACQ       // Channel acquired event 
} EthernetEvent_t;
public:
  EthernetEvent() 
    :  mac(MACAddr::NONE), type(0), size(0)
    { }
  EthernetEvent(Event_t ev) 
    : LinkEvent(ev), mac(MACAddr::NONE), type(0), size(0)
    { }
  EthernetEvent(Event_t ev, Packet* pkt) 
    : LinkEvent(ev, pkt), mac(MACAddr::NONE), type(0), size(pkt->Size())
    { }
 EthernetEvent(Event_t ev,  Size_t s) 
    : LinkEvent(ev),  mac(MACAddr::NONE), type(0), size(s)
    { }
 EthernetEvent(Event_t ev, const MACAddr& dst, Size_t s) 
    : LinkEvent(ev),  mac(dst), type(0), size(s)
    { }
  EthernetEvent(Event_t ev, Packet* pkt, const MACAddr& dst, int t)
    : LinkEvent(ev, pkt),  mac(dst), type(t)
    { }

  virtual ~EthernetEvent() { }

public:
  MACAddr mac;         // Receiver Mac address
  int type;            // Type for sending the packet
  Size_t size;         // The size of the packet
  Interface* sender;   // The sender interface, used to notify the sender that it has acquired the chanell succesfuly 
};

//Doc:ClassXRef
class NodeIfTime {
public:
  NodeIfTime(Node* n, Interface* i, Time_t t)
    : node(n), iface(i), time(t) { }
public:
  Node*      node;
  Interface* iface;
  Time_t   time;
};

typedef std::vector<NodeIfTime> NodeTimeVec_t; // Neighbors,Iface w/time
typedef std::vector<LinkEvent*>  LinkEventVec_t; 

//Doc:ClassXRef
class InterfaceEthernet : public InterfaceReal
{
  //Doc:Class InterfaceEthernet is an interface that handles the collision 
  //Doc:Class detection algorithm in the 802.3 standrad
  
 public:
  //Doc:Method
  InterfaceEthernet(const L2Proto& l2 = L2Proto802_3(),
		    IPAddr_t i   = IPADDR_NONE,
		    Mask_t   m   = MASK_ALL,
		    MACAddr  mac = MACAddr::NONE,
		    bool bootstrap = false);
  //Doc:Desc Constructs a new ethernet interface object.
  //Doc:Arg1 The layer 2 protocol type to associate with this interface.
  //Doc:Arg2 The \IPA\ to assign to this interface.
  //Doc:Arg3 The address mask to assign to this interface.
  //Doc:Arg4 The {\tt MAC} address to assign to this interface.
  
  virtual ~InterfaceEthernet();
  
  virtual bool IsEthernet() const { return true;}              // True if ethernet interface

  // Handler
  virtual void Handle(Event*, Time_t);
  
  //Doc:Method
  bool Send(Packet*, const MACAddr&, int); // Enque and transmit
  //Doc:Desc Send a packet on this interface.
  //Doc:Arg1 Packet to send.
  //Doc:Arg2 MAC address of destination
  //Doc:Arg3 LLC/SNAP type
  //Doc:Return True if packet sent or enqued, false if dropped.
  
   
  //Doc:Method
  virtual void RegisterRxEvent(LinkEvent*); // Register the last rx event issued by this interface      
  //Doc:Desc Keep a reference of the last Rx event made by a packet from this interfcae.
  //Doc:Arg1 Last PACKET_RX event 
  
  //Doc:Method
  virtual void     AddRxEvent(LinkEvent*);   // add to the list of rx events , used in case of broadcast
  //Doc:Desc Add to the list of rx events, This method is used in case the last packet was broadcast.
  //Doc:Arg1 Last PACKET_RX event 

  virtual void Notify(void* v);


  //Doc:Method
  //virtual void     LasPktIsBcast(bool);      // true of last packet is broad cast
  //Doc:Desc Set the bCast flag indicating wether the last packet was broadcast or not


//Doc:Method
  bool Retransmit(Packet* p);
  //Doc:Desc Tries to retransmit the packet on the link
  //Doc:Arg1 The packet to be retransmitted

//Doc:Method
  bool SenseChannel();
  //Doc:Desc Tries to retransmit the packet on the link
  //Doc:Arg1 The packet to be retransmitted

 private:
  
  //Doc:Method
  Time_t  TimeDelay(Node*);
  //Doc:Desc Calculate time between this node and specified peer node.
  //Doc:Arg1 Peer node pointer.
  //Doc:Return Time (Time_t).
  
  //Doc:Method
  void NeighbourList(NodeTimeVec_t& ntv);
  //Doc:Desc Get a list of all neighbor nodes on the same ethernet link
  //Doc:Arg1 vector will be populated with nodes and their delay times
  
  //Doc:Method
  void HandleFBR(EthernetEvent* e, Time_t t);
  //Doc:Desc Handles the first bit received event
  //Doc:Arg1 The Ethernet event
  //Doc:Arg1 Time of the event
  
 //Doc:Method
  Time_t  SlotTime();
  //Doc:Desc Calculate the slot time for this link.
  //Doc:Return slot Time (Time_t).


 private: 
  Time_t              busyendtime;     // Time when the link will be free again
  Time_t              holdtime;        // Time to hold back all packets when collision occurs
  Time_t              maxbackoff;      // Maximum backoff timer
  Time_t              backofftimer;    // Backoff timer
  Time_t              maxwaittime;     // maximum delay
  Count_t             busycount;       // How many stations are transmitting
  Packet*             lastpacketsent;  // last packet sent by this interface 
 
  LinkEvent*          lastrxevent;     // Last rx event scheduled by the link from this interface
  LinkEvent*          lastchanacq;     // Last channel acquired event scheduled by this interface
  LinkEvent*          lasttransmit;    // Last retransmit event scheduled by this interface
  NodeTimeVec_t       nodeTimes;       // vector containing neighbour nodes and their time delay
  Time_t              tx_finishtime;   // Transmit finish time
  bool                bCast;           // True if the last packet sent was a broadcast
  bool                collision;
  LinkEventVec_t      rxEvents;        // A list of rx event pointers used when broadcasting
  Time_t              slottime;        // the slot time in seconds
  Time_t              rtime;           // time to retransmit
  Uniform*            generator;       // random number generator
  
};

#endif
