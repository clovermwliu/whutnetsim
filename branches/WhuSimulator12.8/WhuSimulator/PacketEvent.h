#pragma once

#ifndef _PACKETEVENT_H__
#define _PACKETEVENT_H__

#include "EventBase.h"
#include "Packet.h"

//���¼�����������Ԥ�������ͣ�����
class CPacketEvent : public CEventBase
{
public:
	typedef enum { QUEUEPROCESS,SEND,END } PacktEvent_t;
	CPacketEvent(PacktEvent_t p):CEventBase(p){}
	CPacketEvent(PacktEvent_t p,CHandler* h,Time_t t,Packet* _packet);
	~CPacketEvent(void);
	virtual Packet* GetPacket(){return packet;}

	Packet* packet;
};
#endif