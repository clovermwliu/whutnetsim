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



// Georgia Tech Network Simulator - Common Definitions
// George F. Riley.  Georgia Tech, Spring 2002

#ifndef __common_defs_h__
#define __common_defs_h__

#include <string>

typedef double         Time_t;         // Simulation time
typedef unsigned long  SimulatorUid_t; // Unique ID for each event
typedef unsigned long  Event_t;        // Idenifies events in handler
typedef unsigned long  HandlerId_t;    // Unique ID for each handler
typedef unsigned long  Count_t;        // For counting things
typedef long           NCount_t;       // For counting things, with negative
typedef double         DCount_t;       // Floating point counts
typedef unsigned long  FId_t;          // For flow identifiers
typedef unsigned long  Size_t;         // For sizes
typedef double         Rate_t;         // Rate (bits/sec)
typedef unsigned long  IPAddr_t;       // IP Address
typedef unsigned long  Mask_t;         // IP Address masks
typedef unsigned long  Proto_t;        // Protocol identifier
typedef unsigned long  Priority_t;     // Priorities
typedef unsigned int   Layer_t;        // Protocol layer number
typedef unsigned long  Version_t;      // Which version of protocol
typedef unsigned long  NodeId_t;       // For identifying nodes
typedef unsigned long  PortId_t;       // Port number
typedef unsigned long  Bitmap_t;       // 32 bit flags
typedef unsigned short Byte_t;         // 8 bit quantity (not really..)
typedef unsigned short Word_t;         // 16 bit quantity
typedef unsigned long  Long_t;         // 32 but quantity
typedef unsigned long  Seq_t;          // Sequence numbers
typedef double         Mult_t;         // Dimensionless multiplier
typedef float          Weight_t;       // Link Cost Weight factor
typedef double         Meters_t;       // Meters for node locations
typedef unsigned long  Dist_t;         // For distances in routing
typedef unsigned long  SystemId_t;     // System identifier for distributed
typedef unsigned long  Color_t;        // For R/G/B components of pixels
typedef double         Angle_t;        // Angles (radians)
typedef double         Joules_t;       // Essentially a 3.6e6 factor to kWhr
typedef unsigned long  MPId_t;         // MPI process identifier

typedef unsigned long   Error_t;
typedef std::string     Error_str;

#define ALLBITS        ((Bitmap_t)0xffffffffl)
#define IPADDR_NONE    ((IPAddr_t)0x0)
#define NO_PORT        ((PortId_t)0x0)
#define MASK_ALL       ((Mask_t)0xffffffffl)
#define MAX_COUNT      ((Count_t)0xffffffffl)
#define NODE_NONE      ((NodeId_t)0xffffffffl)
#define INFINITE_TIME  ((Time_t)1e307)
#define INFINITE_VALUE 1e307
#define SPEED_LIGHT    ((Mult_t)299792458.0)

#define nil         	0

#define PRECISION	1e-10

#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 \
			+ __GNUC_PATCHLEVEL__)


#ifdef WIN32
typedef unsigned __int64 u_int64_t ;
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#endif
#endif
