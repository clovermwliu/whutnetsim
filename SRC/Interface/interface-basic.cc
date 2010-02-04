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


#include <iostream>

#include "G_debug.h"
#include "interface-basic.h"
#include "interface.h"
#include "eventcq.h"
#include "simulator.h"
#include "link.h"
#include "gui_ctrl.h"

using namespace std;

InterfaceBasic::InterfaceBasic()
    : pNode(nil), pLink(nil), evList(nil), down(false),used(false)
{
  DEBUG0((cout << "InterfaceBasic default constructor" << endl));

}

InterfaceBasic::InterfaceBasic(Node* n)
    : pNode(n), pLink(nil), evList(nil), down(false),used(false)
{
  DEBUG0((cout << "InterfaceBasic constructor " << this << endl));

}

InterfaceBasic::~InterfaceBasic()
{
	//释放link和本地event队列，需要在子类中调用父类的析构函数 2010-1-29
	//if ( pLink != nil )
	//{
	//	delete pLink;
	//}
	if ( used == true ) 
	{
		delete evList;
		used = false;
	}

}

// Local event list management
// We go ahead and include the pending rx event list in the
// base class, which allows animation to work properly with
// the distrbuted GTNetS.  Ghost nodes at the end of an rlink
// will maintain this list, so packets can animate when trasversing
// an rlink.  Also used by supercomputer interconnects

void InterfaceBasic::ScheduleRx(LinkEvent* e, Time_t t, bool schedule)
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
	  used = true;
    }
  bool needSchedule = evList->empty();
  e->fifoEvent = true; // Note this event is in fifo queue
  Time_t when = Simulator::Now() + t;
  EventPair* evp = evList->next();
  if (!evp)
    {
      evp = new EventPair(when, e);
    }
  else
    {
      evp->time = when;
      evp->event = e;
    }
  evList->push_back(evp);
  if (needSchedule && schedule)
    { 
      Scheduler::Schedule(e, t, this);
    }
}

void InterfaceBasic::Down(bool noReInit)
//设定本地接口状态为关闭
{
  down = true;
  if (!noReInit) Simulator::instance->TopologyChanged(false);
#ifdef HAVE_QT
  //需要重新显示
  gui_ctrl* qtw = gui_ctrl::qtWin;
  Link* link = GetLink();
  if (qtw && link)
    {
      Count_t nc = link->PeerCount();
      if (nc == 1)
        { //p2p链路，获取目的地接口
          InterfaceBasic* ifaceDst = link->GetPeer((Count_t)0);
          qtw->RedrawP2P(this, ifaceDst);
        }
    }
#endif
}

void InterfaceBasic::Up()
//设置本地接口状态为重新启动
{
  down = false;
  Simulator::instance->TopologyChanged(true);
#ifdef HAVE_QT
  //需要重新显示
  gui_ctrl* qtw = gui_ctrl::qtWin;
  Link* link = GetLink();
  if (qtw && link)
    {
      Count_t nc = link->PeerCount();
      if (nc == 1)
        { 
          InterfaceBasic* ifaceDst = link->GetPeer((Count_t)0);
          qtw->RedrawP2P(this, ifaceDst);
        }
    }
#endif
}


