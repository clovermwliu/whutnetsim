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



// Georgia Tech Network Simulator - Port Demultiplexer
// George F. Riley.  Georgia Tech, Spring 2002

// Implements a map of protocol instances by port number

#ifndef __portdemux_h__
#define __portdemux_h__

#include "G_common-defs.h"
#include "protocol.h"

#include <map>

// Minimum port number for transient ports
#define MIN_TRANSIENT 10000

// Largest allowable port number
#define MAX_PORT      0xffff

typedef std::pair<IPAddr_t, PortId_t>  PortIp_t;      // IP Address and Port
typedef std::pair<PortIp_t, PortIp_t>  SrcDstPair_t;  // Local/Remote IP/Port
typedef std::map<SrcDstPair_t, Protocol*> PortDemux_t;
typedef std::map<Proto_t, PortDemux_t> ProtoDemux_t;  // One demux per proto
typedef std::map<Proto_t, PortId_t>    NextPort_t;    // Next available port

//Doc:ClassXRef
class PortDemux {
public:
  PortDemux();
  bool      Bind(Proto_t, PortId_t, Protocol*);   // Local port
  bool      Bind(Proto_t, PortId_t, IPAddr_t, Protocol*);// Local port, ip
  bool      Bind(Proto_t,
                 PortId_t, IPAddr_t,
                 PortId_t, IPAddr_t,
                 Protocol*);                      // Bind to local/remote
  PortId_t  Bind(Proto_t, Protocol*);             // Choose available port
  bool      Rebind(Proto_t,
                   PortId_t, IPAddr_t,
                   PortId_t, IPAddr_t,
                   Protocol*);                    // Rebind to local/remote
  Protocol* Lookup(Proto_t, PortId_t);
  Protocol* Lookup(Proto_t,
                   PortId_t, IPAddr_t,
                   PortId_t, IPAddr_t);
  bool      Unbind(Proto_t, PortId_t, Protocol*); // Release port usage
  bool      Unbind(Proto_t,
                   PortId_t, IPAddr_t,
                   PortId_t, IPAddr_t,
                   Protocol*); // Release port usage
  void      DBDump(PortDemux_t&); // debug
private:
  PortDemux_t::iterator Locate(Proto_t,
                               PortId_t, IPAddr_t,
                               PortId_t, IPAddr_t); // Locate the binding

public:
  ProtoDemux_t protoDemux;
  NextPort_t   nextPort;
};

#endif
