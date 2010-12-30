#pragma once
#ifndef __GNRANTRAFFICEVENT_H__
#define __GNRANTRAFFICEVENT_H__

#include "TrafficEvent.h"


//流量事件的派生类，每时刻产生R个包，包的源目的随机产生
class CGNRanTrafficEvent : public CTrafficEvent
{
	typedef enum { GNRanTrafficEvent = 20 } GNRanTrafficEvent_t;
public:
	// handler [8/3/2010 Administrator]
	CGNRanTrafficEvent(size_t R = 1);
	CGNRanTrafficEvent(Event_t p,CHandler* h,Time_t t,size_t R = 1):CTrafficEvent(p,h,t),packetNumR(R){}
	~CGNRanTrafficEvent(void);

	// handler [8/3/2010 Administrator]
	virtual Packet_t GetPacketNumROnce(){return packetNumR;}
	// handler [8/3/2010 Administrator]
	size_t	packetNumR;//一个流量事件发送的包的数量
};
#endif