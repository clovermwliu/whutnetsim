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


#ifndef __OSPF_H__
#define __OSPF_H__

#include <queue>
#include "G_common_defs.h"
#include "handler.h"
#include "l4demux.h"
#include "l4protocol.h"

#ifdef HAVE_QT
#include <MyColor.h>

#define HELLOCOLOR	Qt::red
#define DATADCOLOR	Qt::yellow
#define LSREMyColor	Qt::green
#define LSUPDCOLOR	Qt::blue
#define LSACKCOLOR	Qt::black
#endif

// OSPF Constants
#define OSPFLAYER		((Layer_t)4) // Layer 4 protocol
#define OSPFPROTO		((Proto_t)89) // Protocol number
#define OSPFVERSION		((Version_t)2) // Protocol version
#define OSPFTTL			((Count_t)1) // TTL

#define OSPFALLROUTERS		IPAddrBroadcast	// ((IPAddr_t)((224 << 24) | 5)) // 224.0.0.5
#define OSPFALLDROUTERS		IPAddrBroadcast	// ((IPAddr_t)((224 << 24) | 6)) // 224.0.0.6	

#define OSPFHELLOINTERVAL	((Time_t)10) // Default hello interval
#define OSPFDEADINTERVAL	((Time_t)40) // Default router dead interval
#define OSPFRXMTINTERVAL	((Time_t)5) // Default retransmission interval

#define OSPFINITSEQUENCE	((OSPFLSSeq_t)0x80000001) // Initial sequence number
#define OSPFMAXSEQUENCE		((OSPFLSSeq_t)0x7fffffff) // Max sequence number

#define OSPFBACKBONE		((OSPFArea_t)IPADDR_NONE)
#define OSPFNODR		((Word_t)0)
#define OSPFMAXAGE		((Word_t)3600)
#define OSPFMAXAGEDIFF		((Word_t)900)
#define OSPFMINARRIVAL		((Word_t)1)

#define OSPFOPT_DC		((Bitmap_t)(1<<5)) // Demand circuits
#define OSPFOPT_EA		((Bitmap_t)(1<<4)) // External-Attributes-LSAs
#define OSPFOPT_NP		((Bitmap_t)(1<<3)) // Type-7 LSAs
#define OSPFOPT_MC		((Bitmap_t)(1<<2)) // IP multicst forward
#define OSPFOPT_E		((Bitmap_t)(1<<1)) // AS-external-LSAs flooding
#define OSPFOPTION		((Bitmap_t)(OSPFOPT_DC | OSPFOPT_EA | OSPFOPT_NP | OSPFOPT_MC | OSPFOPT_E))

// Type definitions
typedef unsigned char	UINT8;
typedef short		INT16;
typedef unsigned short	UINT16;
typedef int		INT32;
typedef unsigned int	UINT32;

typedef UINT16 OSPFCost_t; // OSPF wight
typedef std::vector<IPAddr_t> OSPFIPs; // Interface IP address list
typedef OSPFIPs::iterator OSPFIPSIt; // Interface IP address list iterator
typedef IPAddr_t OSPFRouter_t; // Router ID
typedef std::vector<OSPFRouter_t> OSPFRouters; // Router ID list
typedef OSPFRouters::iterator OSPFRoutersIt; // Router ID list iterator
typedef OSPFRouters::const_iterator OSPFRoutersConstIt; // Router ID list iterator
typedef IPAddr_t OSPFArea_t; // Area ID
typedef IPAddr_t OSPFID_t; // Link state ID
typedef std::vector<OSPFID_t> OSPFIDs; // ID list
typedef OSPFIDs::iterator OSPFIDsIt; // ID list iterator
typedef OSPFIDs::const_iterator OSPFIDsConstIt; // ID list iterator
typedef int OSPFLSSeq_t; // LS sequence

typedef enum { 
  OSPFP2P, 
  OSPFBROADCAST, 
  OSPFNBMA, 
  OSPFP2M, 
  OSPFVLINK 
} OSPFNetwork_t;

typedef enum {
  OSPFROUTER = 1, 
  OSPFNETWORK = 2,
  OSPFSUMIP = 3,
  OSPFSUMASBR = 4,
  OSPFASEXT = 5 
} OSPFLSA_t;

class OSPFDataD;
class OSPFLSReq;

//Doc:ClassXRef
class OSPFLSAHeader {
//Doc:Class Class OSPFLSAHeader defines the LSA header

public:
  //Doc:Method
  OSPFLSAHeader (Bitmap_t, OSPFLSA_t, OSPFID_t, OSPFRouter_t, OSPFLSSeq_t sequence = OSPFINITSEQUENCE); // Constructor 
  //Doc:Desc Constructs a new OSPF LSA header instance
  //Doc:Arg1 Option.
  //Doc:Arg2 Type.
  //Doc:Arg3 LS ID
  //Doc:Arg4 Advertising Router ID.
  //Doc:Arg5 Sequence number.

  //Doc:Method
  OSPFLSAHeader (const OSPFLSAHeader&); // Copy constructor
  //Doc:Desc Constructs a new LSA header instance from a existing instance

  //Doc:Method
  virtual ~OSPFLSAHeader (void) {} // Destructor
  //Doc:Desc Destroys a OSPF LSA header instance.

  //Doc:Method
  virtual Size_t Size (void) const { return (20); }
  //Doc:Desc Return OSPFLSAHeader size
  //Doc:Return OSPFLSAHeader size

  //Doc:Method
  virtual OSPFLSAHeader* Copy() const { return (new OSPFLSAHeader (*this)); } // Create a copy of this header
  //Doc:Desc This method creates a copy of this LSA header
  //Doc:Return A pointer to the copy of this LSA header

  //Doc:Method
  Word_t GetAge (void) const { return (age); }
  //Doc:Desc Get age.

  //Doc:Method
  void SetAge (Word_t age) { this->age = age; }
  //Doc:Desc Set age.
  //Doc:Arg1 New age.

  //Doc:Method
  Word_t IncAge (Word_t inc = 1) { age += inc; if (age > OSPFMAXAGE) { age = OSPFMAXAGE; } return (age); }
  //Doc:Desc Incease age.

  //Doc:Method
  OSPFLSA_t GetType (void) const { return (type); }
  //Doc:Desc Get link type.

  //Doc:Method
  bool CheckType (void) const { return ((type >= OSPFROUTER) && (type <= OSPFASEXT)); }
  //Doc:Desc Check the correctness.

  //Doc:Method
  OSPFID_t GetLinkID (void) const { return (lid); }
  //Doc:Desc Get link ID.

  //Doc:Method
  OSPFRouter_t GetAdvID (void) const { return (rid); }
  //Doc:Desc Get advertising router ID.

  //Doc:Method
  OSPFLSSeq_t GetSequence (void) const { return (sequence); }
  //Doc:Desc Get sequence.

  //Doc:Method
  virtual Word_t CheckSum (void);
  //Doc:Desc Calculate check sum of header part.

  //Doc:Method
  bool CheckChksum (void) { return (chksum == CheckSum ()); }
  //Doc:Desc Check check sum of header part.

  //Doc:Method
  bool IsIt (OSPFLSAHeader* lsa) const { return ((type == lsa->type) && (lid == lsa->lid) && (rid == lsa->rid)); }
  //Doc:Desc Find a LSA
  //Doc:Return If the same LSA (may different instance).

  //Doc:Method
  int Compare (OSPFLSAHeader*);
  //Doc:Desc Compare with another LSA header

  //Doc:Method
  virtual bool IsDifferent (OSPFLSAHeader*);
  //Doc:Desc Compare with another LSA header

  //Doc:Method
  void SetReceive (void) { receive = age; }
  //Doc:Desc Set receive time (age).

  //Doc:Method
  Word_t FromReceive (void) const { if (receive != 0) { return (age - receive); } else { return (0); } }
  //Doc:Desc Get seconds from received.

  virtual void dump (void);

protected:
  Word_t age; // Age
  Bitmap_t option; // Router option
  OSPFLSA_t type; // LS type
  OSPFID_t lid; // LS ID
  OSPFRouter_t rid; // Advertising router ID
  OSPFLSSeq_t sequence; // Sequence number
  Word_t chksum; // Check sum
  Word_t length; // Size
  // Internal data structures
  Word_t receive; // Receive age
};

typedef std::vector<OSPFLSAHeader*> OSPFLSAHeaders; // LSA headers
typedef OSPFLSAHeaders::iterator OSPFLSAHeadersIt; // LSA headers iterator
typedef OSPFLSAHeaders::const_iterator OSPFLSAHeadersConstIt; // LSA headers iterator

//Doc:ClassXRef
class OSPFTOS {
//Doc:Class Class OSPFTOS defines the TOS and TOS metric

public:
  //Doc:Method
  OSPFTOS (Byte_t tos, OSPFCost_t metric) 
  : tos (tos), zero (0), metric (metric) {} // Constructor
  //Doc:Desc Constructs a new TOS pair instance 
  //Doc:Arg1 TOS encoding
  //Doc:Arg2 Metric
  
  //Doc:Method
  OSPFTOS (const OSPFTOS& instance)
  : tos (instance.tos), zero (0), metric (instance.metric) {} // Copy constructor
  //Doc:Desc Constructs a new TOS instance from a existing instance

  //Doc:Method
  ~OSPFTOS (void) {} // Destructor
  //Doc:Desc Destroys a TOS pair instance.

  //Doc:Method
  static Size_t Size (void) { return (4); }
  //Doc:Desc Return TOS pair size
  //Doc:Return TOS pair size

  //Doc:Method
  OSPFTOS* Copy () const { return (new OSPFTOS (*this)); } // Create a copy of this TOS
  //Doc:Desc This method creates a copy of this TOS
  //Doc:Return A pointer to the copy of this TOS

  //Doc:Method
  bool IsDifferent (OSPFTOS* t) { return (tos != t->tos) && (metric != t->metric);}
  //Doc:Desc Compare with another tos

private:
  Byte_t tos; // TOS encoding
  Byte_t zero; // Always 0
  OSPFCost_t metric; // Metric for this TOS
};

typedef std::vector<OSPFTOS*> OSPFTOSs; // TOS pair list
typedef OSPFTOSs::iterator OSPFTOSsIt; // TOS pair list iterator 
typedef OSPFTOSs::const_iterator OSPFTOSsConstIt; // TOS pair list iterator 

//Doc:ClassXRef
class OSPFLink {
//Doc:Class Class OSPFLink defines the link in {\tt OSPFLSARouter}

public:
  typedef enum { 
    P2P = 1, 
    TRANSIT = 2, 
    STUB = 3, 
    VLINK = 4 
  } Type_t;

  //Doc:Method
  OSPFLink (Type_t, OSPFID_t, Long_t, OSPFCost_t); // Constructor
  //Doc:Desc Constructs a new link instance 
  //Doc:Arg1 Link type.
  //Doc:Arg2 Link ID
  //Doc:Arg3 Link data.
  //Doc:Arg4 Metric.

  //Doc:Method
  OSPFLink (const OSPFLink&); // Copy constructor
  //Doc:Desc Constructs a new link instance from a existing instance

  //Doc:Method
  ~OSPFLink (void); // Destructor
  //Doc:Desc Destroys a link instance.

  //Doc:Method
  Size_t Size (void) const { return (12 + OSPFTOS::Size () * toss.size ()); }
  //Doc:Desc Return link structure size
  //Doc:Return Link structure size

  //Doc:Method
  OSPFLink* Copy () const { return (new OSPFLink (*this)); } // Create a copy of this link
  //Doc:Desc This method creates a copy of this link
  //Doc:Return A pointer to the copy of this link

  //Doc:Method
  Type_t  GetType (void) { return (type); }
  //Doc:Desc Get type.

  //Doc:Method
  OSPFID_t GetLinkID (void) { return (lid); }
  //Doc:Desc Get link ID.

  //Doc:Method
  Long_t GetData (void) { return (data); }
  //Doc:Desc Get link data.

  //Doc:Method
  OSPFCost_t GetCost (void) { return (metric); }
  //Doc:Desc Get cost.

  //Doc:Method
  bool IsDifferent (OSPFLink*);
  //Doc:Desc Compare with another link

  void dump (void);

private:
  Type_t type; // Link type
  OSPFID_t lid; // Link ID
  Long_t data; // Link data
  Byte_t number; // #TOS
  OSPFCost_t metric; // Metric
  OSPFTOSs toss; // TOS list
};

typedef std::vector<OSPFLink*> OSPFLinks; // Link list
typedef OSPFLinks::iterator OSPFLinksIt; // Link list iterator 
typedef OSPFLinks::const_iterator OSPFLinksConstIt; // Link list iterator 

class OSPFArea;

//Doc:ClassXRef
class OSPFLSARouter : public OSPFLSAHeader {
//Doc:Class Class OSPFLSARouter defines the Router-LSA

public:
  static const Bitmap_t VBIT = ((Bitmap_t)(1<<5));
  static const Bitmap_t EBIT = ((Bitmap_t)(1<<6));
  static const Bitmap_t BBIT = ((Bitmap_t)(1<<7));
  
  //Doc:Method
  OSPFLSARouter (OSPFArea*, OSPFLSSeq_t sequence = OSPFINITSEQUENCE); // Constructor
  //Doc:Desc Constructs a new OSPF Router-LSA instance
  //Doc:Arg1 Area
  //Doc:Arg2 Sequence number.

  //Doc:Method
  OSPFLSARouter (const OSPFLSARouter&); // Copy constructor
  //Doc:Desc Constructs a new Router LSA instance from a existing instance

  //Doc:Method
  ~OSPFLSARouter (void); // Destructor
  //Doc:Desc Destroys a OSPF Router-LSA instance.

  //Doc:Method
  Size_t Size (void) const { return (length); }
  //Doc:Desc Return Router-LSA size
  //Doc:Return Router-LSA size

  //Doc:Method
  OSPFLSAHeader* Copy() const { return (new OSPFLSARouter (*this)); } // Create a copy of this LSA
  //Doc:Desc This method creates a copy of this LSA
  //Doc:Return A pointer to the copy of this LSA

  //Doc:Method
  bool IsVLinkEnd (void) { return ((veb & VBIT) != 0); }
  //Doc:Desc Get

  //Doc:Method
  void AddLink (OSPFLink* link) { if (link != nil) { length += link->Size (); links.push_back (link); number++; } }
  //Doc:Desc Add a new link into Router-LSA.
  //Doc:Arg1 Link pointer.

  //Doc:Method
  void AddLinks (OSPFArea*);
  //Doc:Desc Add links of a area into Router-LSA.
  //Doc:Arg1 Link pointer.

  //Doc:Method
  void ClearLinks (void);
  //Doc:Desc Clear links.

  //Doc:Method
  OSPFLinks& GetLinks (void) { return (links); }
  //Doc:Desc Get link list.

  //Doc:Method
  Word_t CheckSum (void);
  //Doc:Desc Calculate check sum.

  //Doc:Method
  void Flush (OSPFArea*);
  //Doc:Desc Flush the instance.
  //Doc:Arg1 Area pointer.

  //Doc:Method
  bool IsDifferent (OSPFLSAHeader*);
  //Doc:Desc Compare with another LSA.

  //Doc:Method
  bool IsConnected (OSPFID_t);
  //Doc:Desc If connects a vertex

  void dump (void);

private:
  Bitmap_t veb; // Router type V/E/B
  Long_t number; // #Link
  OSPFLinks links; // Link list
};

typedef std::vector<OSPFLSARouter*> OSPFLSARouters; // Router-LSAs
typedef OSPFLSARouters::iterator OSPFLSARoutersIt; // Router-LSAs iterator
typedef OSPFLSARouters::const_iterator OSPFLSARoutersConstIt; // Router-LSAs iterator

//Doc:ClassXRef
class OSPFLSANetwork {
//Doc:Class Class OSPFLSANetwork defines the Network-LSA
};

typedef std::vector<OSPFLSANetwork*> OSPFLSANetworks; // Network-LSAs
typedef OSPFLSANetworks::iterator OSPFLSANetworksIt; // Network-LSAs iterator
typedef OSPFLSANetworks::const_iterator OSPFLSANetworksConstIt; // Network-LSAs iterator

//Doc:ClassXRef
class OSPFLSASummary {
//Doc:Class Class OSPFLSASummary defines the Summary-LSA
};

typedef std::vector<OSPFLSASummary*> OSPFLSASummaries; // Summary-LSAs 
typedef OSPFLSASummaries::iterator OSPFLSASummariesIt; // Summary-LSAs iterator
typedef OSPFLSASummaries::const_iterator OSPFLSASummariesConstIt; // Summary-LSAs iterator

//Doc:ClassXRef
class OSPFLSAASExt {
//Doc:Class Class OSPFLSAASExt defines the AS-external-LSA
};

typedef std::vector<OSPFLSAASExt*> OSPFLSAASExts; // AS-external-LSAs
typedef OSPFLSAASExts::iterator OSPFLSAASExtsIt; // AS-external-LSAs iterator
typedef OSPFLSAASExts::const_iterator OSPFLSAASExtsConstIt; // AS-external-LSAs iterator

class OSPFInterface; // Forward declare

//Doc:ClassXRef
class OSPFPacket : public L4PDU {
//Doc:Class Class OSPFPacket define the packet header info for {\tt OSPF}.
//Doc:Class This is the basic information common to all OSPF messages.
//Doc:Class This is sub-classed by the more specific OSPF messages (eg. Hello, etc).
//Doc:Class Since OSPF is conceptually a Layer 4 protocol, we subclass L4PDU.

public:
  // Enumerate the OSPF Packet Types
  typedef enum {			
    HELLO = 1, // Hello packet
    DATAD = 2, // Database Description
    LSREQ = 3, // Link State Request
    LSUPD = 4, // Link State Update
    LSACK = 5, // Link State Acknowledgment
  } Type_t;

  //Doc:Method
  OSPFPacket (Type_t, OSPFInterface*); // Constructor with args 
  //Doc:Desc Constructs a new OSPF packet header instance with args.
  //Doc:Arg1 Packet type.
  //Doc:Arg2 Interface.

  //Doc:Method
  OSPFPacket (const OSPFPacket&); // Copy constructor
  //Doc:Desc Constructs a new OSPF packet header instance from a existing instance

  //Doc:Method
  virtual ~OSPFPacket (void) {} // Destructor
  //Doc:Desc Destroys a OSPF packet header instance.

  //Doc:Method
  virtual Size_t Size (void) const { return (24); } // OSPF header size
  //Doc:Desc This method returns the size of the PDU
  //Doc:Return the size of the PDU

  //Doc:Method
  virtual OSPFPacket* Copy () const { return (new OSPFPacket (*this)); } // Create a copy of this header
  //Doc:Desc This method creates a copy of this PDU
  //Doc:Return A pointer to the copy of this PDU

  //Doc:Method  
  virtual Layer_t Layer (void) const { return (OSPFLAYER); } // OSPFLAYER
  //Doc:Desc Each PDU must also return a layer numer as to which protocol layer
  //Doc:Desc it belongs to according to the ISO model
  //Doc:Return the OSPF layer number

  //Doc:Method  
  virtual Version_t Version (void) const { return (OSPFVERSION); } // OSPFVERSION
  //Doc:Desc Return the version number of the PDU we are using.
  //Doc:Return the version number

  //Doc:Method  
  virtual Proto_t Proto (void) const { return (OSPFPROTO); } // OSPFPROTO
  //Doc:Desc Most of the protocols are identified by the numeric values as
  //Doc:Desc listed by IANA. thie method returns that protocol number
  //Doc:Return the protocol number

  //Doc:Method  
  Type_t GetType (void) const { return (type); }
  //Doc:Desc Get packet type.

  //Doc:Method  
  OSPFRouter_t GetAdvID (void) const { return (rid); }
  //Doc:Desc Get advertising router ID.

  //Doc:Method  
  bool CheckPacket (IPAddr_t, IPAddr_t, OSPFInterface*) const; // Check the packet
  //Doc:Desc This method check the correctness of the packet
  //Doc:Arg1 Source IP address.
  //Doc:Arg2 Destination IP address.
  //Doc:Arg3 Receiving interface.
  //Doc:Return The correctness.

  virtual void dump (void) {};

protected:
  static const Version_t version = OSPFVERSION; // OSPF Version number
  Type_t type; // Packet type
  Size_t length; // Length in bytes (including OPSF header)
  OSPFRouter_t rid; // Router ID of source
  OSPFArea_t aid; // Area ID
  Word_t chksum; // Packet check sum
  Word_t atype; // Authentication type
  Long_t akey[2]; // Authentication data  
};

//Doc:ClassXRef
class OSPFHello : public OSPFPacket {
//Doc:Class Class OSPFHello define the hello packet info for {\tt OSPF}.
//Doc:Class Class OSPFHello is a sub-class of {\tt OSPFPacket}.

friend class OSPFNeighbor;
friend class OSPFInterface;

public:
  //Doc:Method
  OSPFHello (OSPFInterface*); // Constructor with args
  //Doc:Desc Constructs a new OSPF hello packet instance with args.
  //Doc:Arg1 Output interface.

  //Doc:Method
  OSPFHello (const OSPFHello&);
  //Doc:Desc Constructs a new OSPF hello packet instance from existing instance.

  //Doc:Method
  ~OSPFHello (void); // Destructor
  //Doc:Desc Destroys a OSPF hello packet instance.

  //Doc:Method
  Size_t Size (void) const { return (length); } // OSPF hello packet size
  //Doc:Desc This method returns the size of the PDU
  //Doc:Return the size of the PDU

  //Doc:Method
  OSPFPacket* Copy () const { return (new OSPFHello (*this)); } // Create a copy of this 
  //Doc:Desc This method creates a copy of this PDU
  //Doc:Return A pointer to the copy of this PDU

  //Doc:Method
  bool CheckPacket (OSPFInterface*) const; // Check the packet
  //Doc:Desc This method checks the correctness of the packet
  //Doc:Arg1 Receiving interface.
  //Doc:Return The correctness.

  //Doc:Method
  bool FindRouter (OSPFRouter_t) const;
  //Doc:Desc This method finds Router ID in neighbor list.
  //Doc:Arg1 Router ID.
  //Doc:Return If exist.

  void dump (void);

private:
  Mask_t mask;
  Time_t helloInterval;
  Bitmap_t option;
  Byte_t priority;
  Time_t deadInterval;
  IPAddr_t designated;
  IPAddr_t backup;
  OSPFRouters neighbors;
};

//Doc:ClassXRef
class OSPFRequest {
//Doc:Class Class OSPFRequest define the LS request item info for {\tt OSPFLSReq}.

public:
  //Doc:Method
  OSPFRequest (OSPFLSAHeader* header) 
  : type (header->GetType ()), lid (header->GetLinkID ()), rid (header->GetAdvID ()) {}
  //Doc:Desc Constructs a new OSPF link state request item instance.

  //Doc:Method
  OSPFRequest (const OSPFRequest& instance) 
  : type (instance.type), lid (instance.lid), rid (instance.rid) {}
  //Doc:Desc Constructs a new OSPF request item instance from existing instance.

  //Doc:Method
  ~OSPFRequest (void) {}
  //Doc:Desc Destroys a OSPF link state request item instance.

  //Doc:Method
  static Size_t Size (void) { return (12); }
  //Doc:Desc Return TOS pair size
  //Doc:Return TOS pair size

  //Doc:Method
  OSPFRequest* Copy () const { return (new OSPFRequest (*this)); } // Create a copy of this instance 
  //Doc:Desc This method creates a copy of this instance
  //Doc:Return A pointer to the copy of this instance 

  //Doc:Method
  OSPFLSA_t GetType (void) const { return (type); }
  //Doc:Desc Get link type.

  //Doc:Method
  OSPFID_t GetLinkID (void) const { return (lid); }
  //Doc:Desc Get link ID.

  //Doc:Method
  OSPFRouter_t GetAdvID (void) const { return (rid); }
  //Doc:Desc Get advertising router ID.

private:
  OSPFLSA_t type; // LS type
  OSPFID_t lid; // LS ID
  OSPFRouter_t rid; // Advertising router ID
}; 

typedef std::vector<OSPFRequest*> OSPFRequests; // LS request list
typedef OSPFRequests::iterator OSPFRequestsIt; // LS request list iterator
typedef OSPFRequests::const_iterator OSPFRequestsConstIt; // LS request list iterator

//Doc:ClassXRef
class OSPFLSUpd : public OSPFPacket {
//Doc:Class Class OSPFLSUpd define the link state update packet info for {\tt OSPF}.
//Doc:Class Class OSPFLSUpd is a sub-class of {\tt OSPFPacket}.

public:
  //Doc:Method
  OSPFLSUpd (OSPFInterface*); // Constructor
  //Doc:Desc Constructs a new OSPF link state update packet instance.
  //Doc:Arg1 Interface pointer.

  //Doc:Method
  OSPFLSUpd (const OSPFLSUpd&);
  //Doc:Desc Constructs a new OSPF link state update packet instance from existing instance.

  //Doc:Method
  ~OSPFLSUpd (void); // Destructor
  //Doc:Desc Destroys a OSPF link state update packet instance.

  //Doc:Method
  Size_t Size (void) const { return (length); } // OSPF link state update packet size
  //Doc:Desc This method returns the size of the PDU
  //Doc:Return the size of the PDU

  //Doc:Method
  OSPFPacket* Copy() const { return (new OSPFLSUpd (*this)); } // Create a copy of this packet
  //Doc:Desc This method creates a copy of this PDU
  //Doc:Return A pointer to the copy of this PDU

  //Doc:Method
  void AddLSA (OSPFLSAHeader*, Time_t);
  //Doc:Desc Add a LSA.
  //Doc:Arg1 LSA pointer.
  //Doc:Arg2 Transmittion delay.

  //Doc:Method
  OSPFLSAHeaders& GetLSAs (void) { return (lsas); }
  //Doc::Desc Get request list.

  void dump (void);

private:
  Long_t number; // #LSA
  OSPFLSAHeaders lsas; // LSAs
};

//Doc:ClassXRef
class OSPFLSAck : public OSPFPacket {
//Doc:Class Class OSPFLSAck define the link state acknowledgment packet info for {\tt OSPF}.
//Doc:Class Class OSPFLSAck is a sub-class of {\tt OSPFPacket}.

public:
  //Doc:Method
  OSPFLSAck (OSPFInterface*); // Constructor with args
  //Doc:Desc Constructs a new OSPF link state acknowledgment packet instance with args.

  //Doc:Method
  OSPFLSAck (const OSPFLSAck&);
  //Doc:Desc Constructs a new OSPF link state acknowledgment packet instance from existing instance.

  //Doc:Method
  ~OSPFLSAck (void); // Destructor
  //Doc:Desc Destroys a OSPF link state acknowledgment packet instance.

  //Doc:Method
  Size_t Size (void) const { return (length); } // OSPF link state acknowledgment packet size
  //Doc:Desc This method returns the size of the PDU
  //Doc:Return the size of the PDU

  //Doc:Method
  OSPFPacket* Copy() const { return (new OSPFLSAck (*this)); } // Create a copy of this packet
  //Doc:Desc This method creates a copy of this PDU
  //Doc:Return A pointer to the copy of this PDU

  //Doc:Method
  void AddLSA (OSPFLSAHeader*);
  //Doc:Desc Add a LSA.
  //Doc:Arg1 LSA pointer.

  //Doc:Method
  OSPFLSAHeaders& GetHeaders (void) { return (headers); }
  //Doc::Desc Get request list.

  void dump (void);

private:
  OSPFLSAHeaders headers;
};

//Doc:ClassXRef
class OSPFRange {
//Doc:Class Class OSPFRange defines the area range

public:
  // Enumerate the OSPF range status
  typedef enum { ADVERTISE, NOADVERTISE } Status_t;

  //Doc:Method
  OSPFRange (IPAddr_t addr, Mask_t mask, Status_t status = NOADVERTISE)
  : addr (addr), mask (mask), status (status) {} // Constructor
  //Doc:Desc Constructs a new OSPF range instance.

  //Doc:Method
  ~OSPFRange (void) {} // Destructor
  //Doc:Desc Destroys a OSPF range instance.

private:
  IPAddr_t addr; // IP address
  Mask_t mask; // Subnet mask
  Status_t status; // If advertise
};

typedef std::vector<OSPFRange*> OSPFRanges; // Ares range
typedef OSPFRanges::iterator OSPFRangesIt; // Area ranger iterator
typedef OSPFRanges::const_iterator OSPFRangesConstIt; // Area ranger iterator

//Doc:ClassXRef
class OSPFNeighbor : public Handler {
//Doc:Class Class OSPFNeighbor defines the router that is one the same 
//Doc:Class network segment

friend class OSPFInterface;

public:
  // Enumerate the OSPF neighbor status
  typedef enum { 
    DOWN, 
    ATTEMPT, 
    INIT, 
    TWOWAY, 
    EXSTART, 
    EXCHANGE, 
    LOADING, 
    FULL 
  } Status_t;

  // Enumerate the OSPF neighbor event
  typedef enum { 
    HELLORECEIVED,
    START,
    TWOWAYRECEIVED,
    NEGOTIATIONDONE,
    EXCHANGEDONE,
    BADLSREQ,
    LOADINGDONE,
    ADJOK,
    SEQMISMATCH,
    ONEWAYRECEIVED,
    KILLNBR,
    INACTIVETIMER,
    LLDOWN,
    // Internal event
    DDRXMTTIMER,
    REQRXMTTIMER,
    UPDRXMTTIMER,
    DDRECEIVED
  } Event_t;

  //Doc:Method
  OSPFNeighbor (OSPFRouter_t, Byte_t, IPAddr_t, Bitmap_t, 
    OSPFRouter_t, OSPFRouter_t, OSPFInterface*); // Constructor with event type 
  //Doc:Desc Constructs a new OSPF neighbor instance.
  //Doc:Arg1 Neighbor ID.
  //Doc:Arg2 Priority.
  //Doc:Arg3 IP Address.
  //Doc:Arg4 Option.
  //Doc:Arg5 Designated router.
  //Doc:Arg6 Backup router.
  //Doc:Arg7 Interface.

  //Doc:Method
  ~OSPFNeighbor (void); // Destructor
  //Doc:Desc Destroys a OSPF event instance.

  //Doc:Method
  void Handle (Event*, Time_t); // Handler methods
  //Doc:Desc The OSPFNeighbor class is a subclass of {\tt Handler}, 
  //Doc:Desc the OSPFNeighbor object must handle events
  //Doc:Arg1 A pointer to the event being handled.
  //Doc:Arg2 The current simulation time.

  //Doc:Method
  void HandleEvent (Event_t);
  //Doc:Desc This method changes the status of neighbor according to the event
  //Doc:Arg1 Event.

  //Doc:Method
  OSPFInterface* GetInterface (void) { return (iface); }
  //Doc:Desc Get interface list.

  //Doc:Method
  Size_t GetMTU (void) const;
  //Doc:Desc This method returns MTU.
  //Doc:Return MTU.

  //Doc:Method
  OSPFRouter_t GetID (void) const { return (nid); }
  //Doc:Desc Get neighbor router ID.

  //Doc:Method
  IPAddr_t GetAddr (void) const { return (addr); }
  //Doc:Desc Get IP address.

  //Doc:Method
  Seq_t GetSequence (void) const { return (sequence); }
  //Doc:Desc Get sequence.

  //Doc:Method
  bool IsDown (void) const { return (status == DOWN); }
  //Doc:Desc Check if the router is down.

  //Doc:Method
  bool IsExchange (void) const { return (status == EXCHANGE); }
  //Doc:Desc Check if the router is down.

  //Doc:Method
  bool IsLoading (void) const { return (status == LOADING); }
  //Doc:Desc Check if the router is down.

  //Doc:Method
  bool IsFull (void) const { return (status == FULL); }
  //Doc:Desc Check if the router is fully adjacent.

  //Doc:Method
  bool IsFlooding (void) const { return (status >= EXCHANGE); }
  //Doc:Desc Check if the router is fully adjacent.

  //Doc:Method
  bool IsMaster (void) const;
  //Doc:Desc Check if the router is the master in database synchronization.

  //Doc:Method
  void ClearInactive (void);
  //Doc:Desc This method cancels and deletes inactive event

  //Doc:Method
  void TriggerInactive (void);
  //Doc:Desc This method triggers a inactive event in deadInterval

  //Doc:Method
  bool ClaimDR (void) const { return (addr == designated); }
  //Doc:Desc If the neighbor claims it as designated router

  //Doc:Method
  bool ClaimBDR (void) const { return (addr == backup); }
  //Doc:Desc If the neighbor claims it as backup designated router

  //Doc:Method
  void ClearLSAs (void); 
  //Doc:Desc This method clear all LSAs.

  //Doc:Method
  void BuildDatabase (void);
  //Doc:Desc Build lsa summary database.

  //Doc:Method
  OSPFLSAHeaders& GetDatabase (void) { return (database); }
  //Doc:Desc Get LSA list.

  //Doc:Method
  OSPFLSAHeaders& GetRequested (void) { return (requested); }
  //Doc:Desc Get LSA list.

  //Doc:Method
  void ClearDDRxmt (void);
  //Doc:Desc This method cancels and deletes rxmt event

  //Doc:Method
  void TriggerDDRxmt (void);
  //Doc:Desc This method triggers a rxmt event in rxmtInterval

  //Doc:Method
  void ClearLast (void) { if (last != nil) { delete last; } }
  //Doc:Desc This method clears last data description packet

  //Doc:Method
  void ClearREQRxmt (void);
  //Doc:Desc This method cancels and deletes rxmt event

  //Doc:Method
  void TriggerREQRxmt (void);
  //Doc:Desc This method triggers a rxmt event in rxmtInterval

  //Doc:Method
  void ClearUPDRxmt (void);
  //Doc:Desc This method cancels and deletes rxmt event

  //Doc:Method
  void TriggerUPDRxmt (void);
  //Doc:Desc This method triggers a rxmt event in rxmtInterval

  //Doc:Method
#ifdef HAVE_QT
  void SendPacket (MyColor, OSPFPacket*);
#else
  void SendPacket (OSPFPacket*);
#endif
  //Doc:Desc Send a packet.
  //Doc:Arg1 Packet pointer.

  //Doc:Method
  bool ReceiveHello (OSPFHello*);
  //Doc:Desc This method deals received hello packet
  //Doc:Arg1 Hello packet pointer
  //Doc:Return If continue to process this packet.

  //Doc:Method
  void ReplyHello (void);
  //Doc:Desc Reply hello packet.

  //Doc:Method
  void SendDataD (void);
  //Doc:Desc This method sends Data Description packet periodicly.

  //Doc:Method
  void ReceiveDataD (OSPFDataD*);
  //Doc:Desc This method receives Data Description packet.
  //Doc:Arg1 Data description packet pointer.

  //Doc:Method
  void ProcessDataD (OSPFDataD*);
  //Doc:Desc This method processes further for data description packet.
  //Doc:Arg1 Data description packet pointer.
  
  //Doc:Method
  void SendDDRxmt (void);
  //Doc:Desc Resends last data description packet.
  
  //Doc:Method
  void BuildRequested (OSPFDataD*);
  //Doc:Desc This method processes LSAs in data description packet.
  //Doc:Arg1 Data description packet pointer.
 
  //Doc:Method
  OSPFLSAHeader* FindRequest (OSPFLSAHeader*);
  //Doc:Desc Find a LSA in request list.
  //Doc:Arg1 LSA pointer.
  //Doc:Return Request item.

  //Doc:Method
  void EraseRequest (OSPFLSAHeader*);
  //Doc:Desc Erase a LSA from request list.
  //Doc:Arg1 LSA pointer.

  //Doc:Method
  void SendLSReq (void);
  //Doc:Desc This method sends LS request packet periodicly.

  //Doc:Method
  void ReceiveLSReq (OSPFLSReq*);
  //Doc:Desc This method receives LS request packet.
  //Doc:Arg1 LS request packet pointer.

  //Doc:Method
  void SendREQRxmt (void) { SendLSReq (); }
  //Doc:Desc Resends LS request packet.
  
  //Doc:Method
  OSPFLSAHeader* FindNoacked (OSPFLSAHeader*);
  //Doc:Desc Find a LSA in retransmit list.
  //Doc:Arg1 LSA pointer.
  //Doc:Return LSA pointer.

  //Doc:Method
  void AddNoacked (OSPFLSAHeader*);
  //Doc:Desc Add a LSA into retransmit list.
  //Doc:Arg1 LSA pointer.

  //Doc:Method
  void EraseNoacked (OSPFLSAHeader*);
  //Doc:Desc Erase a LSA into retransmit list.
  //Doc:Arg1 LSA pointer.

  //Doc:Method
  void SendUPDRxmt (void);
  //Doc:Desc Resends LS update packet.
  
  //Doc:Method
  void ReceiveLSUpd (OSPFLSUpd*);
  //Doc:Desc This method receives LS update packet.
  //Doc:Arg1 LS update packet pointer.

  //Doc:Method
  bool IsSelfOriginated (OSPFLSAHeader*) const;
  //Doc:Desc Check if a LSA is self-originated.

  //Doc:Method
  void SendLSAck (OSPFLSAHeader*);
  //Doc:Desc This method sends LS ack packet.

  //Doc:Method
  void ReceiveLSAck (OSPFLSAck*);
  //Doc:Desc This method receives LS ack packet.
  //Doc:Arg1 LS ack packet pointer.

  void debug (void);

private:
  // Neighbor router data structures
  Status_t status; // Neighbor status
  Event* inactiveEvent; // Inactive timer
  bool master; // Master/slave
  Seq_t sequence; // DD sequence number
  Bitmap_t imms; // Last received DD packet's I/M/MS
  OSPFRouter_t nid; // Neighbor ID
  Byte_t priority; // Priority
  IPAddr_t addr; // Nieghbor IP address
  Bitmap_t option; // Option
  IPAddr_t designated; // Designated router
  IPAddr_t backup; // Backup router
  OSPFLSAHeaders noacked; // LSA need be retransmitted
  OSPFLSAHeaders database; // LSA Database
  OSPFLSAHeaders requested; // LSA request list.
  // Internal data structures
  OSPFInterface* iface; // Interface
  Event* ddrxmtEvent; // Data description retransmission timer
  Event* reqrxmtEvent; // Request retransmission timer
  Event* updrxmtEvent; // Update retransmission timer
  OSPFPacket* last; // Last sent data description packet
};

typedef std::vector<OSPFNeighbor*> OSPFNeighbors; // Neighbor list
typedef OSPFNeighbors::iterator OSPFNeighborsIt; // Neighbor list iterator
typedef OSPFNeighbors::const_iterator OSPFNeighborsConstIt; // Neighbor list iterator

//Doc:ClassXRef
class OSPFInterface : public Handler {
//Doc:Class Class OSPFInterface defines the interface that the {\tt OSPF} 
//Doc:Class protocol use.

public:
  // Enumerate the OSPF interface status
  typedef enum { DOWN, LOOPBACK, WAITING, P2P, DROTHER, BDR, DR } Status_t;

  // Enumerate the OSPF interface event
  typedef enum {
    INTERFACEUP,
    WAITTIMER,
    BACKUPSEEN,
    NEIGHBORCHANGE,
    LOOPIND,
    UNLOOPIND,
    INTERFACEDOWN,
    // Internal event
    HELLOTIMER,
    RXMTTIMER
  } Event_t;

  //Doc:Method
  OSPFInterface (OSPFNetwork_t, IPAddr_t, Mask_t, Byte_t, OSPFCost_t, OSPFArea*); // Constructor with args
  //Doc:Desc Constructs a new OSPF interface instance with args
  //Doc:Arg1 Interface type.
  //Doc:Arg2 Interface IP address.
  //Doc:Arg3 Interface subnet mask.
  //Doc:Arg4 Priority.
  //Doc:Arg5 Output cost.
  //Doc:Arg6 Area pointer.

  //Doc:Method
  ~OSPFInterface (void); // Destructor
  //Doc:Desc Destroys a OSPF interface instance.

  //Doc:Method
  void Handle (Event*, Time_t); // Handler methods
  //Doc:Desc The OSPFInterface class is a subclass of {\tt Handler}, 
  //Doc:Desc the OSPFInterface object must handle events
  //Doc:Arg1 A pointer to the event being handled.
  //Doc:Arg2 The current simulation time.

  //Doc:Method
  static bool IsInterface (Node*, IPAddr_t); // Search a interface with specific IP address
  //Doc:Desc This method lookups interface with specific IP address
  //Doc:Arg1 Attached node.
  //Doc:Arg2 Interface IP address.
  //Doc:Return If the interface exist.

  //Doc:Method
  Node* GetNode (void) const; // Get node pointer
  //Doc:Desc This method is used to get node.
  //Doc:Return Node pointer.

  //Doc:Method
  OSPFRouter_t GetRouterID (void) const; // Get Router ID
  //Doc:Desc This method returns Router ID.
  //Doc:Return Router ID.

  //Doc:Method
  OSPFArea* GetArea (void) const { return (area); }
  //Doc:Desc Get area pointer.

  //Doc:Method
  OSPFArea_t GetAreaID (void) const { return (aid); }
  //Doc:Desc Get area ID.

  //Doc:Method
  bool IsStub (void) const;
  //Doc:Desc This method gets extRoutingCapability.

  //Doc:Method
  Bitmap_t GetOption (void) const;
  //Doc:Desc This method gets option
  //Doc:Return Option.

  //Doc:Method
  void InstallLSA (OSPFLSAHeader*);
  //Doc:Desc Add a LSA into LSA lists.
  //Doc:Arg1 LSA pointer.

  //Doc:Method
  OSPFLSAHeader* FindLSA (OSPFLSAHeader*);
  //Doc:Desc Find a LSA in LSA lists.
  //Doc:Arg1 LSA header pointer.
  //Doc:Return Instance in LSA lists.

  //Doc:Method
  OSPFLSAHeader* FindLSA (OSPFRequest*);
  //Doc:Desc Find a LSA in LSA lists.
  //Doc:Arg1 LSA request pointer.
  //Doc:Return Instance in LSA lists.

  //Doc:Method
  OSPFNetwork_t GetType (void) const { return (type); }
  //Doc:Desc Get interface type.
  //Doc:Return Interface type.

  //Doc:Method
  bool IsDown (void) const { return (status == DOWN); }
  //Doc:Desc This method gets status 
  //Doc:Return If Down.

  //Doc:Method
  bool IsLoopback (void) const { return (status == LOOPBACK); }
  //Doc:Desc This method gets status 
  //Doc:Return If loopback.

  //Doc:Method
  bool IsP2P (void) const { return (status == P2P); }
  //Doc:Desc This method gets status 
  //Doc:Return If point to point.

  //Doc:Method
  bool IsOther (void) const { return (status == DROTHER); }
  //Doc:Desc This method gets status 
  //Doc:Return If DROTHER.

  //Doc:Method
  bool IsDR (void) const { return (status == DR); }
  //Doc:Desc This method gets status 
  //Doc:Return If designated.

  //Doc:Method
  bool IsBDR (void) const { return (status == BDR); }
  //Doc:Desc This method gets status 
  //Doc:Return If backup designated.

  //Doc:Method
  IPAddr_t GetAddr (void) const { return (addr); }
  //Doc:Desc This method returns IP address.
  //Doc:Return IP address.

  //Doc:Method
  Mask_t GetMask (void) const { return (mask); }
  //Doc:Desc This method returns mask.
  //Doc:Return Mask.

  //Doc:Method
  Time_t GetHelloInterval (void) const { return (helloInterval); }
  //Doc:Desc This method returns router hello internal.
  //Doc:Return Hello interval.

  //Doc:Method
  Time_t GetDeadInterval (void) const { return (deadInterval); }
  //Doc:Desc This method returns router dead internal.
  //Doc:Return Router dead interval.

  //Doc:Method
  Time_t GetDelay (void) const { return (delay); }
  //Doc:Desc This method returns transmit delay.

  //Doc:Method
  Byte_t GetPriority (void) const { return (priority); }
  //Doc:Desc This method returns priority.

  //Doc:Method
  void TriggerEvent (Event_t); // Trigger event immediately
  //Doc:Desc This method triggers an event immediately.
  //Doc:Arg1 Event type.

  //Doc:Method
  void ClearHello (void);
  //Doc:Desc This method cancels and deletes hello event

  //Doc:Method
  void TriggerHello (void);
  //Doc:Desc This method triggers a hello event in helloInterval

  //Doc:Method
  void ClearWait (void);
  //Doc:Desc This method cancels and deletes wait event

  //Doc:Method
  void TriggerWait (void);
  //Doc:Desc This method triggers a wait event in deadInterval

  //Doc:Method
  OSPFNeighbor* AddNeighbor (IPAddr_t, OSPFHello*);
  //Doc:Desc This method add a neighbor for interface
  //Doc:Arg1 IP address.
  //Doc:Arg2 Hello packet.
  //Doc:Return Neighbor pointer.

  //Doc:Method
  OSPFNeighbor* FindNeighbor (IPAddr_t, OSPFRouter_t);
  //Doc:Desc This method search a neighbor in interface
  //Doc:Arg1 Source IP address.
  //Doc:Arg2 Router ID.
  //Doc:Return Neighbor pointer.

  //Doc:Method
  OSPFNeighbor* GetPeer (void) const;
  //Doc:Desc This method gets perr for P2P and VLINK router.
  //Doc:Return Neighbor pointer.

  //Doc:Method
  IPAddr_t GetDR (void) const { return (designated); }
  //Doc:Desc This method gets designated router for broadcast and NBMA.
  //Doc:Return Designated router IP addr

  //Doc:Method
  IPAddr_t GetBDR (void) const { return (backup); }
  //Doc:Desc This method gets backup designated router for broadcast and NBMA.
  //Doc:Return Backup designated router IP addr

  //Doc:Method
  OSPFNeighbor* GetDesignated (void);
  //Doc:Desc This method gets designated router for broadcast and NBMA.
  //Doc:Return Neighbor pointer.

  //Doc:Method
  bool FindNeighbor (void);
  //Doc:Desc Find neighbors in EXCHANGE or LOADING status.

  //Doc:Method
  void ClearNeighbors (void);
  //Doc:Desc This method deletes all neighbors

  //Doc:Method
  OSPFNeighbors& GetNeighbors (void) { return (neighbors); } 
  //Doc:Desc Get neighbor list.

  //Doc:Method
  OSPFCost_t GetCost (void) const { return (cost); }
  //Doc:Desc This method returns output cost.
  //Doc:Return Cost.

  //Doc:Method
  Time_t GetRxmtInterval (void) const { return (rxmtInterval); }
  //Doc:Desc This method returns rxmt interval.
  //Doc:Return Rxmt interval.

  //Doc:Method
  Word_t GetAType(void) const { return (atype); }
  //Doc:Desc This method returns Authentication type.
  //Doc:Return Authentication type.

  //Doc:Method
#ifdef HAVE_QT
  void SendToAdjacency (MyColor, OSPFPacket*);
#else
  void SendToAdjacency (OSPFPacket*);
#endif
  //Doc:Desc Send a packet out.
  //Doc:Arg1 Packet pointer.

  //Doc:Method
  void SendHello (void);
  //Doc:Desc This method sends hello packet and triggers next hello event

  //Doc:Method
  void ReceiveHello (IPAddr_t, OSPFHello*);
  //Doc:Desc This method receives hello packet
  //Doc:Arg1 Source IP address.
  //Doc:Arg2 Hello packet pointer.

  //Doc:Method
  bool IsAdjacency (IPAddr_t) const;
  //Doc:Desc This method checks adjcency.
  //Doc:Return Neighbor IP address.
  //Doc:Return If adjacency.

  //Doc:Method
  void SelectRouter (void);
  //Doc:Desc This method selects DR and BDR.

  //Doc:Method
  Size_t GetMTU (void) const { return (mtu); }
  //Doc:Desc This method returns MTU.
  //Doc:Return MTU.

  //Doc:Method
  bool FloodOut (OSPFLSAHeader*, OSPFNeighbor*);
  //Doc:Desc Flood a LSA.
  //Doc:Arg1 LSA pointer.
  //Doc:Arg2 Sender (neighbor) pointer.
  //Doc:Return If send back to receiving interface.

  //Doc:Method
  bool ReceiveLSA (OSPFNeighbor*, OSPFLSAHeader*);
  //Doc:Desc Receive a flooded LSA.
  //Doc:Arg1 Sender (neighbor) pointer.
  //Doc:Arg2 Received LSA.
  //Doc:Return If flood the LSA from this interface

  //Doc:Method
  void EraseNoacked (OSPFLSAHeader*);
  //Doc:Erase a LSA from all neighbors' retransmit list.
  //Doc:Arg1 LSA pointer.

  //Doc:Method
  void ClearRxmt (void);
  //Doc:Desc This method cancels and deletes rxmt event

  //Doc:Method
  void TriggerRxmt (void);
  //Doc:Desc This method triggers a rxmt event in deadInterval

  //Doc:Method
  void ClearAcks (void);
  //Doc:Desc This method deletes all acknowledge LSA.

  //Doc:Method
  void AddAck (OSPFLSAHeader*);
  //Doc:Desc This method adds a LSA into acknowledge list.

  //Doc:Method
  void SendAck (void);
  //Doc:Desc This method sends delay acks.

  void debug (void);

private:
  // Interface data structures
  OSPFNetwork_t type; // Network type
  Status_t status; // Interface status
  IPAddr_t addr; // Interface IP address (unnumbered P2P does not have IP address)
  Mask_t mask; // Interface subnet mask (not defined for P2P and VLINK)
  OSPFArea_t aid; // Area ID
  Time_t helloInterval; // Hello packet sending interval
  Time_t deadInterval; // Router dead interval
  Time_t delay; // Infomation transmit delay
  Byte_t priority; // Router priority
  Event* helloEvent; // Hello timer
  Event* waitEvent; // Wait timer
  OSPFNeighbors neighbors; // Neighbor routers
  IPAddr_t designated; // Designated Router
  IPAddr_t backup; // Backup Router
  OSPFCost_t cost; // Output cost
  Time_t rxmtInterval; // Retransmit interval
  Word_t atype; // Authentication type
  Long_t akey[2]; // Authenication key
  // Internal data structures
  OSPFArea* area; // Attached area
  Size_t mtu; // MTU
  IPAddr_t outaddr; // Output IP address
  Event* rxmtEvent; // Ack rxmt event
  OSPFLSAHeaders acks; // Acknowledge list
};

typedef std::vector<OSPFInterface*> OSPFInterfaces; // Interfaces
typedef OSPFInterfaces::iterator OSPFInterfacesIt; // Interfaces iterator
typedef OSPFInterfaces::const_iterator OSPFInterfacesConstIt; // Interfaces iterator

//Doc:ClassXRef
class OSPFVertex {
//Doc:Class Class OSPFVertex defines the shorest path tree

friend class OSPFArea;

public:
  //Doc:Method
  OSPFVertex (void) {} // Constructor
  //Doc:Desc Constructs a new shorest path tree.

  //Doc:Method
  OSPFVertex (OSPFLSA_t type, OSPFID_t vid)
  : type (type), vid (vid), cost (0), out (IPADDR_NONE), hop (IPADDR_NONE) {} // Constructor 
  //Doc:Desc Constructs a new shorest path tree.
  //Doc:Arg1 Vertex type.
  //Doc:Arg2 Vectex ID.

  //Doc:Method
  OSPFVertex (OSPFVertex*, OSPFLink*);
  //Doc:Desc Constructs a new shorest path tree.
  //Doc:Arg1 Parent node.
  //Doc:Arg2 Link.

  //Doc:Method
  ~OSPFVertex (void) {} // Destructor
  //Doc:Desc Destroys a shorest path tree.

  //Doc:Method
  bool operator < (const OSPFVertex& vertex) const;
  //Doc:Desc Define "<" operator.

  //Doc:Method
  bool operator () (OSPFVertex* v1, OSPFVertex* v2) { return (*v1 < *v2); }
  //Doc:Desc Define "()" operator.

private:
  OSPFLSA_t type; // Vertex type
  OSPFID_t vid; // Vertex ID
  OSPFCost_t cost; // Cost
  IPAddr_t out; // Output interface address
  IPAddr_t hop; // Next hop
};

typedef std::vector<OSPFVertex*> OSPFVertexes; // Vertexes
typedef OSPFVertexes::iterator OSPFVertexesIt; // Vertexes iterator
typedef OSPFVertexes::const_iterator OSPFVertexesConstIt; // Vertexes iterator

//Doc:ClassXRef
class OSPFHop {
//Doc:Class Class OSPFHop defines the next hop

public:
  //Doc:Method
  OSPFHop (IPAddr_t out, IPAddr_t next = IPADDR_NONE, OSPFRouter_t rid = IPADDR_NONE)
  : out (out), next (next), rid (rid) {}
  //Doc:Desc Constructs a new OSPF hop instance.
  //Doc:Arg1 Output interface address.
  //Doc:Arg2 Destination address.
  //Doc:Arg3 Advertising router ID.

  //Doc:Method
  ~OSPFHop (void) {}
  //Doc:Desc Destroy an OSPF hop instance.

  //Doc:Method
  IPAddr_t GetOutput (void) { return (out); }
  //Doc:Desc Get output interface address

  //Doc:Method
  IPAddr_t GetNext (void) { return (next); }
  //Doc:Desc Destination address

  //Doc:Method
  OSPFRouter_t GetAdvID (void) { return (rid); }
  //Doc:Desc Destination address

  void dump (void) { std::cout << (std::string)IPAddr(out); }

private:
  IPAddr_t out; // Output interface address
  IPAddr_t next; // Next hop address
  OSPFRouter_t rid; // Advertising router ID
};

typedef std::vector<OSPFHop*> OSPFHops; // Next hop list
typedef OSPFHops::iterator OSPFHopsIt; // Next hop list iterator
typedef OSPFHops::const_iterator OSPFHopsConstIt; // Next hop list iterator

//Doc:ClassXRef
class OSPFRouting {
//Doc:Class Class OSPFRouting defines the routing table item

public:
  // Enumerate the path type
  typedef enum { INTRA, INTER, EXTER1, EXTER2 } Path_t;

  //Doc:Method
  OSPFRouting (OSPFLSA_t, OSPFID_t, Mask_t, Bitmap_t, OSPFArea_t, Path_t, OSPFCost_t);
  //Doc:Desc Constructs a new OSPF routing instance.
  //Doc:Arg1 Type
  //Doc:Arg2 ID
  //Doc:Arg3 Mask
  //Doc:Arg4 Option
  //Doc:Arg5 Area ID
  //Doc:Arg6 Path type
  //Doc:Arg7 Cost

  //Doc:Method
  ~OSPFRouting (void); // Destructor
  //Doc:Desc Destroys a OSPF routing instance.

  //Doc:Method
  bool IsIt (OSPFRouting* entry);
  //Doc:Desc Find a routing entry.
  //Doc:Return If the routing entry.

  //Doc:Method
  OSPFCost_t GetCost (void) { return (cost); }
  //Doc:Desc Get cost.

  //Doc:Method
  void AddNexthop (IPAddr_t out, IPAddr_t next = IPADDR_NONE, OSPFRouter_t rid = IPADDR_NONE);
  //Doc:Desc Add a equal cost next hop
  //Doc:Arg1 Output interface address.
  //Doc:Arg2 Destination address.
  //Doc:Arg3 Advertising router ID.

  //Doc:Method
  void dump (void);
  //Doc:Desc Dump the entry

private:
  OSPFLSA_t type; // Destination type
  OSPFID_t id; // Destination ID
  Mask_t mask; // Address mask
  Bitmap_t option; // Option capabilities
  OSPFArea_t aid; // Area ID
  Path_t path; // Path type
  OSPFCost_t cost; // Cost for intra, inter and exter2 path
  OSPFCost_t cost2; // Cost for external 2 path.
  OSPFLSAHeader* origin; // Link state origin
  OSPFHops hops; // Next hop list
};

typedef std::vector<OSPFRouting*> OSPFRoutings; // Routing table
typedef OSPFRoutings::iterator OSPFRoutingsIt; // Routing table iterator
typedef OSPFRoutings::const_iterator OSPFRoutingsConstIt; // Routing table iterator

typedef std::priority_queue<OSPFVertex*, std::vector<OSPFVertex*>, OSPFVertex> OSPFVertexQueue; // Vertex priority queue

class OSPF;

//Doc:ClassXRef
class OSPFArea {
//Doc:Class Class OSPFArea defines the area that the {\tt OSPF} 
//Doc:Class protocol should handle.

public:
  //Doc:Method
  OSPFArea (OSPFArea_t, OSPF*); // Constructor
  //Doc:Arg1 Area ID.
  //Doc:Arg2 OSPF pointer.

  //Doc:Method
  virtual ~OSPFArea (void); // Destructor
  //Doc:Desc Destroys a OSPF area instance.

  //Doc:Method
  Node* GetNode (void) const; // Get node pointer
  //Doc:Desc This method is used to get attached node.
  //Doc:Return Node pointer.

  //Doc:Method
  OSPFRouter_t GetRouterID (void) const; // Get Router ID
  //Doc:Desc This method returns Router ID.
  //Doc:Return Router ID.

  //Doc:Method
  Bitmap_t GetRouterType (void) const;
  //Doc:Desc This method gets router type (V/E/B).
  //Doc:Return Router type.

  //Doc:Method
  OSPFArea_t GetAreaID (void) const { return (aid); } // Get Area ID
  //Doc:Desc This method return Area ID.
  //Doc:Return AreaID.

  //Doc:Method
  void ClearRanges (void);
  //Doc:Desc Clear range list.

  //Doc:Method
  void ChangeInterface (int add) { number += add; }
  //Doc:Desc Add/Remove active interface.

  //Doc:Method
  OSPFInterface* AddInterface (OSPFNetwork_t, IPAddr_t, Mask_t, Byte_t, OSPFCost_t);
  //Doc:Desc This method is used to add interface into area.
  //Doc:Arg1 Interface type.
  //Doc:Arg2 Interface IP address.
  //Doc:Arg3 Subnet mask.
  //Doc:Arg4 Priority.
  //Doc:Arg5 Output cost.
  //Doc:Return Interface instance.

  //Doc:Method
  OSPFInterface* FindInterface (IPAddr_t); // Find the interface with specific IP address
  //Doc:Desc This method is used to find interface with specific IP address.
  //Doc:Arg1 Interface IP address.
  //Doc:Return Interface pointer.

  //Doc:Method
  void ClearInterfaces (void);
  //Doc:Desc Clear interface list

  //Doc:Method
  OSPFInterfaces& GetInterfaces (void) { return (ifaces); }
  //Doc:Desc Get interface list.

  //Doc:Method
  void AddRouterLSA (OSPFLSARouter*);
  //Doc:Desc Add/Replace a LSA in Router-LSA list.

  //Doc:Method
  OSPFLSARouter* FindRouterLSA (OSPFRouter_t);
  //Doc:Desc Find a Router-LSA in LSAs.

  //Doc:Method
  void EraseRouterLSA (OSPFRouter_t);
  //Doc:Desc Remove a Router-LSA in LSAs.

  //Doc:Method
  void FlushRouterLSA (void);
  //Doc:Desc Flush local Router-LSA.

  //Doc:Method
  void ClearRouterLSAs (void);
  //Doc:Desc Clear Router-LSAs

  //Doc:Method
  OSPFLSARouters& GetRouterLSAs (void) { return (rlsas); }
  //Doc:Desc Get Router-LSA list.

  //Doc:Method
  void ClearNetworkLSAs (void);
  //Doc:Desc Clear Network-LSAs

  //Doc:Method
  void ClearSummaryLSAs (void);
  //Doc:Desc Clear Summary-LSAs

  //Doc:Method
  void InstallLSA (OSPFLSAHeader*);
  //Doc:Desc Add a LSA into LSA lists.
  //Doc:Arg1 LSA pointer.

  //Doc:Method
  OSPFLSAHeader* FindLSA (OSPFLSA_t, OSPFID_t, OSPFRouter_t rid);
  //Doc:Desc Find a LSA in LSA lists.
  //Doc:Arg1 LSA type.
  //Doc:Arg2 LSA ID.
  //Doc:Arg3 Advertising router ID.
  //Doc:Return Instance in LSA lists.

  //Doc:Method
  static bool IsOnTree (OSPFVertexes&, OSPFVertex*);
  //Doc:Desc Find a vertex on shorest path tree.
  //Doc:Arg1 Shortest tree.
  //Doc:Arg2 A vertex.

  //Doc:Method
  static void ClearTree (OSPFVertexes&);
  //Doc:Desc Clear shorest path tree.
  //Doc:Arg1 Shortest tree.

  //Doc:Method
  static void DumpTree (OSPFVertexes&);
  //Doc:Desc Dump shorest path tree.
  //Doc:Arg1 Shortest tree.

  //Doc:Method
  void BuildTree (void);
  //Doc:Desc Calculate shorest path tree.

  //Doc:Method
  bool IsStub (void) const { return (extRoutingCapability == false); }
  //Doc:Desc This method gets extRoutingCapability.

  //Doc:Method
  Bitmap_t GetOption (void) const;
  //Doc:Desc This method gets option.
  //Doc:Return Option.

  //Doc:Method
  bool FloodOut (OSPFLSAHeader*, OSPFInterface* iface = nil, OSPFNeighbor* neighbor = nil);
  //Doc:Desc Flood a LSA.
  //Doc:Arg1 LSA pointer.
  //Doc:Arg2 Receiving interface pointer.
  //Doc:Arg3 Sender (neighbor) pointer.
  //Doc:Return If send back to receiving interface.

  //Doc:Method
  void IncAge (void);
  //Doc:Desc This method does the one second processing.

  //Doc:Method
  void EraseNoacked (OSPFLSAHeader*);
  //Doc:Desc Clear a old instance form retransmitting list.
  //Doc:Arg1 Old instance.

  //Doc:Method
  void ClearMapping (void) { mappingCount = 0; mapping.clear (); }
  //Doc:Desc Erase interface mapping.

  //Doc:Method
  void AddMapping (IPAddr_t addr) { mappingCount++; mapping.push_back (addr); }
  //Doc:Desc Add a interface mapping item.
  //Doc:Arg1 Interface address.

  //Doc:Method
  IPAddr_t GetMapping (Count_t index) { if (index <= mappingCount) { return (mapping[index]); } else { return (IPADDR_NONE); } }
  //Doc:Desc Get a interface mapping item.
  //Doc:Arg1 Mapping index.

  void debug (void);

protected:
  // OSPF area data structures
  OSPFArea_t aid; // Area ID;
  OSPFRanges ranges; // Area boundaries
  OSPFInterfaces ifaces; // Interfaces
  OSPFLSARouters rlsas; // Router LSAs
  OSPFLSANetworks nlsas; // Network LSAs
  OSPFLSASummaries slsas; // Summary LSAs 
  bool transitCapability; // TransitCapability. (calculated)
  bool extRoutingCapability; // ExternalRoutingCapability. (if false, stub area)
  OSPFCost_t stubDefaultCost; // StubDefaultCost

private:
  // Internal data structures
  OSPF* ospf; // OSPF object
  Word_t number; // Active interface number  
  Count_t mappingCount; // Interface mapping count
  OSPFIPs mapping; // Interface address to STUB network mapping
};

typedef std::vector<OSPFArea*> OSPFAreas; // Area list
typedef OSPFAreas::iterator OSPFAreasIt; // Area list iterator
typedef OSPFAreas::const_iterator OSPFAreasConstIt; // Area list iterator

//Doc:ClassXRef
class OSPFBackbone : public OSPFArea {
//Doc:Class Class OSPFBackbone defines the backbone that the router attached

public:
  //Doc:Method
  OSPFBackbone (OSPF* ospf) 
  : OSPFArea (OSPFBACKBONE, ospf) {} // Constructor
  //Doc:Arg1 OSPF pointer.

  //Doc:Method
  ~OSPFBackbone (void) {} // Destructor
  //Doc:Desc Destroys a OSPF backbone area instance.

  //Doc:Method
  bool ExistVLink (void);
  //Doc:Desc This method is used to check if a virtual link exists.
  //Doc:Return If a virtual link exists.
};

//Doc:ClassXRef
class OSPFEXRouter {
//Doc:Class Class OSPFEXRouter defines the external router

public:
  //Doc:Method
  OSPFEXRouter (void) {} // Constructor
  //Doc:Desc Constructs a new exteranl router instance.

  //Doc:Method
  ~OSPFEXRouter (void) {} // Destructor
  //Doc:Desc Destroys an exteranl router instance.
};

typedef std::vector<OSPFEXRouter*> OSPFEXRouters; // External router list
typedef OSPFEXRouters::iterator OSPFEXRoutersIt; // External router list iterator
typedef OSPFEXRouters::const_iterator OSPFEXRoutersConstIt; // External router list iterator

//Doc:ClassXRef
class OSPF : public Handler, public L4Demux {
//Doc:Class Class OSPF is conceptually a layer 4 protocol, but since its not
//Doc:Class designed to support layer 5 applications, we don't subclass
//Doc:Class {\tt L4Protocol} here, since most of the {\tt L4Protocol} api is
//Doc:Class designed to support applications.

public:
  // Enumerate the OSPF status
  typedef enum { ENABLED, DISABLED } Status_t;

  // Enumerate the OSPF event types
  typedef enum { START, STOP, TIMER } Event_t;

  //Doc:Method
  OSPF (OSPFRouter_t); // Constructor with args
  //Doc:Desc Constructs a new OSPF protocol instance with args.
  //Doc:Arg1 Router ID.

  //Doc:Method
  ~OSPF (void); // Destructor
  //Doc:Desc Destroys a OSPF protocol instance.

  //Doc:Method
  void AttachNode (Node*); // Attach this protocol to a node
  //Doc:Desc Specify which node to which this protocol is attached.
  //Doc:Arg1 Node pointer to attached node.

  //Doc:Method
  void Handle (Event*, Time_t); // Handler methods
  //Doc:Desc The OSPF class is a subclass of {\tt Handler}, the OSPF object
  //Doc:Desc must handle events.
  //Doc:Arg1 A pointer to the event being handled.
  //Doc:Arg2 The current simulation time.

  //Doc:Method
  void DataIndication (Node*, Packet*, IPAddr_t, L3Protocol&, Interface*); // Upcalls from L3 protocol, via L4Demux
  //Doc:Desc This method is used to pass on a packet from any layer 3 protocol
  //Doc:Desc up the stack.
  //Doc:Arg1 A pointer to the node.
  //Doc:Arg2 A pointer to the packet that was received
  //Doc:Arg3 Srouce IP address.
  //Doc:Arg4 L3 protical.
  //Doc:Arg5 A pointer to the interface at which this packet is received

  //Doc:Method
  Layer_t Layer (void) const { return (OSPFLAYER); } // Determine which protocol layer
  //Doc:Desc This method is used to get protocol layer number
  //Doc:Return {\tt OSPF} protocol layer number

  //Doc:Method
  OSPFRouter_t GetRouterID (void) const { return (rid); } // Get Router ID
  //Doc:Desc This method is used to get Router ID.
  //Doc:Return Router ID.

  //Doc:Method
  OSPFArea* AddArea (OSPFArea_t);
  //Doc:Desc This method is used to add an area.
  //Doc:Arg1 Area ID.
  //Doc:Return Area instance pointer.

  //Doc:Method
  OSPFArea* FindArea (OSPFArea_t); // Find the area with specific Area ID
  //Doc:Desc This method is used to find area with specific Area ID.
  //Doc:Arg1 Area ID.
  //Doc:Return Area pointer

  //Doc:Method
  OSPFBackbone* FindBackbone (void) { return ((OSPFBackbone*)FindArea (OSPFBACKBONE)); }
  //Doc:Desc This method is used to find backbone area.
  //Doc:Return Backbone area pointer.

  //Doc:Method
  OSPFInterface* FindInterface (IPAddr_t); // Find the interface with specific IP address
  //Doc:Desc This method is used to find interface with specific IP address.
  //Doc:Arg1 Interface IP address.
  //Doc:Return Interface pointer.

  //Doc:Method
  bool IsBorder (void) const { return (areas.size () > 1); }
  //Doc:Desc if area board router.
 
  //Doc:Method
  bool IsBoundary (void) const { return (boundary); }
  //Doc:Desc if AS boundary router.
 
  //Doc:Method
  bool IsVLinkEnd (void);
  //Doc:Desc if virtual link endpoint.
 
  //Doc:Method
  Node* GetNode (void) const { return (node); } // Get node pointer
  //Doc:Desc This method is used to get node.
  //Doc:Return Node pointer.

  //Doc:Method
  OSPFInterface* CheckPacket (IPAddr_t, IPAddr_t, IPAddr_t);
  //Doc:Desc This method is used check OSPF packet
  //Doc:Arg1 Source IP address.
  //Doc:Arg2 Destination IP address.
  //Doc:Arg3 Interface IP address.
  //Doc:Return Receiving interface pointer.
  
  //Doc:Method
  void ClearTimer (void);
  //Doc:Desc This method cancels and deletes timer event.

  //Doc:Method
  void TriggerTimer (void);
  //Doc:Desc This method triggers a timer event in one second.

  //Doc:Method
  void IncAge (void);
  //Doc:Desc This method does the one second processing.

  //Doc:Method
  void IncASExtAge (void);
  //Doc:Desc This method does the one second processing for ASExt LSAs.

  //Doc:Method
  void ClearRouting (void);
  //Doc:Desc Clear routing table.
  
  //Doc:Method
  void AddRouting (OSPFLSA_t, OSPFID_t, Mask_t, Bitmap_t, OSPFArea_t, OSPFRouting::Path_t, OSPFCost_t, IPAddr_t, bool);
  //Doc:Desc Add an entry into routing table.
  //Doc:Arg1 Type
  //Doc:Arg2 ID
  //Doc:Arg3 Mask
  //Doc:Arg4 Option
  //Doc:Arg5 Area ID
  //Doc:Arg6 Path type
  //Doc:Arg7 Cost
  //Doc:Arg8 Output interface address
  //Doc:Arg9 If stub
  
  //Doc:Method
  void DumpRouting (void);
  //Doc:Desc Dump routing table

  void debug (void) {}

private:
  // OSPF top level data structures
  OSPFRouter_t rid; // Router ID
  OSPFAreas areas; // Areas
  OSPFEXRouters exrouters; // External routers
  OSPFLSAASExts asexts; // AS-external-LSAs
  OSPFRoutings table; // Routing table
  // Internal data structures
  Node* node; // Attached node
  Status_t status; // Status
  bool boundary; // AS boundary router
  Event* timerEvent; // One second event
};


//Doc:ClassXRef
class OSPFDataD : public OSPFPacket {
//Doc:Class Class OSPFDataD define the data description packet info for {\tt OSPF}.
//Doc:Class Class OSPFDataD is a sub-class of {\tt OSPFPacket}.

public:
  static const Bitmap_t IBIT = ((Bitmap_t)(1<<5)); // Initial
  static const Bitmap_t MBIT = ((Bitmap_t)(1<<6)); // More
  static const Bitmap_t MSBIT = ((Bitmap_t)(1<<7)); // Master

  //Doc:Method
  OSPFDataD (Seq_t, OSPFInterface*); // Constructor with args
  //Doc:Desc Constructs a new OSPF data description packet instance with args.
  //Doc:Arg1 Sequence.
  //Doc:Arg2 Interface.

  //Doc:Method
  OSPFDataD (OSPFNeighbor*); // Constructor with args
  //Doc:Desc Constructs a new OSPF data description packet instance with args.
  //Doc:Arg1 Neighbor.

  //Doc:Method
  OSPFDataD (const OSPFDataD&);
  //Doc:Desc Constructs a new OSPF data description packet instance from existing instance.

  //Doc:Method
  ~OSPFDataD (void); // Destructor
  //Doc:Desc Destroys a OSPF data description packet instance.

  //Doc:Method
  Size_t Size (void) const { return (length); } // OSPF data description packet size
  //Doc:Desc This method returns the size of the PDU
  //Doc:Return the size of the PDU

  //Doc:Method
  OSPFPacket* Copy () const { return (new OSPFDataD (*this)); } // Create a copy of this PDU
  //Doc:Desc This method creates a copy of this PDU
  //Doc:Return A pointer to the copy of this PDU

  //Doc:Method
  bool IsInit (void) const { return ((imms & IBIT) != 0); }
  //Doc::Desc If initial packet.

  //Doc:Method
  bool IsMore (void) const { return ((imms & MBIT) != 0); }
  //Doc::Desc If has more packet.

  //Doc:Method
  bool IsMaster (void) const { return ((imms & MSBIT) != 0); }
  //Doc::Desc If declares master.

  //Doc:Method
  Bitmap_t GetIMMS (void) const { return (imms); }
  //Doc:Desc Get imms.

  //Doc:Method
  Size_t GetMTU (void) const { return (mtu); }
  //Doc:Desc Get MTU.

  //Doc:Method
  Bitmap_t GetOption (void) const { return (option); }
  //Doc:Desc Get option.

  //Doc:Method
  Seq_t GetSequence (void) const { return (sequence); }
  //Doc:Desc Get sequence number.

  //Doc:Method
  OSPFLSAHeaders& GetHeaders (void) { return (headers); }
  //Doc::Desc Get LSA list.

  void dump (void);

private:
  Size_t mtu; // Interface MTU
  Bitmap_t option; // Option capability
  Bitmap_t imms; // I/M/MS bits
  Seq_t sequence; // DD sequence number
  OSPFLSAHeaders headers; // LSA headers
};


//Doc:ClassXRef
class OSPFLSReq : public OSPFPacket {
//Doc:Class Class OSPFLSReq define the link state request packet info for {\tt OSPF}.
//Doc:Class Class OSPFLSReq is a sub-class of {\tt OSPFPacket}.

public:
  //Doc:Method
  OSPFLSReq (OSPFNeighbor*);  // Constructor
  //Doc:Desc Constructs a new OSPF link state request packet instance.
  //Doc:Arg1 Neighbor pointer.

  //Doc:Method
  OSPFLSReq (const OSPFLSReq&);
  //Doc:Desc Constructs a new OSPF link state request packet instance from existing instance.

  //Doc:Method
  ~OSPFLSReq (void); // Destructor
  //Doc:Desc Destroys a OSPF link state request packet instance.

  //Doc:Method
  Size_t Size (void) const { return (length); } // OSPF link state request packet size
  //Doc:Desc This method returns the size of the PDU
  //Doc:Return the size of the PDU

  //Doc:Method
  OSPFPacket* Copy() const { return (new OSPFLSReq (*this)); } // Create a copy of this packet
  //Doc:Desc This method creates a copy of this PDU
  //Doc:Return A pointer to the copy of this PDU

  //Doc:Method
  OSPFRequests& GetRequests (void) { return (requests); }
  //Doc::Desc Get request list.

  void dump (void);

private:
  OSPFRequests requests; // Request list
};


#endif /* __OSPF_H__ */

