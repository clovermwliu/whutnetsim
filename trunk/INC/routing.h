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



// Georgia Tech Network Simulator - Routing base class
// George F. Riley.  Georgia Tech, Spring 2002

// Virtual Base class for all routing interfaces

#ifndef __routing_h__
#define __routing_h__

#include <vector>
#include <list>
#include <map>
#include <algorithm>

#include "G_debug.h"
#include "G_common-defs.h"
#include "G_globalstats.h"

class Interface;
class PDU;
class Node;
class Packet;

//Doc:ClassXRef
class RoutingEntry {
//Doc:Class This class defines each entry in the routing table. Currently
//Doc:Class this class supports only IPv4 addresses. It essentially
//Doc:Class defines the destination address and the interface to use to
//Doc:Class reach it.
#ifndef WIN32
public:
  RoutingEntry() : interface(0), nexthop(IPADDR_NONE)
    { DBG((Stats::routingEntAllocated++));}
  RoutingEntry(Interface* i, IPAddr_t a) 
    : interface(i), nexthop(a)
    { DBG((Stats::routingEntAllocated++));}
  RoutingEntry(const RoutingEntry& c)
    : interface(c.interface), nexthop(c.nexthop)
    { DBG((Stats::routingEntAllocated++));}
  ~RoutingEntry() { DBG((Stats::routingEntDeleted++));}
public:
  Interface* interface;  // Interface for route
  IPAddr_t   nexthop;    // Next hop IP Address
};

#else

public:
  RoutingEntry() : interface_(0), nexthop(IPADDR_NONE)
    { DBG((Stats::routingEntAllocated++));}
  RoutingEntry(Interface* i, IPAddr_t a) 
    : interface_(i), nexthop(a)
    { DBG((Stats::routingEntAllocated++));}
  RoutingEntry(const RoutingEntry& c)
    : interface_(c.interface_), nexthop(c.nexthop)
    { DBG((Stats::routingEntAllocated++));}
  ~RoutingEntry() { DBG((Stats::routingEntDeleted++));}
public:
  Interface* interface_;  // Interface for route
  IPAddr_t   nexthop;    // Next hop IP Address
};


#endif

typedef std::map<IPAddr_t, RoutingEntry> FIB_t; // Forwarding Information base
typedef std::vector<FIB_t>       FIBVec_t;      // One FIB per mask length 
typedef std::map<Count_t, FIB_t> FIBMap_t;      // One FIB per mask length


// Virtual base class for all routing
//Doc:ClassXRef
class Routing {
//Doc:Class The class {\tt Routing} is a virtual base class for all the
//Doc:Class routing protocols that may be needed for a simulation. Each of
//Doc:Class these routing protocols have to derive from this class and define
//Doc:Class the routing protocol procedures
public:
  // Routing types
  typedef enum { STATIC, DYNAMIC, MANUAL, NIXVECTOR, NVR, DSR, EIGRP } RType_t;
public:
  //Doc:Method
  Routing();
  //Doc:Desc This method is the default constructor for class {\tt Routing}.

  //Doc:Method
  virtual ~Routing();
  //Doc:Desc This is the default destructor. Each routing protocol must
  //Doc:Desc define its own destructor.
  
public:
  //Doc:Method
  virtual void Default( RoutingEntry r) = 0;     // Specify default route
  //Doc:Desc This method adds the default route to a particular node.
  //Doc:Arg1 The default routing entry or the default gateway
  
  //Doc:Method
  virtual RoutingEntry GetDefault() = 0;         // Get default route
  //Doc:Desc This method returns the default route for this node
  //Doc:return The default routing entry or the default gateway
  
  //Doc:Method
  virtual void Add( IPAddr_t, Count_t,
                    Interface*, IPAddr_t) = 0;   // Add a routing entry
  //Doc:Desc This method is used to add a routing entry into the routing
  //Doc:Desc table.
  //Doc:Arg1 The destination IP Address
  //Doc:Arg2 The subnet Mask of the destination IP Address
  //Doc:Arg3 The interface to use
  //Doc:Arg4 The next hop IP Address

	virtual void Delete( IPAddr_t, Count_t) {};
  
  //Doc:Method
  virtual RoutingEntry Lookup(Node*, IPAddr_t) = 0; // Find a routing entry
  //Doc:Desc This method implements the lookup mechanism at a  node
  //Doc:Desc for a given destination IP address
  //Doc:Arg1 Node which is looking up the routing table
  //Doc:Arg2 Destination IP address
  //Doc:Return The corresponding routing entry or nil
  
  //Doc:Method
  virtual RoutingEntry LookupFromPDU(PDU*) = 0;  // Find from PDU (NixVector)
  //Doc:Desc This method uses the PDU to look up the next route. Such a
  //Doc:Desc mechanism is used in routing protocols like nix vector routing
  //Doc:Arg1 The pointer to the PDU
  //Doc:Return The corresponding routing entry or nil
  
  //Doc:Method
  virtual Routing* Clone() = 0;                  // Get a clone of the object
  //Doc:Desc This method is used to make copies of the routing object.
  //Doc:Desc Such a mechanism is useful for example, in topology creation
  //Doc:Return A pointer to the cloned object
  
  //Doc:Method
  virtual RType_t Type() = 0;                    // Find out routing type
  //Doc:Desc This method enables to query the routing object of the
  //Doc:Desc routing protocol it implements
  //Doc:Return the routing type
  
  //Doc:Method
  virtual void    InitializeRoutes(Node*);       // Initialization
  //Doc:Desc This method initializes the routes and the routing table at a
  //Doc:Desc given node
  //Doc:Arg1 The pointer to the Node at which the routes are to be initialized
  
  //Doc:Method
  virtual void    ReInitializeRoutes(Node*, bool);// Re-Initialization
  //Doc:Desc This method responds to topology changes and re-computes
  //Doc:Desc routes as needed.
  //Doc:Arg1 Node pointer for which the routes are to be re-calculated
  //Doc:Arg2 True if node or interface UP, false if DOWN
  
  //Doc:Method
  virtual bool    NeedInit();                    // True if init needed
  //Doc:Desc This method determines if initialization is needed and returns
  //Doc:Desc a bool
  //Doc:Return true if initialization is needed
  
  virtual bool    NeedReInit();
  //Doc:Desc This method determines if re-initialization is needed and returns
  //Doc:Desc a bool true if this routing protocol needs to re-initialize
  //Doc:Desc in response to a topology change
  //Doc:Return true if initialization is needed

  // Statistics
  //Doc:Method
  virtual Size_t  Size() const = 0;              // Total size (bytes) of FIB
  //Doc:Desc This method gives the size of the routing table. or the
  //Doc:Desc Forwarding Information Base
  //Doc:Desc Size of the FIB

  virtual void    DataRequest(Node* n, Packet* p, void* v) {}
  virtual bool    DataIndication(Interface* i, Packet* p) { return true; }

public:
  // Get the default routing pointer
  //Doc:Method
  static Routing* Default();
  //Doc:Desc This method returns the default routing protocol object
  //Doc:Desc used by default
  //Doc:Return A pointer to the default routing object

  //Doc:Method
  static void SetRouting( Routing*);      // Set a new active protocol
  //Doc:Desc This method is used to set a new routing protocol for a simulation
  //Doc:Arg1 A pointer to the new routing protocol object

  //Doc:Method
  static void TopologyChanged(bool);
    //Doc:Desc Notify the routing object that topology has changed.
    //Doc:Desc Will call the re-initializatoin for each  node.
    //Doc:Arg1  True if node or interface UP, false if DOWN

public:
  Node*  node;                                   // Node to which it's attached

private:
  static Routing* defaultRouting;                // Points to active protocol
};


/*-------- Routing Overhaul -1  -- SDR -----------*/

/* Destination IP prefix ?? can be optimized using a hash func.?? */


class rt_key_t {
 public:
	rt_key_t():addr(0), count(0) {}
	rt_key_t(IPAddr_t addr_):addr(addr_), count(32) {}
	rt_key_t(IPAddr_t addr_, unsigned int count_):
		addr(addr_), count(count_) {}
 public:
	IPAddr_t addr;
	unsigned int count; // The key is in addr/count format.
};

struct ltrt_key_t
{
#ifndef WIN32
	bool operator()(const rt_key_t* rk1, const rt_key_t* rk2) 
#else
	bool operator()(const rt_key_t* rk1, const rt_key_t* rk2) const
#endif


	{
		return ( (rk1->addr < rk2->addr) ||
			 ((rk1->addr == rk2->addr) &&
			  (rk1->count < rk2->count)) );
	}
};

#define RT_BLANK   0x00
#define RT_USABLE  0x01
#define RT_GATEWAY 0x02
#define RT_STATIC  0x80


class rt_value_t {
 public:
	rt_value_t():nexthop(0), iface(NULL), mtu(0), lifetime(0),
		flags(RT_USABLE), maxwin(0), irtt(0){}
	rt_value_t(IPAddr_t addr_, Interface* iface_):
		nexthop(addr_), iface(iface_), mtu(0), lifetime(0),
		flags(RT_USABLE), maxwin(0), irtt(0){}
	
 public:
	IPAddr_t nexthop;
	Interface* iface;
	unsigned int mtu;
	unsigned long  lifetime;	
	unsigned char flags;
	unsigned long  maxwin; /* Window clamp */
	unsigned short irtt; /* initial round trip time */
};

typedef std::list<rt_value_t*> rt_valuelist_t;
typedef std::map<rt_key_t*, rt_valuelist_t*, ltrt_key_t> NewFIB_t;

class RouteTable { /* Was called FIB earlier */
  public:
	 RouteTable();
	 ~RouteTable();

	 /* Called only from policy processes (application-routing here) */
	 void AddRoute(rt_key_t*, rt_value_t*, bool aggr=false);
	 void DelRoute(rt_key_t* , bool aggr=false);
	 void Flush(void); /* Empties the routing table */
	 void Dump(void);  /* Dump the entries of the FIB */
	 int Size(void);  /* return the size of the FIB */

	 /* Called during forwarding , returns a list */
	 rt_valuelist_t* LookupRoute(rt_key_t* key, bool aggr=false); 

  public:
	 NewFIB_t fib;
};

/*-------- Routing Overhaul -1  -- SDR -----------*/

#endif


