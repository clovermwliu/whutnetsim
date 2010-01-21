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



/* Debugging Macros.
// George F. Riley, Georgia Tech, Summer 2000

// Debug is enabled with a bitmask, DEBUG_MASK
// Define DEBUG_MASK prior to calling G_debug.h
// If bit 0 is set, level 0 debugs are enabled.
// up to level 31.
//
// To completely remove all debug (more efficient)
// define NO_DEBUG prior to calling.
*/

#ifndef __DEBUG_H__
#define __DEBUG_H__

// Some qt includes define a DEBUG macro different from this one
#ifdef DEBUG
#undef DEBUG
#endif

#ifdef NO_DEBUG
#define DBG(x)
#define DEBUG(l,x)
#define DEBUG0(x)
#else
#ifndef DEBUG_MASK
#define DEBUG_MASK 0
#endif

#define DBM DEBUG_MASK

#define DBG(x) x
#define DEBUG(l,x) if((1<<l)&DBM) x 
#define DEBUG0(x)if(1&DBM) x 

#endif

#endif
