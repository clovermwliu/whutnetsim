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

#ifndef __interface_basic_h__
#define __interface_basic_h__

#include "G_common_defs.h"
#include "handler.h"
#include "node.h"

class EventCQ;
class Queue;
class Link;

class InterfaceBasic : public Handler
//所有interface的基类
{
public:
//基础函数
  InterfaceBasic();
  InterfaceBasic(Node*);
  virtual ~InterfaceBasic();

  Node* GetNode() const { return pNode;}//返回与本接口绑定的结点
  virtual void SetNode(Node* n) { pNode = n;}//设置与本接口绑定的结点
  virtual Link* GetLink() const { return pLink;} //返回与本接口绑定的链路
  virtual void SetLink(Link* l) { pLink = l;}//设置于本接口绑定的链路
  virtual Queue* GetQueue() const = 0;//返回与本结点相连的队列对象
  virtual void SetQueue(const Queue& q) = 0;//设置一个新的队列对象

  virtual void Neighbors(NodeWeightVec_t&, bool = false) = 0;//通过查询与本接口相连的链路来获取路由上邻接结点的个数

  virtual void ScheduleRx(LinkEvent*, Time_t, bool = true);//在本地先进先出队列中记录接收数据包事件

  EventCQ* GetEventCQ() { return evList; }//返回本地接口的事件列表

//接口状态管理
  bool IsDown() { return down; } //返回接口的状态
  void Down(bool noReInit = false);//接口状态设为关闭
  void Up();//接口状态设为开启
  
protected:
  Node* pNode;//与本接口绑定的结点
  Link* pLink;//与本接口绑定的链路
  EventCQ* evList;//事件列表
  bool down;//接口状态，True表示接口关闭
  bool used; //事件列表是否使用
};

#endif
