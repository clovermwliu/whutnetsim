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



// Circular message passing queue
// George F. Riley, Georgia Tech, Winter 2001

#ifndef __CQ_H__
#define __CQ_H__
#include <stddef.h>

// This defines the Queue management structure. All buffer pointers are
// relative to pBase. This is to allow this to work inshared memory
// regions, where the two processors will not have the same (necessarily)
// logical memory addresses for the common region.
typedef struct {
    size_t pFirst;
    volatile size_t pIn;
    volatile size_t pOut;
    size_t pLimit;
} CQueue;

#ifdef __cplusplus
extern "C" {
#endif
void   QSetBuf(CQueue*, size_t ofs, size_t l); // Set the data buffer address
size_t QAdvance(CQueue*, size_t, size_t); // Advance ptr by specified amt
size_t QData(CQueue* );  // How much data is in the buffer?
size_t QSpace(CQueue* ); // How much space is available in the buffer
void   QPut(CQueue*, char*, char*, size_t); // Put some data in the buffer
void   QGet(CQueue*, char*, char*, size_t); // Get some data from the buffer
size_t QDiff(CQueue*, size_t, size_t); // Distance from p1 to p2
// QPutN puts without chaning pIn, instead returns advance value
size_t QPutN(CQueue*, char*, char*, size_t, size_t);
// QGetN gets without chaning pOut, instead returns advance value
size_t QGetN(CQueue*, char*, char*, size_t, size_t);
// Debug Routines
void   QDump(CQueue*, char*); // Debug..
void   QDumpPtrs(CQueue*); // Debug..
void   QDumpPartial(CQueue*, char*, size_t, size_t);
#ifdef __cplusplus
}
#endif

#ifdef OLD_CLASS_VERSION
class CQueue {
  public :
    CQueue() : pFirst(0), pIn(0), pOut(0), pLimit(0) { };
    CQueue( char* b, size_t l ) : pFirst(b), pIn(b), pOut(b), pLimit(b+l) { };
    void SetBuf(char* buf, size_t l); // Set the data buffer address
    size_t Data();  // How much data is in the buffer?
    size_t Space(); // How much space is available in the buffer
    void   Put(char*, size_t); // Put some data in the buffer
    void   Get(char*, size_t); // Get some data from the buffer
  private :
    char* pFirst;
    char* pIn;
    char* pOut;
    char* pLimit;
};
#endif
#endif

