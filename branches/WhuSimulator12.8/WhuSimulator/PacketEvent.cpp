#include "StdAfx.h"
#include "PacketEvent.h"

CPacketEvent::~CPacketEvent(void)
{
}
// control [8/3/2010 Administrator]
CPacketEvent::CPacketEvent(PacktEvent_t p,CHandler* h,Time_t t,Packet* _packet)
:CEventBase(p,h,t),packet(_packet)
{
}