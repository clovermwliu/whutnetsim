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



// Georgia Tech Network Simulator - Timer Class
// George F. Riley.  Georgia Tech, Spring 2002

#ifndef __timer_h__
#define __timer_h__

#include <list>
#include <deque>
#include <vector>

#include "G_common_defs.h"
#include "handler.h"
#include "event.h"

class Timer;
class TimerHandler;

//typedef std::list<TimerEvent*> FifoEventList_t;       // FIFO event list
typedef std::deque<TimerEvent*> FifoEventList_t;       // FIFO event list

class TimerEvent : public Event {
public:
  TimerEvent() : Event(), timerHandler(0), timer(0),
    remainingTime(0), evList(nil), cancelled(false)
    {Stats::timersAllocated++;}
  TimerEvent(Event_t ev) 
    : Event(ev), timerHandler(0), timer(0), remainingTime(0),
    evList(nil), cancelled(false)
    {Stats::timersAllocated++;}
  virtual ~TimerEvent() {Stats::timersAllocated--;};
public:
  TimerHandler*    timerHandler;
  Timer*           timer;
  Time_t           remainingTime;
  FifoEventList_t* evList;  // FIFO event list containing event, if bucketed
  bool             cancelled;
};

class TimerHandler {
public:
  virtual void Timeout(TimerEvent*) = 0;
  virtual ~TimerHandler() {}
};

class Timer : public Handler, public TimerHandler {
public:
  Timer();

public:
  void Handle(Event*, Time_t);
  virtual void Timeout(TimerEvent*);  // Override if needed
  void Schedule(TimerEvent*, Time_t, TimerHandler*); // Schedule an event
  void Schedule(TimerEvent*, Time_t); // Schedule an event, self handler
  void Cancel(TimerEvent*);
  void Pause(TimerEvent*);
  void Resume(TimerEvent*);
  void ResumeAfter(TimerEvent* ev, Time_t time);
  void Resume(TimerEvent* , TimerHandler*);
};

#endif
