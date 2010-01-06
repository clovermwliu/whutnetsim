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

#ifndef __scheduler_map_h__
#define __scheduler_map_h__

#include <map>

#include "scheduler.h"

// Define the "map" of KeyPair to Event pointer
typedef std::map<KeyPair, Event*, key_less> EventMap_t;

//Doc:ClassXRef
class MapScheduler : public Scheduler {
  //Doc:Class Class {\tt MapScheduler} implements the event scheduler
  //Doc:Class using the STL {\tt map} container.
public:
  //Doc:Method
  MapScheduler();
    //Doc:Desc The constructor for the {\tt MapScheduler} object takes no
    //Doc:Desc arguments.


  virtual ~MapScheduler();

protected:   
  //Doc:Method
  virtual Scheduler* Copy() const;
    //Doc:Desc All scheduler subclasses must be able to copy themselves.
    //Doc:Return Pointer to the new copy.

  //Doc:Method
  virtual void prCancel(Event*);   // cancel event
    //Doc:Desc Cancels a pending event.
    //Doc:Arg1 A pointer to the event to be canceled.  When scheduling events
    //Doc:Arg1 that will later be canceled, a pointer to the event must
    //Doc:Arg1 be retained to allow cancellation.

  //Doc:Method
  virtual void prSchedule(Event*);     // schedule event
    //Doc:Desc Schedules a new event.  The simulation time for the
    //Doc:Desc event must be stored in the event class, member
    //Doc:Desc {\tt time}, by the caller prior to calling Schedule.  The
    //Doc:Desc handler for the event is also assumed to be sored
    //Doc:Desc in the event.
    //Doc:Arg1 A pointer to the event being scheduled.

  //Doc:Method
  virtual void prSchedule(Event*, Time_t);  // schedule event
    //Doc:Desc Schedules a new event.  The simulation time for the
    //Doc:Desc event is specified in the argument list and will
    //Doc:Desc be stored in the event class, member {\tt time},
    //Doc:Desc by this method.  The
    //Doc:Desc handler for the event must be already specified
    //Doc:Desc in the passed {\tt Event}.
    //Doc:Arg1 A pointer to the event being scheduled.
    //Doc:Arg2 A amount of time {\em in the future} for this event.
    //Doc:Arg2 Note the time is a relative time from the current simulation
    //Doc:Arg2 time. The actual time for the event is computed by adding the
    //Doc:Arg2 current simulation time to the value specified.

  //Doc:Method
  virtual void prSchedule(Event*, Time_t, Handler*);
    //Doc:Desc Schedules a new event.  The simulation time for the
    //Doc:Desc event is specified in the argument list and will
    //Doc:Desc be stored in the event class, member {\tt time},
    //Doc:Desc by this method.  The 
    //Doc:Desc handler for the event is also specified in the
    //Doc:Desc argument list.
    //Doc:Arg1 A pointer to the event being scheduled.
    //Doc:Arg2 A amount of time {\em in the future} for this event.
    //Doc:Arg2 Note the time is a relative time from the current simulation
    //Doc:Arg2 time. The actual time for the event is computed by adding the
    //Doc:Arg2 current simulation time to the value specified.
    //Doc:Arg3 A pointer to any object that is a subclass of class
    //Doc:Arg3 {\tt Handler}, specifying the event handler for this event.

  //Doc:Method
  virtual void prSchedule(Event*, Time_t, Handler&);
    //Doc:Desc Schedules a new event.  The simulation time for the
    //Doc:Desc event is specified in the argument list and will
    //Doc:Desc be stored in the event class, member {\tt time},
    //Doc:Desc by this method.  The 
    //Doc:Desc handler for the event is also specified in the
    //Doc:Desc argument list.
    //Doc:Arg1 A pointer to the event being scheduled.
    //Doc:Arg2 A amount of time {\em in the future} for this event.
    //Doc:Arg2 Note the time is a relative time from the current simulation
    //Doc:Arg2 time. The actual time for the event is computed by adding the
    //Doc:Arg2 current simulation time to the value specified.
    //Doc:Arg3 A reference to any object that is a subclass of class
    //Doc:Arg3 {\tt Handler}, specifying the event handler for this event.

  // Schedule an "early" event, near the front of the event list.
  // Some implementations can take advantage of this to be more efficient.
  // Not documented in manual at this point.
  virtual void prScheduleEarly(Event*, Time_t, Handler*);

  //Doc:Method
  virtual Event* prDeQueue();	     // get next event
    //Doc:Desc Removes the earliest pending event from the event queue
    //Doc:Desc and returns a pointer to it.
    //Doc:Return A pointer to the earliest pending event in the
    //Doc:Return event queue.

  //Doc:Method
  virtual Event* prPeekEvent();    // Peek at next event (do not remove)
    //Doc:Desc   Returns a pointer to the earliest pending event in the event
    //Doc:Desc   queue, but does not remove it from the queue.
    //Doc:Return A pointer to the earliest pending event in the
    //Doc:Return event queue.
public:
  //Doc:Method
  void    PrintStats();              // Print debug statistics
    //Doc:Desc Prints some fairly detailed statistics about the simulation.
    //Doc:Desc Normally would be called only after the 
    //Doc:Desc {\tt Run} method has exited and the simulation has completed.

  bool    IsScheduled(Event*);// Debug...see if event is scheduled

private:
  EventMap_t      eventList;   // The actual event list
};

#endif


