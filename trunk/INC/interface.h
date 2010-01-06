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
//数据包发送
  virtual bool Send(Packet*, IPAddr_t, int) = 0;//通过本接口传输数据包
  ////virtual bool Send(Packet*, const MACAddr&, int) = 0;//通过本接口传输数据包，不使用ARP
  virtual bool Send(Packet*, Word_t) = 0;//通过本接口广播数据包
  ////virtual bool Xmit(Packet*, IPAddr_t, int){ return false;}//通过本接口传输数据包，检查是否需要使用ARP
  virtual void Handle(Event*, Time_t);//处理数据包接收事件
  virtual void Notify(void*);//通知队列空间是否充足
  virtual Mult_t   TrafficIntensity() const = 0;//计算本接口的业务量强度(每秒传输数据量/链路带宽)  
//IP、Mac地址
  ////virtual void AddARPEntry(IPAddr_t, MACAddr) {}//在本接口arp缓存中新增arp项。（参数一——邻接结点的IP  参数二——相应的mac地址）
  void         SetIPAddr(IPAddr_t i) { ip = i;}//设置本接口的IP地址
  IPAddr       GetIPAddr() const { return ip;}//获取本接口的IP地址
  Mask_t       GetIPMask() const { return mask;}//获取本接口IP地址的掩码
  void         SetMACAddr(const MACAddr& m);//设置本接口的MAC地址
  MACAddr      GetMACAddr() const { return macAddr;} //获取本接口的MAC地址
//结点相关
  virtual Count_t  PeerCount() const = 0;         //获取与本接口相连的结点的个数
  virtual IPAddr   PeerIP(int) const = 0;         //根据结点index获取与本接口相连的结点的IP地址
  virtual IPAddr   NodePeerIP(Node*) const = 0;   //根据结点获取与本接口相连的结点的IP地址
  virtual bool     NodeIsPeer(Node*) const = 0;   //判断目标结点是不是与本接口直接相连，若是返回 true
  virtual Count_t  NodePeerIndex(Node*) const = 0;//获取与本接口相连的结点的index
  Node*            GetNode() const { return pNode;}//获取与本接口相连的结点
#ifdef MOVED_TO_BASE
  virtual void     SetNode(Node* n) = 0;//设置与本接口相连的结点
  virtual Link*    GetLink() const = 0;//获取与本接口相连的链路
  virtual void     SetLink(Link*) = 0;//设置与本接口相连的链路
#endif
//队列
  virtual Queue*   GetQueue() const = 0;//获取与本接口相连的队列
  virtual void     SetQueue(const Queue& q) = 0;//设置与本接口相连的队列
  virtual bool     QueueDetailed() const = 0;// True表示队列已被定义
//协议  
  virtual L2Proto* GetL2Proto() const = 0;////返回与本接口相连的第二层协议对象
  virtual void     SetL2Proto(const L2Proto&) = 0;//设置与本接口相连的第二层协议对象
//通知  
  virtual void     AddNotify(NotifyHandler*,void*) = 0;//添加一个notify对象，用来通知与本接口相连的链路是空闲的
  virtual void     CancelNotify(NotifyHandler*) = 0;//删除一个notify通知
  

//获取邻接结点列表
  virtual MACAddr  IPToMac(IPAddr_t) const = 0;//根据远程结点的IP地址，返回其MAC地址 
  virtual Count_t  NeighborCount(Node*) const = 0;//通过查询与本接口相连的链路来获取路由上邻接结点的个数
  virtual void     Neighbors(NodeWeightVec_t&, bool = false) = 0;//获取一串与本接口直接相连的邻接结点
  virtual IPAddr_t DefaultPeer() = 0;//返回绑定本接口的默认结点IP地址，若没有通过链路默认结点IP地址
  virtual bool     IsLocalIP(IPAddr_t) const = 0;//判断指定IP是否与本接口在同一子网

//分布式、无线、Ethernet、ghost
  ////virtual void     AddRemoteIP(IPAddr_t, Mask_t, Count_t = 0) = 0;//分布式路由
  virtual void     HandleLLCSNAP(Packet*, bool) = 0;//处理 LLCSnap数据包包头
  ////virtual bool     IsWireless() const = 0; 
  ////virtual bool     IsEthernet() const { return false; }
  ////virtual bool     IsReal() const = 0;  
  ////virtual void     RegisterRxEvent(LinkEvent*){return;} ; //Ethernet相关 
  ////virtual void     AddRxEvent(LinkEvent*){return;} ; //Ethernet相关

#ifdef MOVED_TO_BASIC
  virtual void     ScheduleRx(LinkEvent*, Time_t, bool = true);//在本地先进先出队列中记录接收数据包事件
  EventCQ*   GetEventCQ() { return evList; }//返回本地接口的事件列表
#endif
//蠕虫相关
  ////virtual void UseWormContainment(bool b){ usewormcontainment = b;}
  ////virtual bool UseWormContainment(){ return usewormcontainment;}
  ////virtual void SetWormContainment(WormContainment*) = 0;
  ////virtual WormContainment* GetWormContainment() = 0;
  ////virtual void UseARP(bool) = 0;//设置是否使用ARP模型
#ifdef MOVED_TO_BASIC
//接口状态管理
  bool IsDown() { return down; }//返回接口的状态
  void Down(bool noReInit = false);//接口状态设为关闭
  void Up();//接口状态设为开启
#endif

  bool FirstBitRx(){return firstBitRx;}
  void FirstBitRx(bool is){firstBitRx = is;}
  
  ////static void DefaultUseARP(bool b = true);//根据参数确定是否使用ARP，true代表使用
  static void Add(Interface*, MACAddr_t);//向mac地址列表中添加指定的接口
  static Interface* GetByMac(MACAddr_t);//在mac地址列表中根据制定mac地址获取相应的接口

protected:
  IPAddr ip;// 本接口的IP地址
  Mask_t mask;// 所在网络的掩码
  MACAddr macAddr;// 本接口MAC地址
  bool down;// True表示本接口关闭
  ////bool usewormcontainment;// use wormcontainment module
  bool firstBitRx;// True表示是satellite接口类型

public:
  bool   bootstrap;  //true代表需要所有的接口指针的mac地址列表
  static bool recordMacAddresses;//true代表需要记录mac地址
  static std::vector<Interface*> macIfVec;//接口指针的mac地址列表
  ////static bool defaultUseARP;//true代表使用ARP
};

#endif
