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


//File Information: Scheduler class using STL map container
//File Name: scheduler-map.cc
//File Purpose: Event list management
//Original Author:  George F. Riley.
//Author Organization:  Georgia Tech
//Construct Data:  Fall 2005
//Modify Author:  YueYuntao
//Author Organization: WHU
//Modify Data: JAN, 2010
// Copyright 2004
// Dr. George F. Riley
// Georgia Tech Research Corporation
// Atlanta, Georgia 30332-0415
// All Rights Reserved

#include "scheduler-map.h"
#include "simulator.h"
#include "event.h"
#include "handler.h"

using namespace std;

MapScheduler::MapScheduler()
{ 
}
   
MapScheduler::~MapScheduler()
{ 
}

Scheduler* MapScheduler::Copy() const
{
  return new MapScheduler(*this);
}

//受保护的方法
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
{ //安排事件在经过时间t后触发
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

Event* MapScheduler::prDeQueue()		
//下一个事件（从队列中删除）
{
  EventMap_t::iterator i = eventList.begin();
  if (i == eventList.end()) return nil; //队列为空
  Event* p = i->second;
  eventList.erase(i);
  totrm++;
  evlSize--;
  return p;
}

Event* MapScheduler::prPeekEvent()
{ //返回下一个事件但不从队列中删除
  EventMap_t::iterator i = eventList.begin();
  if (i == eventList.end()) return nil; //队列为空
  return eventList.begin()->second;
}

bool MapScheduler::IsScheduled(Event* p )
{
  EventMap_t::iterator i = eventList.find(KeyPair(p->Time(), p->uid));
  return i != eventList.end();
}


