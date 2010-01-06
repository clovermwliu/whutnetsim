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

#include <iostream>

#include "G_debug.h"
#include "hex.h"
#include "interface.h"
#include "simulator.h"
#include "link.h"
#include "link-real.h"
#include "qtwindow.h"

using namespace std;

bool               Interface::recordMacAddresses = false;
vector<Interface*> Interface::macIfVec;
//bool               Interface::defaultUseARP = false;//v1.01

Interface::Interface() 
    : InterfaceBasic(), ip(IPADDR_NONE), mask(Mask(32)), macAddr(0),
      down(false),
      //usewormcontainment(false), //v1.01
	  firstBitRx(false)
{
  DEBUG0((cout << "Interface default constructor" << endl));
}

Interface::Interface(const L2Proto& l2,
                     IPAddr_t i,
                     Mask_t   m,
                     MACAddr  mac,
					 bool bootstrap)
: InterfaceBasic(), ip(i), mask(m), macAddr(mac),
  down(false), 
  //usewormcontainment(false),//v1.01
  firstBitRx(false)
{
  DEBUG0((cout << "Interface constructor " << this << " evList "
          << evList << endl));
}

Interface::Interface(const Interface& i)
    : InterfaceBasic(i), ip(i.GetIPAddr()),
      mask(i.GetIPMask()), macAddr(i.GetMACAddr()),
      down(false),
      //usewormcontainment(i.usewormcontainment),//v1.01
	  firstBitRx(i.firstBitRx)

{
  DEBUG0((cout << "Interface copy constructor " << endl));
}

Interface::~Interface()
{
}


void Interface::Handle(Event*, Time_t)
{
}

void Interface::Notify(void*)
{
}

#ifdef MOVED_TO_BASIC

void Interface::ScheduleRx(LinkEvent* e, Time_t t, bool schedule)
/*
描述：在本地先进先出队列中记录接收数据包事件
参数：e——接收数据包事件
	 t——该事件发生时间与当前时间的差值
	 schedule——一般默认为true
*/
{ 
  if (!evList)
    {
      evList = new EventCQ();
      DEBUG0((cout << "Allocated new eventcq " << evList 
              << " size " << evList->size()
              << endl));
    }
  
  bool needSchedule = evList->empty();
  e->fifoEvent = true; // Note this event is in fifo queue
  Time_t when = Simulator::Now() + t;
  DEBUG(2,(cout << "Interface::SRX, node " << GetNode()->Id()
           << " pushing time " << when << endl));
  EventPair* evp = evList->next();
  if (!evp)
    {
      DEBUG(3,(cout << "eventcq " << evList
               << " got nill, allocating new " << endl));
      evp = new EventPair(when, e);
    }
  else
    {
      DEBUG(3,(cout << "eventcq " << evList
               << " reusing evp " << evp << " old time "
               << evp->time << " new time " << when << endl));
      evp->time = when;
      evp->event = e;
    }
  evList->push_back(evp);
  DEBUG(3,(cout << "eventcq " << evList << " after pb, in " << evList->in
           << " out " << evList->out
           << " limit " << evList->limit
           << " size " << evList->size() 
           << " full " << evList->full()
           << endl));
  DBG((Stats::sequentialEvList++));
  if (needSchedule && schedule)
    { // List was empty, need to schedule in main simulator event list
      DEBUG(2,(cout << "Interface::SRX, node " << GetNode()->Id()
               << " when " << when << endl));
      Scheduler::Schedule(e, t, this);
    }
}

//失败管理
void Interface::Down(bool noReInit)
//设定本地接口状态为关闭
{
  down = true;
  // The noReInit parameter is used by the node down procedure
  // which in turn sets all interfaces down, and then calls
  // the TopologyChanged method.
  if (!noReInit) Simulator::instance->TopologyChanged(false);
#ifdef HAVE_QT
  // May need to redisplay as well
  QTWindow* qtw = Simulator::instance->GetQTWindow();
  Link* link = GetLink();
  if (qtw && link)
    {
      Count_t nc = link->PeerCount();
      if (nc == 1)
        { // Point to point link, get destination interface
          Interface* ifaceDst = link->GetPeer((Count_t)0);
          qtw->RedrawP2P(this, ifaceDst);
        }
    }
#endif
}

void Interface::Up()
//设置本地接口状态为重新启动
{
  down = false;
  Simulator::instance->TopologyChanged(true);
#ifdef HAVE_QT
  // May need to redisplay as well
  QTWindow* qtw = Simulator::instance->GetQTWindow();
  Link* link = GetLink();
  if (qtw && link)
    {
      Count_t nc = link->PeerCount();
      if (nc == 1)
        { // Point to point link, get destination interface
          Interface* ifaceDst = link->GetPeer((Count_t)0);
          qtw->RedrawP2P(this, ifaceDst);
        }
    }
#endif
}
#endif

//void Interface::DefaultUseARP(bool b)//v1.01
////根据参数确定是否使用ARP，true代表使用
//{ 
//  defaultUseARP = b;
//}


void Interface::Add(Interface* i, MACAddr_t m)
//向mac地址列表中添加指定的接口
{
  if (recordMacAddresses)
    {
      while(macIfVec.size() < m) macIfVec.push_back(nil);
      macIfVec.push_back(i);
    }
}

Interface* Interface::GetByMac(MACAddr_t m)
//在mac地址列表中根据制定mac地址获取相应的接口
{
  if (m >= macIfVec.size()) return nil;
  return macIfVec[m];
}

