#pragma once

#include "TrafficStrategy.h"

//ָ��ԴĿ�Ľڵ㣬���������¼��Ĵ��������
class CGNPairsTrafficStrategy : public CTrafficStrategy
{
public:
	// handler [8/3/2010 Administrator]
	CGNPairsTrafficStrategy(){}
	~CGNPairsTrafficStrategy(void);
	void Handle(CEventBase*, Time_t);

	virtual HandlerType_t GetHandlerType() {return GNPAIRTRAFFIC;}//���ش����������
};
