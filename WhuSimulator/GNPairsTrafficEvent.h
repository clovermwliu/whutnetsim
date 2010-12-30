#pragma once
#ifndef __GNPAIRSTRAFFICEVENT_H__
#define __GNPAIRSTRAFFICEVENT_H__

#include "TrafficEvent.h"

//流量事件的派生类，在指定的节点对之间产生流量
class CGNPairsTrafficEvent : public CTrafficEvent
{
	typedef enum { GNPairsTrafficEvent } GNPairsTrafficEvent_t;
public:
	CGNPairsTrafficEvent(void);
	~CGNPairsTrafficEvent(void);
};
#endif