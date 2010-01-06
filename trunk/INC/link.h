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
/* 2010.1.4  关怡萍                                                     */
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
  PACKET_RX,//接收数据包
  PACKET_FIRSTBIT_RX, //接收数据包第一位
  PACKET_TX,  //数据包发送完毕
  NOTIFY,     //通知对象缓存区已经空闲
  LINK_FAIL,  //关闭一个连接
  LINK_RESTORE//重新建立连接
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
  Packet* p; //绑定的数据包
  Count_t numberBytes; //数据包的大小（仅用于PACKET_TX事件)
  bool    hasBitError; //检查接收者是否检测位错误
  bool    fifoEvent;   //说明事件是否在本地interface的fifo队列中
};

//基类，实际的类包括p2p、Ethernet、802.11
class Link : public Handler {

public:

  Link();//创建一条默认的链路
  Link(Rate_t b, Time_t d);//创建一条链路，指定带宽和延时
  Link(const Link&); //根据指定链路对象，创建一条新链路
  virtual ~Link() { };//析构函数
  virtual Link* Copy() const = 0; //复制该链路并返回
  static void     Default(const Link& l);//为本链路设置一个指定的链路对象
  static const    Link& Default();//返回本链路的链路对象
//接口有关
  virtual void  SetPeer(InterfaceBasic* i) = 0;//设置链路另一端对等的接口
  virtual InterfaceBasic* GetLocalIf() const; //返回本地接口的指针
  virtual void SetLocalIf(InterfaceBasic*); //设置本地传输接口
  virtual InterfaceBasic* GetPeer(Packet*) = 0;//根据传输的数据包获取链路另一端结点的接口
  virtual InterfaceBasic* GetPeer(Count_t) = 0;//根据结点index获取链路另一端结点的接口
  virtual Interface* GetNewInterface(IPAddr_t = IPADDR_NONE,//为本链路新增一个指定接口
	  Mask_t = MASK_ALL) const;

//传输有关
  virtual bool Transmit(Packet*, Interface*, Node*) = 0; //通过本链路传输一个数据包
  virtual bool Transmit(Packet*, Interface*, Node*, Rate_t) = 0;//通过本链路以一定的带宽传输一个数据包
  virtual bool Transmit(LinkEvent*, Interface*, Time_t) = 0;//通过一个链路事件传输数据包 
  virtual bool TransmitWithDelay(Packet*, Time_t) = 0;//传输数据包，考虑队列延迟
  void    Handle(Event*, Time_t) = 0;//处理本链路上的事件列表
  virtual void AddNotify(NotifyHandler*,void*) = 0;//增加一个通知对象  
  virtual Time_t NextFreeTime() = 0;//预测链路空闲时的系统时间（不考虑jitter和contention）
  virtual bool   RxBroadcast() = 0;         //判断本链路是否接收一个自己广播的数据包，返回true则代表接收
//设置 
  virtual bool   Busy() const = 0;//true代表该链路当前在传输数据包
  virtual void   Bandwidth(Rate_t b) = 0;//设置链路的带宽
  virtual Rate_t Bandwidth() const = 0;//返回链路的带宽
  virtual void   Delay(Time_t d) = 0;//设置链路的延时
  virtual Time_t Delay() const = 0;//返回链路的延时
  virtual void BitErrorRate(Mult_t b) = 0;//设定本链路在传输时的差错率
  virtual Mult_t BitErrorRate() = 0;//返回本链路在传输时的差错率
  virtual void ResetUtilization() = 0;//设置本链路的链路利用率为0
  virtual Mult_t Utilization() = 0;//返回本链路的链路利用率
  virtual Weight_t Weight() = 0;//获取本链路的权重
  virtual void Weight(Weight_t w) = 0;//设置本链路的权重
  virtual void Jitter(const Random&) = 0;  //设置链路在传输时的jitter延迟（该函数是使jitter生效的唯一方法）
  virtual bool   ScheduleFailure(Time_t t) = 0;//设置在特定仿真时间t本链路失效，若设置成功返回true
  virtual bool   ScheduleRestore(Time_t t) = 0;//设置在特定仿真时间t本链路重新激活，若设置成功返回true
//结点相关
  virtual Count_t  PeerCount() = 0; //返回本链路连接的结点数（real中未实现，ghost中返回1）
  virtual IPAddr_t PeerIP(Count_t) = 0;//根据结点index返回其IP地址，若不存在返回空
  virtual  IPAddr_t NodePeerIP(Node*) = 0;   //返回与本链路相连的指定结点的ip地址
  virtual  bool NodeIsPeer(Node*) = 0;   //判断指定结点是不是与本链路相连的一个结点，如果是，返回true
  virtual  Count_t  NodePeerIndex(Node*) = 0;//根据指定结点，返回其index
  virtual  IPAddr_t DefaultPeer(Interface*) = 0; //返回与本链路指定接口相连的结点的ip地址
  virtual Count_t NeighborCount(Node*) = 0;//获取指定结点通过本链路相连的路由上邻接结点的个数
  virtual void Neighbors(NodeWeightVec_t&, Interface*, bool) = 0;//向包含所有路由邻接结点的数组中根据本地接口添加与之对应的邻接结点
  virtual void AllNeighbors(NodeWeightVec_t&) = 0;//获取该链路的所有邻接结点
  virtual MACAddr  IPToMac(IPAddr_t) = 0;     //根据结点IP地址获取其MAC地址并返回
  virtual DCount_t BytesSent() = 0;//返回该链路所发送的数据总量
  virtual DCount_t PktsSent() = 0;//返回该链路所发送的数据包的总数

  virtual LinkMonitor* AddLinkMonitor(const LinkMonitor&);//为本链路添加一个LinkMonitor对象

//动画相关
  virtual void AnimateLink();//在动画显示时画出该链路
  virtual void ReAnimateLink();//当链路状态改变后，重现画出该链路
  virtual void AnimatePackets();//画出在该链路上传输的数据包
public:
//设置带宽、抖动
  static void     DefaultRate(Rate_t r);//设置链路的默认带宽
  static Rate_t   DefaultRate() { return defaultRate;}//获取链路的默认带宽
  static void     DefaultDelay(Time_t t);//设置链路的默认延迟
  static Time_t   DefaultDelay() { return defaultDelay;}//获取链路的默认延迟
  static void     DefaultJitter(const Random& j);//设置链路的默认jitter
  static Random*  DefaultJitter();//返回链路的默认jitter

  static void     UseSeqEvents(bool u) { useSeqEvents = u;}//u为true则设置linkevent发生的位置在本链路事件队列，否则设置其发生位置在系统事件队列

public:
  InterfaceBasic* localIf; //本链路接口
  
protected:
  static Rate_t   defaultRate;      //默认带宽
  static Time_t   defaultDelay;     //默认延时
  static Link*    defaultLink;      //默认链路类型
  static Random*  defaultJitter;    //默认jitter随机数
  static Random*  berRng;           //位错误比例随机数
  static bool     useSeqEvents;     // True代表将linkevent放在本地事件队列，否则放在系统事件队列
};

#include "l2proto.h"

#endif

