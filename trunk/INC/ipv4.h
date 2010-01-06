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



// Georgia Tech Network Simulator - Internet Protocol base class
// George F. Riley.  Georgia Tech, Spring 2002
// Peng Zhen, Mon, 4, Jan, 2009 

// Implements the Internet Protocol Version 4, Layer 3

#ifndef __ipv4_h__
#define __ipv4_h__

#include <list>

#include "G_common-defs.h"
#include "l3protocol.h"
#include "pdu.h"
#include "ipaddr.h"
#include "node.h"
//v1.01
//#include "backplane.h"

class IPV4Header;

class IPV4ReqInfo;
//Doc:ClassXRef
class IPV4 : public L3Protocol {
//Doc:Class The class {\tt IPV4} implements the Internet Protocol Version 4
//Doc:Class It derives 	from the class {\tt L3Protocol}. Since IPV4 is a
//Doc:Class a static instance of this is enough for a simulation that uses
//Doc:Class IPv4.
	
public:
  typedef enum { DefaultTTL = 64 } DefTTL_t;
  typedef enum { VERSION, HEADERLENGTH, SERVICETYPE,
                 TOTALLENGTH, IDENTIFICATION,
                 FLAGS, FRAGMENTOFFSET,
                 TTL, PROTOCOL, HEADERCHECKSUM,
                 SRC, DST, UID, OPTIONS } IPV4Trace_t; // Tracing options
  //Doc:Method
  IPV4();
  //Doc:Desc Default constructor

  //Doc:Method
  void DataRequest(Node*, Packet*, void*);     // From upper layer
  //Doc:Desc This method is used by layer 4 protocols like udp/tcp to
  //Doc:Desc hand a packet to IP.
  //Doc:Arg1 Pointer to the node requesting the data transfer
  //Doc:Arg2 Pointer to the packet to be transmitted
  //Doc:Arg3 Any auxiliary information that layer 4 might want to convey
  
  //Doc:Method
  void DataIndication(Interface*, Packet*);    // From lower layer
  //Doc:Desc This method is used to pass on a packet from any layer 2 protocol
  //Doc:Desc up the stack.
  //Doc:Arg1 A pointer to the interface at which this packet is received
  //Doc:Arg2 A pointer to the packet that was received
  
  //Doc:Method
  Interface* PeekInterface(Node*, void*);      // Find forwarding interface
  //Doc:Desc This method returns the interface pointer. This interface is
  //Doc:Desc chosen based on the node pointer and some arbitrary data
  //Doc:Desc passed to it.
  //Doc:Arg1 the pointer to the assocaited node
  //Doc:Arg2 the data based on which we return the address
  //Doc:Return the pointer to the interface object
  
  //Doc:Method
  virtual Count_t Version() { return 4;}       // Get IP version number
  //Doc:Desc This method returns the version of Internet Protocol
  //DOc:Return returns the version
  
  //Doc:Method
  virtual Proto_t Proto()   { return 0x800;}   // Get the protocol number
  //Doc:Desc This method returns the IANA assigned protocol number
  //Doc:Return the protocol number
  
private:
  void Broadcast(Node*, Packet*, IPV4ReqInfo*);// Network layer broadcast
private:
  static IPV4* instance; // Points to single instance of proto object
public:
  //Doc:Method
  static IPV4* Instance();   // Get a pointer to the single global instance
  //Doc:Desc This method returns the pointer to the global instance
  //Doc:Desc of the IPV4 object
  //Doc:Return the pointer to the global instance

  // Register data items with the dynamic simulation backplane
#ifndef WIN32
  static void  RegisterBackplane(DSHandle_t);
#endif
  static int   IPV4ExportQuery(char*, int, void*);
  static int   IPV4DefaultCallback(char*, void*);
  static int   IPV4ExportCallback(char*, char*, int, void*);
  static int   IPV4ImportCallback(char*, int, char*, void*);
public:
  // Static members
  static IPV4Header* bpHeader; // IPV4 header imported from backplane
  
  bool route_locally; // Allows transport layer protocols on the same
                      // node to communicate with each other. Off by
                      // default.
};

//Doc:ClassXRef
class IPV4Header : public L3PDU {
//Doc:Class	
public :
  //Doc:Method
  IPV4Header();
  //Doc:Desc This is the default constructor
 
  //Doc:Method 
  IPV4Header(const IPV4Header& r);
  //Doc:Desc This constructor takes the reference to an IPV4 header
  
  //Doc:Method
  IPV4Header(char*, Size_t&, Packet*); // Construct from serialized buffer
  //Doc:Desc This method creates a PDU and inserts in the packet from
  //Doc:Desc a serialized buffer
  //Doc:Arg1 pointer to the buffer
  //Doc:Arg2 size of the buffer
  //Doc:Arg3 the pointer to the packet
  
  //Doc:Method
  virtual ~IPV4Header();
  //Doc:Desc Destructor
  
  //Doc:Method
  Size_t  Size() const;
  //Doc:Desc This method returns the size of the PDU
  //Doc:Return the size of the PDU
  
  //Doc:Method
  PDU*    Copy() const{ return new IPV4Header(*this);}
  //Doc:Desc This method creates a copy of this PDU
  //Doc:Return A pointer to the copy of this PDU
  
  //Doc:Method
  Count_t Version() { return 4;}       // Indicate IPV4
  //Doc:Desc Returns the version number of the protocol
  //Doc:Return version 4 of IP
  
  //Doc:Method
  Proto_t Proto() { return 0x800;}        // IPV4 is protocol 0x800
  //Doc:Method Return the IPV4 protocol number
  //Doc:Return 0x800 for IPV4

  //Doc:Method
  Priority_t Priority();
  //Doc:Desc   Get the priority for this header. For IPV4, it is the
  //Doc:Desc diffserv codepoint field, which overlays the
  //Doc:Desc type-of-service field.
  //Doc:Return Priority for this packet.

  void    Trace(Tfstream&, Bitmap_t,  // Trace the contents of this pdu
                     Packet* = nil, const char* = nil);
  //Doc:Desc This method traces the contents of the PDU
  //Doc:Arg1 File output stream descriptor
  //Doc:Arg2 Bitmap that specifies the level of detail of the trace
  //Doc:Arg3 Packet that contains this PDU (optional)
  //Doc:Arg4 Extra text information for trace file (optional)
  
  // Serialization
  //Doc:Method
  Size_t SSize();                   // Size needed for serialization
  //Doc:Desc The actual size of all the PDU contents in terms of bytes for
  //Doc:Desc serialization of the contents.
  //Doc:Return the size
  
  //Doc:Method
  char*  Serialize(char*, Size_t&); // Serialize to a buffer
  //Doc:Desc This method is used to serialize the contents of the PDU
  //Doc:Desc into a serialized character buffer b . the size of this buffer is
  //Doc:Desc in the size arguement
  //Doc:Arg1 This is the pointer to the character buffer
  //Doc:Arg2 a reference to the size variable
  //Doc:Return the pointer to the updated character buffer
  
  //Doc:Method
  char*  Construct(char*, Size_t&); // Construct from buffer
  //Doc:Desc This method is the reverse of the Serialize . It constructs the
  //Doc:Desc PDU object from the character buffer that has been received over
  //Doc:Desc the network
  //Doc:Arg1 This is the pointer to the character buffer
  //Doc:Arg2 a reference to the size variable
  //Doc:Return the pointer to the character buffer of the remaining data

  // Inherited from L3PDU
  //Doc:Method
  virtual IPAddr_t GetSrcIP(){return src;}
  //Doc:Desc Return the source IP address.
  //Doc:Return source IP Address

  //Doc:Method
  virtual IPAddr_t GetDstIP() { return dst;}
  //Doc:Desc Return the destination IP address.
  //Doc:Return destination IP Address

public:
  Byte_t   version;
  Byte_t   headerLength;
  Byte_t   serviceType;
  Word_t   totalLength;
  Word_t   identification;
  Byte_t   flags;
  Word_t   fragmentOffset;
  Byte_t   ttl;       // Time to live
  Byte_t   protocol;  // Layer 4 protocols
  Word_t   headerChecksum;
  IPAddr   src;
  IPAddr   dst;
  PDUVec_t options;   // IP Options
};

//Doc:ClassXRef
class IPV4Options : public PDU { // Base class for ipv4 options
//Doc:Class The Class {\tt IPV4Options } defines the IP options part
//Doc:Class of the IP Header. As all other headers this is also derived from
//Doc:Class the class {\tt PDU}
public:
  //Doc:Method
  IPV4Options(Byte_t o) : optionNumber(o) { }
  //Doc:Desc Constructor takes a byte as the arguement
  
public:
  //Doc:Method
  virtual Size_t  Size() const { return 0;}
  //Doc:Desc Returns the size of the PDU
  //Doc:Return size of the PDU
  
  //Doc:Method  
  virtual PDU*    Copy() const { return new IPV4Options(*this);}
  //Doc:Desc This method creates a copy of this PDU
  //Doc:Return A pointer to the copy of this PDU
  
  //Doc:Method
  virtual void  Trace(Tfstream&, Bitmap_t,     // Trace this pdu 
                     Packet* = nil, const char* = nil) { }
  //Doc:Desc This method traces the contents of the PDU
  //Doc:Arg1 File output stream descriptor
  //Doc:Arg2 Bitmap that specifies the level of detail of the trace
  //Doc:Arg3 Packet that contains this PDU (optional)
  //Doc:Arg4 Extra text information for trace file (optional)
  
  //Doc:Method
  Byte_t  Option() { return optionNumber;}
  //Doc:Desc returns the options of the PDU
  //Doc:Return the options
public:
  Byte_t  optionNumber;
};

//Doc:ClassXRef
class IPV4ReqInfo {  // Information passed from L4 for IP Data Request
//Doc:Class This class defines the data structure that defines the information
//Doc:Class that is passed from layer 4 to layer 3
public:
  IPV4ReqInfo() : dst(IPADDR_NONE), src(IPADDR_NONE) { };
  IPV4ReqInfo(IPAddr_t d, IPAddr_t s, Count_t t, Proto_t p) 
      : dst(d), src(s), ttl(t), l4proto(p), tos(0) { }
  IPV4ReqInfo(IPAddr_t d, IPAddr_t s, Count_t t, Proto_t p, Byte_t ts) 
      : dst(d), src(s), ttl(t), l4proto(p), tos(ts) { }
  IPAddr   dst;    // Destination
  IPAddr   src;    // Src IP (if spoofed, 0 if normal lookup)
  Count_t  ttl;    // Time to Live
  Proto_t  l4proto;// Layer 4 protocol
  Byte_t   tos;    // Value for l3 Type-of-Service field
  // Probably need more later
};

#endif

