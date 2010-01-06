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

#ifndef __BITMAP_H__
#define __BITMAP_H__

#include <iostream>

#include "G_common-defs.h"

const unsigned  int BITS_ULONG = 32;

//Doc:ClassXRef
class BitMap {
  public :
    BitMap      ( );
    BitMap      ( Count_t Size, Count_t BitsPerEntry = 1);
    ~BitMap     ();
    void    Set  ( Count_t Which, Count_t Value = 1);
    void    Clear( Count_t Which);
    Count_t Get  ( Count_t Which);  // Return the specified entry
    size_t  Size ( void );         // Return storage used
    void    Log  ( std::ostream& os);   // Log to ostream
    void    DBPrint();
    static Count_t FindBPE( Count_t );
    static size_t  EstimateSize( Count_t Size, Count_t BitsPerEntry);
  private :
    Count_t*  GetWordAddress(Count_t Which); // Get pointer to the word needed
    Count_t   GetBitMask(Count_t Which);     // Get a bit mask for needed bits
    short     GetShiftCount( Count_t Which); // Get a shift count for position
    void      Validate(Count_t Which);       // Validate which not too large
    Count_t   size; // how many entries
    Count_t   BPE;  // Bits per entry
    Count_t   words;// Number of words needed for bitmap
    short     EPW;  // Entries per word
    Bitmap_t* pM;   // Pointer to the actual map (or the data if < 32 bits)
};

#endif
