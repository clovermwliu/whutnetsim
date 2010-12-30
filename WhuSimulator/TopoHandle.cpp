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

	//�ڵ���ϣ����˸ı䣬����·�ɱ�
	CTopoEvent* topoEvent = (CTopoEvent*)e;
	switch (e->event)
	{
	case CTopoEvent::ADDNODE:
		{
			int ret = pToApp->graph->add_node(topoEvent->GetNodeId());
			if ( ret==1 )
			{
				//����ɹ�
				pToApp->graph->ClearRouteTable();
				pToApp->graph->SetDefaultRoute();
			}
			else {}//����ʧ��
			break;
		}
	case CTopoEvent::DELETENODE:
		{
			//ɾ���㣬��Ϊ���˹�ϵ���ǵ���ߣ��д��Ż�
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