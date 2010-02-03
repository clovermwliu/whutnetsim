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
/* 2010.1.4  关怡萍                                                     */
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

class IpMaskCount { // IP地址，掩码和hopcount
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
接口被分配给结点，有相应的链路对象代表与链路的交流。接口有队列来存储链路来不及存储的数据包队列， 
和一个用来交流的第二协议层对象。接口不是直接被仿真器配置的，而是在创建链路的过程中被间接创建的
*/
{
public:

  InterfaceReal(const L2Proto& l2 = L2Proto802_3(),
				IPAddr_t i   = IPADDR_NONE,
				Mask_t   m   = MASK_ALL,
				MACAddr  mac = MACAddr::NONE,
				bool bootstrap = false);    //创建一个新的real型接
  virtual ~InterfaceReal();
  ////void SetARPTimeout(Time_t t){ arptimeout = t;}//ARP时间

//传输数据包
  bool Send(Packet*, IPAddr_t, int); //通过本接口传输数据包
  bool Xmit(Packet*, IPAddr_t, int); //通过本接口传输数据包，检查是否需要使用ARP
  bool Send(Packet*, const MACAddr&, int); //通过本接口传输数据包，不使用ARP
  bool Send(Packet*, Word_t); //通过本接口广播数据包
  bool EnquePacket(Packet*);//将数据包加入队列，若成功返回true，否则返回false

  virtual void Handle(Event*, Time_t);//处理数据包接收事件
  virtual void Notify(void*);//通知队列空间是否充足
  
//结点相关操作
  Count_t PeerCount() const; //返回与该接口相连的结点个数
  IPAddr PeerIP(int npeer) const;//根据结点index查找其IP地址并返回，若不存在则返回空
  IPAddr NodePeerIP(Node*) const;//根据结点查找它与本接口相连的接口，并将其IP地址返回，若不存在则返回空
  bool NodeIsPeer(Node*) const;//检查结点是否是与本接口直接相连的结点，若是则返回真
  Count_t NodePeerIndex(Node*) const; //获取结点的index
  void SetNode(Node*);//设置本接口连接的结点

#ifdef MOVED_TO_BASIC
//链路相关操作
  Link* GetLink() const;//返回与本接口相连的链路对象
  void SetLink(Link*);//设置与本接口相连的链路对象
#endif
//队列相关操作
  Queue* GetQueue() const;//返回与本结点相连的队列对象
  void SetQueue(const Queue& q);//设置一个新的队列对象
  bool QueueDetailed() const;//若队列已被定义，则返回true，否则返回flase
//协议相关操作 
  L2Proto* GetL2Proto() const { return pL2Proto;} //返回与本接口相连的第二层协议对象
  void SetL2Proto(const L2Proto& l2);//设置与本接口相连的第二层协议对象
  void AddNotify(NotifyHandler*,void*);//添加一个notify对象，用来通知与本接口相连的链路是空闲的
  void CancelNotify(NotifyHandler*);//删除一个notify通知
  MACAddr IPToMac(IPAddr_t) const;//根据远程结点的IP地址，返回其MAC地址 
  Count_t NeighborCount(Node*) const;//通过查询与本接口相连的链路来获取路由上邻接结点的个数

//获取邻接结点列表
  void Neighbors(NodeWeightVec_t&, bool forceAll = false);//获取一串与本接口直接相连的邻接结点
  IPAddr_t DefaultPeer();//返回绑定本接口的默认结点IP地址，若没有通过链路默认结点IP地址
  bool IsLocalIP(IPAddr_t) const;//判断指定IP是否与本接口在同一子网
  Mult_t TrafficIntensity() const;//计算本接口的业务量强度(每秒传输数据量/链路带宽)
//分布式、无线、Ethernet  
  ////void AddRemoteIP(IPAddr_t, Mask_t, Count_t = 0);//分布式路由
  void HandleLLCSNAP(Packet*, bool);//处理无线数据包，加上LLC包头
  ////virtual bool IsWireless() const { return false;}//true代表是无线接口
  ////virtual bool IsReal() const { return true;}//true表示是real型接口
  ////virtual bool IsEthernet() const {return false;}// True表示是Ethernet型接口
//蠕虫相关 
  ////void SetWormContainment(WormContainment*);
  ////WormContainment* GetWormContainment() { return wormcontainment;}
//ARP
  ////void UseARP(bool);  //根据参数确定是否使用ARP协议

protected:

  virtual void        PacketRxStart(LinkEvent*);
  virtual void        PacketRxEnd(bool, Size_t);

private:
  Queue*           pQueue;          //与本接口相连的队列
  NList_t          notifications;   //notification队列（“通知”队列）
  LinkEvent*       notifEvent;      //notify时间(“通知”事件)
  DCount_t         totBytesSent;    //记录待发送的所有数据总量
  L2Proto*         pL2Proto;        //与本接口相连的第二层协议对象
  IPAddr_t         defaultPeer;     //与本接口相连的默认结点IP地址

  ////bool             usearp;          //true代表使用ARP协议
  ////ARP*             arpmodule;       //ARP模型
  ////Time_t           arptimeout;      //arp时间阀
  ////WormContainment* wormcontainment; //蠕虫功能

private:
//分布式相关
  ////IPMaskCountVec_t remoteIPList;
private:
  void CallNotification(bool); 
public:
  static Count_t count;//调试所用
};

#endif
