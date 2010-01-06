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



// Georgia Tech Network Simulator - IPAddress Class
// George F. Riley.  Georgia Tech, Spring 2002

#include <iostream>
#include <vector>
#include <stdio.h>

#include "ipaddr.h"
#include "stringtok.h"

using namespace std;

static char dotted[16];    // Common buffer for dotted notation conversion

// Constructors

IPAddr::IPAddr(string& s) : ip(ToIP(s))
{
}

IPAddr::IPAddr(const char* c) : ip(ToIP(string(c)))
{
}

IPAddr::operator string() const
{
  return string(ToDotted(ip));
}

// Static methodds
IPAddr_t IPAddr::ToIP(const string s)
{
  unsigned long r = 0;
  vector<string> t;
  stringtok(t, s, ".");
  if (t.size() != 4)
    {
      cout << "Error in IP Dotted.  Expected a.b.c.d Found " << s << endl;
      return 0;
    }
  for (vector<string>::size_type i = 0; i < t.size(); ++i)
    { // Process each subfield
      int v = atol(t[i].c_str());
      r <<= 8;
      r += v;
    }
  return r;
}

char* IPAddr::ToDotted(IPAddr_t ip)
{
  sprintf(dotted, "%ld.%ld.%ld.%ld",
          (ip >> 24) & 0xff,
          (ip >> 16) & 0xff,
          (ip >> 8)  & 0xff,
          ip & 0xff);
  return dotted;
}

// Non-member operators
ostream& operator<<(ostream& os, const IPAddr& ip)
{
  if (ip.ip == IPAddrBroadcast) os << "BCAST";
  else os << (string)ip;
  return os;
}

