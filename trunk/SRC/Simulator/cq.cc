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


// Circular message passing queue
// George F. Riley, Georgia Tech, Winter 2001

#include <iostream>
#include <stdio.h>
#include <string.h>

#include "cq.h"

using namespace std;

void QSetBuf(CQueue* pQ, size_t ofs, size_t l)
{ // Set the buffer address
  //  pQ->pBase = pBase;
  pQ->pFirst = pQ->pIn = pQ->pOut = ofs;
  pQ->pLimit = pQ->pFirst + l;
}

size_t QAdvance(CQueue* pQ, size_t ptr, size_t amt)
{
size_t r = ptr + amt;

  while(r >= pQ->pLimit) r -= (pQ->pLimit - pQ->pFirst);
  return r;
}

size_t QData(CQueue* pQ)
{ // Compute amount of data in buffer
  if (pQ->pIn >= pQ->pOut) return(pQ->pIn - pQ->pOut);
  return (pQ->pLimit - pQ->pFirst + pQ->pIn - pQ->pOut);
}

size_t QSpace(CQueue* pQ)
{ // Compute amount of space in buffer
  if(pQ->pOut > pQ->pIn) return(pQ->pOut - pQ->pIn - 1);
  return (pQ->pLimit - pQ->pFirst + pQ->pOut - pQ->pIn - 1);
}

void QPut(  CQueue* pQ, char* pBase, char* pD, size_t l)
{ // Copy data to buffer (caller must insure enough space)

  // Just call QPutN to do the hard work
  pQ->pIn = QPutN(pQ, pBase, pD, l, pQ->pIn);
}

void QGet( CQueue* pQ, char* pBase, char* pD, size_t l)
{// Copy data from buffer

  // Just call QGetN to do the hard work
  pQ->pOut = QGetN(pQ, pBase, pD, l, pQ->pOut);
}

size_t QDiff(  CQueue* pQ, size_t p1, size_t p2)
{
  if (p2 >= p1) return(p2 - p1);
  return (pQ->pLimit - pQ->pFirst) - (p1 - p2);
}

size_t QPutN(  CQueue* pQ, char* pBase, char* pD, size_t l, size_t pIn)
{ // Copy data to buffer (caller must insure enough space)
  // Do NOT update IN. This allows a message to be written to a buffer
  // piecemeal, with IN updating only on the last call.
size_t toend = pQ->pLimit - pIn;
size_t pNewIn = pIn;

  if (toend > l) toend = l;
  memcpy(pBase + pNewIn, pD, toend);
  l -= toend;
  pD += toend;
  pNewIn += toend;
  if (pNewIn == pQ->pLimit) pNewIn = pQ->pFirst;
  if (l)
    { // More to copy
      memcpy(pBase + pNewIn, pD, l);
      pNewIn += l;
    }
  return pNewIn;
}

size_t QGetN( CQueue* pQ, char* pBase, char* pD, size_t l, size_t pOut)
{ // Copy data from buffer, caller must insure data exists
  // Do NOT update out. Do NOT use pOut from buffer, but use supplied param
size_t toend = pQ->pLimit - pOut;
size_t pNewOut = pOut;

  if (toend > l) toend = l;
  memcpy(pD, pBase + pNewOut, toend);
  l -= toend;
  pD += toend;
  pNewOut += toend;
  if (pNewOut == pQ->pLimit) pNewOut = pQ->pFirst;
  if (l)
    { // More to copy
      memcpy(pD, pBase + pNewOut, l);
      pNewOut += l;
    }
  return pNewOut;
}

void QDump(  CQueue* pQ, char* pBase)
{
size_t pOut = pQ->pOut;
unsigned long* pL;

  cout << "pQ->first "  << pQ->pFirst
       << " pQ->in "    << pQ->pIn
       << " pQ->out "   << pQ->pOut
       << " pQ->limit " << pQ->pLimit << endl;

  pL = (unsigned long*)(&pBase[pOut]);
  while(pOut != pQ->pIn)
    {
      cout << "Data offs " << pOut << " " << *pL << endl;
      pL++;
      pOut += 4;
      if (pOut == pQ->pLimit) pOut = pQ->pFirst;
    }
}

void QDumpPtrs( CQueue* pQ)
{
  cout << "pQ->first "  << pQ->pFirst
       << " pQ->in "    << pQ->pIn
       << " pQ->out "   << pQ->pOut
       << " pQ->limit " << pQ->pLimit << endl;

}


void QDumpPartial(  CQueue* pQ, char* pBase, size_t pOut, size_t pIn)
{
unsigned long* pL;

  pL = (unsigned long*)(&pBase[pOut]);
  while(pOut != pIn)
    {
      cout << "Data offs " << pOut << " " << *pL << endl;
      pL++;
      pOut += 4;
      if (pOut == pQ->pLimit) pOut = pQ->pFirst;
    }
}



