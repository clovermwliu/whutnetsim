#pragma once


#include "EventBase.h"
#include "G_common_defs.h"
class CEdge;
class CTopoEvent : public CEventBase
{
public:
	typedef enum {ADDNODE = 200,
				  DELETENODE,
				  ADDEDGE,
				  DELETEEDGE,
				  EDGESETCOST} TopoEvent_t;
	~CTopoEvent(void);

	CTopoEvent(TopoEvent_t type);
	CTopoEvent(TopoEvent_t type,NodeId_t _eventNodeId);
	CTopoEvent(TopoEvent_t type,CEdge* _edge);

	void     SetNodeId(NodeId_t _eventNodeId){	eventNodeId = _eventNodeId;}
	void	 SetEdge(CEdge* _edge){ edge = _edge;}

	NodeId_t GetNodeId(){	return eventNodeId;	}
	CEdge*	 GetEdge(){	return edge;}
private:
	NodeId_t eventNodeId;
	CEdge* edge;
};
