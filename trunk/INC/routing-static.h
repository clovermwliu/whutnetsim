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



// Georgia Tech Network Simulator - Static Routing class
// George F. Riley.  Georgia Tech, Spring 2002

// Statically (one-time) computed and specified routes
// Static routing is simple and easy to use by the user,
// but is quite memory intensive.  It is recommended for
// simulation topologies on the order of a few hundred
// nodes.  Larger topologies should use either Manual
// or NixVector routing.

#ifndef __routingstatic_h__
#define __routingstatic_h__

#include "routing.h"

//Doc:ClassXRef
class RoutingStatic : public Routing {
//Doc:Class The class {\tt RoutingStatic} defines the static routing class.
//Doc:Class This routing object calculates all the routes needed at a single
//Doc:Class instance. Static routing is simple and easy to use by the user,
//Doc:Class but is quite memory intensive.  It is recommended for
//Doc:Class simulation topologies on the order of a few hundred
//Doc:Class nodes.  Larger topologies should use either Manual
//Doc:Class or NixVector routing.

 public:
  //RoutingStatic() : fib(32, FIB_t()){ }
  //Doc:Method	
  RoutingStatic() { }
  //Doc:Desc The default constructor
  
  //Doc:Method  
  virtual ~RoutingStatic() { }
  //Doc:Desc The destructor
  
  //Doc:Method  
  virtual void Default(RoutingEntry);        // Specify default route
  //Doc:Desc This method adds the default route to a particular node.
  //Doc:Arg1 The default routing entry or the default gateway
  
  //Doc:Method  
  virtual RoutingEntry GetDefault();         // Get default route
  //Doc:Desc This method returns the default route for this node
  //Doc:return The default routing entry or the default gateway

  //Doc:Method  
  virtual void Add( IPAddr_t, Count_t,
                    Interface*, IPAddr_t);   // Add a routing entry
  //Doc:Desc This method is used to add a routing entry into the routing
  //Doc:Desc table.
  //Doc:Arg1 The destination IP Address
  //Doc:Arg2 The subnet Mask of the destination IP Address
  //Doc:Arg3 The interface to use
  //Doc:Arg4 The next hop IP Address
  

  //Doc:Method  
  virtual RoutingEntry Lookup(Node*, IPAddr_t); // Find a routing entry
  //Doc:Desc This method implements the lookup mechanism at a  node
  //Doc:Desc for a given destination IP address
  //Doc:Arg1 Node which is looking up the routing table
  //Doc:Arg2 Destination IP address
  //Doc:Return The corresponding routing entry or nil

  
  //Doc:Method  
  virtual RoutingEntry LookupFromPDU(PDU*);  // Find from PDU (NixVector)
  //Doc:Desc This method uses the PDU to look up the next route. Such a
  //Doc:Desc mechanism is used in routing protocols like nix vector routing
  //Doc:Arg1 The pointer to the PDU
  //Doc:Return The corresponding routing entry or nil
  
  //Doc:Method  
  virtual Routing* Clone();                  // Get a Clone of the object
  //Doc:Desc This method is used to make copies of the routing object.
  //Doc:Desc Such a mechanism is useful for example, in topology creation
  //Doc:Return A pointer to the cloned object

  //Doc:Method  
  virtual RType_t Type();                    // Find out routing type
  //Doc:Desc This method enables to query the routing object of the
  //Doc:Desc routing protocol it implements
  //Doc:Return the routing type
  

  //Doc:Method  
  virtual void    InitializeRoutes(Node*);   // Initialization
  //Doc:Desc This method initializes the routes and the routing table at a
  //Doc:Desc given node
  //Doc:Arg1 The pointer to the Node at which the routes are to be initialized
  
  //Doc:Method  
  virtual void    ReInitializeRoutes(Node*, bool); // Initialization
  //Doc:Desc This method initializes the routes and the routing table at a
  //Doc:Desc given node
  //Doc:Arg1 The pointer to the Node at which the routes are to be initialized
  //Doc:Arg2 True if node or interface UP, false if DOWN
  

  //Doc:Method
  virtual bool    NeedInit();                // True if init needed
  //Doc:Desc This method determines if initialization is needed and returns
  //Doc:Desc a bool
  //Doc:Return true if initialization is needed
  
  //Doc:Method
  virtual bool    NeedReInit();              // True if re-init needed
  //Doc:Desc This method determines if initialization is needed and returns
  //Doc:Desc a bool
  //Doc:Return true if initialization is needed
  

  //Doc:Method  
  virtual Size_t  Size() const;              // Total size (bytes) of FIB
  //Doc:Desc This method gives the size of the routing table. or the
  //Doc:Desc Forwarding Information Base
  //Doc:Desc Size of the FIB
  
public:
  FIBMap_t     fib;                          // The routing database
  RoutingEntry defaultRoute;                 // Default route
};
#endif

