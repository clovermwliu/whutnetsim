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



// Base class for re-using malloc'ed memory
// George F. Riley.  Georgia Tech, Winter 2002

#include <iostream>

#include "G_debug.h"
#include "G_common_defs.h"
#include "memreuse.h"

using namespace std;

// Static ReuseBase variables
PtrPtrVec_t   ReuseBase::q;          // A queue of deleted heap elements
unsigned long ReuseBase::reused;     // Statistics only
unsigned long ReuseBase::newalloc;
unsigned long ReuseBase::deleted;

void* ReuseBase::operator new(std::size_t s)
{
#ifdef NO_DEBUG
  // First compute size in words (32 bit units)
  register std::size_t sw = (s + sizeof(unsigned long) - 1) /
    sizeof(unsigned long);
  register void* b;
  if (q.size() <= sw || q[sw] == nil || q[sw]->size() == 0)
    { // No buffered heap elements of this size.  Allocate one and note lth
      // Allocate an extra 32 bit value for length field
      unsigned long* pl =(unsigned long*)malloc((sw+1)*sizeof(unsigned long));
      DEBUG0((cout << "Mallocing " << (sw+1) * sizeof(unsigned long) << endl));
      *pl = sw;
      b = &pl[1];  // Address to construct
      newalloc++;
    }
  else
    { // Reuse deleted element
      b = q[sw]->back();
      q[sw]->pop_back();
      reused++;
    }
  return ::operator new(s, b);// Call the in-place new
#else
  return ::operator new(s);   // In debug  mode, just use the normal "new"
#endif
}

void ReuseBase::operator delete(void* p)
{
#ifdef NO_DEBUG
  register std::size_t* pLth = (std::size_t*)p - 1;
  register std::size_t l = *pLth; // Size in 32 bit words
  while(q.size() <= l) q.push_back(nil); // Expand to correct size
  if (!q[l]) q[l] = new PtrVec_t;        // Allocate new vector if needed
  q[l]->push_back(p);                    // Add to saved queue
  deleted++;
#else
  ::delete (char*)p;                     // In debug mode, use normal delete
#endif
}

void ReuseBase::DumpQ()
{
  Count_t tot = 0;
  cout << "Printing HeapReuse information" << endl;
  cout << "Reused " << reused << " newalloc " << newalloc
       << " deleted " << deleted << endl;
  for (PtrPtrVec_t::size_type i = 0; i < q.size(); ++i)
    {
      cout << "Size " << i * sizeof(unsigned long);
      if (!q[i])
        {
          cout << " nil " << endl;
        }
      else
        {
          cout << " capacity " << q[i]->capacity() 
               << " size " << q[i]->size()
               << " total bytes " << q[i]->size() * i * sizeof(unsigned long)
               << endl;
          tot += q[i]->size() * i;
        }
    }
  cout << "Total bytes in reuse queue " << tot * sizeof(unsigned long) << endl;
}

