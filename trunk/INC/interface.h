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

#ifndef __interface_h__
#define __interface_h__

#include "G_common-defs.h"
#include "interface-basic.h"
#include "handler.h"
#include "notifier.h"
#include "mask.h"
#include "node.h"
#include "macaddr.h"

class VirusThrottle;
class PortMatching;
class L2Proto;
//class Node;
class Queue;
//class LinkEvent;
//class Link;
//class EventCQ;

class Interface : public InterfaceBasic, public NotifyHandler {

public:
  Interface();
  Interface(const L2Proto& l2 = L2Proto802_3(),
            IPAddr_t i   = IPADDR_NONE,
            Mask_t   m   = MASK_ALL,
            MACAddr  mac = MACAddr::NONE,
			bool  bootstrap = false);
  Interface(const Interface&);
  virtual ~Interface();
//���ݰ�����
  virtual bool Send(Packet*, IPAddr_t, int) = 0;//ͨ�����ӿڴ������ݰ�
  ////virtual bool Send(Packet*, const MACAddr&, int) = 0;//ͨ�����ӿڴ������ݰ�����ʹ��ARP
  virtual bool Send(Packet*, Word_t) = 0;//ͨ�����ӿڹ㲥���ݰ�
  ////virtual bool Xmit(Packet*, IPAddr_t, int){ return false;}//ͨ�����ӿڴ������ݰ�������Ƿ���Ҫʹ��ARP
  virtual void Handle(Event*, Time_t);//�������ݰ������¼�
  virtual void Notify(void*);//֪ͨ���пռ��Ƿ����
  virtual Mult_t   TrafficIntensity() const = 0;//���㱾�ӿڵ�ҵ����ǿ��(ÿ�봫��������/��·����)  
//IP��Mac��ַ
  ////virtual void AddARPEntry(IPAddr_t, MACAddr) {}//�ڱ��ӿ�arp����������arp�������һ�����ڽӽ���IP  ������������Ӧ��mac��ַ��
  void         SetIPAddr(IPAddr_t i) { ip = i;}//���ñ��ӿڵ�IP��ַ
  IPAddr       GetIPAddr() const { return ip;}//��ȡ���ӿڵ�IP��ַ
  Mask_t       GetIPMask() const { return mask;}//��ȡ���ӿ�IP��ַ������
  void         SetMACAddr(const MACAddr& m);//���ñ��ӿڵ�MAC��ַ
  MACAddr      GetMACAddr() const { return macAddr;} //��ȡ���ӿڵ�MAC��ַ
//������
  virtual Count_t  PeerCount() const = 0;         //��ȡ�뱾�ӿ������Ľ��ĸ���
  virtual IPAddr   PeerIP(int) const = 0;         //���ݽ��index��ȡ�뱾�ӿ������Ľ���IP��ַ
  virtual IPAddr   NodePeerIP(Node*) const = 0;   //���ݽ���ȡ�뱾�ӿ������Ľ���IP��ַ
  virtual bool     NodeIsPeer(Node*) const = 0;   //�ж�Ŀ�����ǲ����뱾�ӿ�ֱ�����������Ƿ��� true
  virtual Count_t  NodePeerIndex(Node*) const = 0;//��ȡ�뱾�ӿ������Ľ���index
  Node*            GetNode() const { return pNode;}//��ȡ�뱾�ӿ������Ľ��
#ifdef MOVED_TO_BASE
  virtual void     SetNode(Node* n) = 0;//�����뱾�ӿ������Ľ��
  virtual Link*    GetLink() const = 0;//��ȡ�뱾�ӿ���������·
  virtual void     SetLink(Link*) = 0;//�����뱾�ӿ���������·
#endif
//����
  virtual Queue*   GetQueue() const = 0;//��ȡ�뱾�ӿ������Ķ���
  virtual void     SetQueue(const Queue& q) = 0;//�����뱾�ӿ������Ķ���
  virtual bool     QueueDetailed() const = 0;// True��ʾ�����ѱ�����
//Э��  
  virtual L2Proto* GetL2Proto() const = 0;////�����뱾�ӿ������ĵڶ���Э�����
  virtual void     SetL2Proto(const L2Proto&) = 0;//�����뱾�ӿ������ĵڶ���Э�����
//֪ͨ  
  virtual void     AddNotify(NotifyHandler*,void*) = 0;//���һ��notify��������֪ͨ�뱾�ӿ���������·�ǿ��е�
  virtual void     CancelNotify(NotifyHandler*) = 0;//ɾ��һ��notify֪ͨ
  

//��ȡ�ڽӽ���б�
  virtual MACAddr  IPToMac(IPAddr_t) const = 0;//����Զ�̽���IP��ַ��������MAC��ַ 
  virtual Count_t  NeighborCount(Node*) const = 0;//ͨ����ѯ�뱾�ӿ���������·����ȡ·�����ڽӽ��ĸ���
  virtual void     Neighbors(NodeWeightVec_t&, bool = false) = 0;//��ȡһ���뱾�ӿ�ֱ���������ڽӽ��
  virtual IPAddr_t DefaultPeer() = 0;//���ذ󶨱��ӿڵ�Ĭ�Ͻ��IP��ַ����û��ͨ����·Ĭ�Ͻ��IP��ַ
  virtual bool     IsLocalIP(IPAddr_t) const = 0;//�ж�ָ��IP�Ƿ��뱾�ӿ���ͬһ����

//�ֲ�ʽ�����ߡ�Ethernet��ghost
  ////virtual void     AddRemoteIP(IPAddr_t, Mask_t, Count_t = 0) = 0;//�ֲ�ʽ·��
  virtual void     HandleLLCSNAP(Packet*, bool) = 0;//���� LLCSnap���ݰ���ͷ
  ////virtual bool     IsWireless() const = 0; 
  ////virtual bool     IsEthernet() const { return false; }
  ////virtual bool     IsReal() const = 0;  
  ////virtual void     RegisterRxEvent(LinkEvent*){return;} ; //Ethernet��� 
  ////virtual void     AddRxEvent(LinkEvent*){return;} ; //Ethernet���

#ifdef MOVED_TO_BASIC
  virtual void     ScheduleRx(LinkEvent*, Time_t, bool = true);//�ڱ����Ƚ��ȳ������м�¼�������ݰ��¼�
  EventCQ*   GetEventCQ() { return evList; }//���ر��ؽӿڵ��¼��б�
#endif
//������
  ////virtual void UseWormContainment(bool b){ usewormcontainment = b;}
  ////virtual bool UseWormContainment(){ return usewormcontainment;}
  ////virtual void SetWormContainment(WormContainment*) = 0;
  ////virtual WormContainment* GetWormContainment() = 0;
  ////virtual void UseARP(bool) = 0;//�����Ƿ�ʹ��ARPģ��
#ifdef MOVED_TO_BASIC
//�ӿ�״̬����
  bool IsDown() { return down; }//���ؽӿڵ�״̬
  void Down(bool noReInit = false);//�ӿ�״̬��Ϊ�ر�
  void Up();//�ӿ�״̬��Ϊ����
#endif

  bool FirstBitRx(){return firstBitRx;}
  void FirstBitRx(bool is){firstBitRx = is;}
  
  ////static void DefaultUseARP(bool b = true);//���ݲ���ȷ���Ƿ�ʹ��ARP��true����ʹ��
  static void Add(Interface*, MACAddr_t);//��mac��ַ�б������ָ���Ľӿ�
  static Interface* GetByMac(MACAddr_t);//��mac��ַ�б��и����ƶ�mac��ַ��ȡ��Ӧ�Ľӿ�

protected:
  IPAddr ip;// ���ӿڵ�IP��ַ
  Mask_t mask;// �������������
  MACAddr macAddr;// ���ӿ�MAC��ַ
  bool down;// True��ʾ���ӿڹر�
  ////bool usewormcontainment;// use wormcontainment module
  bool firstBitRx;// True��ʾ��satellite�ӿ�����

public:
  bool   bootstrap;  //true������Ҫ���еĽӿ�ָ���mac��ַ�б�
  static bool recordMacAddresses;//true������Ҫ��¼mac��ַ
  static std::vector<Interface*> macIfVec;//�ӿ�ָ���mac��ַ�б�
  ////static bool defaultUseARP;//true����ʹ��ARP
};

#endif
