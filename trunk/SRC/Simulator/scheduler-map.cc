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


// Georgia Tech Network Simulator - Scheduler class using STL map container
// George F. Riley.  Georgia Tech, Fall 2005

#include "scheduler-map.h"
#include "simulator.h"
#include "event.h"
#include "handler.h"

using namespace std;

MapScheduler::MapScheduler()
{ // Nothing needed
}
   
MapScheduler::~MapScheduler()
{ // Nothing needed
}

Scheduler* MapScheduler::Copy() const
{
  return new MapScheduler(*this);
}

// Protected methods
void MapScheduler::prCancel(Event* p)
{
  EventMap_t::iterator i = eventList.find(KeyPair(p->Time(), p->uid));
  if (i != eventList.end())
    {
      eventList.erase(i);
      totrm++;
      totevc++;
      evlSize--;
    }
}

void MapScheduler::prSchedule(Event* p)
{
  prSchedule(p, p->Time() - Simulator::Now());
}

void MapScheduler::prSchedule(Event* p, Time_t t)
{
  prSchedule(p, t, p->handler);
}

void MapScheduler::prSchedule(Event* p, Time_t t, Handler* h)
{ // Schedule event for t secs in future
  p->uid = uid++;
  p->Time(t + Simulator::Now());
  p->handler = h;
  eventList.insert(
             EventMap_t::value_type(KeyPair(p->Time(), p->uid), p));
  evlSize++;
  totevs++;  
  if ( maxSize < evlSize) maxSize = evlSize;
}

void MapScheduler::prSchedule(Event* p, Time_t t, Handler& h)
{
  prSchedule(p, t, &h);
}

void MapScheduler::prScheduleEarly(Event* p, Time_t t, Handler* h)
{ // Schedule event for t secs in future, using STL hint
  // Further testing shows this makes no difference, so just
  // use normal schedule.
  Schedule(p, t, h);
}

Event* MapScheduler::prDeQueue()		// next event (removes from q)
{
  EventMap_t::iterator i = eventList.begin();
  if (i == eventList.end()) return nil; // Empty on deque
  Event* p = i->second;
  eventList.erase(i);
  totrm++;
  evlSize--;
  return p;
}

Event* MapScheduler::prPeekEvent()
{ // Return next event without removal
  EventMap_t::iterator i = eventList.begin();
  if (i == eventList.end()) return nil; // Empty on deque
  return eventList.begin()->second;
}

bool MapScheduler::IsScheduled(Event* p )
{
  EventMap_t::iterator i = eventList.find(KeyPair(p->Time(), p->uid));
  return i != eventList.end();
}


