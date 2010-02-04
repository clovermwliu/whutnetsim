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



// Georgia Tech Network Simulator - Global Statistics 
// George F. Riley.  Georgia Tech, Spring 2002

#include <iostream>

#include "G_debug.h"
#include "G_globalstats.h"
#include "node.h"

using namespace std;

// Static members
Size_t             Stats::initialMemory;
Size_t             Stats::modelMemory;
Size_t             Stats::routingMemory;
Size_t             Stats::finalMemory;
unsigned long long Stats::linksCreated;
unsigned long long Stats::pktsEnqued;
unsigned long long Stats::pktsDequed;
unsigned long long Stats::pktsDropped;
unsigned long long Stats::pktsTransmitted;
unsigned long long Stats::pktsAllocated;
unsigned long long Stats::pktsDeleted;
unsigned long long Stats::pdusAllocated;
unsigned long long Stats::pdusDeleted;
unsigned long long Stats::dataPdusAllocated;
unsigned long long Stats::dataPdusDeleted;
unsigned long long Stats::eventsAllocated;
unsigned long long Stats::eventsDeleted;
unsigned long long Stats::timersAllocated;
unsigned long long Stats::timersDeleted;
unsigned long long Stats::nvCreated;
unsigned long long Stats::nvAllocated;
unsigned long long Stats::nvDeleted;
unsigned long long Stats::nvReused;
unsigned long long Stats::routingEntAllocated;
unsigned long long Stats::routingEntDeleted;
unsigned long long Stats::tcpAllocated;
unsigned long long Stats::tcpDeleted;
unsigned long long Stats::tcpTimers;
unsigned long long Stats::tcpTimeouts;
unsigned long long Stats::tcpDupAckRetx;
unsigned long long Stats::tcpRetxTimeouts;
unsigned long long Stats::sequentialEvList;

void Stats::Print()
{
  //DBG((
  cout << "Global Statistics" << endl\
       << "  Initial Memory       " << initialMemory << endl\
       << "  After Model Memory   " << modelMemory << endl\
       << "  After Routing Memory " << routingMemory << endl\
       << "  Final Memory         " << finalMemory << endl\
       << "  Links Created        " << linksCreated << endl\
       << "  Nodes Created        " << Node::nextId << endl\
       << "  Packets Enqued       " << pktsEnqued << endl\
       << "  Packets Dequed       " << pktsDequed << endl\
       << "  Packets Dropped      " << pktsDropped << endl\
       << "  Packets Transmitted  " << pktsTransmitted << endl\
       << "  Packets Allocated    " << pktsAllocated << endl\
       << "  Packets Deleted      " << pktsDeleted << endl\
       << "  PDUs Allocated       " << pdusAllocated << endl\
       << "  PDUs Deleted         " << pdusDeleted << endl\
       << "  Data PDUs Allocated  " << dataPdusAllocated << endl\
       << "  Data PDUs Deleted    " << dataPdusDeleted << endl\
       << "  Events Allocated     " << eventsAllocated << endl\
       << "  Events Deleted       " << eventsDeleted << endl\
       << "  Timers Allocated     " << timersAllocated << endl\
       << "  Timers Deleted       " << timersDeleted << endl\
       << "  NixVec Created       " << nvCreated << endl\
       << "  NixVec Allocated     " << nvAllocated << endl\
       << "  NixVec Deleted       " << nvDeleted << endl\
       << "  NixVec Reused        " << nvReused << endl\
       << "  RtgEntries Allocated " << routingEntAllocated << endl\
       << "  RtgEntries Deleted   " << routingEntDeleted << endl\
       << "  TCP Allocated        " << tcpAllocated << endl\
       << "  TCP Deleted          " << tcpDeleted << endl\
       << "  TCP Timers           " << tcpTimers << endl\
       << "  TCP Timeouts         " << tcpTimeouts << endl\
       << "  TCP DupAck Retx      " << tcpDupAckRetx << endl\
       << "  TCP Retransmits      " << tcpRetxTimeouts << endl\
       << "  Sequential Events    " << sequentialEvList << endl\
    //       ));
    ;
}
