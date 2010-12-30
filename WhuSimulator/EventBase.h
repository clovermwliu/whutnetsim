#pragma once

#ifndef __EVENTBASE_H__
#define __EVENTBASE_H__


#include "G_common_defs.h"
#include "Handler.h"
#include "ErrorHandler.h"
#include "GlobalVariable.h"


//还没有处理跟踪包的部分
class CHandler;
class Packet;
#define NO_EVENT 0xffffffff

//事件的基类
class CEventBase : public CErrorHandler
{
public:
	Error_str GetLastErrorEx(){return "hello";}
	CEventBase() : event(NO_EVENT), uid(++uid_all), time(0), handler(0)
	{GlobalVariable::eventsAllocated++;}//事件基类构造函数

	CEventBase(Event_t ev) : event(ev), uid(++uid_all), time(0), handler(0)
	{GlobalVariable::eventsAllocated++;}//事件基类构造函数

	CEventBase(Event_t ev, CHandler* h) : event(ev), uid(++uid_all), time(0), handler(h)
	{GlobalVariable::eventsAllocated++;}//事件基类构造函数

	CEventBase(CHandler* h) : event(0), uid(++uid_all), time(0), handler(h)
	{GlobalVariable::eventsAllocated++;}//事件基类构造函数

	CEventBase(Event_t p,CHandler* h,Time_t t);
	
	virtual ~CEventBase(void);
	virtual Packet* GetPacket(){return NULL;}

	inline Time_t         Time() const { return time;} //返回事件执行的时间
	inline void           Time(Time_t t){  time = t; } //修改事件执行的时间


	// handler [8/3/2010 Administrator]
	virtual Packet_t GetPacketNumROnce(){return 0;}
private:
	Time_t         time;	 //事件执行的时间
public:
	CHandler*       handler; //事件处理的指针	
	Event_t        event;//事件类型
	SimulatorUid_t uid;

	static SimulatorUid_t uid_all;	 //事件的ID
	
};
#endif