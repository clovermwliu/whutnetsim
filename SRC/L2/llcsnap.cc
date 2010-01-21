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


//File Information: 数据链路层LLC子层类
//File Name: llcsnap.cc
//File Purpose: LLC子层仿真
//Original Author:  George F. Riley.
//Author Organization:  Georgia Tech
//Construct Data:  Winter 2003
//Modify Author:  PengZhen
//Author Organization: WHU
//Modify Data: JAN, 2010


#include "llcsnap.h"
#include "ipv4.h"


Size_t LLCSNAPHeader::txSize = 8;
/*数据链路层发送报头大小*/

LLCSNAPHeader::LLCSNAPHeader(Word_t type)
/*LLC子层协议报头默认构造函数*/
{
  llc_dsap = 0xAA;
  llc_ssap = 0xAA;
  llc_ctrl = 0x3;
  snap_oui = 0;
  snap_ethtype = type;
}

LLCSNAPHeader::LLCSNAPHeader(char* b, Size_t& sz, Packet* p)
/*从串行化buffer中构造LLC协议报头*/
{
  Size_t s = 0;
  b = Serializable::GetSize(b, sz, s);
  s  -= sizeof(Size_t);
  sz -= s;
  b  = Construct(b, s);
  p->PushPDUBottom(this);
  switch (snap_ethtype) { // Constuct appropriate l3pdu
  case 0x0800 : // IPV4
      new IPV4Header(b, sz, p); // Create and push IPV4 header
      break;
    }

}

char*  LLCSNAPHeader::Serialize(char* b, Size_t& sz)
/*将LLC协议报头串行化到buffer中*/
{ 
  char dum = 0;
  b = SerializeToBuffer(b, sz, llc_dsap);
  b = SerializeToBuffer(b, sz, llc_ssap);
  b = SerializeToBuffer(b, sz, llc_ctrl);
  b = SerializeToBuffer(b, sz, dum); // Include an extra char for alignment
  b = SerializeToBuffer(b, sz, snap_oui);
  b = SerializeToBuffer(b, sz, snap_ethtype);
  return b;
}

char*  LLCSNAPHeader::Construct(char* b, Size_t& sz)
/*从串行化buffer中，重构LLC子层*/
{
  char dum;
  
  b = ConstructFromBuffer(b, sz, llc_dsap);
  b = ConstructFromBuffer(b, sz, llc_ssap);
  b = ConstructFromBuffer(b, sz, llc_ctrl);
  b = ConstructFromBuffer(b, sz, dum);
  b = ConstructFromBuffer(b, sz, snap_oui);
  b = ConstructFromBuffer(b, sz, snap_ethtype);
  
  return b;
}
