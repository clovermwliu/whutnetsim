#pragma once

#include "EventBase.h"
#include "G_common_defs.h"
class CEventBase;

//�¼�������Ļ���
class CHandler
{
public:
	CHandler(void);
	~CHandler(void);
	virtual void Handle(CEventBase*, Time_t) = 0;//�¼�������
	virtual Handler_status_str GetHandleStatus(){return "";}
	// handler [8/3/2010 Administrator]
	//virtual Packet_t GetPacketNumROnce(){return 0;}

	// handler [8/3/2010 Administrator]
	typedef enum{ ENDEVENT, GNPAIRTRAFFIC, GNRANTRAFFIC, TOPO, OTHERS } HandlerType_t;
	virtual HandlerType_t GetHandlerType() {return OTHERS;}//���ش����������
};
