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



// Georgia Tech Network Simulator - Trace fstream object class
// George F. Riley.  Georgia Tech, Spring 2002

#ifndef __tfstream_h__
#define __tfstream_h__

#include <fstream>

#include "G_common_defs.h"
#include "ipaddr.h"
#include "macaddr.h"
#include "simulator.h"

class Node;

// Tfstream is a trace stream; similar to fstream with some extensions
//Doc:ClassXRef
class Tfstream : public std::fstream {
public:
  Tfstream();
public:
  Tfstream& operator<<(char c);
  Tfstream& operator<<(unsigned char c) { return (*this) << (char)c; }
  Tfstream& operator<<(signed char c) { return (*this) << (char)c; }
  Tfstream& operator<<(const char *s);
  Tfstream& operator<<(const unsigned char *s)
    { return (*this) << (const char*)s; }
  Tfstream& operator<<(const signed char *s)
    { return (*this) << (const char*)s; }
  Tfstream& operator<<(const void *p);
  Tfstream& operator<<(int n);
  Tfstream& operator<<(unsigned int n);
  Tfstream& operator<<(long n);
  Tfstream& operator<<(unsigned long n);
  Tfstream& operator<<(short n) {return operator<<((int)n);}
  Tfstream& operator<<(unsigned short n) {return operator<<((unsigned int)n);}
  Tfstream& operator<<(bool b) { return operator<<((int)b); }
  Tfstream& operator<<(double n);
  Tfstream& operator<<(float n) { return operator<<((double)n); }
  Tfstream& operator<<(IPAddr&);
  Tfstream& operator<<(MACAddr&);
public:
  bool AppendEOL();      // Append eol to current line if needed
  void NewNode(Node* n); // Log the node for protocol logging
  void SetNode(Node* n); // Set node pointer for subsequent traces
  void IPDotted(bool d) { ipDotted = d;}
  void TimePrecision(Count_t); // Set new precision
public:
  bool    pendingEOL;    // True if EndOfLine needed
  Time_t  lastLogTime;   // Simtime of last log
private:
  Count_t timePrecision; // Number significant digits on time in trace file
  Node* node;
  bool  ipDotted;        // True if trace IP Addresses with dotted notation
  Time_t digitsThreshold;// Time that sprintf format must change
  std::string format;    // Holds the current time format
private:
  inline void CheckLogTime()
    {
      if (is_open())
        {
          if (!pendingEOL || Simulator::Now() != lastLogTime)
            {
              LogSimTime();
            }
        }
    }
  void LogSimTime();
};

#endif
