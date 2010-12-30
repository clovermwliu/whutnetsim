// new route [7/23/2010 Administrator]
#pragma once
#include "EventBase.h"

class CUpdateAvgQueueLengthEvent :
	public CEventBase
{
public:
	typedef enum { UpdateAvgQueueLengthEvent = 30 } UpdateAvgQueueLengthEvent_t;

	CUpdateAvgQueueLengthEvent() {}

	CUpdateAvgQueueLengthEvent(Time_t t):t_interval(t) {}

	//CUpdateAvgQueueLengthEvent(UpdateAvgQueueLengthEvent_t type, Time_t t):t_interval(t), CEventBase(type) {}

	CUpdateAvgQueueLengthEvent(Event_t p,CHandler* h,Time_t t, Time_t tInterval): t_interval(tInterval), CEventBase(p,h,t){}
	
	~CUpdateAvgQueueLengthEvent(void);

	Time_t get_t() { return t_interval; }

private:
	Time_t t_interval;
		
};
