#pragma once

#include "TrafficStrategy.h"

//指定源目的节点，产生流量事件的处理策略类
class CGNPairsTrafficStrategy : public CTrafficStrategy
{
public:
	// handler [8/3/2010 Administrator]
	CGNPairsTrafficStrategy(){}
	~CGNPairsTrafficStrategy(void);
	void Handle(CEventBase*, Time_t);

	virtual HandlerType_t GetHandlerType() {return GNPAIRTRAFFIC;}//返回处理类的类型
};
