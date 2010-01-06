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



// Georgia Tech Network Simulator - Circular Queue for Sequential Events
// George F. Riley.  Georgia Tech, Fall 2002

// Defines an efficient circular queue for events that are inserted
// and removed in timestamp order.

#ifndef __eventcq_h__
#define __eventcq_h__

#include <vector>

#include "G_common-defs.h"
#include "simulator.h"

typedef std::vector<EventPair*> EventCQVec_t;

class EventCQ {
public:
  typedef enum {DefaultInitialSize = 4} EvCQSize_t;
public:
  EventCQ();

  bool empty()
  {
    return in == out;
  }

  void push_back(EventPair* e)         // Add a new entry (grow size if needed)
  {
    timeLastIn = e->time;
    EventCQVec_t::size_type newin = NextIn();
#ifdef DONT_NEED
    // Don't need this since we grow the queue if needed after then enqueue
    if (newin == out)
      {
        Grow();               // Need more room
        newin = NextIn();
      }
#endif
    events[in] = e;           // Store at the in location
    in = newin;               // Advance in
    if (full()) Grow();       // Insure room for next
  }

  void pop_front()                     // Remove the front entry
    { // Remove front element
      out = NextOut(); 
    }

  EventPair*   front()                 // Return the front entry
  { // Get a reference to the front object
    return events[out];
  }

  EventPair*   next()                  // Next entry to be pushed
  {
    return events[in];
  }

  Count_t      size()                  // Number of entries in the list
  {
    if (in >= out) return in - out;
    return limit - (out - in);
  }

  bool         full()                  // True if no more room
  {
    return (NextIn() == out);
  }
  
  EventPair*   operator[](Count_t i)   // Index as array
  {
    EventCQVec_t::size_type k = out + i;
    if(k >= limit) k-=limit;
    return events[k];
  }

private:
  EventCQVec_t::size_type NextIn()     // Get advanced in pointer
  { // Get the advanced in pointer, modulo limit
    EventCQVec_t::size_type r = in + 1;
    if (r != limit) return r;
    return 0;
  }

  EventCQVec_t::size_type NextOut()    // Get advanced out pointer
  { // Get the advanced out pointer, modulo limit
    EventCQVec_t::size_type r = out + 1;
    if (r != limit) return r;
    return 0;
  }

  void       Grow();                   // Increase size by power of 2
private:
  public: // debug..change later
  EventCQVec_t            events;      // Pending events in the circular queue
  EventCQVec_t::size_type limit;       // Current size (always a power of 2)
  EventCQVec_t::size_type in;          // In pointer
  EventCQVec_t::size_type out;         // Out pointer
  Time_t timeLastIn;
};

#endif


