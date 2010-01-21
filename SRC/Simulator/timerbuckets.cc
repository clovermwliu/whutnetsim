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



// Bucketed Timers for GTNetS
// George F. Riley, Georgia Tech, Winter 2002

// Bucketed timers use a fifo queue for all timer events for a given "k"
// milliseconds in the future.  This allows the main event list to remain
// relatively small.

#include "timerbuckets.h"
#include "simulator.h"

using namespace std;

#ifdef OLD_NOT_USED
// TimerBucketEvent methods
TBucketEvent::TBucketEvent() : client(nil), clientEvent(nil),
                               evList(nil), expiration(0),
                               cancelled(false)

{
}

TBucketEvent::TBucketEvent(TimerHandler* h, TimerEvent* e,
                           FifoEventList_t* f, Time_t t)
  : client(h), clientEvent(e), evList(f), expiration(t), cancelled(false)
{
}
#endif

// TimerBucket static variables
TimerBucket* TimerBucket::instance;  // A single instance for each simulation
Count_t      TimerBucket::scheduled;
Count_t      TimerBucket::simScheduled; // Number scheduled in simualtor
Count_t      TimerBucket::handled;
Count_t      TimerBucket::cancelled;
Count_t      TimerBucket::skipped;
Count_t      TimerBucket::processed;

// TimerBucket methods
TimerBucket::TimerBucket() : bucketSize(10) // Default 10ms
{
}

void TimerBucket::Handle(Event* e, Time_t t)
{
  handled++;
  TimerEvent* ev = (TimerEvent*)e;
  FifoEventList_t* evl = ev->evList;
  Time_t now = Simulator::Now();
#define EXTRA_CHECKS
#ifdef EXTRA_CHECKS
  // Extra checks for debugging, remove later
  if (!evl->size())
    {
      cout << "HuH?  TimerBucket::Handle w/ empty evl" << endl;
      exit(1);
    }
#endif
  TimerEvent* tme = evl->front();
#ifdef EXTRA_CHECKS
  // More extra checks
  if (tme != ev)
    {
      cout << "HuH?  TimerBucket::Handle, ev mismatch" << endl;
      exit(1);
    }
#endif
  evl->pop_front(); // Remove current
  // Find next sequential event to schedule
  while (!evl->empty())
    { // Check for any new events to schedule
      tme = evl->front();
      if (!tme->cancelled)
        { // Found one, schedule it and exit
          Scheduler::Schedule(tme, tme->Time() - now, this);
          simScheduled++;
          break;
        }
      // Event is cancelled, remove and delete
      evl->pop_front();
      delete tme; // Event is cancelled, just delete it
    }
  // Pass current event to handler if not cancelled
  ev->evList = nil; // Note no longer scheduled (in case cancelled again)
  if (!ev->cancelled)
    { // Call the original event handler if not cancelled
      processed++;
      ev->timerHandler->Timeout(ev);
    }
  else
    { // otherwise delete it
      skipped++;
      delete ev;
    }
}

void TimerBucket::BucketSize(Count_t ms)
{
  if (buckets.size())
    {
      cout << "TimerBucket::BucketSize called on non-empty buckets. Aborting."
           << endl;
      exit(1);
    }
  bucketSize = ms;
}

void TimerBucket::Schedule(TimerEvent* ev, Time_t t, TimerHandler* h)
{
  Time_t  now = Simulator::Now();
  Count_t futureMs = (Count_t)(t * 1000.0); // Convert to milliseconds
  Count_t bucket = 0;
  if (futureMs)
    { // 1..10 go in bucket 0, 11..20 in bucket 1, etc
      // assuming bucketSize is 10
      bucket = (futureMs - 1) / bucketSize;
    }
  while (buckets.size() <= bucket)
    { // Expand size of bucket vector to include this one
      buckets.push_back(nil);
    }
  FifoEventList_t* evList = buckets[bucket];
  if (evList == nil)
    { // Need new one
      evList = new FifoEventList_t;
      buckets[bucket] = evList;
    }
  Time_t futureSec = (Time_t)((bucket + 1) * bucketSize) / 1000.0;
  bool empty = evList->empty();
  ev->evList = evList;
  ev->timerHandler = h;
  ev->Time(futureSec + now);
  evList->push_back(ev); // Add to fifo queue
  if (empty)
    { // Need to schedule an event to the main event queue
      Scheduler::Schedule(ev, futureSec, this);
      simScheduled++;
    }
  scheduled++;
}

void TimerBucket::Cancel(TimerEvent* ev)
{
  ev->cancelled = true;
  cancelled++;
}

void TimerBucket::DBDump()
{
  cout << "TimerBucket Stats" << endl;
  cout << "scheduled " << scheduled;
  cout << " simScheduled " << simScheduled;
  cout << " handled " << handled;
  cout << " cancelled " << cancelled;
  cout << " skipped " << skipped;
  cout << " processed " << processed;
  cout << endl;
}

// Static methods

TimerBucket* TimerBucket::Instance()
{
  if (!instance) instance = new TimerBucket();
  return instance;
}

