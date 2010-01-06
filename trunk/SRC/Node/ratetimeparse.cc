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



// Georgia Tech Network Simulator - Rate and Time Parsing classes
// George F. Riley.  Georgia Tech, Spring 2002

// Classes to allow rate and time specification with common abbreviations

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

#include "ratetimeparse.h"

Rate::Rate(const string s)
{
  rate = Parse(s);
}

Rate::Rate(const char* s)
{
  rate = Parse(s);
}

Rate::operator Rate_t () const
{
  return rate;
}

Rate_t Rate::Parse(const string s)
{
  string::size_type n = s.find_first_not_of("0123456789.");
  if (n != string::npos)
    { // Found non-numeric
      string numeric = s.substr(0, n);
      string trailer = s.substr(n, string::npos);
      Rate_t r = atof(numeric.c_str());
      if (trailer == string("b")) return r;     // Bits
      if (trailer == string("B")) return r * 8.0; // Bytes
      if (trailer == string("kb"))return r * 1000.0; // KiloBits
      if (trailer == string("Kb"))return r * 1000.0; // KiloBits
      if (trailer == string("KB"))return r * 8000.0; // KiloBytes
      if (trailer == string("Mb"))return r * 1000000.0; // MegaBits
      if (trailer == string("MB"))return r * 8000000.0; // MegaBytes
      if (trailer == string("Gb"))return r * 1000000000.0; // GigaBits
      if (trailer == string("GB"))return r * 8000000000.0; // GigaBytes
      cout << "Can't parse rate " << s << endl;
      exit(0);
    }
  return atof(s.c_str()); // No special formatting, just convert value
}

// Time conversion
Time::Time(const string s)
{
  time = Parse(s);
}

Time::Time(const char* s)
{
  time = Parse(s);
}

Time::operator Time_t () const
{
  return time;
}

Time::operator string () const
{
  char work[64];
  // ! need to add significant digits class var
  sprintf(work, "%8.4f", time);
  return string(work);
}

Time_t Time::Parse(const string s)
{
  string::size_type n = s.find_first_not_of("0123456789.");
  if (n != string::npos)
    { // Found non-numeric
      string numeric = s.substr(0, n);
      string trailer = s.substr(n, string::npos);
      Time_t r = atof(numeric.c_str());
      if (trailer == string("h")) return r * 3600.0;       // Hours
      if (trailer == string("m")) return r * 60.0;         // Minutes
      if (trailer == string("s")) return r;                // Seconds
      if (trailer == string("sec")) return r;              // Seconds
      if (trailer == string("ms"))return r / 1000.0;       // Milliseconds
      if (trailer == string("us"))return r / 1000000.0;    // Microseconds
      if (trailer == string("ns"))return r / 1000000000.0; // Nanoseconds
      cout << "Can't parse time " << s << endl;
      exit(0);
    }
  return atof(s.c_str()); // No special formatting, just convert value
}

