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

#include <iostream>
#include <vector>
#include <map>
#include <string>

// Uncomment below to enable debug level 0
// #define DEBUG_MASK 0x01
//#define DEBUG_MASK 0x4
//#define DEBUG_MASK 0x7
#include "G_debug.h"
#include "routing.h"
#include "routing-static.h"
#include "mask.h"
#include "ipaddr.h"
#include "node.h"
#include "bfs.h"
#include "hex.h"
#include "interface.h"

using namespace std;

void RoutingStatic::Default(RoutingEntry r)
{
  defaultRoute = r; // Set the default route
#ifndef WIN32
  DEBUG(1,(cout << "RoutingStatic::Default if " << r.interface
          << " ip " << (string)IPAddr(r.nexthop) << endl));
#else
  DEBUG(1,(cout << "RoutingStatic::Default if " << r.interface_
          << " ip " << (string)IPAddr(r.nexthop) << endl));

#endif
}

RoutingEntry RoutingStatic::GetDefault()
{
  return defaultRoute;
}

void RoutingStatic::Add( IPAddr_t a, Mask_t m, Interface* i, IPAddr_t ip)
{
  Count_t mBits = Mask(m).NBits();
  DEBUG(2,(cout << "StaticRouting::Add, target ip " << (string)IPAddr(a)
           << " next hop ip " << (string)IPAddr(ip) 
           << " maskBits " << mBits
           << endl));
  DEBUG(2,(cout << "Befor insert, size of " << mBits 
           << " fib is " << fib[mBits].size() << endl));
  fib[mBits].insert(FIB_t::value_type(a & (Mask_t)Mask(m),
                                  RoutingEntry(i, ip))); // Insert in FIB
  DEBUG(2,(cout << "After insert, size of " << mBits 
           << " fib is " << fib[mBits].size() << endl));
}

RoutingEntry RoutingStatic::Lookup(Node*, IPAddr_t t)
{ // Lookup routing entry for specified target address
  for (FIBMap_t::reverse_iterator ri = fib.rbegin(); ri != fib.rend(); ++ri)
    {
      DEBUG0((cout << "RoutingStatic::Lookup, checking mask lth " 
              << ri->first << endl));
      FIB_t::iterator i = ri->second.find(t & Mask(ri->first));
      if (i != ri->second.end()) return i->second; // Return the routing entry
    }
  return defaultRoute; // Return the default route
}

RoutingEntry RoutingStatic::LookupFromPDU(PDU*)
{// Find from PDU (NixVector)..not used for static
  return RoutingEntry();
}

Routing* RoutingStatic::Clone()
{
  return new RoutingStatic();
}

Routing::RType_t RoutingStatic::Type()
{
  return STATIC;
}

void    RoutingStatic::InitializeRoutes(Node* r)
{ // Initialization, Computing routing table for this node
  // For static routing, use BFS if all link weights default,
  // otherwise use Dijkstra (note...Dijkstra not implemented yet)
  // But first, find out if single neighbor (leaf) node, in which
  // case, no routing computations are necessary
  Count_t nc = r->NeighborCount();
  if (nc <= 1)
    {
      DEBUG0((cout << "Node " << r->Id()
              << " has only 1 neighbor, skipping routes" << endl));
      return;
    }
  DEBUG0((cout << "Node " << r->Id()
          << " has " << nc << " neighbors, calculating routes" << endl));
  NodeIfVec_t nextHop;      // Next hop vector
  NodeVec_t   parent;       // Parent vector
  const NodeVec_t&  nodes = Node::GetNodes();
  IPAddrVec_t       aliases;
  BFS(nodes, r, nextHop, parent, IPADDR_NONE, aliases);
  // For debug, print the nh vector
  DEBUG0((cout << "Node " << r->Id() << " printing nh table" << endl));
  DEBUG0({for (NodeVec_t::size_type i = 0; i < nextHop.size(); ++i)
    {
      cout << "  NextHop for dest " << i << " is ";
      if (nextHop[i].node)
        {
          cout << nextHop[i].node->Id() << endl;
        }
      else
        {
          cout << "nil pointer" << endl;
        }
    }});
  // First count the most common nexthop, and use that for default
  vector<Count_t> c(nodes.size(), 0);
  Count_t maxCount = 0;      // Maximum value found
  NodeIf  maxNodeIf(nil,nil);
 
  for (NodeVec_t::size_type i = 0; i < nextHop.size(); ++i)
    {
      if (nextHop[i].node)
        { // Next hop exists
          if (++c[nextHop[i].node->Id()] > maxCount)
            { // Found new maximum
              maxCount  = c[nextHop[i].node->Id()];
              maxNodeIf = nextHop[i];
            }
        }
    }
  DEBUG0((cout << "Node " << r->Id() << " max count is " << maxCount
          << " to node " << maxNodeIf.node->Id() << endl));
  if (maxNodeIf.iface)
    { // At least one found
      Default(RoutingEntry(maxNodeIf.iface,
                           maxNodeIf.iface->NodePeerIP(maxNodeIf.node)));
    }
  // Now create routing table entries
  for (NodeVec_t::size_type i = 0; i < nextHop.size(); ++i)
    {
      Node*      target = nodes[i];
      Node*      node = nextHop[i].node;
      Interface* iface = nextHop[i].iface;
      if (node) 
        { // Node exists
          if (node  != maxNodeIf.node ||
              iface != maxNodeIf.iface)
            { // Non-default found, add the routes
              DEBUG(2,(cout << "Node " << r->Id()
                       << " Non-default route for target " << i << endl));
              // Get a list of all IP addresses for the target node
              IPMaskVec_t ips;
              target->IPAddrs(ips);
              for (IPMaskVec_t::size_type j = 0; j < ips.size(); ++j)
                {
                  DEBUG(2,(cout << "  IPAddr is " << (string)IPAddr(ips[j].ip)
                           << " mask " << Hex8(ips[j].mask) 
                           << " iface " << iface << endl));
                  // Add to routing table
                  Add(ips[j].ip, ips[j].mask, iface, iface->NodePeerIP(node));
                }
            }
        }
    }  
}

void    RoutingStatic::ReInitializeRoutes(Node* r, bool  u)
{ // Respond to topology changes
  // Naieve approach, just clear the fibs and re-calculate
  for (FIBMap_t::iterator i = fib.begin(); i != fib.end(); ++i)
    {
      i->second.clear();
    }
  InitializeRoutes(r);
}
  
bool    RoutingStatic::NeedInit()
{ // Initialization needed for static
  return true;
}

bool    RoutingStatic::NeedReInit()
{ // Initialization needed for static
  return true;
}

Size_t  RoutingStatic::Size() const
{ // For statistics, return the size (bytes) of the FIB
  Size_t s = 0;
  DEBUG0((cout << "Hello from RoutingStatic::Size() fibsize " << fib.size() 
          << endl));
  for (FIBMap_t::const_iterator i = fib.begin(); i != fib.end(); ++i)
    {
      s += i->second.size() * (sizeof(IPAddr_t) + sizeof(RoutingEntry));
      DEBUG0((cout << "Working on mask length " << i->first
              << " fibsize " << i->second.size()
              << " totsize " << s << endl));
    }
  return s;
}

