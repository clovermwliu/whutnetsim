#include "StdAfx.h"
#include "TopoEvent.h"


CTopoEvent::~CTopoEvent(void)
{
}
CTopoEvent::CTopoEvent(TopoEvent_t type):CEventBase(type)
{

}
CTopoEvent::CTopoEvent(TopoEvent_t type,NodeId_t _eventNodeId)
:CEventBase(type),eventNodeId(_eventNodeId)
{

} 
CTopoEvent::CTopoEvent(TopoEvent_t type,CEdge* _edge)
:CEventBase(type),edge(_edge)
{ 

}