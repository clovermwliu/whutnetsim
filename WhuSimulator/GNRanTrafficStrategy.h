#pragma once

#include "TrafficStrategy.h"

//���ָ��ԴĿ�ģ�ÿ�Ĳ���R�������¼��Ĵ��������
class CGNRanTrafficStrategy : public CTrafficStrategy
{
public:
	// handler [8/3/2010 Administrator]
	CGNRanTrafficStrategy(){}
	~CGNRanTrafficStrategy(void);

	void Handle(CEventBase*, Time_t);
	Handler_status_str GetHandleStatus();

	virtual HandlerType_t GetHandlerType() {return GNRANTRAFFIC;}//���ش����������
};
