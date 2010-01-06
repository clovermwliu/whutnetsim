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



// RngStream by Pierre L'Ecuyer, University of Montreal
// Adapted to GTNetS by George F. Riley, Georgia Tech.

#ifndef RNGSTREAM_H
#define RNGSTREAM_H
#include <string>

 
//Doc:ClassXRef
class RngStream {
public:
  RngStream ();
  //void bool SetPackageSeed (const unsigned long seed[6]);
  static bool CheckSeed(const unsigned long seed[6]);
  void ResetStartStream ();
  void ResetStartSubstream ();
  void ResetNextSubstream ();
  void SetAntithetic (bool a);
  void IncreasedPrecis (bool incp);
  bool SetSeed (const unsigned long seed[6]);
  void AdvanceState (long e, long c);
  void GetState (unsigned long seed[6]) const;
  double RandU01 ();
  long RandInt (long i, long j);
private:
  double Cg[6], Bg[6], Ig[6];
  bool anti, incPrec;
  //double nextSeed[6];
  double U01 ();
  double U01d ();
};

#endif
 

