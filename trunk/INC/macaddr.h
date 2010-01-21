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



// Georgia Tech Network Simulator -  MAC Address Class
// George F. Riley.  Georgia Tech, Spring 2002

#ifndef __macaddr_h__
#define __macaddr_h__

// Defines a class for managing 48 bit MAC addresses

#include <iostream>

#include "G_common_defs.h"

// GFR Removed the silly 48 bit representations.
// 32 bits is enough!
typedef unsigned long MACAddr_t;

//Doc:ClassXRef
class MACAddr {
public:
public:
  MACAddr();
  MACAddr(MACAddr_t l);
  //MACAddr(MACAddr_t u, MACAddr_t l);
  MACAddr(const MACAddr& c);
  // Typecasts
  //operator std::string() const;          // Typecast to a string (removed)
  operator MACAddr_t() const;              // Get the 32 bit value
  // Define some comparison operators
  bool   operator==(const MACAddr& r) const;
  // Define some arithmetic operators
  // Addition advances lower only, ignores overflow
  MACAddr operator++();
  MACAddr operator++(int);
  // Member access
  //MACAddr_t Upper() const;
  //MACAddr_t Lower() const;
  //void      Upper(MACAddr_t);
  //void      Lower(MACAddr_t);
  //void      IncUpper();    // Increment upper
  bool      IsBroadcast() const; // True if address is broadcast address
  void      SetBroadcast();// Set to broadcast address
public:
  MACAddr_t macAddr;
private:
  static char work[20];       // Enough for 32 bit hex and slight fudge
public:
  static MACAddr   Allocate();  // Allocate a new MAC Range
  static MACAddr   Broadcast(); // Create a broadcast mac address
  static MACAddr_t next;        // Next available value
  static MACAddr_t NONE;        // Define a "NONE" for comparision
};

// Non-Member operators

std::ostream& operator<<(std::ostream&, const MACAddr&); // Output to a stream
bool operator< (const MACAddr& l, const MACAddr& r);
bool operator<=(const MACAddr& l, const MACAddr& r); 
bool operator> (const MACAddr& l, const MACAddr& r);
bool operator>=(const MACAddr& l, const MACAddr& r);

// Non-Member Arithmetic operators
MACAddr operator-(const MACAddr& l, const MACAddr& r);
MACAddr operator+(const MACAddr& l, const Count_t  r);

#endif
