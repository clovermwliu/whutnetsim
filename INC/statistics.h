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



// Georgia Tech Network Simulator - General Purpose Statistics base class
// George F. Riley.  Georgia Tech, Spring 2002

// Defines a general purpose statistics base class

#ifndef __statistics_h__
#define __statistics_h__

//Doc:ClassXRef
class Statistics {
  //Doc:Class Class {\tt Statistics} defines a virtual base class that
  //Doc:Class defines the behavior  of  all statistics collection objects
  //Doc:Class in \GTNS.
public:
  //Doc:Method
  virtual bool Record(double) = 0; // Record the specified value
    //Doc:Desc Record a new statistics sample.
    //Doc:Arg1 Sample to record.
    //Doc:Return True if recorded ok.

  //Doc:Method
  virtual bool Record(Time_t, double) = 0; // Record value at a specified time
    //Doc:Desc Records a value associated with a timestamp
    //Doc:Arg1 Timestamp of value
    //Doc:Arg2 Corresponding value
    //Doc:Return True if recorded ok.

  //Doc:Method
  virtual void Reset() = 0;        // Reset
    //Doc:Desc Remove all samples from the statistics collector and restore
    //Doc:Desc initial conditions.

  //Doc:Method
  virtual void Log(std::ostream&,
                   const char* hdr = nil,
                   const char sep = ',') = 0; // Log the recorded info
    //Doc:Desc Record the statistics on an output stream.
    //Doc:Arg1 Output stream to record  the  statistics on.
    //Doc:Arg2 Header line to add to beginning of the statistics log
    //Doc:Arg2 (if not {\tt nil}).
    //Doc:Arg3 Separator  character between statistics elements.

  virtual Statistics* Copy() const = 0;       // Make a copy of the statistics

  virtual ~Statistics() {};
};

#endif

