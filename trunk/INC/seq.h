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



// Georgia Tech Network Simulator - Manage 32 bit unsigned sequence numbers
// George F. Riley.  Georgia Tech, Spring 2002

// Class to manage arithmetic operations on sequence numbers (mod 2^32)

#ifndef __seq_h__
#define __seq_h__

#include "G_common-defs.h"

#define MAX_SEQ ((Seq_t)0xffffffff)

//Doc:ClassXRef
class Seq {
public:
  // Constructors
  Seq() : seq(0) { }
  Seq(const Seq_t s) : seq(s) { }
  // Typecast
  operator Seq_t() const { return seq;}
  // Assignment
  Seq& operator=(const Seq_t s) { seq = s; return *this;}
  // Arithmetic operators
  Seq& operator+=(const Seq_t s) { seq += s; return *this;}
  Seq  operator++()              { seq++; return *this;}
  Seq  operator++(int)           { Seq ss(seq); seq++; return ss;}
  Seq& operator-=(const Seq_t s) { seq -= s; return *this;}
  Seq  operator--()              { seq--; return *this;}
  Seq  operator--(int)           { Seq ss(seq); seq--; return ss;}
public:
  Seq_t seq;
};

// Comparison operators

bool operator< (const Seq l, const Seq r);
bool operator<=(const Seq l, const Seq r); 
bool operator> (const Seq l, const Seq r);
bool operator>=(const Seq l, const Seq r);

// Non-member arithmetic operators
Seq  operator+ (const Seq l, const Seq r);
Seq  operator- (const Seq l, const Seq r);

#endif


