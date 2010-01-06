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



// Define a bitmap object
// George F. Riley, Georgia Tech, Winter 2000

// Creates a bitmap object.  The 'entries' can be more than one bit,
// but default to one bit.  Bits per entry can't be more than 32.
// Entries DO NOT span words, for ease of coding

#include <iostream>
#include <stdlib.h>

#include "G_debug.h"
#include "bitmap.h"
#include "hex.h"

using namespace std;

#ifndef TEST_BM
BitMap::BitMap() :  size(0), BPE(0), words(0), EPW(0), pM(0)
{ // Default constructor ...used only to input from log file
}

BitMap::BitMap( Count_t s, Count_t BitsPerEntry)
  : size(s), BPE(BitsPerEntry)
{
  EPW = BITS_ULONG / BitsPerEntry; // Entries per word
  words = (size + EPW - 1) / EPW;
  DEBUG0((cout << "Hello from bitmap constructor size " << size
          << " bpe " << BitsPerEntry
          << " epw " << EPW
          << " words " << words << endl));
  if (words > 1)
    {
      pM = new Count_t[words];
      for (Count_t i = 0; i < words; i++) pM[i] = 0;
    }
  else
    {
      pM = (Count_t*)(0);
    }
  DEBUG0((cout << "Exiting bitmap constructor" << endl));
}

BitMap::~BitMap()
{
  if (words > 1) delete [] pM;
}

void BitMap::Set(Count_t Which, Count_t Value)
{
Count_t* pW;
Count_t  Mask;
short   Shift;

  pW    = GetWordAddress(Which);
  Mask  = GetBitMask(Which);
  Shift = GetShiftCount(Which);
  *pW  &= (~Mask); // Clear existing bits
  *pW  |= (Value << Shift);
}

void BitMap::Clear(Count_t Which)
{
Count_t* pW;
Count_t  Mask;

  pW    = GetWordAddress(Which);
  Mask  = GetBitMask(Which);
  *pW  &= (~Mask); // Clear existing bits
}

Count_t BitMap::Get(Count_t Which)
{
Count_t* pW;
Count_t  Mask;
short   Shift;
Count_t  r;

  pW    = GetWordAddress(Which);
  Mask  = GetBitMask(Which);
  Shift = GetShiftCount(Which);
  DEBUG0((cout << "Get which " << Which
          << " Mask " << Mask
          << " Shift " << Shift << endl));
  r = *pW;

  r  &= (Mask); // get existing bits
  return (r >> Shift);
}


// Private subroutines

Count_t* BitMap::GetWordAddress(
    Count_t Which) // Get a pointer to the word needed
{
Count_t ind;

 Validate(Which);
 ind = Which / EPW; 
 if (words == 1)
   { // not indirected
     return((Count_t*)&pM);
   }
 return(&pM[ind]);
}

Count_t BitMap::GetBitMask( // Get a bit mask to the needed bits
    Count_t Which)
{
long  m;
short c;
Count_t um;

  m = 0x80000000;
  m >>= (BPE - 1);
  c = GetShiftCount(Which);
  um = m;
  um = um >> (32 - (c + BPE));
  DEBUG0((cout << "Get Bit Mask m " << m
          << " shifted m " << um << endl));
  return(um);
}


short BitMap::GetShiftCount( // Get a shift count for position
    Count_t Which)  
{
Count_t ind;
short  left;

 Validate(Which);
 ind = Which / EPW; 
 left = Which - (ind * EPW);  // Entry number in the actual word
 return(left * BPE);
}

void    BitMap::Validate(Count_t Which)// Validate which not too large
{
 if (Which >= size)
   {
     cout << "BITMAP Error! Which " << Which << " size " << size << endl;
     exit(7);
   }
}


size_t BitMap::Size( void )
{
  size_t r;

  r = sizeof(Count_t*) + // pM
      sizeof(Count_t) + // size
      sizeof(Count_t) + // BPE
      sizeof(Count_t) + // words
      sizeof(short); // EPW
  if (size * BPE > BITS_ULONG)
    r += ((size * BPE) + BITS_ULONG - 1 / BITS_ULONG) * 
         sizeof(Count_t) ; // Account for the actual map
  return(r);
}

void BitMap::DBPrint()
{
  cout << "size " << size
       << " BPE " << BPE
       << " words " << words
       << " EPW " << EPW << endl;
  if (words == 1)
    {
      cout << "Word 0 " << pM << endl;
    }
  else
    {
      for (Count_t i = 0; i < words; i++)
        cout << "Word " << i << " " << pM[i] << endl;
    }
}

Count_t BitMap::FindBPE( Count_t m ) // Find how many bits/entry we need
{
  Count_t bpe = 32;
  Count_t k = 0x80000000;

  while(k)
    {
      if (m & k) return(bpe);
      bpe--;
      k >>= 1;
    }
  return(0);
}

size_t BitMap::EstimateSize( Count_t size, Count_t BitsPerEntry)
{
  size_t r;

  r = sizeof(Count_t*) + // pM
      sizeof(Count_t) + // size
      sizeof(Count_t) + // BPE
      sizeof(Count_t) + // words
      sizeof(short); // EPW
  if (size * BitsPerEntry > BITS_ULONG)
    r += ((size * BitsPerEntry) + BITS_ULONG - 1 / BITS_ULONG) * 
         sizeof(Count_t) ; // Account for the actual map
  return (r);
}

void BitMap::Log( ostream& os)
{
  os << " " << size;
  os << " " << BPE;
  os << " " << words;
  os << " " << EPW;
  if (words == 1)
    { // Not a pointer, but the actual map
      os << " " << Hex8((unsigned long)pM);
    }
  else
    {
      for (unsigned int i = 0; i < words; i++)
        os << " " << Hex8(pM[i]);
    }
}

#endif

#ifdef TEST_BM
int main()
{
BitMap B(64);
BitMap B2(64, 2);
BitMap B3(64, 3);
BitMap B4(200000, 1);
 
  B.DBPrint();
  B.Set(0);
  B.DBPrint();
  cout << "Entry 0 is " << B.Get(0) << endl;
  B.Set(31);
  B.DBPrint();
  B.Set(32);
  B.DBPrint();
  B.Set(63);
  B.DBPrint();

  B2.DBPrint();
  B2.Set(0, 1);
  B2.DBPrint();
  B2.Set(31, 2);
  B2.DBPrint();
  B2.Set(32, 2);
  B2.DBPrint();
  B2.Set(63, 3);
  B2.DBPrint();

  B3.DBPrint();
  B3.Set(0, 1);
  B3.DBPrint();
  B3.Set(31, 2);
  B3.DBPrint();
  B3.Set(32, 2);
  B3.DBPrint();
  B3.Set(63, 7);
  B3.DBPrint();

}
#endif
