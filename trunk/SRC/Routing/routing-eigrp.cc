/*

* GTNetS provides a portable C++ class library for network simulations
*
* Copyright (C) 2003 George F. Riley
*
* No guarantees or warranties or anything are provided or implied in any way
* whatsoever.  Use this program at your own risk.  Permission to use this
* program for any purpose is given, as long as the copyright is kept intact.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

* Permission is hereby granted for any non-commercial use of this code

*/

// Georgia Tech Network Simulator - EIGRP Routing class
// George F. Riley, Talal M.Jaafar.  Georgia Tech, Summer 2004


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
#include "routing-eigrp.h"
#include "mask.h"
#include "ipaddr.h"
#include "node.h"
#include "bfs.h"
#include "hex.h"

using namespace std;

void RoutingEIGRP::Default(RoutingEntry r)
{
  defaultRoute = r; // Set the default route
#ifndef WIN32
  cout << "RoutingEIGRP::Default if " << r.interface
          << " ip " << (string)IPAddr(r.nexthop) << endl;
#else
  cout << "RoutingEIGRP::Default if " << r.interface_
          << " ip " << (string)IPAddr(r.nexthop) << endl;

#endif
}

RoutingEntry RoutingEIGRP::GetDefault()
{
  return defaultRoute;
}

void RoutingEIGRP::Add( IPAddr_t a, Mask_t m, Interface* i, IPAddr_t ip)
{
  Count_t mBits = Mask(m).NBits();
  DEBUG(2,(cout << "EIGRPRouting::Add, target ip " << (string)IPAddr(a)
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

RoutingEntry RoutingEIGRP::Lookup(Node*, IPAddr_t t)
{ // Lookup routing entry for specified target address
  for (FIBMap_t::reverse_iterator ri = fib.rbegin(); ri != fib.rend(); ++ri)
    {
      DEBUG0((cout << "RoutingEIGRP::Lookup, checking mask lth " 
              << ri->first << endl));
      FIB_t::iterator i = ri->second.find(t & Mask(ri->first));
      if (i != ri->second.end()) return i->second; // Return the routing entry
    }
  return defaultRoute; // Return the default route
}

RoutingEntry RoutingEIGRP::LookupFromPDU(PDU*)
{// Find from PDU (NixVector)..not used for eigrp
  return RoutingEntry();
}

Routing* RoutingEIGRP::Clone()
{
  return new RoutingEIGRP();
}

Routing::RType_t RoutingEIGRP::Type()
{
  return EIGRP;
}

void    RoutingEIGRP::InitializeRoutes(Node* r)
{ // Initialization (none needed for eigrp routing)
}

bool    RoutingEIGRP::NeedInit()
{ // Initialization not needed for eigrp routing (need to wait for convergence)
  return false;
}

Size_t  RoutingEIGRP::Size() const
{ // For statistics, return the size (bytes) of the FIB
  Size_t s = 0;
  DEBUG0((cout << "Hello from RoutingEIGRP::Size() fibsize " << fib.size() 
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


void   RoutingEIGRP::Clear()
{ //erase all elements in fib
  fib.clear();
}

void   RoutingEIGRP::displayFIB()
{
  IPAddr_t DestIP,HopIP;
  for( FIBMap_t::iterator i = fib.begin(); i != fib.end(); ++i)
    {
      FIB_t tempMap = i->second;
	  for(FIB_t::iterator j = tempMap.begin(); j != tempMap.end(); ++j)
	    {
	      DestIP =  j->first;
	      RoutingEntry tempRE = j->second;
	      HopIP = tempRE.nexthop;
              cout << "************destinationIP is " << DestIP
                   << ", nextHopIP is " << HopIP
                   << " *************" << endl;
	    }// for
    }// for
}
