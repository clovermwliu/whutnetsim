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



// Georgia Tech Network Simulator - Rate and Time Parsing classes
// George F. Riley.  Georgia Tech, Spring 2002

// Classes to allow rate and time specification with common abbreviations

#ifndef __ratetimeparse_h__
#define __ratetimeparse_h__

#include <string>

#include "G_common_defs.h"

//Doc:ClassXRef
class Rate {
  //Doc:Class Defines an object that can be used to define rates
  //Doc:Class (bits / second) in familiar units, such as 10Mb.
  //Doc:Class Objects of class {\tt Rate} have an automatic typecast
  //Doc:Class define to type {\tt Rate\_t}, so a {\tt Rate} object can be
  //Doc:Class used anywhere a {\tt Rate\_t} variable is needed.
public:
  //Doc:Method
  Rate(const std::string);  // Specify rate as a string with Mb, Kb, etc.
    //Doc:Desc Construct a {\tt Rate} object specifying a rate as a string.
    //Doc:Arg1 String specifying the rate.
    //Doc:Arg1 The rates can have the suffixes "b" (bits), "B" (bytes),
    //Doc:Arg1 "Kb" (kilobits), "KB" (kilobytes)
    //Doc:Arg1 "Mb" (megabits), "MB" (megabytes)
    //Doc:Arg1 "Gb" (gigabits), "GB" (gigabytes).

  Rate(const char*);        // Specify rate as a char* with Mb, Kb, etc.
public:
  operator Rate_t () const; // Provide automatic typecast to Rate_t
private:
  Rate_t Parse(const std::string); // Convert from string to rate_t
private:
  Rate_t rate;
};

//Doc:ClassXRef
class Time {
  //Doc:Class Defines an object that specifies time values with familiar
  //Doc:Class suffixes such as "ms".
  //Doc:Class Objects of class {\tt Time} have an automatic typecast
  //Doc:Class define to type {\tt Time\_t}, so a {\tt Time} object can be
  //Doc:Class used anywhere a {\tt Time\_t} variable is needed.
public:
  //Doc:Method
  Time(const std::string); // Specify time as a string with ms, us, etc.
    //Doc:Desc Construct a {\tt Time} object specifying a time as a string.
    //Doc:Arg1 String specifying the time.
    //Doc:Arg1 The times can have the suffixes "h" (hours), "m" (minutes),
    //Doc:Arg1 "s" (seconds), "sec" (seconds)
    //Doc:Arg1 "ms" (milliseconds), "us" (microseconds)
    //Doc:Arg1 "ns" (nanoseconds)
  Time(const char*);       // Specify time as a char* with ms, us, etc.
public:
  operator Time_t () const;       // Provide automatic typecast to Time_t
  operator std::string() const;   // Provide automatic typecast to string
private:
  Time_t Parse(const std::string);// Convert from string to Time_t
private:
  Time_t time;
};

#endif
