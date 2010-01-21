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

/*
  This is the file for the 802.2 llc/snap frame
  A low overhead (4 bytes less than ethernet encapsulation)
  We get to pass the 'ethertype' into the packet.
  For now SNAP is enough but maybe we need LLC later
*/

#ifndef __llcsnap_h__
#define __llcsnap_h__

#include "l2proto.h"
#include "tfstream.h"
#include "macaddr.h"

class LLCSNAPHeader : public PDU {
  public:
   LLCSNAPHeader(Word_t type = 0x800);
   LLCSNAPHeader(char*, Size_t&, Packet*);
   
   //Size_t Size() const { return 8; }
   Size_t Size() const { return txSize; }
   PDU*   Copy() const {return new LLCSNAPHeader(*this); }
   Layer_t Layer() { return 2;}
   Layer_t SubLayer() { return 1;} // Sublayer 1, above actual l2 PDU

   Count_t Version() { return 1; }
   Size_t SSize(){ return 8;};
   char*  Serialize(char*, Size_t&);
   char*  Construct(char*, Size_t&);
   void   Trace(Tfstream& tos, Bitmap_t b,
                Packet* = nil, const char* = nil) { }
  public:
   char   llc_dsap;
   char   llc_ssap;
   char   llc_ctrl;
   Word_t snap_oui;  /* org. unique id  usually 3 chars (accounted in Size) */
   Word_t snap_ethtype; /* right now this is what matters to me */
public:
  static Size_t txSize;
  static void TxSize(Size_t txs) { txSize = txs; }
  
};


#endif
