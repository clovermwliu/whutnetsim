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



// Georgia Tech Network Simulator -  MAC Address Class
// George F. Riley.  Georgia Tech, Spring 2002

#include <stdio.h>

#include "macaddr.h"

using namespace std;

// Static members
MACAddr_t MACAddr::next = 1; // Next available MAC
MACAddr_t MACAddr::NONE = 0;
char    MACAddr::work[20];

// Constructors
MACAddr::MACAddr()
  : macAddr(0)
{
}

MACAddr::MACAddr(MACAddr_t l)
  : macAddr(l)
{
}

MACAddr::MACAddr(const MACAddr& c) 
    : macAddr(c.macAddr)
{
}

// Typecast to string
#ifdef OLD_HEX
MACAddr::operator std::string() const
{
  string r("0x");
  if (Upper()) 
    {
      sprintf(work, "%lx", Upper());
      r += string(work);
    }
  sprintf(work, "%lx", MacAddr());
  return r + string(work);
}
#endif

// Changed to decimal by GFR Mar 8,2004
// Removed by GFR Jan 19, 2005.  See the << operator below
#ifdef REMOVED_BY_GFR
MACAddr::operator std::string() const
{
  sprintf(work, "%ld", macAddr);
  return string(work);
}
#endif

// Typecast to MACAddr_t
MACAddr::operator MACAddr_t() const
{
  return macAddr; // Just macAddr bits
}

// Equivalence test
bool   MACAddr::operator==(const MACAddr& r) const
{
  return macAddr == r.macAddr;
 ;
}

// De fine some arithmetic operators
MACAddr MACAddr::operator++()
{
  macAddr++; 
 return *this;
}

MACAddr MACAddr::operator++(int)
{ 
  MACAddr ma(macAddr);
  ++macAddr; 
  return ma;
}

// Member access
bool MACAddr::IsBroadcast() const
{
  return (macAddr == 0xffffffff);
}

void MACAddr::SetBroadcast()
{
  macAddr = 0xffffffff;
}

// Non-Member operators

ostream&  operator<< (ostream& os, const MACAddr& m)
{
  if (m.IsBroadcast()) os << "BCAST";
  else os << m.macAddr;
  return os;
}

bool operator< (const MACAddr& l, const MACAddr& r)
{
  return (l.macAddr < r.macAddr);
}

bool operator<=(const MACAddr& l, const MACAddr& r)
{
  return (l.macAddr <- r.macAddr);
}

bool operator> (const MACAddr& l, const MACAddr& r)
{
  return (l.macAddr > r.macAddr);
}

bool operator>=(const MACAddr& l, const MACAddr& r)
{
  return (l.macAddr >= r.macAddr);
}

MACAddr operator-(const MACAddr& l, const MACAddr& r)
{
  return MACAddr(l.macAddr - r.macAddr);
}

MACAddr operator+(const MACAddr& l, const Count_t r)
{
  return MACAddr(l.macAddr + r);
}


// Static functions
MACAddr MACAddr::Allocate()
{ // Allocate a new MAC address
  return MACAddr(next++);
}

MACAddr MACAddr::Broadcast()
{
  MACAddr r;
  r.SetBroadcast();
  return r;
}

  
