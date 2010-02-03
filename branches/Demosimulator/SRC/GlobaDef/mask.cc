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



// Georgia Tech Network Simulator - Manage IP Address Masks
// George F. Riley.  Georgia Tech, Spring 2002

#include "mask.h"

Mask_t RoutingMasks[Mask::MAX_MASK + 1] = {
  0x00000000, // Zero length mask not used
  0x80000000,
  0xc0000000,
  0xe0000000,
  0xf0000000,
  0xf8000000,
  0xfc000000,
  0xfe000000,
  0xff000000,
  0xff800000,
  0xffc00000,
  0xffe00000,
  0xfff00000,
  0xfff80000,
  0xfffc0000,
  0xfffe0000,
  0xffff0000,
  0xffff8000,
  0xffffc000,
  0xffffe000,
  0xfffff000,
  0xfffff800,
  0xfffffc00,
  0xfffffe00,
  0xffffff00,
  0xffffff80,
  0xffffffc0,
  0xffffffe0,
  0xfffffff0,
  0xfffffff8,
  0xfffffffc,
  0xfffffffe,
  0xffffffff};

// Constructor
Mask::Mask(Mask_t m) : mask(RoutingMasks[24]), nBits(24) // Default to 24 bit
{
  if (m <= MAX_MASK)
    {
      mask = RoutingMasks[m];  // Bit count specified
      nBits = m;
    }
  if (m >= RoutingMasks[1])
    {
      mask = m;         // Valid mask specified
      // Need to find bit count
      for (nBits = 1; nBits < MAX_MASK; ++nBits)
        {
          if (m == RoutingMasks[nBits]) break;
        }
    }
}

