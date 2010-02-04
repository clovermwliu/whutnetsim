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
// $Id: ethernet.h,v 1.5 2005/11/15 20:48:03 riley Exp $



// Georgia Tech Network Simulator - Ethernet LAN class
// George F. Riley.  Georgia Tech, Spring 2002

// Define the Ethernet LAN Class

#ifndef __ethernet_h__
#define __ethernet_h__

#include "G_common_defs.h"
#include "link-real.h"
#include "ratetimeparse.h"
#include "l4protocol.h"
#include "interface-ethernet.h"

// The Ethernet model has two detail levels:
// PARTIAL does model collisions and insures only one node is transmitting
//      at once.  However, collisions are detected instantaneously,
//      (all nodes can hear the carrier with zero delay).  This is
//      more realistic than the NONE detail level, at the expense of
//      slight computational overhead.
// FULL models collisions and time delay between nodes on the LAN.
//      When using this model, the nodes MUST specify an X/Y position
//      (meters) to allow for propogation delay calculations.

typedef std::vector<Node*>  NodeVec_t; 

//Doc:ClassXRef
class Ethernet : public LinkReal {
  //Doc:Class Class {\tt Ethernet} defines the Ethernet Link object. It is
  //Doc:Class derived from the {\tt Link} object. The ethernet object
  //Doc:Class creates an ethernet with a user-defined number of nodes.
  
public:
  typedef enum { NONE, PARTIAL, FULL } Detail_t;
  // Constructors
  // This one specifies how many nodes
  //Doc:Method
  Ethernet(Count_t,               // Node count
           IPAddr_t, Mask_t,      // IP addr start, mask
           SystemId_t = 0,
           Rate_t r = Rate("10Mb"), 
           Time_t dly = Time("1us"),
           Detail_t d = PARTIAL);
  //Doc:Desc The constructor for the {\tt Ethernet} object that creates an
  //Doc:Desc an ethernet, with a given number of nodes and the associated
  //Doc:Desc interfaces. This constructor assumes that we create a subnet
  //Doc:Desc without any default gateway.
  //Doc:Arg1 The number of nodes in this ethernet
  //Doc:Arg2 The beginning IP address of this ethernet
  //Doc:Arg3 The subnet mask to be used for this ethernet.
  //Doc:Arg4 Responsible system ID for distributed simulation
  //Doc:Arg5 The Transmission Rate of the link in bps. (may use
  //Doc:Arg5 ratetimeparse for simplicity)
  //Doc:Arg6 The link delay in secs. (may use ratetimeparse for simplicity)
  //Doc:Arg7 A detail type whether PARTIAL, FULL or NONE

  //Doc:Method
  Ethernet(Count_t,               // Node count
           IPAddr_t, Mask_t,      // IP addr start, mask
           const L4Protocol& l4,  // Layer 4 protocol to bind to each node 
           SystemId_t = 0,
           Rate_t r = Rate("10Mb"), 
           Time_t dly = Time("1us"),
           Detail_t d = PARTIAL);
  //Doc:Desc The constructor for the {\tt Ethernet} object that creates an
  //Doc:Desc an ethernet, with a given number of nodes and the associated
  //Doc:Desc interfaces. This constructor assumes that we create a subnet
  //Doc:Desc without any default gateway.  This one also binds
  //Doc:Desc   the specified l4 protocol to each node (presumably a TCP Server)
  //Doc:Arg1 The number of nodes in this ethernet
  //Doc:Arg2 The beginning IP address of this ethernet
  //Doc:Arg3 The subnet mask to be used for this ethernet.
  //Doc:Arg4 The layer 4 protocol to bind to each node
  //Doc:Arg5 Responsible system ID for distributed simulation
  //Doc:Arg6 The Transmission Rate of the link in bps. (may use
  //Doc:Arg6 ratetimeparse for simplicity)
  //Doc:Arg7 The link delay in secs. (may use ratetimeparse for simplicity)
  //Doc:Arg8 A detail type whether PARTIAL, FULL or NONE

  // This one specifies a single "gateway" node for packet forwarding
  // This is quite memory efficient, as all nodes other than the
  // gateway just use a single default route to the gateway
  // The gateway node is assumed to already exist

  //Doc:Method
  Ethernet(Count_t,  Node*,  // Node count, gateway node
           IPAddr_t, Mask_t, // IP addr start, mask
           SystemId_t = 0,
           Rate_t r = Rate("10Mb"),
           Time_t dly = Time("1us"),
           Detail_t d = PARTIAL);
  //Doc:Desc This constructor creates an {\tt Ethernet} object that has a
  //Doc:Desc gateway node and all of the rest of the nodes use this as a
  //Doc:Desc default route. The gateway node is assumed to already exist.
  //Doc:Arg1 The number of nodes in this ethernet
  //Doc:Arg2 Pointer to the gateway node object
  //Doc:Arg3 The beginning IP address of this ethernet
  //Doc:Arg4 The subnet mask to be used for this ethernet.
  //Doc:Arg5 Responsible system ID for distributed simulation
  //Doc:Arg6 The Transmission Rate of the link in bps. (may use
  //Doc:Arg6 ratetimeparse for simplicity)
  //Doc:Arg7 The link delay in secs. (may use ratetimeparse for simplicity)
  //Doc:Arg8 A detail type whether PARTIAL, FULL or NONE
  
  //Doc:Method
  Ethernet(Count_t,  Node*,        // Node count, gateway node
           IPAddr_t, Mask_t,       // IP addr start, mask
           const L4Protocol& l4,   // Layer 4 protocol to bind to each node 
           SystemId_t = 0,
           Rate_t r = Rate("10Mb"),
           Time_t dly = Time("1us"),
           Detail_t d = PARTIAL);
  //Doc:Desc This constructor creates an {\tt Ethernet} object that has a
  //Doc:Desc gateway node and all of the rest of the nodes use this as a
  //Doc:Desc default route. The gateway node is assumed to already exist.
  //Doc:Desc Also specifies a layer 4 protocol object to bind at each node.
  //Doc:Arg1 The number of nodes in this ethernet
  //Doc:Arg2 Pointer to the gateway node object
  //Doc:Arg3 The beginning IP address of this ethernet
  //Doc:Arg4 The subnet mask to be used for this ethernet.
  //Doc:Arg5 The layer 4 protocol to bind to each node
  //Doc:Arg6 Responsible system ID for distributed simulation
  //Doc:Arg7 The Transmission Rate of the link in bps. (may use
  //Doc:Arg7 ratetimeparse for simplicity)
  //Doc:Arg8 The link delay in secs. (may use ratetimeparse for simplicity)
  //Doc:Arg9 A detail type whether PARTIAL, FULL or NONE
  
  ~Ethernet();      // Destructor
  // Link methods
  //Doc:Method
  virtual Link*  Copy() const;               // All subclasses must have Copy
  //Doc:Desc This is a copy constructor

  //Doc:Method
  virtual InterfaceBasic* GetPeer(Packet*);       // Get peer interface
  //Doc:Desc This method returns the destination interface by looking
  //Doc:Desc at the destination MAC address of the packet.
  //Doc:Arg1 The packet to be transmitted.
  //Doc:Return The destination interface.

  // Get peer interface for peer index
  //Doc:Method
  virtual InterfaceBasic* GetPeer(Count_t);
  //Doc:Desc Get the interface pointer for the remote endpoint of
  //Doc:Desc this link for the specified peer index
  //Doc:Arg1 Which peer desired (0 <- i < NeighborCount())
  //Doc:Return A pointer to the receiving interface for the specified peer

  virtual void  SetPeer(InterfaceBasic*) ;

  //Doc:Method
  virtual bool   Transmit(Packet*, Interface*, Node*); // Transmit a packet
  //Doc:Desc Transmit a packet.
  //Doc:Arg1 The packet to transmit.
  //Doc:Arg2 A pointer to the local (transmitting) interface
  //Doc:Arg3 A pointer to the local (transmitting) node.
  //Doc:Return True if the transmit succeeded, false if packet was dropped.
  virtual  bool Transmit(Packet*, Interface*, Node*, Rate_t);

  //Doc:Method
  Count_t  Size();                      // Return number of nodes
  //Doc:Desc Returns the number of nodes on this link
  //Doc:Return number of nodes
  
  //Doc:Method
  Count_t  PeerCount();                      // Return number of peers
  //Doc:Desc Returns the number of peers on this link
  //Doc:Return number of peers

  //Doc:Method
  IPAddr_t PeerIP(Count_t);                  // Get peer IP Address (if known)
  //Doc:Desc Since each node on this link is also associated with a count
  //Doc:Desc this returns the IP address of the n'th  peer
  //Doc:Arg1 the count index n
  //Doc:Return the corresopnding IPAddress

  //Doc:Method
  IPAddr_t NodePeerIP(Node*);                // Get peer IP Address (if known)
  //Doc:Desc Return the IP address of the node if it is on this link and
  //Doc:Desc is found
  //Doc:Arg1 The pointer to the node object
  //Doc:Return The IP Address

  //Doc:Method
  bool     NodeIsPeer(Node*);                // Find out if node is a peer
  //Doc:Desc Find out if node is a peer
  //Doc:Arg1 The Node
  //Doc:Return boolean result of the test

  //Doc:Method
  Count_t  NodePeerIndex(Node*);             // Get peer index for node
  //Doc:Desc Returns the index of the node in this link
  //Doc:Arg1 The pointer to the node
  //Doc:Return The index of the peer

  //Doc:Method
  IPAddr_t DefaultPeer(Interface*);          // Get IP Addr of default peer
  //Doc:Desc Find the default peer's IP Address, this will find the default
  //Doc:Desc gateway's IP Address in the same subnet
  //Doc:Arg1 The interface whose default peerIP is to be known
  //Doc:Return The default peer's IP
  
  //Doc:Method 
  Count_t  NeighborCount(Node*);             // Number of routing neighbors
  //Doc:Desc Returns the number of neighbors on this link which are routers
  //Doc:Desc technically, count of routing peers
  //Doc:Arg1 Node querying neighbors (significant for Ethernet links)
  //Doc:Return The number of routing peers.

  // Build neighbor list

  //Doc:Method
  void     Neighbors(NodeWeightVec_t&, Interface*, bool);
  //Doc:Desc Build the vector of neighbor,interface list
  //Doc:Arg1 The NodeWightVec element where we will fill in the neighbors
  //Doc:Arg2 The interface, with respect to which we need to find these
  //Doc:Arg2 neighbors
  //Doc:Arg3 boolean to specify if we need all the neighbors OR if we need
  //Doc:Arg3 only routing peers
  
  //Doc:Method
  void     AllNeighbors(NodeWeightVec_t&);   // Build neighbor list
  //Doc:Desc AllNeighbors is used by broadcasts even if they are
  //Doc:Desc not routing peers. Use this to find all the peers
  //Doc:Arg1 The NodeWeightVec element, with respect to which,
  //Doc:Arg1 we need all the peers in this link
  

  //Doc:Method
  MACAddr  IPToMac(IPAddr_t);                // Convert peer IP to peer MAC
  //Doc:Desc Convert peer IP to peer MAC
  //Doc:Arg1 The IP Address whose MAC is to be determined
  //Doc:Return The corresponding MAC address


  //Doc:Method
  bool     RxBroadcast() { return receiveOwnBroadcast;}
  // Ethernet Methods
  //Doc:Desc   Checks whether nodes receive their own broadcasts on this link
  //Doc:Return Return true if node receive their own broadcasts;
  //Doc:Return false otherwise

  //Doc:Method
  void       AddNode(Node*);                 // Add existing node to LAN
  //Doc:Desc Add a new node to the ethernet
  //Doc:Arg1 The pointer to the node to be added

  
  //Doc:Method
  Node*      GetNode(Count_t);               // Get the i'th node in network
  //Doc:Desc  Get the node by its index
  //Doc:Arg1  The index to the list of nodes
  //Doc:Return Pointer to the node
  
  //Doc:Method
  Interface* GetIf(Count_t);                 // Get the i'th interface
  //Doc:Desc Get the interface of the node connected to this link by its index
  //Doc:Arg1 the index of the node
  //Doc:Return the interface of the node that attaches to this link

  
  //Doc:Method 
  IPAddr_t   GetIP(Count_t);                 // Get the i'th ip address
  //Doc:Desc Get IP of the node by the index of the node
  //Doc:Arg1 int index of the node
  //Doc:Return IP address if the index is within range, else IPADDR_NONE
  
  //Doc:Method
  Interface* GetIfByMac(MACAddr);            // Get interface by MAC addr
  //Doc:Desc Get a pointer to the interface by its MAC address
  //Doc:Arg1 the MAC address
  //Doc:Return pointer to the interface
  
  //Doc:Method
  Node*      Gateway();                      // Get the gateway node
  //Doc:Desc Returns the gateway node of this LAN
  //Doc:Return Pointer to the gateway node
  
  //Doc:Method
  void       SetLeafQLimit(Count_t);         // Set q limit for all leafs
  //Doc:Desc Method to set the queue limits of all the nodes in this link
  //Doc:Desc to a specified value
  //Doc:Arg1 the size of each queue
  
  //Doc:Method
  void       RxOwnBroadcast(bool rob = true) { receiveOwnBroadcast = rob;}
  //Doc:Desc   Checks whether nodes receive their own broadcasts on this link
  //Doc:Return Return true if node receive their own broadcasts;
  //Doc:Return false otherwise

  //Doc:Method
  virtual bool ScheduleFailure(Time_t t);
  //Doc:Desc This method scedules the failure of the current link in future
  //Doc:Arg1 Time when we want to schedule it
  //Doc:Return bool, true on success and false on failure (if schedule fails)

  //Doc:Method
  virtual bool ScheduleRestore(Time_t t);
  //Doc:Desc This method scedules the link being restored from current failure
  //Doc:Desc state. If it is not currently failed, it has no effect
  //Doc:Arg1 Time when we want to schedule the event
  //Doc:Return bool, true on success and false on failure (if schedule fails)
    
public:
  NodeId_t   first;    // Since there is a global node list, we don't need one 
  NodeId_t   last;     // Just log first/last+1 entries
  IPAddr_t   firstIP;  // First IP Address
  Mask_t     mask;
  IPAddr_t   nextIP;   // Next available IP Address
  MACAddr_t  firstMac; // First MAC Address
  Detail_t   detail;   // Level of detail
  Node*      gw;       // Points to gateway node if specified
  Interface* gwIf;     // Gateway interface
  bool       receiveOwnBroadcast; // True if nodes receive their own broadcasts
  NodeVec_t nodes;     // Vector of new nodes
  Count_t   size;      // Number of connected nodes
  L4Protocol* l4proto;  // L4 protocol to bind to each node
private:
  void ConstructorHelper(Count_t,          // Node count
                         Node*,            // Gateway node
                         IPAddr_t, Mask_t, // IP addr start, mask
                         SystemId_t,       // Responsible System for dist sim
                         L4Protocol* l4,   // Layer 4 protocol to bind to each 
                         Rate_t r, Time_t dly, Detail_t d);
};


#endif


