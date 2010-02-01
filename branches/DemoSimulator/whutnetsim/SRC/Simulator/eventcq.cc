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


// Georgia Tech Network Simulator - Circular Queue for Sequential Events
// George F. Riley.  Georgia Tech, Fall 2002

// Defines an efficient circular queue for events that are inserted
// and removed in timestamp order.

//#define DEBUG_MASK 0x01
#include "G_debug.h"
#include "eventcq.h"

using namespace std;

// Constructor
EventCQ::EventCQ() 
  : events(DefaultInitialSize, nil),
    limit(DefaultInitialSize), in(0), out(0),
    timeLastIn(0)
{
}

EventCQ::~EventCQ()
//添加了EVENTCQ的析构函数
{
	EventCQVec_t::iterator iter;
	for ( iter=events.begin(); iter!=events.end(); iter++ )
	{
		delete *iter;
	}
}

#ifdef  MOVED_TO_H
// Data access functions
void EventCQ::push_back(EventPair* e)
{
#ifdef EXTRA_CHECK
  if (e->time < timeLastIn)
    {
      cout << "HuH?  EventCQ::push_back out of order" << endl;
      cout << "Time " << e->time << " lastIn " << timeLastIn << endl;
      exit(1);
    }
#endif
  timeLastIn = e->time;
  EventCQVec_t::size_type newin = NextIn();
  if (newin == out)
    {
      Grow();               // Need more room
      newin = NextIn();
    }
  events[in] = e;           // Store at the in location
  in = newin;               // Advance in
}

void EventCQ::pop_front()
{ // Remove front element
  out = NextOut(); 
}

EventPair* EventCQ::front()
{ // Get a reference to the front object
  return events[out];
}

EventPair* EventCQ::next()
{
  return events[in];
}

Count_t EventCQ::size()
{
  if (in >= out) return in - out;
  return limit - (out - in);
}

EventPair* EventCQ::operator[](Count_t i)
{
  EventCQVec_t::size_type k = out + i;
  while(k >= limit) k-=limit;
  return events[k];
}

// Private Methods
EventCQVec_t::size_type EventCQ::NextIn()
{ // Get the advanced in pointer, modulo limit
  EventCQVec_t::size_type r = in + 1;
  if (r != limit) return r;
  return 0;
}

EventCQVec_t::size_type EventCQ::NextOut()
{ // Get the advanced out pointer, modulo limit
  EventCQVec_t::size_type r = out + 1;
  if (r != limit) return r;
  return 0;
}
#endif
  
void EventCQ::Grow()
{ // Increase size to next power of 2
  EventCQVec_t::size_type oldLim = limit;
#undef  VERBOSE
#ifdef  VERBOSE
  cout << "eventcq " << this << " dumping orig before grow" << endl;
  cout << "In " << in << " out " << out << endl;
  for(EventCQVec_t::size_type i = 0; i < limit; ++i)
    {
      if (events[i])
        {
          cout << "time " << events[i]->time 
               << " event " << events[i]->event << endl;
        }
      else
        {
          cout << "nil";
          
        }
    }
#endif
  limit *= 2;
  DEBUG0((cout << "Growing EvenvCQ to size " << limit << endl));
  events.reserve(limit); // Reserve new space
  //EventPair evp(0.0, nil);
  events.insert(events.end(),  oldLim, nil);
  if (out != 0)
    { // out == 0 is simplest case, no moving around needed
      for (EventCQVec_t::size_type i = 0; i != in; ++i)
        { // Move data to newly reserved locations
          events[oldLim + i] = events[i];
          events[i] = nil;
        }
      in = oldLim + in;
    }
#ifdef VERBOSE
  cout << "Dumping orig after grow" << endl;
  for(EventCQVec_t::size_type i = 0; i < limit; ++i)
    {
      if (events[i])
        {
          cout << "time " << events[i]->time
               << " event " << events[i]->event << endl;
        }
      else
        {
          cout << "nil" << endl;
        }
    }
  cout << "New In " << in << " out " << out << endl;
#endif
}
