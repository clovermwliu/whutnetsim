#pragma once

#include "Handler.h"

//拓扑事件处理类，包括点的添加，删除，边的添加，删除，边的花费的加、减
class CTopoHandle : public CHandler
{
public:
	CTopoHandle(void);
	~CTopoHandle(void);
	void Handle(CEventBase*, Time_t);

	virtual HandlerType_t GetHandlerType() {return TOPO;}//返回处理类的类型
};
