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
  LinkReal();//创建一条默认的链路
  LinkReal(Rate_t b, Time_t d);//创建一条链路，指定带宽和延时
  LinkReal(const LinkReal&); //根据指定链路对象，创建一条新链路
  virtual ~LinkReal();//析构函数
  virtual Link*  Copy() const = 0; //复制该链路并返回
  virtual LinkReal& operator=(const LinkReal&);
//链路管理
  virtual void  SetPeer(InterfaceBasic* i) = 0;//设置链路另一端对等的接口
  virtual InterfaceBasic* GetPeer(Packet*) = 0;//根据传输的数据包获取链路另一端结点的接口
  virtual InterfaceBasic* GetPeer(Count_t) = 0;//根据结点index获取链路另一端结点的接口
  virtual  bool Transmit(Packet*, Interface*, Node*) = 0;//通过本链路传输一个数据包
  virtual  bool Transmit(Packet*, Interface*, Node*, Rate_t) = 0;//通过本链路以一定的带宽传输一个数据包
  virtual  bool Transmit(LinkEvent*, Interface*, Time_t);//通过一个链路事件传输数据包 
  virtual  bool TransmitWithDelay(Packet*, Time_t);//传输数据包，考虑队列延迟
  void     Handle(Event*, Time_t);//处理本链路上的事件列表
  void     Bandwidth(Rate_t b) { bw = b;}//设置带宽
  Rate_t   Bandwidth() const   { return bw; }//返回链路带宽
  void     Delay(Time_t d)     { delay = d;}//设置链路延迟
  Time_t   Delay() const       { return delay;}//返回链路延迟
  bool     Busy() const        { return busy;}//返回链路状态是否为繁忙
  virtual  Count_t  PeerCount() = 0;        //返回本链路连接的结点数（real中未实现，ghost中返回1）
  virtual  IPAddr_t PeerIP(Count_t) = 0;    //根据结点index返回其IP地址，若不存在返回空
  virtual  IPAddr_t NodePeerIP(Node*) = 0;  //返回与本链路相连的指定结点的ip地址
  virtual  bool     NodeIsPeer(Node*) = 0;  //判断指定结点是不是与本链路相连的一个结点，如果是，返回true
  virtual  Count_t  NodePeerIndex(Node*) = 0;//根据指定结点，返回其index
  virtual  IPAddr_t DefaultPeer(Interface*) = 0; //返回与本链路指定接口相连的结点的ip地址
  void     BitErrorRate(Mult_t b) { bitErrorRate = b;}//设置位差错率
  Mult_t   BitErrorRate() { return bitErrorRate;}//返回位差错率
  void     ResetUtilization();//设置本链路的链路利用率为0
  Mult_t   Utilization();//返回本链路的链路利用率
  void     AddNotify(NotifyHandler*,void*);//高层协议对象会在链路空闲时请求通知，本函数在列表中添加一个handler
  Weight_t Weight() { return weight;}//返回链路权重
  void     Weight(Weight_t w) { weight = w;}//设置链路权重
  void     Jitter(const Random&);            //设置jitter参数
  Time_t   NextFreeTime();                   //返回链路下一次空闲的时刻
  virtual Count_t  NeighborCount(Node*) = 0; //获取指定结点通过本链路相连的路由上邻接结点的个数
  virtual void     Neighbors(NodeWeightVec_t&, Interface*, bool) = 0;//向包含所有路由邻接结点的数组中根据本地接口添加与之对应的邻接结点
  virtual void     AllNeighbors(NodeWeightVec_t&) = 0;//获取该链路的所有邻接结点
  DCount_t         BytesSent() { return bytesSent;}//返回该链路所发送的数据总量
  DCount_t         PktsSent() { return pktsSent;}//返回该链路所发送的数据包的总数
protected:
  virtual void     TransmitComplete(Count_t); //数据包传输完成，设置相关参数
  virtual void     SendNotification();        // 处理通知列表中的第一个通知

  virtual bool     TransmitHelper(Packet*, 
								  Interface*,
								  Node*, 
								  Interface*, 
								  bool bCast = false);//构造发送和接受数据包事件

public:
  virtual MACAddr  IPToMac(IPAddr_t) = 0;     //根据结点IP地址获取其MAC地址并返回
  virtual bool     RxBroadcast() { return false;} //判断本链路是否接收一个自己广播的数据包，返回true则代表接收
  
  virtual bool     ScheduleFailure(Time_t) = 0;//设置在特定仿真时间t本链路失效，若设置成功返回true
  virtual bool     ScheduleRestore(Time_t) = 0;//设置在特定仿真时间t本链路重新激活，若设置成功返回true

  virtual LinkMonitor* AddLinkMonitor(const LinkMonitor&);//为本链路添加一个LinkMonitor对象
private: 
  void ConstructorHelper(Rate_t, Time_t);//链路成员初始化
protected:
  bool     busy;             // True表示链路正在传输数据包
  Random*  jitter;           // 数据包传输过程中的噪声
protected:
  Rate_t   bw;               // 链路带宽
  Time_t   delay;            // 延迟
  Weight_t weight;           // 链路权重
  Mult_t   bitErrorRate;     // 位差错率
  DCount_t bytesSent;        // 本链路上发送数据的总量
  DCount_t pktsSent;         // 本链路上发送数据包的总数
  Time_t   utilizationStart; // 链路利用率开始计算时间
  NList_t  notifications;    // 通知队列
  NotifyHandler* notify;     // 数据包发送完成通知
  Time_t   nextFreeTime;     // 链路空闲时刻
  LinkMonitor* linkMonitor;  // 可选的LinkMonitor对象
protected:
  bool     debugVerbose;     // True代表需要verbose debug
};

#include "l2proto.h"

#endif

