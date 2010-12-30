#pragma once

#ifndef __TRAFFICEVENT_H__
#define __TRAFFICEVENT_H__

#include "EventBase.h"

//流量事件，向模型中添加负载
class CTrafficEvent	: public CEventBase
{
public:
	// monitor [7/20/2010 Administrator]
	typedef enum { TRAFFICEVENT  = 100} TrafficEvent_t;
public:
	CTrafficEvent();
	CTrafficEvent(Event_t p,CHandler* h,Time_t t):CEventBase(p,h,t){}
	virtual ~CTrafficEvent(void);
};
#endif