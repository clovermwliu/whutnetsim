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



// Georgia Tech Network Simulator - Event class
// George F. Riley.  Georgia Tech, Spring 2002

#ifndef __event_h__
#define __event_h__

#include "G_debug.h"
#include "G_common_defs.h"
#include "G_globalstats.h"
#include "memreuse.h"

class Handler;

#define NO_EVENT 0xffffffff
//Doc:ClassXRef
class Event : public ReuseBase {
public:
  Event() 
    : event(NO_EVENT), uid(0), time(0), handler(0)
    {Stats::eventsAllocated++;}
  Event(Event_t ev) 
    : event(ev), uid(0), time(0), handler(0)
    {Stats::eventsAllocated++;}
  Event(Event_t ev, Handler* h) 
    : event(ev), uid(0), time(0), handler(h)
    {Stats::eventsAllocated++;}
  //Ìí¼Óhandler³ÉÔ±É¾³ýÓï¾ä
  virtual ~Event() 
  { 
	  Stats::eventsDeleted++; 
	  //delete handler; 
  };
  inline Time_t         Time() const { return time;}
  inline void           Time(Time_t t){  time = t; }
public:
  Event_t        event;
  SimulatorUid_t uid;
private:
  Time_t         time;
public:
  Handler*       handler;
};
#endif
