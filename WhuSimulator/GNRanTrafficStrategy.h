#pragma once

#include "TrafficStrategy.h"

//随机指定源目的，每拍产生R个流量事件的处理策略类
class CGNRanTrafficStrategy : public CTrafficStrategy
{
public:
	// handler [8/3/2010 Administrator]
	CGNRanTrafficStrategy(){}
	~CGNRanTrafficStrategy(void);

	void Handle(CEventBase*, Time_t);
	Handler_status_str GetHandleStatus();

	virtual HandlerType_t GetHandlerType() {return GNRANTRAFFIC;}//返回处理类的类型
};
