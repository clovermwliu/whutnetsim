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



// Georgia Tech Network Simulator - Histogram Class
// George F. Riley.  Georgia Tech, Spring 2002

// Defines a Histogram and CDF statistics collection

#ifndef __histogram_h__
#define __histogram_h__

#include <iostream>
#include <vector>

#include "G_common_defs.h"
#include "statistics.h"

//Doc:ClassXRef
class Histogram : public Statistics {
  //Doc:Class Class {\tt Histogram} defines a statistics collection
  //Doc:Class object that tracks samplees in fixed size buckets
  //Doc:Class and counts the number of samples falling in each
  //Doc:Class bucket interval.
public:
  //Doc:Method
  Histogram();
    //Doc:Desc Default constructor, no arguments.  Not particularly
    //Doc:Desc useful, since the default constructor sets the maximum
    //Doc:Desc measurement to zero, resulting in all measurements
    //Doc:Desc being out of range.

  //Doc:Method
  Histogram(double);
    //Doc:Desc Construct with specified maximum measurement.
    //Doc:Arg1 Maximum measurement.

  //Doc:Method
  Histogram(double, Count_t, double = 0);
    //Doc:Desc Construct with specified maximum value and bin count.
    //Doc:Desc Optional third parameter with minimum value
    //Doc:Desc This is the preferred constuctor for Histograms.
    //Doc:Arg1 Maximum measurement.
    //Doc:Arg2 Desired bin count.
    //Doc:Arg2 Minimum measurement

  virtual ~Histogram();
  //Doc:Method
  void Bins(double, Count_t, double = 0); // Max, number of bins, min
    //Doc:Desc Specify the maximum value and bin count.
    //Doc:Arg1 Maximum measurement.
    //Doc:Arg2 Desired bin count.
    //Doc:Arg3 Minimum measurement.

  bool Record(double);
  bool Record(Time_t, double);
  void Reset();
  Statistics* Copy() const;   // Make a copy of the statistics

  //Doc:Method
  void Log(std::ostream&, const char* hdr = NULL, const char sep = ',');
    //Doc:Desc Record the statistics on an output stream.  The
    //Doc:Desc histogram is recorded with two values per line, the minimum 
    //Doc:Desc value for a bin, and the count  of measurements in that bin.
    //Doc:Arg1 Output stream to record  the  statistics on.
    //Doc:Arg2 Header line to add to beginning of the statistics log
    //Doc:Arg2 (if not {\tt nil}).
    //Doc:Arg3 Separator  character between statistics elements.

  // Log in CDF format   
  //Doc:Method
  void CDF(std::ostream&, const char* hdr = NULL, const char sep = ',');
    //Doc:Desc Record the statistics on an output stream, in Cumulative 
    //Doc:Desc Distribution Function (CDF) form.  The CDF is recorded with 
    //Doc:Desc two values per line, the maximum value for a bin, and the
    //Doc:Desc cumulative  probability that  the measurement is less than
    //Doc:Desc that value.
    //Doc:Arg1 Output stream to record  the  statistics on.
    //Doc:Arg2 Header line to add to beginning of the statistics log
    //Doc:Arg2 (if not {\tt nil}).
    //Doc:Arg3 Separator  character between statistics elements.

public:
  std::vector<Count_t> bins;
  double               min;
  double               max;
  Count_t              outOfRange;
};
#endif



