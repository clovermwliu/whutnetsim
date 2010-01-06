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
/*File Purpose: ���ļ�����ģ���й��߼���·����/��������Э�飨Logic Link Control / SubNetwork Access Protocol,LLCSNAP����
				��RFC1483�ж�����һ�ּ���LLC/SNAPͷ�Ļ��ƣ���ΪLLC/SNAP��װ��
				��LLC/SNAPͷ�а��������ֽڵ�LLC�����ֽڵ�OUI�����ֽڵ�PID�������������SNAPͷ����
				LLCͷָ�����ص�Э�������0xAA-AA-03����·�ɷ�ISOЭ�����ݵ�Ԫ����
				SNAPͷָ��Э��Ĺ�����֯�;���Э�飨��0x00-00-00-08-00��ʾ��̫���͵�IP���飩��

				802.3/802.2 LLC������IEEE ��ʽ��802.3��׼������DIX Ethernet V2��չ������
				����Ethernet V2֡ͷ��Э�������ֶ��滻Ϊ֡�����ֶ�(ȡֵΪ0000-05dc;ʮ���Ƶ�1500)��
				������802.2 LLCͷ���Ա�־�ϲ�Э�飬LLCͷ�а���DSAP��SSAP�Լ�Ctrl�ֶ�. 

				802.3/802.2 SNAP������IEEEΪ��֤��802.2 LLC��֧�ָ�����ϲ�Э��ͬʱ���õ�֧��IPЭ��������ı�׼��
				��802.3/802.2 LLCһ��802.3/802.2 SNAPҲ����LLCͷ��
				�������һ��2Bytes��Э��������ͬʱ��SSAP��ֵ��ΪAA�����Ӷ�ʹ����Ա�ʶ������ϲ�Э�����ͣ�
				���������һ��3Bytes��OUI�ֶ����ڴ���ͬ����֯��
				RFC 1042������IP������802.2�����еķ�װ������ARPЭ����802.2 SANP�е�ʵ��.

				WhuTNetSim�����뱾�㣬��ҪΪ����߲�Э���Լ��ṩ������������ṩ��̿����ӿ�
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
