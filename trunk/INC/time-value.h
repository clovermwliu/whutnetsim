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

// Define a Time-Value statistics object
// George F. Riley, Georgia Tech, Fall 2004

#ifndef __time_value_h__
#define __time_value_h__

#include <vector>
#include <iostream>

#include "G_common-defs.h"
#include "statistics.h"

class TimeValue {
public:
  TimeValue() : time(0), value(0) {}
  TimeValue(Time_t t, double v) : time(t), value(v) {}
public:
  Time_t time;
  double value;
};

typedef std::vector<TimeValue> TimeValueVec_t;

class TimeValueStats : public Statistics {
public:
  TimeValueStats();
  virtual ~TimeValueStats();
public:
  bool Record(double); // Record the specified value
  bool Record(Time_t, double);  // Record the time and value
  void Reset();        // Clear all values
  // Log the recorded info
  void Log(std::ostream&, const char* = nil, const char = ',');
  Statistics* Copy() const;

private:
  TimeValueVec_t timeValues;
};

#endif
