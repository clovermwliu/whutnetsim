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



// Georgia Tech Network Simulator - Manual Routing class
// George F. Riley.  Georgia Tech, Spring 2002

// Manually computed and specified routes
// Manual routing has no way to delete or aggregate entries

#include <vector>
#include <map>
#include <string>
#include <iostream>

// Uncomment below to enable debug level 0
// #define DEBUG_MASK 0x01
#include "G_debug.h"
#include "routing.h"
#include "routing-manual.h"
#include "mask.h"
#include "ipaddr.h"

using namespace std;

void RoutingManual::Default(RoutingEntry r)
{
  defaultRoute = r; // Set the default route
#ifndef WIN32
  DEBUG0((cout << "RoutingManual::Default if " << r.interface
          << " ip " << (string)IPAddr(r.nexthop) << endl));

#else
  DEBUG0((cout << "RoutingManual::Default if " << r.interface_
          << " ip " << (string)IPAddr(r.nexthop) << endl));



#endif
}

RoutingEntry RoutingManual::GetDefault()
{
  return defaultRoute;
}

void RoutingManual::Add( IPAddr_t a, Mask_t m, Interface* i, IPAddr_t ip)
{
  fib[m].insert(FIB_t::value_type(a & (Mask_t)Mask(m),
                                  RoutingEntry(i, ip))); // Insert in FIB
}

void RoutingManual::Delete( IPAddr_t a, Mask_t m)
{
  fib[m].erase(a & (Mask_t)Mask(m));
}

RoutingEntry RoutingManual::Lookup(Node*, IPAddr_t t)
{ // Lookup routing entry for specified target address
  // Lookup in FIB starting with longest mask working backwards
  for (FIBMap_t::reverse_iterator ri = fib.rbegin(); ri != fib.rend(); ++ri)
    {
      DEBUG0((cout << "RoutingManual::Lookup, checking mask lth " 
              << ri->first << endl));
      FIB_t::iterator i = ri->second.find(t & Mask(ri->first));
      if (i != ri->second.end()) return i->second; // Return the routing entry
    }
  return defaultRoute; // Return the default route
}

RoutingEntry RoutingManual::LookupFromPDU(PDU*)
{// Find from PDU (NixVector)..not used for manual
  return RoutingEntry();
}

Routing* RoutingManual::Clone()
{
  return new RoutingManual();
}

Routing::RType_t RoutingManual::Type()
{
  return MANUAL;
}

Size_t RoutingManual::Size() const
{ // For statistics, return the size (bytes) of the FIB
  Size_t s = 0;
  for (FIBMap_t::const_iterator i = fib.begin(); i != fib.end(); ++i)
    {
      s += i->second.size() * (sizeof(IPAddr_t) + sizeof(RoutingEntry));
    }
  return s;
}

