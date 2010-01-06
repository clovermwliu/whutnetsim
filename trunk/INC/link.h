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

/************************************************************************/
/* 2010.1.4  ����Ƽ                                                     */
/***********************************************************************/

#ifndef __link_h__
#define __link_h__

#include <deque>

#include "G_common-defs.h"
#include "handler.h"
#include "l2proto802.3.h"
#include "event.h"
#include "notifier.h"
#include "rng.h"
#include "node.h"

class Queue;
class LinkMonitor;
class InterfaceBasic;

class LinkEvent : public Event {
public:
typedef enum { 
  PACKET_RX,//�������ݰ�
  PACKET_FIRSTBIT_RX, //�������ݰ���һλ
  PACKET_TX,  //���ݰ��������
  NOTIFY,     //֪ͨ���󻺴����Ѿ�����
  LINK_FAIL,  //�ر�һ������
  LINK_RESTORE//���½�������
} LinkEvent_t;
public:
  LinkEvent() 
    : p(0), numberBytes(0), hasBitError(false), fifoEvent(false)
    { }
  LinkEvent(Event_t ev) 
    : Event(ev), p(0), numberBytes(0), hasBitError(false), fifoEvent(false)
    { }
  LinkEvent(Event_t ev, Packet* pkt) 
    : Event(ev), p(pkt), numberBytes(0), hasBitError(false), fifoEvent(false)
    { }
  virtual ~LinkEvent() { }
public:
  Packet* p; //�󶨵����ݰ�
  Count_t numberBytes; //���ݰ��Ĵ�С��������PACKET_TX�¼�)
  bool    hasBitError; //���������Ƿ���λ����
  bool    fifoEvent;   //˵���¼��Ƿ��ڱ���interface��fifo������
};

//���࣬ʵ�ʵ������p2p��Ethernet��802.11
class Link : public Handler {

public:

  Link();//����һ��Ĭ�ϵ���·
  Link(Rate_t b, Time_t d);//����һ����·��ָ���������ʱ
  Link(const Link&); //����ָ����·���󣬴���һ������·
  virtual ~Link() { };//��������
  virtual Link* Copy() const = 0; //���Ƹ���·������
  static void     Default(const Link& l);//Ϊ����·����һ��ָ������·����
  static const    Link& Default();//���ر���·����·����
//�ӿ��й�
  virtual void  SetPeer(InterfaceBasic* i) = 0;//������·��һ�˶ԵȵĽӿ�
  virtual InterfaceBasic* GetLocalIf() const; //���ر��ؽӿڵ�ָ��
  virtual void SetLocalIf(InterfaceBasic*); //���ñ��ش���ӿ�
  virtual InterfaceBasic* GetPeer(Packet*) = 0;//���ݴ�������ݰ���ȡ��·��һ�˽��Ľӿ�
  virtual InterfaceBasic* GetPeer(Count_t) = 0;//���ݽ��index��ȡ��·��һ�˽��Ľӿ�
  virtual Interface* GetNewInterface(IPAddr_t = IPADDR_NONE,//Ϊ����·����һ��ָ���ӿ�
	  Mask_t = MASK_ALL) const;

//�����й�
  virtual bool Transmit(Packet*, Interface*, Node*) = 0; //ͨ������·����һ�����ݰ�
  virtual bool Transmit(Packet*, Interface*, Node*, Rate_t) = 0;//ͨ������·��һ���Ĵ�����һ�����ݰ�
  virtual bool Transmit(LinkEvent*, Interface*, Time_t) = 0;//ͨ��һ����·�¼��������ݰ� 
  virtual bool TransmitWithDelay(Packet*, Time_t) = 0;//�������ݰ������Ƕ����ӳ�
  void    Handle(Event*, Time_t) = 0;//������·�ϵ��¼��б�
  virtual void AddNotify(NotifyHandler*,void*) = 0;//����һ��֪ͨ����  
  virtual Time_t NextFreeTime() = 0;//Ԥ����·����ʱ��ϵͳʱ�䣨������jitter��contention��
  virtual bool   RxBroadcast() = 0;         //�жϱ���·�Ƿ����һ���Լ��㲥�����ݰ�������true��������
//���� 
  virtual bool   Busy() const = 0;//true�������·��ǰ�ڴ������ݰ�
  virtual void   Bandwidth(Rate_t b) = 0;//������·�Ĵ���
  virtual Rate_t Bandwidth() const = 0;//������·�Ĵ���
  virtual void   Delay(Time_t d) = 0;//������·����ʱ
  virtual Time_t Delay() const = 0;//������·����ʱ
  virtual void BitErrorRate(Mult_t b) = 0;//�趨����·�ڴ���ʱ�Ĳ����
  virtual Mult_t BitErrorRate() = 0;//���ر���·�ڴ���ʱ�Ĳ����
  virtual void ResetUtilization() = 0;//���ñ���·����·������Ϊ0
  virtual Mult_t Utilization() = 0;//���ر���·����·������
  virtual Weight_t Weight() = 0;//��ȡ����·��Ȩ��
  virtual void Weight(Weight_t w) = 0;//���ñ���·��Ȩ��
  virtual void Jitter(const Random&) = 0;  //������·�ڴ���ʱ��jitter�ӳ٣��ú�����ʹjitter��Ч��Ψһ������
  virtual bool   ScheduleFailure(Time_t t) = 0;//�������ض�����ʱ��t����·ʧЧ�������óɹ�����true
  virtual bool   ScheduleRestore(Time_t t) = 0;//�������ض�����ʱ��t����·���¼�������óɹ�����true
//������
  virtual Count_t  PeerCount() = 0; //���ر���·���ӵĽ������real��δʵ�֣�ghost�з���1��
  virtual IPAddr_t PeerIP(Count_t) = 0;//���ݽ��index������IP��ַ���������ڷ��ؿ�
  virtual  IPAddr_t NodePeerIP(Node*) = 0;   //�����뱾��·������ָ������ip��ַ
  virtual  bool NodeIsPeer(Node*) = 0;   //�ж�ָ������ǲ����뱾��·������һ����㣬����ǣ�����true
  virtual  Count_t  NodePeerIndex(Node*) = 0;//����ָ����㣬������index
  virtual  IPAddr_t DefaultPeer(Interface*) = 0; //�����뱾��·ָ���ӿ������Ľ���ip��ַ
  virtual Count_t NeighborCount(Node*) = 0;//��ȡָ�����ͨ������·������·�����ڽӽ��ĸ���
  virtual void Neighbors(NodeWeightVec_t&, Interface*, bool) = 0;//���������·���ڽӽ��������и��ݱ��ؽӿ������֮��Ӧ���ڽӽ��
  virtual void AllNeighbors(NodeWeightVec_t&) = 0;//��ȡ����·�������ڽӽ��
  virtual MACAddr  IPToMac(IPAddr_t) = 0;     //���ݽ��IP��ַ��ȡ��MAC��ַ������
  virtual DCount_t BytesSent() = 0;//���ظ���·�����͵���������
  virtual DCount_t PktsSent() = 0;//���ظ���·�����͵����ݰ�������

  virtual LinkMonitor* AddLinkMonitor(const LinkMonitor&);//Ϊ����·���һ��LinkMonitor����

//�������
  virtual void AnimateLink();//�ڶ�����ʾʱ��������·
  virtual void ReAnimateLink();//����·״̬�ı�����ֻ�������·
  virtual void AnimatePackets();//�����ڸ���·�ϴ�������ݰ�
public:
//���ô�������
  static void     DefaultRate(Rate_t r);//������·��Ĭ�ϴ���
  static Rate_t   DefaultRate() { return defaultRate;}//��ȡ��·��Ĭ�ϴ���
  static void     DefaultDelay(Time_t t);//������·��Ĭ���ӳ�
  static Time_t   DefaultDelay() { return defaultDelay;}//��ȡ��·��Ĭ���ӳ�
  static void     DefaultJitter(const Random& j);//������·��Ĭ��jitter
  static Random*  DefaultJitter();//������·��Ĭ��jitter

  static void     UseSeqEvents(bool u) { useSeqEvents = u;}//uΪtrue������linkevent������λ���ڱ���·�¼����У����������䷢��λ����ϵͳ�¼�����

public:
  InterfaceBasic* localIf; //����·�ӿ�
  
protected:
  static Rate_t   defaultRate;      //Ĭ�ϴ���
  static Time_t   defaultDelay;     //Ĭ����ʱ
  static Link*    defaultLink;      //Ĭ����·����
  static Random*  defaultJitter;    //Ĭ��jitter�����
  static Random*  berRng;           //λ������������
  static bool     useSeqEvents;     // True����linkevent���ڱ����¼����У��������ϵͳ�¼�����
};

#include "l2proto.h"

#endif

