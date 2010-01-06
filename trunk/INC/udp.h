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

// Georgia Tech Network Simulator - UDP Base Class
// George F. Riley.  Georgia Tech, Spring 2002

// Base class for all UDP variants

#ifndef __udp_h__
#define __udp_h__

#include <deque>

#include "pdu.h"
#include "l4protocol.h"
#include "datapdu.h"
#include "notifier.h"

//Doc:ClassXRef
class UDPHeader : public L4PDU {
public :
  UDPHeader();
  UDPHeader(PortId_t, PortId_t, Word_t, Word_t);
  UDPHeader(PortId_t, PortId_t, Word_t, Word_t, FId_t);
  UDPHeader(char* b, Size_t& sz, Packet* p);
  UDPHeader(const UDPHeader&);
  ~UDPHeader();
  Size_t  Size() const;
  PDU*    Copy() const;
  Proto_t Proto() { return 17;}        // UDP is protocol 17
  // Serialization
  Size_t  SSize();                     // Size needed for serialization
  char*   Serialize(char*, Size_t&);   // Serialize to a buffer
  char*   Construct(char*, Size_t&);   // Construct from buffer
  void    Trace(Tfstream&, Bitmap_t, // Trace the contents of this pdu
                Packet* = nil, const char* = nil);
public:
  PortId_t sourcePort;
  PortId_t destPort;
  Word_t   messageLength;
  Word_t   checksum;
  FId_t    fid;           // Flow id's are not part of UDP, but useful
  Seq_t    seq;           // Sequences are also not part of UDP, but useful
};

// Typedefs for the pending data queue
//Doc:ClassXRef
class UDPPending {
public:
  UDPPending();
  UDPPending(Data*, IPAddr_t, PortId_t) ;
  UDPPending(const UDPPending&);   // Copy constructor
  ~UDPPending();                   // Destructor
public:
  Data*    data;
  IPAddr_t ip;
  PortId_t port;
  Size_t   sent;   // Sent so far
};
typedef std::deque<UDPPending> Pending_t;

//Doc:ClassXRef
class UDP : public L4Protocol, public NotifyHandler {
  //Doc:Class Class {\tt UDP} defines a model of the User Datagram Protocol.
  //Doc:Class {\tt UDP} inherits from {\tt L4Protocol} and implements most of
  //Doc:Class the methods defined there.
public:
  typedef enum { ProtoNumber =  17 } UDPProto_t; // UDP Layer4 Protocol number
  typedef enum { SOURCE_PORT, DEST_PORT, MSG_LENGTH,
                 CHECKSUM, FID, SEQ } UDPTrace_t; // Tracing options
  // Constructors
  //Doc:Method
  UDP();
    //Doc:Desc Default constructor, no arguments.

  //Doc:Method
  UDP(Node* n);
    //Doc:Desc Constructor, specify a node to attached to.
    //Doc:Arg1 Node this {\tt UPD} protocol is attached to.

  //Doc:Method
  UDP(const UDP&);
    //Doc:Desc Copy constructor.
    //Doc:Arg1 {\tt UDP} object to copy.

  // Destructor
  virtual ~UDP(); 
public:
  // Notifier Methods
  void Notify(void*);

  // L4Protocol methods
  // Upcall From lower layer
  void DataIndication(Node*, Packet*, IPAddr_t, Interface*); 
  NCount_t Send(Size_t);                         // Send empty data
  NCount_t Send(char*, Size_t);                  // Send with data
  NCount_t Send(Data&);                          // Send with data
  NCount_t SendTo(Size_t, IPAddr_t, PortId_t);   // Send to specified target
  NCount_t SendTo(char*, Size_t, IPAddr_t, PortId_t); // Send w/data, to target
  NCount_t SendTo(Data&, IPAddr_t, PortId_t);    // Send w/ data
  Proto_t  Proto() const { return 17;}           // Return the layer 4 protoco
  bool     Connect(IPAddr_t, PortId_t);          // Connect to remote host
  bool     Close();                              // Close connection
  L4Protocol* Copy() const;                      // Create a copy of protocol
  Proto_t  ProtocolNumber();                     // Return UDP protocol number
  bool     IsTCP();                              // True if TCP
  //Doc:Method
  void     PacketSize(Size_t);                   // Set packet size
    //Doc:Desc Set the packet size for this {\tt UDP} endpoint.
    //Doc:Arg1 Packet size (bytes).

  //Doc:Method
  Size_t   PacketSize() const { return pktSize;} // Get packet size
    //Doc:Return Query packet size for this {\tt UDP} endpoint.

private:
  NCount_t Send(Packet*,IPAddr_t,PortId_t);      // Common send code
  void     SendPending();                        // Send pending data
private:
  Size_t    pktSize;                             // Size of packets
  Seq_t     seq;                                 // Seq number of next pkt
protected:
  Pending_t pendingData;                         // Data to be sent
public:
  // Static methods/members
  //Doc:Method
  static void   DefaultPacketSize(Size_t);       // Set the default pkt size
    //Doc:Desc Set default packet size for all {\tt UDP} protocol objects.
    //Doc:Arg1 Default packet size (bytes).  Default size is 512 bytes unless
    //Doc:Arg1 otherwise specified.

  static Size_t defaultPacketSize;               // Global default
  static Count_t totalUDP;                       // Debug
  static Count_t totalUDPDel;                    // Debug

};



#endif


