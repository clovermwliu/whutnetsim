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
// $Id: link.h,v 1.9 2005/11/15 20:48:03 riley Exp $



// Georgia Tech Network Simulator - Link base class
// George F. Riley.  Georgia Tech, Spring 2002

// Virtual Base class for all Link types.

#ifndef __link_h__
#define __link_h__

#include <deque>

#include "common-defs.h"
//#include "handler.h"
//#include "l2proto802.3.h"
//#include "event.h"
//#include "notifier.h"
//#include "rng.h"
#include "node.h"

// Link class
// This is a base class; actual links (peer-to-peer, Ethernet, 802.11)
// derive from this.
class Queue;
class LinkMonitor;
class InterfaceBasic;

// Define the event subclass for link events
//Doc:ClassXRef
class LinkEvent : public Event {
public:
typedef enum { 
  PACKET_RX,
  PACKET_FIRSTBIT_RX, // Used by satelllite interfaces
  PACKET_TX,  // Packet transmit completed, link free for another
  NOTIFY,     // Notify clients of available buffer space
  LINK_FAIL,
  LINK_RESTORE
} LinkEvent_t;
public:
  LinkEvent() 
    : p(0), numberBytes(0), hasBitError(false), fifoEvent(false)
    { }
  LinkEvent(Event_t ev) 
    : Event(ev), p(0), numberBytes(0), hasBitError(false), fifoEvent(false)
    { }
  LinkEvent(Event_t ev, Packet* pkt) 
    : Event(ev), p(pkt), numberBytes(0), hasBitError(false), fifoEvent(false)
    { }
  virtual ~LinkEvent() { }
public:
  Packet* p; // Associated packet
  Count_t numberBytes; // Number of bytes (for PACKET_TX event)
  bool    hasBitError; // True if receiver should detect bit error
  bool    fifoEvent;   // True if event is in local interface's fifo queue
};

//Doc:ClassXRef
class Link : public Handler {
  //Doc:Class Class {\em Link} is a virtual base class, describing funtionality
  //Doc:Class needed by all \GTNS\ links.  Since there are several pure
  //Doc:Class virtual  functions, no objects of class {\tt Link} can
  //Doc:Class be created.  The \GTNS\ objects {\tt Linkp2p}, {\tt Ethernet}
  //Doc:Class and {\tt WirelessLink} are subclasses and provide the
  //Doc:Class specific functionality of those types of links.
public:
  // Constructors
  //Doc:Method
  Link();
    //Doc:Desc Creates a link with the default bandwidth and delay.

  //Doc:Method
  Link(Rate_t b, Time_t d);
    //Doc:Desc Creates a link with the default bandwidth and delay.
    //Doc:Arg1 Bandwidth for the link.
    //Doc:Arg2 Propagation delay for the link

  Link(const Link&); // Copy constructor

  // Destructor
  virtual ~Link() { };

  // Make a copy of this link
  //Doc:Method
  virtual Link*  Copy() const = 0;        // All subclasses must provide a copy
    //Doc:Desc Make a copy of this {\tt Link} object.
    //Doc:Return A pointer to the  copied object.

  //Doc:Method
  virtual void  SetPeer(InterfaceBasic* i) = 0;
    //Doc:Desc Set the peer interface for this link.
    //Doc:Arg1 Desired peer (receiving) interface.

  
  // Managing the link
  //Doc:Method
  virtual InterfaceBasic* GetLocalIf() const; // Get sending interface pointer
    //Doc:Desc   Get a pointer to the local (transmitting) interface.
    //Doc:Return Pointer to transmitting interface.

  //Doc:Method
  virtual void SetLocalIf(InterfaceBasic*); // Set sending interface pointer
    //Doc:Desc   Set the pointer to the local (transmitting) interface.
    //Doc:Arg1 Pointer to transmitting interface.

  //Doc:Method
  virtual InterfaceBasic* GetPeer(Packet*) = 0;// Get peer interface for packet
    //Doc:Desc Get the interface pointer for the remote endpoint of
    //Doc:Desc this link for the specified packet.
    //Doc:Arg1 The packet being transmitted.
    //Doc:Return A pointer to the receiving interface for the specified packet.

  // Get peer interface for peer index
  //Doc:Method
  virtual InterfaceBasic* GetPeer(Count_t) = 0;
    //Doc:Desc Get the interface pointer for the remote endpoint of
    //Doc:Desc this link for the specified peer index
    //Doc:Arg1 Which peer desired (0 <- i < NeighborCount())
    //Doc:Return A pointer to the receiving interface for the specified peer

  //Doc:Method
  virtual  bool Transmit(Packet*, Interface*, Node*) = 0; // Transmit a packet
    //Doc:Desc Transmit a packet.
    //Doc:Arg1 The packet to transmit.
    //Doc:Arg2 A pointer to the local (transmitting) interface
    //Doc:Arg3 A pointer to the local (transmitting) node.
    //Doc:Return True if the transmit succeeded, false if packet was dropped.

  //Doc:Method
  virtual  bool Transmit(Packet*, Interface*, Node*, Rate_t) = 0;
    //Doc:Desc Transmit a packet.
    //Doc:Arg1 The packet to transmit.
    //Doc:Arg2 A pointer to the local (transmitting) interface
    //Doc:Arg3 A pointer to the local (transmitting) node.
    //Doc:Arg4 The rate at which the packet is transmitted.
    //Doc:Return True if the transmit succeeded, false if packet was dropped.

  //Doc:Method
  virtual  bool Transmit(LinkEvent*, Interface*, Time_t) = 0;
    //Doc:Desc Transmit using an existing LinkEvent pointer.
    //Doc:Desc Used by satellite nodes.
    //Doc:Arg1 Existing LinkEvent pointer
    //Doc:Arg2 Receiving Interface pointer
    //Doc:Arg3 Receipt time

  // Transmit including queuing delay
  virtual  bool TransmitWithDelay(Packet*, Time_t) = 0;

  //Doc:Method
  void     Handle(Event*, Time_t) = 0;
    //Doc:Desc Link objects are subclass of {\tt Handler} and must define
    //Doc:Desc a {\tt Handle} method to handle events scheduled for this
    //Doc:Desc link.
    //Doc:Arg1 The event to handle.
    //Doc:Arg2 The current simulation time.

  //Doc:Method
  virtual void     Bandwidth(Rate_t b) = 0;
    //Doc:Desc Set the link bandwidth to the specified value.
    //Doc:Arg1 Link bandwidth to set.

  //Doc:Method
  virtual   Rate_t   Bandwidth() const = 0;
    //Doc:Desc Return the bandwidth for this link.
    //Doc:Return The bandwidth for the link.

  virtual   void     Delay(Time_t d) = 0;
    //Doc:Desc Set the link propagation delay to the specified value.
    //Doc:Arg1 Link delay to set.

  virtual   Time_t   Delay() const = 0;
    //Doc:Desc Return the propagation delay for this link.
    //Doc:Return The the propagation delay for the link.

  //Doc:Method
  virtual   bool     Busy() const = 0;
    //Doc:Desc Determine if the link is  currently busy transmitting a packet.
    //Doc:Return True if link busy, false if not.

  //Doc:Method
  virtual  Count_t  PeerCount() = 0; // Return number of peers
    //Doc:Desc Returns the number of peers (nodes) that this link provides
    //Doc:Desc connectivity to. \purev
    //Doc:Return Count of peers for this link.

  //Doc:Method
  virtual  IPAddr_t PeerIP(Count_t) = 0;// Get peer IP Addr(if known)
    //Doc:Desc Get the \IPA\ of the specified peer. \purev
    //Doc:Arg1 Index of the peer desired, in the range [0..PeerCount).
    //Doc:Return \IPA\ of the requested peer.
    //Doc:Return Returns \IPAN\ if peer does not exist.

  //Doc:Method
  virtual  IPAddr_t NodePeerIP(Node*) = 0;   // Get peer IP Address (if known)
    //Doc:Desc Get the \IPA\ of the specified peer. \purev
    //Doc:Arg1 Pointer to desired peer node.
    //Doc:Return \IPA\ of the requested peer.
    //Doc:Return Returns \IPAN\ if specified node is not a peer.

  //Doc:Method
  virtual  bool     NodeIsPeer(Node*) = 0;   // Find out if node is a peer
    //Doc:Desc Determine if specified node is a peer. \purev
    //Doc:Arg1 Pointer to desired peer node.
    //Doc:Return True if node is a peer, false if not.

  //Doc:Method
  virtual  Count_t  NodePeerIndex(Node*) = 0;// Get peer index for node
    //Doc:Desc Get the peer index, in the range [0..PeerCount) for the
    //Doc:Desc specified node.
    //Doc:Arg1 Pointer to desired peer node.
    //Doc:Return Peer index for specified node.

  //Doc:Method
  virtual  IPAddr_t DefaultPeer(Interface*) = 0; // Get IP Addr of default peer
    //Doc:Desc Find the \IPA\ of the default peer.  Link classes with
    //Doc:Desc multiple peers (such as an Ethernet link) can specify
    //Doc:Desc one of the peers as the {\em Default Gateway}.  This
    //Doc:Desc simplifies routing decisions in some cases. \purev
    //Doc:Arg1 Interface pointer for the interface of this link.
    //Doc:Return \IPA\ of the default peer.
    //Doc:Return Returns \IPAN\ if no default peer exists.

  //Doc:Method
  virtual void     BitErrorRate(Mult_t b) = 0;
    //Doc:Desc Specify a bit error rate for this link.
    //Doc:Arg1 The desired bit error rate.

  //Doc:Method
  virtual   Mult_t   BitErrorRate() = 0;
    //Doc:Desc Return the bit error rate for this link.
    //Doc:Return The bit error rate assigned to this link.

  //Doc:Method
  virtual   void     ResetUtilization() = 0;
    //Doc:Desc Reset the link utilization statistics for this link to zero.

  //Doc:Method
  virtual   Mult_t   Utilization() = 0;
    //Doc:Desc Return the utilization for this link, in the range [0..1].
    //Doc:Return The link utilization for this link.

  //Doc:Method
  virtual   void     AddNotify(NotifyHandler*,void*) = 0;// Add a notify client
    //Doc:Desc Higher layer protocol entities can request a notification
    //Doc:Desc when links go non--busy.  This method adds a notification
    //Doc:Desc handler to the list.
    //Doc:Arg1 Any object subclassed from {\tt NotifyHandler}.  The
    //Doc:Arg1 {\tt Notify} method for that handler will be called
    //Doc:Arg1 when this link goes non--busy.
    //Doc:Arg2 A blind pointer to be passed to the {\tt Notify} method.

  //Doc:Method
  virtual   Weight_t Weight() = 0;
    //Doc:Desc Query the link weight (used by some routing protocols)
    //Doc:Desc for this link.
    //Doc:Return The weight for this link.

  //Doc:Method
  virtual   void   Weight(Weight_t w) = 0;
    //Doc:Desc Assign a weight (used by some routing protocols) to this link.
    //Doc:Arg1 Weight to assign to this link.

  //Doc:Method
  virtual   void   Jitter(const Random&) = 0;  // Set random jitter factor
    //Doc:Desc Specify a random number generator object used for {\em jitter}.
    //Doc:Desc Jitter is used  to randomly delay the packet transmission time
    //Doc:Desc by small pertubations, to model CPU processing time in
    //Doc:Desc routers.  No jitter value is used unless this method is used.
    //Doc:Arg1 Any random number generator object.  The object is copied.

  //Doc:Method
  virtual   Time_t NextFreeTime() = 0;         // Get time link is free
    //Doc:Desc Predict the time this link will be free.  This is useful
    //Doc:Desc only for links without jitter, and without contention.  
    //Doc:Return The predicted time this link will be free.

  //Doc:Method
  virtual Count_t  NeighborCount(Node*) = 0;   // Number of rtg neighbors
    //Doc:Desc Return the number of routing neighbors on this link.
    //Doc:Arg1 Node querying neighbors (significant for Ethernet links)
    //Doc:Return Count of routing neighbors.

  // Build the neighbor and interface list        
  //Doc:Method
  virtual void     Neighbors(NodeWeightVec_t&, Interface*, bool) = 0;
    //Doc:Desc Get a vector of all routing neighbors.
    //Doc:Arg1 Returned vector of routing neighbors.
    //Doc:Arg2 Interface pointer to associated interface.
    //Doc:Arg3 True if include all neighbors, even for single  gateway
    //Doc:Arg3 Ethernet.

  // AllNeighbors is used by broadcasts to find all peers on a link
  // even if they are not routing peers
  //Doc:Method
  virtual void     AllNeighbors(NodeWeightVec_t&) = 0;
    //Doc:Desc Find all neighbors for this link, even if they are
    //Doc:Desc not routing neighbors.  Used by broadcasts to find
    //Doc:Desc all receivers.
    //Doc:Arg1 Returned vector of neighbors.

  // Statistics
  //Doc:Method
  virtual   DCount_t         BytesSent() = 0;
    //Doc:Desc Get a count of the total number of bytes sent on this link.
    //Doc:Return Count of total bytes sent on this link.

  //Doc:Method
  virtual   DCount_t         PktsSent() = 0;
    //Doc:Desc Get a count of the total number of packets sent on this link.
    //Doc:Return Count of total packets sent on this link.

  //Doc:Method
  virtual MACAddr  IPToMac(IPAddr_t) = 0;     // Convert peer IP to peer MAC
    //Doc:Desc Convert a peer \IPA\ to peer MAC address.
    //Doc:Arg1 \IPA\ of desired peer.
    //Doc:Return MAC address for that peer.

  //Doc:Method
  virtual bool     RxBroadcast() = 0;         // True if receive own bcast
    //Doc:Desc Query if this link should receive a copy of its own
    //Doc:Desc broadcasts.
    //Doc:Return True if receive own broadcast, false if not.

  //Doc:Method
  virtual bool     ScheduleFailure(Time_t t) = 0;
  //Doc:Desc This method scedules the failure of the current link in future
  //Doc:Arg1 Time when we want to schedule the failure
  //Doc:Return bool, true on success and false on failure (if schedule fails)

  //Doc:Method
  virtual bool     ScheduleRestore(Time_t t) = 0;
  //Doc:Desc This method scedules the link restore
  //Doc:Desc state. If it is not currently failed, it has no effect
  //Doc:Arg1 Time when we want to schedule the restore
  //Doc:Return bool, true on success and false on failure (if schedule fails)

  //Doc:Method
  virtual LinkMonitor* AddLinkMonitor(const LinkMonitor&);
    //Doc:Desc Add a LinkMonitor object to this link
    //Doc:Arg1   Link monitor object to add (using Copy())

  //Doc:Method
  virtual Interface* GetNewInterface(IPAddr_t = IPADDR_NONE,
                                     Mask_t = MASK_ALL) const;
    //Doc:Desc Some links require a specific interface type, such as 
    //Doc:Desc the Link16 TDM link needs a Link16 interface type.
    //Doc:Desc This method is called when a new interface is needed
    //Doc:Desc and allows the link to create and return the
    //Doc:Desc correct interface type.
    //Doc:Arg1 IP Address for this interface.
    //Doc:Arg2 Address mask for this interface
    //Doc:Return New interface pointer

  // Animation related methods
  //Doc:Method
  virtual void AnimateLink();
    //Doc:Desc Draw the link on the animation display.

  //Doc:Method
  virtual void ReAnimateLink();
    //Doc:Desc   Redraw link after state change (up/down)

  //Doc:Method
  virtual void AnimatePackets();
    //Doc:Desc Draw the packets on the link.

public:
  //Doc:Method
  static void     DefaultRate(Rate_t r);
    //Doc:Desc Set a default transmission rate to be used for any
    //Doc:Desc new link created.
    //Doc:Arg1 Desired default transmission rate (bits/second)

  //Doc:Method
  static Rate_t   DefaultRate()          { return defaultRate;}
    //Doc:Desc Return the default transmission rate.
    //Doc:Return The default transmission rate (bits/second).

  //Doc:Method
  static void     DefaultDelay(Time_t t);
    //Doc:Desc Set a default propagation delay to be used for any
    //Doc:Desc new link created.
    //Doc:Arg1 Desired default propagation delay (second)

  //Doc:Method
  static Time_t   DefaultDelay()         { return defaultDelay;}
    //Doc:Desc Return the default propagation delay
    //Doc:Return The default propagation delay (second).

  //Doc:Method
  static void     DefaultJitter(const Random& j);
    //Doc:Desc Set a default random variable for link transmission jitter.
    //Doc:Arg1 Any random number generator object.

  //Doc:Method
  static Random*  DefaultJitter();
    //Doc:Desc Get a pointer to the default jitter random number generator.
    //Doc:Return Pointer to the default jitter random number generator.

  //Doc:Method
  static void     Default(const Link& l);
    //Doc:Desc Set a default link object to use when a link object is needed
    //Doc:Desc and none has been specified.
    //Doc:Arg1 Link object to use for the default link.

  //Doc:Method
  static const Link& Default();
    //Doc:Desc Get a reference to the default link object.
    //Doc:Return A constant reference to the default link object.

  //Doc:Method
  static void     UseSeqEvents(bool u) { useSeqEvents = u;}
    //Doc:Desc A small performance improvement can be gained by leveraging
    //Doc:Desc the fact that link receive events are always in strictly
    //Doc:Desc increasing timestamp order for a single link.  Setting
    //Doc:Desc UseSeqEvents to true enables this optimization.
    //Doc:Desc NOTE.  The GTNetS packet animation code depends on this setting
    //Doc:Desc to  find which packets are in--flight on the links.  If this
    //Doc:Desc setting is FALSE, no packets will be displayed on the
    //Doc:Desc animation display.
    //Doc:Arg1 True if using sequential events optimization, false if not.

public:
  InterfaceBasic* localIf; // Local interface
  
protected:
  // Defaults
  static Rate_t   defaultRate;      // Default link speed
  static Time_t   defaultDelay;     // Default delay
  static Link*    defaultLink;      // Default link type
  static Random*  defaultJitter;    // Default jitter rng
  static Random*  berRng;           // Random numbers for bit-error-rate
  static bool     useSeqEvents;     // True if bypass central ev. list
};

#include "l2proto.h"

#endif

