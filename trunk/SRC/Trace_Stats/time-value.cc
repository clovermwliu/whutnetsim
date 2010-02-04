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
// Implement a Time-Value statistics object
// George F. Riley, Georgia Tech, Fall 2004

#include "time-value.h"
#include "simulator.h"

using namespace std;

TimeValueStats::TimeValueStats()
{ // Nothing needed
}

TimeValueStats::~TimeValueStats()
{ // Nothing needed
}

bool TimeValueStats::Record(double v)
{  // For time/value stats, use Simulator::Now() as time
  return Record(Simulator::Now(), v);
}

bool TimeValueStats::Record(Time_t t, double v)
{  // Record a new valud
  timeValues.push_back(TimeValue(t, v));
  return true;
}

void TimeValueStats::Reset()
{
  timeValues.clear();
}

void TimeValueStats::Log(ostream& os, const char* hdr, const char sep)
{
  if (hdr) os << hdr << endl;
  for (TimeValueVec_t::iterator i = timeValues.begin();
       i != timeValues.end(); ++i)
    {
      os << i->time << sep << i->value << endl;
    }
}

Statistics*  TimeValueStats::Copy() const
{
  return new TimeValueStats(*this);
}




  
  
  
