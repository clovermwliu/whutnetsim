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
/*File Purpose: 本文件负责模拟有关逻辑链路控制/子网接入协议（Logic Link Control / SubNetwork Access Protocol,LLCSNAP）。
				在RFC1483中定义了一种加上LLC/SNAP头的机制，称为LLC/SNAP封装。
				在LLC/SNAP头中包含了三字节的LLC，三字节的OUI和两字节的PID（后两部分组成SNAP头）。
				LLC头指明负载的协议类别（如0xAA-AA-03表明路由非ISO协议数据单元），
				SNAP头指明协议的管理组织和具体协议（如0x00-00-00-08-00表示以太类型的IP分组）。

				802.3/802.2 LLC：这是IEEE 正式的802.3标准，它由DIX Ethernet V2发展而来。
				它将Ethernet V2帧头的协议类型字段替换为帧长度字段(取值为0000-05dc;十进制的1500)；
				并加入802.2 LLC头用以标志上层协议，LLC头中包含DSAP，SSAP以及Ctrl字段. 

				802.3/802.2 SNAP：这是IEEE为保证在802.2 LLC上支持更多的上层协议同时更好的支持IP协议而发布的标准，
				与802.3/802.2 LLC一样802.3/802.2 SNAP也带有LLC头，
				新添加了一个2Bytes的协议类型域（同时将SSAP的值置为AA），从而使其可以标识更多的上层协议类型；
				另外添加了一个3Bytes的OUI字段用于代表不同的组织，
				RFC 1042定义了IP报文在802.2网络中的封装方法和ARP协议在802.2 SANP中的实现.

				WhuTNetSim中引入本层，主要为更多高层协议以及提供无线网络仿真提供后继开发接口
*/
//Original Author:
//Author Organization:
//Construct Date:
//Modify Author:
//Author Organization:
//Modify Date:

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
