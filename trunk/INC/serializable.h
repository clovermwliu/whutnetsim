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



// Georgia Tech Network Simulator - Packet/PDU Serializaton
// George F. Riley.  Georgia Tech, Spring 2002

#ifndef __serializable_h__
#define __serializable_h__

#include "G_common-defs.h"
#include "macaddr.h"

// Defines an abstract base class for serializable objects
// Serialization occurs in distributed simulations when packets
// must be moved from one system to another.

class Packet;
//Doc:ClassXRef
class Serializable {
public:
  virtual Size_t SSize() = 0;          // Size needed for serialization
  virtual char*  Serialize(char*, Size_t&) = 0;// Serialize to a buffer
  virtual char*  Construct(char*, Size_t&) = 0;// Construct from buffer
  virtual ~Serializable() {};
public:
  // Helpers
  char*  SerializeToBuffer(char*, Size_t&, size_t&);
#if defined (Linux) || defined (SunOS) || defined (CYGWIN_NT_5_1)
  char*  SerializeToBuffer(char*, Size_t&, unsigned long&);
#endif
  char*  SerializeToBuffer(char*, Size_t&, unsigned short&);
  char*  SerializeToBuffer(char*, Size_t&, float&);
  char*  SerializeToBuffer(char*, Size_t&, double&);
  char*  SerializeToBuffer(char*, Size_t&, MACAddr&);
  char*  SerializeToBuffer(char*, Size_t&, char&);

  char*  ConstructFromBuffer(char*, Size_t&, size_t&);
#if defined (Linux) || defined (SunOS) || defined (CYGWIN_NT_5_1)
  char*  ConstructFromBuffer(char*, Size_t&, unsigned long&);
#endif
  char*  ConstructFromBuffer(char*, Size_t&, unsigned short&);
  char*  ConstructFromBuffer(char*, Size_t&, float&);
  char*  ConstructFromBuffer(char*, Size_t&, double&);
  char*  ConstructFromBuffer(char*, Size_t&, MACAddr&);
  char*  ConstructFromBuffer(char*, Size_t&, char&);

  // Static methods
  static char* GetSize(char*, Size_t&, Size_t&);      // Get the size field
  static char* PutSize(char*, Size_t&, Size_t&);      // Put the size field
  static char* Dump(char*, Size_t);                   // Dump for debugging
  static void  Validate(Packet*);                     // Debug validation
  // Private methods
private:
  static void  CheckSize(Size_t&, Size_t);  // Check if sufficient size left
};

#endif


