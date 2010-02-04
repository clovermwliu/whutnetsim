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



// Georgia Tech Network Simulator - Port Demultiplexer
// George F. Riley.  Georgia Tech, Spring 2002

// Implements a map of protocol instances by port number

#include <iostream>

//#define DEBUG_MASK 0x02
#include "G_debug.h"
#include "portdemux.h"

using namespace std;

// Bindings are by local port, local IP, remote port, remote IP
// Local port can be IPADDR_NONE
// Both remote port, remote IP can be NO_PORT, IPADDR_NONE

// Define some syntactic macros, since the lookup key is a pair of pairs
#define LOCAL_PORT  first.second
#define LOCAL_IP    first.first
#define REMOTE_PORT second.second
#define REMOTE_IP   second.first

PortDemux::PortDemux()
{
}

bool PortDemux::Bind(Proto_t proto, PortId_t port, Protocol* p)
{ // Local port only
  return Bind(proto, port, IPADDR_NONE, p);
}

bool PortDemux::Bind(Proto_t proto, PortId_t port, IPAddr_t ip, Protocol* p)
{ // Local port, local ip
  return Bind(proto, port, ip, NO_PORT, IPADDR_NONE, p);
}

bool PortDemux::Bind(Proto_t proto,
                     PortId_t lport, IPAddr_t laddr,
                     PortId_t rport, IPAddr_t raddr,
                     Protocol* p)
{ // Local port/ip, remote port/ip
  PortDemux_t::iterator i;
  SrcDstPair_t key;
  key.LOCAL_PORT = lport;
  key.LOCAL_IP = laddr;
  key.REMOTE_PORT = rport;
  key.REMOTE_IP = raddr;
  i = protoDemux[proto].find(key);
  if (i != protoDemux[proto].end())
    {
      DEBUG0((cout << "PortDemux::Bind, port " << lport 
              << " already in use" << endl));
      return false; // port already in use
    }
  
  protoDemux[proto].insert(PortDemux_t::value_type(key,p)); // Insert protocol 
  DEBUG0((cout << "After bind lip " << (string)IPAddr(laddr)
          << " lport " << lport
          << " rip " << (string)IPAddr(raddr)
          << " rport " << rport
          << " proto " << p
          << endl));
  DEBUG0((DBDump(protoDemux[proto])));
  return true;
}

PortId_t PortDemux::Bind(Proto_t proto, Protocol* p)
{ // Bind to an available port (port must be > MIN_TRANSIENT)
  NextPort_t::const_iterator i = nextPort.find(proto);
  if (i == nextPort.end())
    { // First lookup, initialize to MIN_TRANSIENT
      nextPort.insert(NextPort_t::value_type(proto, MIN_TRANSIENT));
    }
  PortId_t port = nextPort[proto]++;
  Bind(proto, port, IPADDR_NONE, p);
  return port;
  // Need better..need to bound ports at MAX_PORT
}

bool PortDemux::Rebind(Proto_t proto,
                       PortId_t lport, IPAddr_t laddr,
                       PortId_t rport, IPAddr_t raddr,
                       Protocol* p)
{ // Change to more specific binding
  PortDemux_t::iterator i;
  i = Locate(proto, lport, laddr, rport, raddr);
  if (i == protoDemux[proto].end()) return false; // Not found
  protoDemux[proto].erase(i); // Remove old binding
  return Bind(proto, lport, laddr, rport, raddr, p);
}
                       
Protocol* PortDemux::Lookup(Proto_t proto, PortId_t port)
{
  return Lookup(proto, port, IPADDR_NONE, NO_PORT, IPADDR_NONE);
}

Protocol* PortDemux::Lookup(Proto_t proto,
                             PortId_t lport, IPAddr_t laddr,
                             PortId_t rport, IPAddr_t raddr)
{
  PortDemux_t::iterator i;
  i = Locate(proto, lport, laddr, rport, raddr);
  if (i == protoDemux[proto].end()) return NULL; // Not found
  return i->second;                // Return the protocol pointer
}

bool PortDemux::Unbind(Proto_t proto, PortId_t port, Protocol* p)
{
  return Unbind(proto, port, IPADDR_NONE, NO_PORT, IPADDR_NONE, p);
}


bool PortDemux::Unbind(Proto_t proto,
                        PortId_t lport, IPAddr_t laddr,
                        PortId_t rport, IPAddr_t raddr,
                        Protocol* p)
{
  PortDemux_t::iterator i;
  SrcDstPair_t key;
  key.LOCAL_PORT = lport;
  key.LOCAL_IP = laddr;
  key.REMOTE_PORT = rport;
  key.REMOTE_IP = raddr;
  DEBUG0((DBDump(protoDemux[proto])));
  i = protoDemux[proto].find(key);
  if (i == protoDemux[proto].end())
    {
      DEBUG0((cout << "Can't find unbind for localport " << lport
              << " rport " << rport
              << " raddr " << (string)IPAddr(raddr)
              << endl));
      return false; // Not bound
    }
  
  if (i->second != p)
    {
      DEBUG0((cout << "Protocol mis-match localport " << lport
              << " rport " << rport
              << " raddr " << (string)IPAddr(raddr)
              << " p " << p
              << " second " << i->second
              << endl));
      return false;               // Not bound to this protocol
    }
  
  DEBUG0((cout << "Erasing localport " << lport
          << " rport " << rport
          << " raddr " << (string)IPAddr(raddr)
          << endl));
  protoDemux[proto].erase(i);                     // Remove binding
  DEBUG(1,(cout << "portdemux unbinding" << endl));
  return true;
}

PortDemux_t::iterator PortDemux::Locate(Proto_t proto,
                                        PortId_t lport, IPAddr_t laddr,
                                        PortId_t rport, IPAddr_t raddr)
{ // Locate the binding
  PortDemux_t::iterator i;
  SrcDstPair_t key;
  key.LOCAL_PORT = lport;
  key.LOCAL_IP = laddr;
  key.REMOTE_PORT = rport;
  key.REMOTE_IP = raddr;
  DEBUG0((DBDump(protoDemux[proto])));
  i = protoDemux[proto].find(key);
  if (i != protoDemux[proto].end()) return i; // Found it
  // Try without rport/raddr
  key.REMOTE_PORT = NO_PORT;
  key.REMOTE_IP = IPADDR_NONE;
  i = protoDemux[proto].find(key);
  if (i != protoDemux[proto].end()) return i; // Found it
  // Try without source ip
  key.LOCAL_IP = IPADDR_NONE;
  return protoDemux[proto].find(key);
}

 void      PortDemux::DBDump(PortDemux_t& dm) // debug
{
  PortDemux_t::iterator i;
  cout << "protoDemux size " 
       << dm.size() << endl;
  for (i = dm.begin(); i != dm.end(); ++i)
    {
      cout << "DM srcip " << (string)IPAddr(i->first.first.first)
           << " srdport " << i->first.first.second
           << " dstip " << (string)IPAddr(i->first.second.first)
           << " dstport " << i->first.second.second << endl;
    }

}


