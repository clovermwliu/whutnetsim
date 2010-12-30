#pragma once
#include "handler.h"

class CEndEventStrategy :
	public CHandler
{
public:
	CEndEventStrategy(void);
	~CEndEventStrategy(void);
	void Handle(CEventBase*, Time_t);
	Handler_status_str GetHandleStatus(){return "EndEvent";}

	virtual HandlerType_t GetHandlerType() {return ENDEVENT;}//返回处理类的类型
};
