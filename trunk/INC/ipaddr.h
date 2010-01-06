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

#ifndef __ipaddr_h__
#define __ipaddr_h__

#include <string>
#include <algorithm>

#include "G_common-defs.h"

#define IPAddrBroadcast (IPAddr_t)0xffffffff

//Doc:ClassXRef
class IPAddr {
  //Doc:Class Defines an object that specifies an \IPA\ using the familiar
  //Doc:Class "dotted" location.  Objects of class {\tt IPAddr} have
  //Doc:Class an automatic  typecast to type {\tt IPAddr\_t}, so an
  //Doc:Class {\tt IPAddr} object can be used whenever a variable of
  //Doc:Class {\tt IPAddr\_t} is needed.
public:
  //Doc:Method
  IPAddr() : ip(0) { }
    //Doc:Desc Constructor for {\tt IPAddr}.  The \IPA\ defaults to
    //Doc:Desc \IPAN\ (0).

  //Doc:Method
  IPAddr(std::string&);  // Construct from dotted notation
    //Doc:Desc Construct an {\tt IPAddr} object using a string in the
    //Doc:Desc familiar "dotted" notation.
    //Doc:Arg1 String value specifying the desired \IPA\ in dotted notation.

  //Doc:Method
  IPAddr(IPAddr_t i) : ip(i) { }
    //Doc:Desc Construct an {\tt IPAddr} object with the specified
    //Doc:Desc 32 bit address.
    //Doc:Arg1 \IPA\ as a 32 bit unsigned quantity.

  IPAddr(const char*);    // Construct from char to string
         operator IPAddr_t() const { return ip;}  // Typecast to unsigned long
         operator std::string() const;            // Typecast to a string
  bool   operator==(const IPAddr& i) const { return ip == i.ip;}
  bool   operator< (const IPAddr& i) const { return ip < i.ip;}

  //Doc:Method
  static IPAddr_t ToIP(const std::string);
    //Doc:Desc Convert a string (dotted notation) \IPA\ to 32 bit unsigned.
    //Doc:Arg1 String to convert.
    //Doc:Return 32 bit unsigned \IPA.

  //Doc:Method
  static char*    ToDotted(IPAddr_t);
    //Doc:Desc Convert a 32 bit \IPA\ to a string in dotted notation.
    //Doc:Desc \IPA\ to convert.
    //Doc:Arg1 Pointer to ASCIIZ string with the \IPA\ in dotted notation.
    //Doc:Return The buffer pointed to is only valid until another
    //Doc:Return call to {\tt IPAddr} functions.

public:
  IPAddr_t ip;
};

// Define a pair for IPAddress/Port
typedef std::pair<IPAddr_t, PortId_t> IPPort_t;

// Define an output operator
std::ostream& operator<<(std::ostream&, const IPAddr&);

#endif


