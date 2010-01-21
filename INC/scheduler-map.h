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


//File Information: Scheduler class using STL map container
//File Name: scheduler-map.h
//File Purpose: Event list management
//Original Author:  George F. Riley.
//Author Organization:  Georgia Tech
//Construct Data:  Fall 2005
//Modify Author:  YueYuntao
//Author Organization: WHU
//Modify Data: JAN, 2010

#ifndef __scheduler_map_h__
#define __scheduler_map_h__

#include <map>

#include "scheduler.h"

// 定义map类型保存事件
typedef std::map<KeyPair, Event*, key_less> EventMap_t;

class MapScheduler : public Scheduler {
  //MapScheduler类用STL容器实现了事件调度机制
public:
  MapScheduler();
  //无参数的MapScheduler构造方法

  virtual ~MapScheduler();

protected:   

  virtual Scheduler* Copy() const;//所有的scheduler的子类必须可以复制自身，返回新拷贝的指针
  virtual void prCancel(Event*); //取消一个正在队列中的事件
  virtual void prSchedule(Event*);//安排一个新事件，当前事件的仿真时间以及handler需要在建立事件的类中确定
  virtual void prSchedule(Event*, Time_t);//安排一个新事件，当前事件的仿真时间以及handler需要在建立事件的类中确定
  virtual void prSchedule(Event*, Time_t, Handler*);//安排一个新事件，当前事件的仿真时间需要在建立事件的类中确定
  virtual void prSchedule(Event*, Time_t, Handler&);//安排一个新事件，当前事件的仿真时间需要在建立事件的类中确定
  virtual void prScheduleEarly(Event*, Time_t, Handler*);
  virtual Event* prDeQueue();//取得事件队列中要出队的事件并返回它的指针
  virtual Event* prPeekEvent();//返回事件队列中要出队的事件并返回它的指针，但不将它从队列中移除

public:
  
  void    PrintStats();//打印仿真当中的一些细节信息
  bool    IsScheduled(Event*);//调试用，检测事件是否放入事件队列中

private:
  EventMap_t      eventList;   //事件队列
};

#endif


