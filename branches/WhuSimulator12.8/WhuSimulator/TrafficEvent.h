#pragma once

#ifndef __TRAFFICEVENT_H__
#define __TRAFFICEVENT_H__

#include "EventBase.h"

//�����¼�����ģ������Ӹ���
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