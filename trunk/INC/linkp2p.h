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
#ifndef __linkp2p_h__
#define __linkp2p_h__

#include <vector>

class InterfaceBasic;

#include "link-real.h"
#include "interface.h"
#include "l2proto802.3.h"

class QCanvasLine;
class QCanvasPolygon;
class QColor;

typedef std::vector<QCanvasPolygon*> PacketVec_t;//����ʾ���ݰ�������


class Linkp2p : public LinkReal 
{

public:
  
  Linkp2p();//����һ����·
  Linkp2p(Rate_t b, Time_t d);//����һ����·���趨��������ʱ
  Linkp2p(const Linkp2p&);//����ָ����·���󴴽�һ����·     
  ~Linkp2p();
  Link* Copy() const ;//���Ƹ���·������
  void  SetPeer(InterfaceBasic* i) { pPeer = i;}//������·��һ�˶ԵȵĽӿ�
  InterfaceBasic* GetPeer(Packet*) { return pPeer;}//���ݴ�������ݰ���ȡ��·��һ�˽��Ľӿ�
  InterfaceBasic* GetPeer(Count_t) { return (Interface*)pPeer;}//���ݽ��index��ȡ��·��һ�˽��Ľӿ�
  bool Transmit(Packet*, Interface*, Node*);//ͨ������·����һ�����ݰ�
  bool Transmit(Packet*, Interface*, Node*, Rate_t);//ͨ������·��һ���Ĵ�����һ�����ݰ�
  MACAddr    IPToMac(IPAddr_t);    //���ݽ��IP��ַ��ȡ��MAC��ַ������
//�����ڽӽ���
  Count_t    NeighborCount(Node*);   //���ر���·���ӵĽ������real��δʵ�֣�ghost�з���1��
  void       Neighbors(NodeWeightVec_t&, Interface*, bool);//���������·���ڽӽ��������и��ݱ��ؽӿ������֮��Ӧ���ڽӽ��
  void       AllNeighbors(NodeWeightVec_t&);//��ȡ����·�������ڽӽ��
  
//Linkp2p���еķ���
  Count_t    PeerCount(); //���ر���·���ӵĽ������real��δʵ�֣�ghost�з���1��
  IPAddr_t   PeerIP(Count_t);//���ݽ��index������IP��ַ���������ڷ��ؿ�
  IPAddr_t   NodePeerIP(Node*);//�����뱾��·������ָ������ip��ַ
  bool       NodeIsPeer(Node*); //�ж�ָ������ǲ����뱾��·������һ����㣬����ǣ�����true
  Count_t    NodePeerIndex(Node*); //����ָ����㣬������index
  IPAddr_t   DefaultPeer(Interface*); //�����뱾��·ָ���ӿ������Ľ���ip��ַ
  virtual bool ScheduleFailure(Time_t t);//�������ض�����ʱ��t����·ʧЧ�������óɹ�����true
  virtual bool ScheduleRestore(Time_t t);//�������ض�����ʱ��t����·���¼�������óɹ�����true

  virtual void AnimateLink();//�ڶ�����ʾʱ��������·
  virtual void ReAnimateLink();//����·״̬�ı�����ֻ�������·
  virtual void AnimatePackets();//�����ڸ���·�ϴ�������ݰ�
private:
  void DisplayOnePacket(Node*, Node*, Mult_t, Mult_t, const QColor&);//��ʾ���������ݰ�����
public:
  InterfaceBasic* pPeer;       //p2p��·�Ե�һ�˵Ľӿ�
  QCanvasLine* linkAnimation;  //��·��ʾ�Ļ�����Ԫ
  PacketVec_t animatedPackets;//�洢���ݰ����������飬�Ա��ظ�ʹ��
  PacketVec_t::size_type usedAnim;//�Ѿ�ʹ�ù������ݰ�������Ŀ

public:

  static Linkp2p& Default();  //����Ĭ�ϵ�p2p��·ָ��

  static Linkp2p& Default(const Linkp2p&); //�����µ�Ĭ�ϵ�p2p��·ָ��

private:
  static Linkp2p* defaultp2p;//Ĭ�ϵ�p2p��·ָ��
public:
  static Count_t  count;
};

#endif


