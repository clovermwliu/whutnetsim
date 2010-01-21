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

//Ϊ�˱�֤���ظ��ԵĽ�����¼������¼���UID����������
class KeyPair {
public:
  KeyPair(Time_t t, SimulatorUid_t u) : time(t), uid(u) { }
public:
  Time_t         time;
  SimulatorUid_t uid;
};

// �¼����й���Ķ���
typedef std::pair<Time_t,SimulatorUid_t> KeyPair_t; //map�ṹ��key
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
  //Scheduler����һ������Ļ��࣬�������˷�������ķ���
public:

  Scheduler();
    //Scheduler�Ĺ��췽��û���κβ���


  virtual ~Scheduler();
public:   

  virtual Scheduler* Copy() const = 0;//���е�scheduler�����������Ը�������,�����¿�����ָ��

protected:

  virtual void prCancel(Event*) = 0; //ȡ��һ�����ڶ����е��¼�
  virtual void prSchedule(Event*) = 0;//����һ�����¼�����ǰ�¼��ķ���ʱ���Լ�handler��Ҫ�ڽ����¼�������ȷ��
  virtual void prSchedule(Event*, Time_t) = 0;   //����һ�����¼�����ǰ�¼��ķ���ʱ���Լ�handler��Ҫ�ڽ����¼�������ȷ��
  virtual void prSchedule(Event*, Time_t, Handler*) = 0; //����һ�����¼�����ǰ�¼��ķ���ʱ����Ҫ�ڽ����¼�������ȷ��
  virtual void prSchedule(Event*, Time_t, Handler&) = 0; //����һ�����¼�����ǰ�¼��ķ���ʱ����Ҫ�ڽ����¼�������ȷ��
  virtual void prScheduleEarly(Event*, Time_t, Handler*) = 0;
  virtual Event* prDeQueue() = 0;	//ȡ���¼�������Ҫ���ӵ��¼�����������ָ��
  virtual Event* prPeekEvent() = 0; //�����¼�������Ҫ���ӵ��¼�����������ָ�룬���������Ӷ������Ƴ�
public:
  virtual bool    IsScheduled(Event*) = 0;//�����ã�����¼��Ƿ�����¼�����
  void    PrintStats(); //��ӡ���浱�е�һЩϸ����Ϣ
  Count_t TotalEventsProcessed() { return totevp;}//���ط��洦������¼�����
  Count_t TotalEventsScheduled() { return totevs;}//���ط��氲�Ž����¼����е��¼�����
  Count_t EventListSize() { return evlSize;}//���ص�ǰ�¼����еĴ�С���������Ŷӵģ�δ������¼�����
  //��̬����
  static Scheduler* Instance();//ȡ��Scheduler�����ָ�룬���û�еĻ��½�һ��MapScheduler�Ķ���
  static void Cancel(Event*);//ȡ��һ�����ڶ����е��¼�
  static void Schedule(Event*);//����һ�����¼�����ǰ�¼��ķ���ʱ���Լ�handler��Ҫ�ڽ����¼�������ȷ��
  static void Schedule(Event*, Time_t);//����һ�����¼�����ǰ�¼��ķ���ʱ���Լ�handler��Ҫ�ڽ����¼�������ȷ��
  static void Schedule(Event*, Time_t, Handler*); //����һ�����¼�����ǰ�¼��ķ���ʱ����Ҫ�ڽ����¼�������ȷ��
  static void Schedule(Event*, Time_t, Handler&); //����һ�����¼�����ǰ�¼��ķ���ʱ����Ҫ�ڽ����¼�������ȷ��
  static void ScheduleEarly(Event*, Time_t, Handler*);
  static Event* DeQueue();//ȡ���¼�������Ҫ���ӵ��¼�����������ָ��
  static Event* PeekEvent(); //�����¼�������Ҫ���ӵ��¼�����������ָ�룬���������Ӷ������Ƴ�
public:

  Count_t         totevs;      //�����Ź����¼�����
  Count_t         totrm;       //�Ӷ������Ƴ������¼�����
  Count_t         totevp;      //���й����¼�����
  Count_t         totevc;      //ȡ�������¼�����
  Count_t         evlSize;     //�¼������е��¼�����
  Count_t         maxSize;     //�������¼��������¼����������ֵ
protected:
  static SimulatorUid_t  uid;       //��һ���¼����õ�ID
  static Scheduler*      instance;  //��ǰ��scheduler��ʵ��
};

#endif


