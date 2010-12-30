#include "StdAfx.h"
#include "EventBase.h"
#include "PacketEvent.h"

SimulatorUid_t CEventBase::uid_all = 0;

CEventBase::~CEventBase(void)
{
	GlobalVariable::eventsAllocated--;

	if (event == CPacketEvent::QUEUEPROCESS)
	{
		GlobalVariable::processNum--;
	}
}
CEventBase::CEventBase(Event_t p,CHandler* h,Time_t t) : event(p), uid(++uid_all),time(t),handler(h)
{
	GlobalVariable::eventsAllocated++;
	GlobalVariable::allEvents++;
	//  [8/2/2010 Administrator]
	if (event == CPacketEvent::QUEUEPROCESS)
	{
		GlobalVariable::processNum++;
	}
}//事件基类构造函数
