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
//File Name: scheduler.h
//File Purpose: Event list management
//Original Author:  George F. Riley.
//Author Organization:  Georgia Tech
//Construct Data:  Fall 2005
//Modify Author:  YueYuntao
//Author Organization: WHU
//Modify Data: JAN, 2010

#ifndef __scheduler_h__
#define __scheduler_h__

#include "G_common_defs.h"
#include "object.h"
#include "handler.h"
#include "event.h"

//为了保证可重复性的结果，事件利用事件的UID进行了排序
class KeyPair {
public:
  KeyPair(Time_t t, SimulatorUid_t u) : time(t), uid(u) { }
public:
  Time_t         time;
  SimulatorUid_t uid;
};

// 事件队列管理的定义
typedef std::pair<Time_t,SimulatorUid_t> KeyPair_t; //map结构的key
class key_less
{
public:
  key_less() { }
  bool operator()(const KeyPair& l, const KeyPair& r) const {
    return (l.time < r.time ||
            l.time == r.time && l.uid < r.uid);
  }
};

class MapScheduler;


class Scheduler  { 
  //Scheduler类是一个抽象的基类，它定义了仿真所需的方法
public:

  Scheduler();
    //Scheduler的构造方法没有任何参数


  virtual ~Scheduler();
public:   

  virtual Scheduler* Copy() const = 0;//所有的scheduler的子类必须可以复制自身,返回新拷贝的指针

protected:

  virtual void prCancel(Event*) = 0; //取消一个正在队列中的事件
  virtual void prSchedule(Event*) = 0;//安排一个新事件，当前事件的仿真时间以及handler需要在建立事件的类中确定
  virtual void prSchedule(Event*, Time_t) = 0;   //安排一个新事件，当前事件的仿真时间以及handler需要在建立事件的类中确定
  virtual void prSchedule(Event*, Time_t, Handler*) = 0; //安排一个新事件，当前事件的仿真时间需要在建立事件的类中确定
  virtual void prSchedule(Event*, Time_t, Handler&) = 0; //安排一个新事件，当前事件的仿真时间需要在建立事件的类中确定
  virtual void prScheduleEarly(Event*, Time_t, Handler*) = 0;
  virtual Event* prDeQueue() = 0;	//取得事件队列中要出队的事件并返回它的指针
  virtual Event* prPeekEvent() = 0; //返回事件队列中要出队的事件并返回它的指针，但不将它从队列中移除
public:
  virtual bool    IsScheduled(Event*) = 0;//调试用，检测事件是否进入事件队列
  void    PrintStats(); //打印仿真当中的一些细节信息
  Count_t TotalEventsProcessed() { return totevp;}//返回仿真处理过的事件总数
  Count_t TotalEventsScheduled() { return totevs;}//返回仿真安排进过事件队列的事件总数
  Count_t EventListSize() { return evlSize;}//返回当前事件队列的大小，即仍在排队的，未处理的事件总数
  //静态方法
  static Scheduler* Instance();//取得Scheduler自身的指针，如果没有的话新建一个MapScheduler的对象
  static void Cancel(Event*);//取消一个正在队列中的事件
  static void Schedule(Event*);//安排一个新事件，当前事件的仿真时间以及handler需要在建立事件的类中确定
  static void Schedule(Event*, Time_t);//安排一个新事件，当前事件的仿真时间以及handler需要在建立事件的类中确定
  static void Schedule(Event*, Time_t, Handler*); //安排一个新事件，当前事件的仿真时间需要在建立事件的类中确定
  static void Schedule(Event*, Time_t, Handler&); //安排一个新事件，当前事件的仿真时间需要在建立事件的类中确定
  static void ScheduleEarly(Event*, Time_t, Handler*);
  static Event* DeQueue();//取得事件队列中要出队的事件并返回它的指针
  static Event* PeekEvent(); //返回事件队列中要出队的事件并返回它的指针，但不将它从队列中移除
public:

  Count_t         totevs;      //被安排过的事件总数
  Count_t         totrm;       //从队列中移除过的事件总数
  Count_t         totevp;      //运行过的事件总数
  Count_t         totevc;      //取消过的事件总数
  Count_t         evlSize;     //事件队列中的事件总数
  Count_t         maxSize;     //仿真中事件队列里事件数量的最大值
protected:
  static SimulatorUid_t  uid;       //下一个事件可用的ID
  static Scheduler*      instance;  //当前的scheduler的实例
};

#endif


