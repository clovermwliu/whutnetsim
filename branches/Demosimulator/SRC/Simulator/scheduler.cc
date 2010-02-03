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


//File Information: Scheduler class
//File Name: scheduler.cc
//File Purpose: Event list management
//Original Author:  George F. Riley.
//Author Organization:  Georgia Tech
//Construct Data:  Fall 2005
//Modify Author:  YueYuntao
//Author Organization: WHU
//Modify Data: JAN, 2010

#include <iostream>

#include "scheduler.h"
#include "scheduler-map.h"

using namespace std;

SimulatorUid_t  Scheduler::uid = 0;       //下一个可用的事件ID
Scheduler*      Scheduler::instance = nil;//当前scheduler的实例

//成员函数

Scheduler::Scheduler()
{ 
}

Scheduler::~Scheduler()
{ 
	Scheduler::uid = 0;       //下一个事件可用的ID
	Scheduler::instance = nil;  //当前的scheduler的实例
}

  
void Scheduler::PrintStats()              //打印调试信息
{
  cout << "Simulator::PrintStats, totevs " << totevs
       << " totrm " << totrm 
       << " totevp " << totevp 
       << " totevc " << totevc 
       << " size " << evlSize << endl;
}

//静态方法
Scheduler* Scheduler::Instance()
{
  if (!instance) instance = new MapScheduler();
  return instance;
}

void Scheduler::Cancel(Event* e)   //取消事件
{
  Instance()->prCancel(e);
}

void Scheduler::Schedule(Event* e)     //安排事件
{
  Instance()->prSchedule(e);
}

void Scheduler::Schedule(Event* e, Time_t t)  //安排事件
{
  Instance()->prSchedule(e, t);
}

void Scheduler::Schedule(Event* e, Time_t t, Handler* h)
{
  Instance()->prSchedule(e, t, h);
}

void Scheduler::Schedule(Event* e, Time_t t, Handler& h)
{
  Instance()->prSchedule(e, t, h);
}

void Scheduler::ScheduleEarly(Event* e, Time_t t, Handler* h)
{
  Instance()->prScheduleEarly(e, t, h);
}

Event* Scheduler::DeQueue()
{
  return Instance()->prDeQueue();
}

Event* Scheduler::PeekEvent()
{
  return Instance()->prPeekEvent();
}

bool Scheduler::ClearEventlist()
{
	return Instance()->Clearlist();
}
