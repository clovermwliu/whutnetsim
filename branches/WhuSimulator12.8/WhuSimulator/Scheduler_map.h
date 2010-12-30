#pragma once


#ifndef __SCHEDULER_MAP_H__
#define __SCHEDULER_MAP_H__


#include <map>
#include "G_common_defs.h"
#include "EventBase.h"
#include "ErrorHandler.h"
#include "Simulator.h"
#include <vector>
#include "EndEvent.h"
#include <math.h>

//为了保证可重复性的结果，利用事件的UID进行了排序
class KeyPair {
public:
	KeyPair(Time_t t, SimulatorUid_t u) : time(t), uid(u) { }
public:
	Time_t         time; //事件的执行时间
	SimulatorUid_t uid;  //事件的ID号
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


#ifdef EventMap
// 定义map类型保存事件
typedef std::map<KeyPair, CEventBase*, key_less> EventMap_t;

class CScheduler_map : public CErrorHandler
{
public:
	CScheduler_map(void);
	~CScheduler_map(void); 
	static void InsertEvent(CEventBase*);//插入事件
	static size_t GetEventlistSize(){return eventList.size();}
	static size_t EventLength();//插入事件
	static CEventBase* GetTopEvent();//以从队列中删除
	static bool EventListEmptyT();  //判断队列是否为空
	static void  ResetUid(){ uid = 0;}       //下一个事件可用的ID
private:
	static void   DebugOutputEventList(string debugType);
	static void   DebugString(CEventBase* p);

public:
	static void	  Clearlist();
	
private:
	static EventMap_t      eventList;   //事件队列
	static SimulatorUid_t  uid;       //下一个事件可用的ID

public:

	Count_t         totevs;      //被安排过的事件总数
	Count_t         totrm;       //从队列中移除过的事件总数
	Count_t         totevp;      //运行过的事件总数
	Count_t         totevc;      //取消过的事件总数
	Count_t         evlSize;     //事件队列中的事件总数
	Count_t         maxSize;     //仿真中事件队列里事件数量的最大值
};
#endif//end of #ifdef EventMap

#ifdef EventMapVector 
typedef std::map<KeyPair, CEventBase*, key_less> EventMap_t;
typedef std::vector<CEventBase* > EventVec_t;
//  [7/29/2010 Administrator]
typedef std::vector< vector<CEventBase* >* > EventLaterVec_t; 


#define CHECK_TIME
class CScheduler_map : public CErrorHandler
{
public:
	CScheduler_map(void) {}
	~CScheduler_map(void) {}

	static void InsertEvent(CEventBase* pEv);//插入事件
	static void InsertEndEvent(CEndEvent* pEndEv);//插入end事件到map中
	static size_t GetEventlistSize();
	static size_t GetMapSize();
	static CEventBase* GetTopEvent();//以从队列中删除
	static bool EventListEmptyT(); //判断队列是否为空
	static void  ResetUid(){ uid = 0;}       //下一个事件可用的ID

	static void	  Clearlist();
	// B [7/29/2010 Administrator]
	static  void SetVecLaterSize(double vecLaterTime, int p = 3);

private:
	static void   DebugOutputEventList(string debugType);
	static void   DebugString(CEventBase* p);
	static size_t GeteventVecLaterSize();

private:
	static EventMap_t      eventMap;   //事件map
	static EventLaterVec_t      eventVecLater; //存放较晚执行事件
	static EventVec_t      eventVecEarlier;//存放较早执行事件
	static SimulatorUid_t  uid;       //下一个事件可用的ID

	static size_t ptop;

	static double gran;
	static Time_t maxMapTime;////eventMap中除EndEvent最晚执行事件的时间
	static Time_t minEVecTime;//eventVecEarlier中最早执行事件的时间

private:
	// B修改 [7/29/2010 Administrator]根据事件时间入vectorlater
	static void UpdateVecLaterToMap();//更新，VecLater中的所有事件转存到map中
	static void MergeVecEarlierToMap(); //归并，VecEarlier中的所有事件转存到map中

	// B修改 [7/29/2010 Administrator]根据事件时间入vectorlater
	static void EnVecLater(CEventBase* pEv);//入VecLater
	static void EnVecEarlier(CEventBase* pEv); //入VecEarlier

	static Time_t GetMaxMapTime(); //获取除EndEvent外map中的最晚事件处理时间，如果只有EndEvent一个事件，则返回EndEvent的时间

	static double GetCeil(double num, double g = 1);
};

#endif//end of #ifdef EventMapVector
#endif//end of #ifndef __SCHEDULER_MAP_H__