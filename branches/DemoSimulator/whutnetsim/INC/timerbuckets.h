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



// Georgia Tech Network Simulator - Efficient Bucketed timers
// George F. Riley.  Georgia Tech, Winter 2002

// Timer buckets reduce the size of the simulator's pending event list
// by taking advantage of the fact that any timer that is "k" milliseconds
// in the future will always be sequentially after the previously scheduled
// timer that is "k" milliseconds in the future. Thus only the earliest such
// time need be scheduled in the simulator's main event list.  All others
// can be in a fifo queue.  We also observe that in "real" tcp implementations,
// the timers are typically in 10ms units, since most hardware implementations
// perform process switching on 10ms interrupts.  Thus all TCP retransmit
// timers are in units of 10ms, allowing the bucketing of all TCP timers in
// the same [k - k+10ms] range.

#ifndef __timerbuckets_h__
#define __timerbuckets_h__

#include <vector>

#include "timer.h"

typedef std::vector<FifoEventList_t*> BucketVec_t;       // The buckets

class TimerBucket : public Handler {
public:
  TimerBucket();
  // Handler for events
  void Handle(Event*, Time_t);
  void BucketSize(Count_t); // Size in milliseconds of each bucket
  void Schedule(TimerEvent*, Time_t, TimerHandler*);
  void Cancel(TimerEvent*); // Cancel a pending timer
  void DBDump();            // Debug dump statistics
public:
  static TimerBucket* Instance();
private:
  BucketVec_t         buckets;     // The set of FIFO timer buckets
  Count_t             bucketSize;  // Size of each bucket in milliseconds
private:
  static TimerBucket* instance;
  static Count_t      scheduled;
  static Count_t      simScheduled; // Number actually scheduled in simualtor
  static Count_t      handled;
  static Count_t      cancelled;
  static Count_t      skipped;
  static Count_t      processed;
};

#endif
