#include "StdAfx.h"
#include "TopoHandle.h"
#include "TopoEvent.h"
#include "WhuSimulator.h"

CTopoHandle::CTopoHandle(void)
{
}

CTopoHandle::~CTopoHandle(void)
{
}

void CTopoHandle::Handle(CEventBase* e, Time_t t)
{
	CWhuSimulatorApp* pToApp = (CWhuSimulatorApp*)AfxGetApp();

	//节点故障，拓扑改变，更新路由表
	CTopoEvent* topoEvent = (CTopoEvent*)e;
	switch (e->event)
	{
	case CTopoEvent::ADDNODE:
		{
			int ret = pToApp->graph->add_node(topoEvent->GetNodeId());
			if ( ret==1 )
			{
				//插入成功
				pToApp->graph->ClearRouteTable();
				pToApp->graph->SetDefaultRoute();
			}
			else {}//插入失败
			break;
		}
	case CTopoEvent::DELETENODE:
		{
			//删除点，因为拓扑关系，是单向边，有待优化
			break;
		}
	case CTopoEvent::ADDEDGE:
		{
			pToApp->graph->add_edge(topoEvent->GetEdge());

			pToApp->graph->ClearRouteTable();
			pToApp->graph->SetDefaultRoute();
			break;
		}
	case CTopoEvent::DELETEEDGE:
		{
			pToApp->graph->del_edge(topoEvent->GetEdge());

			pToApp->graph->ClearRouteTable();
			pToApp->graph->SetDefaultRoute();
			break;
		}
	case CTopoEvent::EDGESETCOST:
		{
			pToApp->graph->set_edge_cost(topoEvent->GetEdge()->get_sou_id(),
										topoEvent->GetEdge()->get_des_id(),
										topoEvent->GetEdge()->get_cost());
			pToApp->graph->ClearRouteTable();
			pToApp->graph->SetDefaultRoute();
			break;
		}
	}
}