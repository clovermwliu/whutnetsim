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




// Georgia Tech Network Simulator - NixVector Routing class
// George F. Riley.  Georgia Tech, Spring 2002

// NixVector routing method

//2010.01.04 modified by 张笑盈

#ifndef __routingnixvector_h__
#define __routingnixvector_h__

#include <iostream>


#include "routing.h"
//#include "ipv4.h"
#include "node.h"
#include "interface.h"
#include "link.h"
#include "bfs.h"

#include "hex.h"
#include "G_globalstats.h"
#include "G_debug.h"

using namespace std;

// NixVector routing is described in two research papers:
//    "Stateless Routing in Network Simulations", MASCOTS 2000
//    "Efficient Routing using NixVectors", HPSR 2001
// both by George F. Riley

typedef unsigned long NixBits_t;             // Use 32 bit values for nix bits
// NIX_NONE is used to specify NONE
#define NIX_NONE (NixBits_t)0xffffffff
#define NIX_BPW  32

//Doc:ClassXRef
class NixVectorOption : public PDU {   // Nix vector for routing
/*
从PDU类中派生出来，为nix vextor routing定义协议头
*/
//Doc:Class The class {\tt NixVectorOption } derives from the class {\tt PDU}
//Doc:Class and defines a protocol header for the nix vextor routing
//Doc:Class For Nix Vector refer to the description of class
//Doc:Class {\tt RoutingNixVector }
	
public:
  typedef enum {Number = 24} NVOpt_t;
public:
  //Doc:Method  
  NixVectorOption();
  //Doc:Desc The Default Constructor
  
  //Doc:Method    
  NixVectorOption(const NixVectorOption&);   // Copy constructor
  //Doc:Desc This constructor takes a reference to a NixVectorOption and
  //Doc:Desc instantiates itself.

  //Doc:Method    
  NixVectorOption(char*, Size_t&);           // Construct from serial buff
  //Doc:Desc This method contructs a NixVector PDU from a serialized buffer
  //Doc:Desc of a given size.
  
  //Doc:Method    
  virtual ~NixVectorOption();                // Destructor
  //Doc:Desc The desctructor
  
  //  //Doc:Method    
  NixVectorOption& operator=(const NixVectorOption& s); // Assignment operator
  //  //Doc:Desc This method overloads the assignment operator
  //  //Doc:Return Reference to the PDU

  //Doc:Method    
  Size_t   Size() const;                     // Required for all PDU's
  //Doc:Desc This method returns the size of the PDU
  //Doc:Return size of the pdu
public:

  //Doc:Method  
  PDU*      Copy() const;                     // Make a copy of this NixVector
  //描述：复制PDU并返回其指针
  //Doc:Desc  This method creates a copy of this PDU
  //Doc:Return A pointer to the copy of this PDU
  
  // Serialization

  //Doc:Method    
  Size_t    SSize();                         // Size needed for serialization
  //Doc:Desc The actual size of all the PDU contents in terms of bytes for
  //Doc:Desc serialization of the contents.
  //Doc:Return the size  

  //Doc:Method    
  char*     Serialize(char*, Size_t&);       // Serialize to a buffer
  //Doc:Desc This method is used to serialize the contents of the PDU  
  //Doc:Desc into a serialized character buffer b . the size of this buffer is 
  //Doc:Desc in the size arguement
  //Doc:Arg1 This is the pointer to the character buffer
  //Doc:Arg2 a reference to the size variable
  //Doc:Return the pointer to the updated character buffer
  
  //Doc:Method    
  char*     Construct(char*, Size_t&);       // Construct from buffer
  //Doc:Desc This method is the reverse of the Serialize . It constructs the
  //Doc:Desc PDU object from the character buffer that has been received over
  //Doc:Desc the network
  //Doc:Arg1 This is the pointer to the character buffer
  //Doc:Arg2 a reference to the size variable
  //Doc:Return the pointer to the character buffer of the remaining data
  
  //Doc:Method    
  void      Add(NixBits_t, Count_t);         // Add bits
  //Doc:Desc This method adds the n bits specified by the Nix_Bits bitmap
  //Doc:Desc The bits are right justified
  //Doc:Arg1 The bitmap
  //Doc:Arg2 The number of bits
  
  //Doc:Method    
  NixBits_t Extract(Count_t);                // Extract bits
  //Doc:Desc This method extracts the the number of bits specified by the
  //Doc:Desc arguement
  //Doc:Desc The extracted bitmap
  
  //Doc:Method    
  Count_t   Remaining();                     // Remaining unused buts
  //Doc:Desc This method returns the number of remaining bits
  //Doc:Return number of bits remaining

  //Doc:Method    
  void      Reset();                         // Reset used to zero
  //Doc:Desc This method saves the used count and resets used to zero.
  
  //Doc:Method    
  void      Dump();                          // Debug...print it
  //Doc:Desc Thie method is for debugging only

public:
  // The pointer below is actually the 32 bit NV if size <= 32.
  NixBits_t* nixVector;// 协议头内容指针                   // Points to the nix vector
  // Use "word" for the fields below so we get 16 bit values.
  // These will never require more than 16 bits and we save memory
  // Both these are specified in bits
  Word_t     used;//NixVectorOption已经使用的长度
  Word_t     size;//NixVectorOption报文头的总长度
private:
  Count_t    WSize();                        // Size in words
  Mask_t     BitMask(Count_t, Count_t);      // Get a mask at pos, nbits
  void       Expand();                       // Expand malloc'ed by 1 word
  // Insert bits into a 32  bit value
  void       InsertBits(NixBits_t&, Count_t, NixBits_t, Count_t);
public:
  Count_t    BitCount(Count_t);              // Find number of bits needed
};

// Define a hash table from IPAddress to NixVector
typedef std::map<IPAddr_t, NixVectorOption> NixMap_t;

//Doc:ClassXRef
class RoutingNixVector : public Routing {
//Doc:Class The class {\tt RoutingNixVector } derives from class {\tt Routing}
//Doc:Class and defines the NixVector Routing. This is useful for simulating
//Doc:Class large topologies with comparitively lesser memory requirements.
	
public:
  //Doc:Method
  RoutingNixVector();
  //Doc:Desc Default Constructor

  //Doc:Method
  virtual ~RoutingNixVector();
  //Doc:Desc The Destructor
  
  //Doc:Method
  virtual void Default( RoutingEntry r);         // Specify default route
  //Doc:Desc This method adds the default route to a particular node.
  //Doc:Arg1 The default routing entry or the default gateway
  
  //Doc:Method  
  virtual RoutingEntry GetDefault();         // Get default route
  //Doc:Desc This method returns the default route for this node
  //Doc:return The default routing entry or the default gateway

  //Doc:Method  
  virtual void Add( IPAddr_t, Count_t,
                    Interface*, IPAddr_t);       // Add routing entry
  //Doc:Desc This method is used to add a routing entry into the routing
  //Doc:Desc table.
  //Doc:Arg1 The destination IP Address
  //Doc:Arg2 The subnet Mask of the destination IP Address
  //Doc:Arg3 The interface to use
  //Doc:Arg4 The next hop IP Address
  
  //Doc:Method  
  virtual RoutingEntry Lookup(Node*, IPAddr_t);  // Find a routing entry
  //Doc:Desc This method implements the lookup mechanism at a  node
  //Doc:Desc for a given destination IP address
  //Doc:Arg1 Node which is looking up the routing table
  //Doc:Arg2 Destination IP address
  //Doc:Return The corresponding routing entry or nil
  
  //Doc:Method    
  virtual RoutingEntry LookupFromPDU(PDU*);      // Find from PDU (NixVector)
  //Doc:Desc This method uses the PDU to look up the next route. Such a
  //Doc:Desc mechanism is used in routing protocols like nix vector routing
  //Doc:Arg1 The pointer to the PDU
  //Doc:Return The corresponding routing entry or nil
  
  //Doc:Method    
  virtual Routing* Clone();                      // Get a Clone of the object
  //Doc:Desc This method is used to make copies of the routing object.
  //Doc:Desc Such a mechanism is useful for example, in topology creation
  //Doc:Return A pointer to the cloned object
  
  //Doc:Method    
  virtual RType_t Type();                        // Find out routing type
  //Doc:Desc This method enables to query the routing object of the
  //Doc:Desc routing protocol it implements
  //Doc:Return the routing type
  
    //Doc:Method
  virtual void    ReInitializeRoutes(Node*, bool);// Re-Initialization
  //Doc:Desc This method responds to topology changes and re-computes
  //Doc:Desc routes as needed.
  //Doc:Arg1 Node pointer for which the routes are to be re-calculated
  //Doc:Arg2 True if node or interface UP, false if DOWN

//Doc:Method    
  virtual bool    NeedReInit();                  // True if re-init needed
  //Doc:Desc This method determines if re-initialization is needed 
  //Doc:Desc in response to a topology change
  //Doc:Return true if initialization is needed
  
  //Doc:Method    
  virtual Size_t  Size() const;                  // Total size (bytes) of FIB
  //Doc:Desc This method gives the size of the routing table. or the
  //Doc:Desc Forwarding Information Base
  //Doc:Desc Size of the FIB
  
  //Doc:Method    
  NixVectorOption*  GetCachedNixVector(IPAddr_t);
  //Doc:Desc Thie method locates a previously cached copy of
  //Doc:Desc the nix vector to the specified  IP address,
  //Doc:Arg1 The destination address
  //Doc:Return the pointer to the NixVectorOption PDU, or nil of none

  //Doc:Method    
  NixVectorOption* GetNixVector(Node*, IPAddr_t);// Get a NixVector to address
  //Doc:Desc Thie method returns a nix vector options PDU correcponding to
  //Doc:Desc an IP address at a specified node
  //Doc:Arg1 The node at which we need the NixVector
  //Doc:Arg2 The destination address
  //Doc:Return the pointer to the NixVectorOption PDU
  
  //Doc:Method
  bool            ValidateNixVector(Node*, IPAddr_t, NixVectorOption);
    //Doc:Desc Validate an existing nixvector.  Used to insure still valid
    //Doc:Desc after a topology change.
    //Doc:Arg1 Source node pointer.
    //Doc:Arg2 Destination IPAddress.
    //Doc:Arg3 Existing NixVector
    //Doc:Return True if valid (does not trasverse a down node  or interface)

  //Doc:Method  
  void            DBDump(Node*);                 // Print debug info
  //Doc:Desc This method is for debugging
  //Doc:Arg1 Pointer to the node at which we need debug info
  
private:
  void    BuildNixVector(Node*, Node*,
                         NodeVec_t&,
                         NixVectorOption&);      // Build the nix vector
private:
  // NixVector caches the NixVectors to each destination
  NixMap_t cache;//重要变量，存储已经计算过的到达某一结点的NixVectorOption对象
  RoutingEntry defaultRoute; //默认路由                // Default route
};

#endif

