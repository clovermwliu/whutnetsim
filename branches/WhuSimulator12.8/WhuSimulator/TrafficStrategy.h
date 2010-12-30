#pragma once

#ifndef __TRAFFICSTRATEGY_H__
#define __TRAFFICSTRATEGY_H__

#include "Handler.h"
#include "G_common_defs.h"

//流量策略的处理类
class CTrafficStrategy : public CHandler
{
public:
	Error_str GetLastErrorEx(){return "hello";}
	// error [7/20/2010 mpiuser]
	CTrafficStrategy(){}
	~CTrafficStrategy(void);
	virtual void Handle(CEventBase*, Time_t)  = 0;

	// handler [8/3/2010 Administrator]
//Packet_t GetPacketNumROnce(){return packetNumR;}
//void  SetEnd(bool t){endT = t;}
//protected:
//	Packet_t	packetNumR;//一次发送数据包的数量
//	bool		endT;	   // 结束事件
};
#endif