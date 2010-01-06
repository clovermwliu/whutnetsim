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

typedef std::vector<QCanvasPolygon*> PacketVec_t;//待显示数据包的数组


class Linkp2p : public LinkReal 
{

public:
  
  Linkp2p();//创建一条链路
  Linkp2p(Rate_t b, Time_t d);//创建一条链路，设定其带宽和延时
  Linkp2p(const Linkp2p&);//根据指定链路对象创建一条链路     
  ~Linkp2p();
  Link* Copy() const ;//复制该链路并返回
  void  SetPeer(InterfaceBasic* i) { pPeer = i;}//设置链路另一端对等的接口
  InterfaceBasic* GetPeer(Packet*) { return pPeer;}//根据传输的数据包获取链路另一端结点的接口
  InterfaceBasic* GetPeer(Count_t) { return (Interface*)pPeer;}//根据结点index获取链路另一端结点的接口
  bool Transmit(Packet*, Interface*, Node*);//通过本链路传输一个数据包
  bool Transmit(Packet*, Interface*, Node*, Rate_t);//通过本链路以一定的带宽传输一个数据包
  MACAddr    IPToMac(IPAddr_t);    //根据结点IP地址获取其MAC地址并返回
//构建邻接结点表
  Count_t    NeighborCount(Node*);   //返回本链路连接的结点数（real中未实现，ghost中返回1）
  void       Neighbors(NodeWeightVec_t&, Interface*, bool);//向包含所有路由邻接结点的数组中根据本地接口添加与之对应的邻接结点
  void       AllNeighbors(NodeWeightVec_t&);//获取该链路的所有邻接结点
  
//Linkp2p独有的方法
  Count_t    PeerCount(); //返回本链路连接的结点数（real中未实现，ghost中返回1）
  IPAddr_t   PeerIP(Count_t);//根据结点index返回其IP地址，若不存在返回空
  IPAddr_t   NodePeerIP(Node*);//返回与本链路相连的指定结点的ip地址
  bool       NodeIsPeer(Node*); //判断指定结点是不是与本链路相连的一个结点，如果是，返回true
  Count_t    NodePeerIndex(Node*); //根据指定结点，返回其index
  IPAddr_t   DefaultPeer(Interface*); //返回与本链路指定接口相连的结点的ip地址
  virtual bool ScheduleFailure(Time_t t);//设置在特定仿真时间t本链路失效，若设置成功返回true
  virtual bool ScheduleRestore(Time_t t);//设置在特定仿真时间t本链路重新激活，若设置成功返回true

  virtual void AnimateLink();//在动画显示时画出该链路
  virtual void ReAnimateLink();//当链路状态改变后，重现画出该链路
  virtual void AnimatePackets();//画出在该链路上传输的数据包
private:
  void DisplayOnePacket(Node*, Node*, Mult_t, Mult_t, const QColor&);//显示单个的数据包动画
public:
  InterfaceBasic* pPeer;       //p2p链路对等一端的接口
  QCanvasLine* linkAnimation;  //链路显示的画布单元
  PacketVec_t animatedPackets;//存储数据包动画的数组，以便重复使用
  PacketVec_t::size_type usedAnim;//已经使用过的数据包动画数目

public:

  static Linkp2p& Default();  //返回默认的p2p链路指针

  static Linkp2p& Default(const Linkp2p&); //设置新的默认的p2p链路指针

private:
  static Linkp2p* defaultp2p;//默认的p2p链路指针
public:
  static Count_t  count;
};

#endif


