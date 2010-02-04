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

#ifndef __global_stats_h__
#define __global_stats_h__

#include "G_common_defs.h"

//Doc:ClassXRef
class Stats {
public:
  static void Print();
public:
  static Size_t             initialMemory;
  static Size_t             modelMemory;
  static Size_t             routingMemory;
  static Size_t             finalMemory;
  static unsigned long long linksCreated;
  static unsigned long long pktsEnqued;
  static unsigned long long pktsDequed;
  static unsigned long long pktsDropped;
  static unsigned long long pktsTransmitted;
  static unsigned long long pktsAllocated;
  static unsigned long long pktsDeleted;
  static unsigned long long pdusAllocated;
  static unsigned long long pdusDeleted;
  static unsigned long long dataPdusAllocated;
  static unsigned long long dataPdusDeleted;
  static unsigned long long eventsAllocated;
  static unsigned long long eventsDeleted;
  static unsigned long long timersAllocated;
  static unsigned long long timersDeleted;
  static unsigned long long nvCreated;
  static unsigned long long nvAllocated;
  static unsigned long long nvDeleted;
  static unsigned long long nvReused;
  static unsigned long long routingEntAllocated;
  static unsigned long long routingEntDeleted;
  static unsigned long long tcpAllocated;
  static unsigned long long tcpDeleted;
  static unsigned long long tcpTimers;
  static unsigned long long tcpTimeouts;
  static unsigned long long tcpDupAckRetx;
  static unsigned long long tcpRetxTimeouts;
  static unsigned long long sequentialEvList;
};

#endif
