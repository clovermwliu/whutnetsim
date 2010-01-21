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

// ����map���ͱ����¼�
typedef std::map<KeyPair, Event*, key_less> EventMap_t;

class MapScheduler : public Scheduler {
  //MapScheduler����STL����ʵ�����¼����Ȼ���
public:
  MapScheduler();
  //�޲�����MapScheduler���췽��

  virtual ~MapScheduler();

protected:   

  virtual Scheduler* Copy() const;//���е�scheduler�����������Ը������������¿�����ָ��
  virtual void prCancel(Event*); //ȡ��һ�����ڶ����е��¼�
  virtual void prSchedule(Event*);//����һ�����¼�����ǰ�¼��ķ���ʱ���Լ�handler��Ҫ�ڽ����¼�������ȷ��
  virtual void prSchedule(Event*, Time_t);//����һ�����¼�����ǰ�¼��ķ���ʱ���Լ�handler��Ҫ�ڽ����¼�������ȷ��
  virtual void prSchedule(Event*, Time_t, Handler*);//����һ�����¼�����ǰ�¼��ķ���ʱ����Ҫ�ڽ����¼�������ȷ��
  virtual void prSchedule(Event*, Time_t, Handler&);//����һ�����¼�����ǰ�¼��ķ���ʱ����Ҫ�ڽ����¼�������ȷ��
  virtual void prScheduleEarly(Event*, Time_t, Handler*);
  virtual Event* prDeQueue();//ȡ���¼�������Ҫ���ӵ��¼�����������ָ��
  virtual Event* prPeekEvent();//�����¼�������Ҫ���ӵ��¼�����������ָ�룬���������Ӷ������Ƴ�

public:
  
  void    PrintStats();//��ӡ���浱�е�һЩϸ����Ϣ
  bool    IsScheduled(Event*);//�����ã�����¼��Ƿ�����¼�������

private:
  EventMap_t      eventList;   //�¼�����
};

#endif


