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

/************************************************************************/
/* 2010.1.4  ����Ƽ                                                     */
/***********************************************************************/
#ifndef __interface_real_h__
#define __interface_real_h__

#include "G_common_defs.h"
#include "simulator.h"
#include "protocol.h"
#include "packet.h"
#include "handler.h"
#include "ipaddr.h"
#include "notifier.h"
#include "macaddr.h"
#include "node.h"
#include "link.h"
#include "link-real.h"
#include "eventcq.h"
#include "interface.h"
////#include "arp.h"
////#include "worm-containment.h"

class L2Proto;
class Node;
class Queue;
class LinkEvent;
class NotifyHandler;

#define CLASS_C_MASK 0xffffff00
#define CLASS_B_MASK 0xffff0000
#define CLASS_A_MASK 0xff000000
#define MASK_NONE    0x00000000

class IpMaskCount { // IP��ַ�������hopcount
public:
  IpMaskCount(IPAddr_t i, Mask_t m, Count_t c = 0) 
      : ip(i), mask(m), hopCount(c) { }
public:
  IPAddr_t ip;
  Mask_t   mask;
  Count_t  hopCount;
};

typedef std::vector<IpMaskCount> IPMaskCountVec_t;

class InterfaceReal : public Interface
/*
�ӿڱ��������㣬����Ӧ����·�����������·�Ľ������ӿ��ж������洢��·�������洢�����ݰ����У� 
��һ�����������ĵڶ�Э�����󡣽ӿڲ���ֱ�ӱ����������õģ������ڴ�����·�Ĺ����б���Ӵ�����
*/
{
public:

  InterfaceReal(const L2Proto& l2 = L2Proto802_3(),
				IPAddr_t i   = IPADDR_NONE,
				Mask_t   m   = MASK_ALL,
				MACAddr  mac = MACAddr::NONE,
				bool bootstrap = false);    //����һ���µ�real�ͽ�
  virtual ~InterfaceReal();
  ////void SetARPTimeout(Time_t t){ arptimeout = t;}//ARPʱ��

//�������ݰ�
  bool Send(Packet*, IPAddr_t, int); //ͨ�����ӿڴ������ݰ�
  bool Xmit(Packet*, IPAddr_t, int); //ͨ�����ӿڴ������ݰ�������Ƿ���Ҫʹ��ARP
  bool Send(Packet*, const MACAddr&, int); //ͨ�����ӿڴ������ݰ�����ʹ��ARP
  bool Send(Packet*, Word_t); //ͨ�����ӿڹ㲥���ݰ�
  bool EnquePacket(Packet*);//�����ݰ�������У����ɹ�����true�����򷵻�false

  virtual void Handle(Event*, Time_t);//�������ݰ������¼�
  virtual void Notify(void*);//֪ͨ���пռ��Ƿ����
  
//�����ز���
  Count_t PeerCount() const; //������ýӿ������Ľ�����
  IPAddr PeerIP(int npeer) const;//���ݽ��index������IP��ַ�����أ����������򷵻ؿ�
  IPAddr NodePeerIP(Node*) const;//���ݽ��������뱾�ӿ������Ľӿڣ�������IP��ַ���أ����������򷵻ؿ�
  bool NodeIsPeer(Node*) const;//������Ƿ����뱾�ӿ�ֱ�������Ľ�㣬�����򷵻���
  Count_t NodePeerIndex(Node*) const; //��ȡ����index
  void SetNode(Node*);//���ñ��ӿ����ӵĽ��

#ifdef MOVED_TO_BASIC
//��·��ز���
  Link* GetLink() const;//�����뱾�ӿ���������·����
  void SetLink(Link*);//�����뱾�ӿ���������·����
#endif
//������ز���
  Queue* GetQueue() const;//�����뱾��������Ķ��ж���
  void SetQueue(const Queue& q);//����һ���µĶ��ж���
  bool QueueDetailed() const;//�������ѱ����壬�򷵻�true�����򷵻�flase
//Э����ز��� 
  L2Proto* GetL2Proto() const { return pL2Proto;} //�����뱾�ӿ������ĵڶ���Э�����
  void SetL2Proto(const L2Proto& l2);//�����뱾�ӿ������ĵڶ���Э�����
  void AddNotify(NotifyHandler*,void*);//���һ��notify��������֪ͨ�뱾�ӿ���������·�ǿ��е�
  void CancelNotify(NotifyHandler*);//ɾ��һ��notify֪ͨ
  MACAddr IPToMac(IPAddr_t) const;//����Զ�̽���IP��ַ��������MAC��ַ 
  Count_t NeighborCount(Node*) const;//ͨ����ѯ�뱾�ӿ���������·����ȡ·�����ڽӽ��ĸ���

//��ȡ�ڽӽ���б�
  void Neighbors(NodeWeightVec_t&, bool forceAll = false);//��ȡһ���뱾�ӿ�ֱ���������ڽӽ��
  IPAddr_t DefaultPeer();//���ذ󶨱��ӿڵ�Ĭ�Ͻ��IP��ַ����û��ͨ����·Ĭ�Ͻ��IP��ַ
  bool IsLocalIP(IPAddr_t) const;//�ж�ָ��IP�Ƿ��뱾�ӿ���ͬһ����
  Mult_t TrafficIntensity() const;//���㱾�ӿڵ�ҵ����ǿ��(ÿ�봫��������/��·����)
//�ֲ�ʽ�����ߡ�Ethernet  
  ////void AddRemoteIP(IPAddr_t, Mask_t, Count_t = 0);//�ֲ�ʽ·��
  void HandleLLCSNAP(Packet*, bool);//�����������ݰ�������LLC��ͷ
  ////virtual bool IsWireless() const { return false;}//true���������߽ӿ�
  ////virtual bool IsReal() const { return true;}//true��ʾ��real�ͽӿ�
  ////virtual bool IsEthernet() const {return false;}// True��ʾ��Ethernet�ͽӿ�
//������ 
  ////void SetWormContainment(WormContainment*);
  ////WormContainment* GetWormContainment() { return wormcontainment;}
//ARP
  ////void UseARP(bool);  //���ݲ���ȷ���Ƿ�ʹ��ARPЭ��

protected:

  virtual void        PacketRxStart(LinkEvent*);
  virtual void        PacketRxEnd(bool, Size_t);

private:
  Queue*           pQueue;          //�뱾�ӿ������Ķ���
  NList_t          notifications;   //notification���У���֪ͨ�����У�
  LinkEvent*       notifEvent;      //notifyʱ��(��֪ͨ���¼�)
  DCount_t         totBytesSent;    //��¼�����͵�������������
  L2Proto*         pL2Proto;        //�뱾�ӿ������ĵڶ���Э�����
  IPAddr_t         defaultPeer;     //�뱾�ӿ�������Ĭ�Ͻ��IP��ַ

  ////bool             usearp;          //true����ʹ��ARPЭ��
  ////ARP*             arpmodule;       //ARPģ��
  ////Time_t           arptimeout;      //arpʱ�䷧
  ////WormContainment* wormcontainment; //��湦��

private:
//�ֲ�ʽ���
  ////IPMaskCountVec_t remoteIPList;
private:
  void CallNotification(bool); 
public:
  static Count_t count;//��������
};

#endif
