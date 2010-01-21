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


//File Information: ������·���802.3Э����
//File Name: l2proto802.3.cc
//File Purpose: ����������·��802.3Э�����
//Original Author:  George F. Riley. , 
//Author Organization:  Georgia Tech
//Construct Data: Spring 2002
//Modify Author: Peng Zhen
//Author Organization: WHU
//Modify Data: JAN 2010


#include <iostream>

//#define DEBUG_MASK 0x02
#include "G_debug.h"
#include "l2proto802.3.h"
#include "interface.h"
#include "ipv4.h"
#include "trace.h"
#include "node.h"
#include "hex.h"
#include "macaddr.h"
#include "llcsnap.h"
#include "link.h"
#include "packet-callbacks.h"

using namespace std;

L2Proto802_3::L2Proto802_3()
/*���캯��*/
{
}

void L2Proto802_3::BuildDataPDU(MACAddr src, MACAddr dst, Packet* p)
/*���������·�㱨ͷ*/
{
  L2802_3Header* l2pdu = new L2802_3Header(src, dst, p->Size()); //����������·�㱨ͷ
  p->PushPDU(l2pdu); //��������·�㱨ͷ�ӵ����ݰ���
}

void L2Proto802_3::DataRequest(Packet* p)
/*������·��Э�����������������*/
{
  // ȷ�����ݰ��Ƿ���Ȼ����
  if(!iface->GetNode()->CallCallbacks(
         Layer(), 0, PacketCallbacks::TX, p, iface))
    return;
  // �õ�������·��Э�鱨ͷ
  L2802_3Header* l2pdu = (L2802_3Header*)p->PeekPDU();
  //trace�������
  if (Trace::Enabled())
    iface->GetNode()->TracePDU(this, l2pdu, p, "-");
  //ͨ���ӿڷ������ݰ�
  iface->GetLink()->Transmit(p, iface, iface->GetNode());
}


void L2Proto802_3::DataIndication(Packet* p, LinkEvent* le)
/*������·��Э�鱨ͷ����������*/
{
  bool fbrx = false; //����fbrx�¼����Ա���
  if (le)
    {
      fbrx = (le->event == LinkEvent::PACKET_FIRSTBIT_RX);
	  //fbrxΪ��ʱ�����������ֽڽ����¼�
    }
  if (le && !p)
    { 
      p = le->p;
	  // ʹ�ô������¼��еĵõ������ݰ�
    }
  if (le) delete le;
  
  // �������ݰ��Ƿ���Ȼ����
  if(!iface->GetNode()->CallCallbacks(
         Layer(), 0, PacketCallbacks::RX, p, iface))
    return;
  //�õ�������·��Э�鱨ͷָ��
  L2802_3Header* l2pdu = (L2802_3Header*)p->PeekPDU();
  //trace��·��Э���������
  if (Trace::Enabled()) iface->GetNode()->TracePDU(this, l2pdu, p, "+");
  //ɾ��802.3��ͷ
  p->PopPDU();
  //����LLC�Ӳ��������
  iface->HandleLLCSNAP(p, fbrx);
}


bool L2Proto802_3::Busy()
/*����802.3Э���Ƿ�æµ����802.3���Ե�ͬ����·link�Ƿ�æµ*/
{ 
  //ͨ���ӿڵõ�link��ָ��
  Link* l = iface->GetLink();
  //����link��״̬���Է���
  return l->Busy();
}



Size_t L2802_3Header::txSize = 12;
/*����802.3��Э�鷢�ͱ�ͷ����*/


// Construct from serialized buffer
L2802_3Header::L2802_3Header(char* b, Size_t& sz, Packet* p)
/*�Ӵ��л�buffer�й���802.3Э�鱨ͷ*/
{
  DEBUG0((cout << "Constructing l2803-2, initial size " << sz << endl));
  Size_t s = 0;
  b = Serializable::GetSize(b, sz, s);
  DEBUG0((cout << "l2pdu size is " << s << endl));
  s -= sizeof(Size_t);
  sz -= s;
  b = Construct(b, s);
  DEBUG0((cout << "Constructing l2803-2, final  size " << sz << endl));
  p->PushPDUBottom(this);  // ���Э�鱨ͷ����ջ
  new LLCSNAPHeader(b, sz, p);
}

void L2802_3Header::Trace(Tfstream& tos, Bitmap_t b, Packet* p, const char* s)
/*trace802.3Э�鱨ͷ�����Ϣ*/
{
  tos << " ";
  if (s) 
    tos << s;
  tos << "L2";
  if (Detail(L2Proto802_3::TYPE, b))  tos << "-802.3";
  // Below moved to LLC/SNAP header
  //if (Detail(L2Proto802_3::PROTO, b)) tos << " " << Hex4(l3proto);
  if (Detail(L2Proto802_3::LENGTH, b)) 
    { // Check if packet passed, which we need for length
      if (p)
        tos << " " << p->Size();
    }
  if (Detail(L2Proto802_3::SRC, b))   tos << " " << src;
  if (Detail(L2Proto802_3::DST, b))   tos << " " << dst;
  if (Detail(L2Proto802_3::UID, b))
    {
      if (p) tos << " " << p->uid;
    }
}

Size_t L2802_3Header::SSize()
/*����802.3��ͷ���ȣ�ԴMAC��6��+Ŀ��MAC��6��+�����Э��ָ�루2����*/
{

  return 6 + 6 + 2;
}

char*  L2802_3Header::Serialize(char* b, Size_t& sz)
/*��802.3Э�鱨ͷ���л���buffer*/
{
  DEBUG(1,(cout << "Serializing l28023 src " << Hex2(src.macAddr)
           << " dst " << Hex2(dst.macAddr)
           << " lth " << length << endl));
  b = SerializeToBuffer(b, sz, src.macAddr);
  b = SerializeToBuffer(b, sz, dst.macAddr);
  Word_t p = length;
  b = SerializeToBuffer(b, sz, p);
  return b;
}

char*  L2802_3Header::Construct(char* b, Size_t& sz)
/*�Ӵ��л�buffer�ع�802.3��ͷ*/
{
  b = ConstructFromBuffer(b, sz, src);
  b = ConstructFromBuffer(b, sz, dst);
  Word_t p = 0;
  b = ConstructFromBuffer(b, sz, p);
  length = p;
  DEBUG0((cout << "L2Hdr::Construct, src " << Hex2(src.macAddr)
          << " dst " << Hex2(dst.macAddr)
          << " length " << length << endl));
  return b;
}

