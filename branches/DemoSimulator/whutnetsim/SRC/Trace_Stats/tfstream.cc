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

// Define trace stream object

#include <stdio.h>

#include <iostream>

#include "tfstream.h"
#include "simulator.h"
#include "node.h"

using namespace std;

Tfstream::Tfstream() : pendingEOL(false), lastLogTime(0.0),  timePrecision(5),
                       node(0), ipDotted(true), digitsThreshold(10.0),
                       format("%7.5f")
{
}

void Tfstream::TimePrecision(Count_t d)
{
  char work[20];

  timePrecision = d; // Number of significant digits
  if (timePrecision > 10) timePrecision = 10; // Bound at 10 for sprintf
  sprintf(work, "%c%ld%c%ld%c", '%', timePrecision+2, '.', timePrecision, 'f');
  format = string(work);
}

// Output operators

Tfstream& Tfstream::operator<<(char c)
{
  CheckLogTime();
  (ostream&)*this << c;
  pendingEOL = true;
  return *this;
}

Tfstream& Tfstream::operator<<(const char *s)
{
  CheckLogTime();
  (ostream&)*this << s;
  pendingEOL = true;
  return *this;
}

Tfstream& Tfstream::operator<<(const void *p)
{
  CheckLogTime();
  (ostream&)*this << p;
  pendingEOL = true;
  return *this;
}

Tfstream& Tfstream::operator<<(int n)
{
  CheckLogTime();
  (ostream&)*this << n;
  pendingEOL = true;
  return *this;
}

Tfstream& Tfstream::operator<<(unsigned int n)
{
  CheckLogTime();
  (ostream&)*this << n;
  pendingEOL = true;
  return *this;
}

Tfstream& Tfstream::operator<<(long n)
{
  CheckLogTime();
  (ostream&)*this << n;
  pendingEOL = true;
  return *this;
}

Tfstream& Tfstream::operator<<(unsigned long n)
{
  CheckLogTime();
  (ostream&)*this << n;
  pendingEOL = true;
  return *this;
}

Tfstream& Tfstream::operator<<(double n)
{
  CheckLogTime();
  (ostream&)*this << n;
  pendingEOL = true;
  return *this;
}

Tfstream& Tfstream::operator<<(IPAddr& i)
{
  CheckLogTime();
  if (ipDotted) (ostream&)*this << ((string)i).c_str();
  else          (ostream&)*this << i;
  pendingEOL = true;
  return *this;
}

Tfstream& Tfstream::operator<<(MACAddr& m)
{
  CheckLogTime();
  (ostream&)*this << m;
  pendingEOL = true;
  return *this;
}

bool Tfstream::AppendEOL()
{
  if (pendingEOL)
    {
      (ostream&)*this << '\n';
      pendingEOL = false;
      return true; // appended
    }
  return false; // not needed
}

void Tfstream::NewNode(Node* n) // Log the node for protocol logging
{
  if (n == node) return;        // Same as existing
  AppendEOL();
  node = n;
}

void Tfstream::SetNode(Node* n) // Set node pointer for subsequent traces
{
  node = n;
}

// Private methods
void Tfstream::LogSimTime()
{
  char work[256];
  Time_t now = Simulator::Now();
  if (now >= digitsThreshold)
    { // Need to make new format
      Count_t newTot = 0;
      if      (now >= 1000000) newTot = 8 + timePrecision;
      else if (now >= 100000 ) newTot = 7 + timePrecision;
      else if (now >= 10000  ) newTot = 6 + timePrecision;
      else if (now >= 1000   ) newTot = 5 + timePrecision;
      else if (now >= 100    ) newTot = 4 + timePrecision;
      else if (now >= 10     ) newTot = 3 + timePrecision;
      else                     newTot = 2 + timePrecision;
      sprintf(work, "%c%ld%c%ld%c", '%', newTot, '.', timePrecision, 'f');
      format = string(work);
      switch (newTot - timePrecision) {
        case 8 :
          digitsThreshold = 10000000;
          break;
        case 7 :
          digitsThreshold = 1000000;
          break;
        case 6 :
          digitsThreshold = 100000;
          break;
        case 5 :
          digitsThreshold = 10000;
          break;
        case 4 :
          digitsThreshold = 1000;
          break;
        case 3 :
          digitsThreshold = 100;
          break;
        case 2 :
          digitsThreshold = 10;
          break;
      }
    }
  sprintf(work, format.c_str(), Simulator::Now());
  if (pendingEOL) (ostream&)*this << endl;
  (ostream&)*this << work;
  (ostream&)*this << " N";
  (ostream&)*this << node->Id();
  pendingEOL = true;
  lastLogTime = now;
}

