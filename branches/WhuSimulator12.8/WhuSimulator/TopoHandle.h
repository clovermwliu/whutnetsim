#pragma once

#include "Handler.h"

//�����¼������࣬���������ӣ�ɾ�����ߵ���ӣ�ɾ�����ߵĻ��ѵļӡ���
class CTopoHandle : public CHandler
{
public:
	CTopoHandle(void);
	~CTopoHandle(void);
	void Handle(CEventBase*, Time_t);

	virtual HandlerType_t GetHandlerType() {return TOPO;}//���ش����������
};
