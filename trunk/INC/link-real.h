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
#ifndef __linkreal_h__
#define __linkreal_h__

#include <deque>

#include "G_common_defs.h"
#include "link.h"
#include "handler.h"
#include "l2proto802.3.h"
#include "event.h"
#include "notifier.h"
#include "rng.h"
#include "node.h"

class Queue;
class LinkEvent;
class LinkMonitor;

class LinkReal : public Link {

public:
  LinkReal();//����һ��Ĭ�ϵ���·
  LinkReal(Rate_t b, Time_t d);//����һ����·��ָ���������ʱ
  LinkReal(const LinkReal&); //����ָ����·���󣬴���һ������·
  virtual ~LinkReal();//��������
  virtual Link*  Copy() const = 0; //���Ƹ���·������
  virtual LinkReal& operator=(const LinkReal&);
//��·����
  virtual void  SetPeer(InterfaceBasic* i) = 0;//������·��һ�˶ԵȵĽӿ�
  virtual InterfaceBasic* GetPeer(Packet*) = 0;//���ݴ�������ݰ���ȡ��·��һ�˽��Ľӿ�
  virtual InterfaceBasic* GetPeer(Count_t) = 0;//���ݽ��index��ȡ��·��һ�˽��Ľӿ�
  virtual  bool Transmit(Packet*, Interface*, Node*) = 0;//ͨ������·����һ�����ݰ�
  virtual  bool Transmit(Packet*, Interface*, Node*, Rate_t) = 0;//ͨ������·��һ���Ĵ�����һ�����ݰ�
  virtual  bool Transmit(LinkEvent*, Interface*, Time_t);//ͨ��һ����·�¼��������ݰ� 
  virtual  bool TransmitWithDelay(Packet*, Time_t);//�������ݰ������Ƕ����ӳ�
  void     Handle(Event*, Time_t);//������·�ϵ��¼��б�
  void     Bandwidth(Rate_t b) { bw = b;}//���ô���
  Rate_t   Bandwidth() const   { return bw; }//������·����
  void     Delay(Time_t d)     { delay = d;}//������·�ӳ�
  Time_t   Delay() const       { return delay;}//������·�ӳ�
  bool     Busy() const        { return busy;}//������·״̬�Ƿ�Ϊ��æ
  virtual  Count_t  PeerCount() = 0;        //���ر���·���ӵĽ������real��δʵ�֣�ghost�з���1��
  virtual  IPAddr_t PeerIP(Count_t) = 0;    //���ݽ��index������IP��ַ���������ڷ��ؿ�
  virtual  IPAddr_t NodePeerIP(Node*) = 0;  //�����뱾��·������ָ������ip��ַ
  virtual  bool     NodeIsPeer(Node*) = 0;  //�ж�ָ������ǲ����뱾��·������һ����㣬����ǣ�����true
  virtual  Count_t  NodePeerIndex(Node*) = 0;//����ָ����㣬������index
  virtual  IPAddr_t DefaultPeer(Interface*) = 0; //�����뱾��·ָ���ӿ������Ľ���ip��ַ
  void     BitErrorRate(Mult_t b) { bitErrorRate = b;}//����λ�����
  Mult_t   BitErrorRate() { return bitErrorRate;}//����λ�����
  void     ResetUtilization();//���ñ���·����·������Ϊ0
  Mult_t   Utilization();//���ر���·����·������
  void     AddNotify(NotifyHandler*,void*);//�߲�Э����������·����ʱ����֪ͨ�����������б������һ��handler
  Weight_t Weight() { return weight;}//������·Ȩ��
  void     Weight(Weight_t w) { weight = w;}//������·Ȩ��
  void     Jitter(const Random&);            //����jitter����
  Time_t   NextFreeTime();                   //������·��һ�ο��е�ʱ��
  virtual Count_t  NeighborCount(Node*) = 0; //��ȡָ�����ͨ������·������·�����ڽӽ��ĸ���
  virtual void     Neighbors(NodeWeightVec_t&, Interface*, bool) = 0;//���������·���ڽӽ��������и��ݱ��ؽӿ������֮��Ӧ���ڽӽ��
  virtual void     AllNeighbors(NodeWeightVec_t&) = 0;//��ȡ����·�������ڽӽ��
  DCount_t         BytesSent() { return bytesSent;}//���ظ���·�����͵���������
  DCount_t         PktsSent() { return pktsSent;}//���ظ���·�����͵����ݰ�������
protected:
  virtual void     TransmitComplete(Count_t); //���ݰ�������ɣ�������ز���
  virtual void     SendNotification();        // ����֪ͨ�б��еĵ�һ��֪ͨ

  virtual bool     TransmitHelper(Packet*, 
								  Interface*,
								  Node*, 
								  Interface*, 
								  bool bCast = false);//���췢�ͺͽ������ݰ��¼�

public:
  virtual MACAddr  IPToMac(IPAddr_t) = 0;     //���ݽ��IP��ַ��ȡ��MAC��ַ������
  virtual bool     RxBroadcast() { return false;} //�жϱ���·�Ƿ����һ���Լ��㲥�����ݰ�������true��������
  
  virtual bool     ScheduleFailure(Time_t) = 0;//�������ض�����ʱ��t����·ʧЧ�������óɹ�����true
  virtual bool     ScheduleRestore(Time_t) = 0;//�������ض�����ʱ��t����·���¼�������óɹ�����true

  virtual LinkMonitor* AddLinkMonitor(const LinkMonitor&);//Ϊ����·���һ��LinkMonitor����
private: 
  void ConstructorHelper(Rate_t, Time_t);//��·��Ա��ʼ��
protected:
  bool     busy;             // True��ʾ��·���ڴ������ݰ�
  Random*  jitter;           // ���ݰ���������е�����
protected:
  Rate_t   bw;               // ��·����
  Time_t   delay;            // �ӳ�
  Weight_t weight;           // ��·Ȩ��
  Mult_t   bitErrorRate;     // λ�����
  DCount_t bytesSent;        // ����·�Ϸ������ݵ�����
  DCount_t pktsSent;         // ����·�Ϸ������ݰ�������
  Time_t   utilizationStart; // ��·�����ʿ�ʼ����ʱ��
  NList_t  notifications;    // ֪ͨ����
  NotifyHandler* notify;     // ���ݰ��������֪ͨ
  Time_t   nextFreeTime;     // ��·����ʱ��
  LinkMonitor* linkMonitor;  // ��ѡ��LinkMonitor����
protected:
  bool     debugVerbose;     // True������Ҫverbose debug
};

#include "l2proto.h"

#endif

