#include "StdAfx.h"
#include "Node.h"
#include "PacketEvent.h"
#include "Scheduler_map.h"
#include "ShortestPath.h"
#include <sstream>
#include <assert.h>
#include <time.h>

CNode::CNode(NodeId_t id)
{
	_id = id;
	queue = new CQueue();
	//route = new CShortestPath(this);

	DEBUG3(GlobalVariable::out_new<<"New Node"<<endl);
}

CNode::~CNode(void) 
{
	clear_neighbor();
	delete queue;
	//delete route;
}

CNode::CNode(const CNode &n):
_id(n._id)
{
	map<NodeId_t, CEdge*>::const_iterator it = n._neighbor.begin();
	while(it != n._neighbor.end())
	{
		_neighbor.insert(make_pair(it->first, new CEdge(*(it->second))));
		it++;
	}
}

CNode& CNode::operator=(const CNode &n)
{
	_id = n._id;

	map<NodeId_t, CEdge*>::const_iterator it = n._neighbor.begin();
	while(it != n._neighbor.end())
	{
		_neighbor.insert(make_pair(it->first, new CEdge(*(it->second))));
		it++;
	}

	return *this;
}

/////////////////
int CNode::del_neighbor(NodeId_t dst)//ɾ����ǰ��㵽dst�ıߣ����ɾ��ʧ�ܻ����ʧ�ܷ���-1
{
	map<NodeId_t, CEdge*>::iterator it = _neighbor.find(dst);
	if (it != _neighbor.end())
	{
		delete it->second;
		_neighbor.erase(it);
		return 1;
	}
	return -1;
}

void
CNode::clear_neighbor()///////////////
{
	map<NodeId_t, CEdge*>::iterator neighbor_it = _neighbor.begin();
	while(neighbor_it != _neighbor.end())
	{
		delete neighbor_it->second;
		neighbor_it++;
	}
	_neighbor.clear();
}

void 
CNode::ClearRouteTable()
{
	route->clear_route_table();
}

void 
CNode::ClearQueue()
{
	queue->Clear();
}

void 
CNode::ClearRoute()
{
	delete route;
}

//debug new route
void
CNode::set_current_length_w(double w)
{
	queue->set_current_length_w(w);	
}

double 
CNode::get_neighbor_cost(NodeId_t dst)///////////////
{
	map<NodeId_t, CEdge*>::iterator it = _neighbor.find(dst);
	if (it != _neighbor.end())
	{
		return it->second->get_cost();
	}
	return -1;
}
////////////////
int 
CNode::set_neighbor_cost(NodeId_t dst, double w)
{
	map<NodeId_t, CEdge*>::iterator it = _neighbor.find(dst);
	if (it != _neighbor.end())
	{
		it->second->set_cost(w);
		return 1;
	}
	return -1;

}

double 
CNode::get_neighbor_delay(NodeId_t dst)///////////////
{
	map<NodeId_t, CEdge*>::iterator it = _neighbor.find(dst);
	if (it != _neighbor.end())
	{
		return it->second->get_delay();
	}
	return -1;
}
////////////////
int 
CNode::set_neighbor_delay(NodeId_t dst, double w)
{
	map<NodeId_t, CEdge*>::iterator it = _neighbor.find(dst);
	if (it != _neighbor.end())
	{
		it->second->set_delay(w);
		return 1;
	}
	return -1;

}

void CNode::Handle(CEventBase* e, Time_t t)
/*
���������¼��Ĵ�����
������[IN] e   ��Ҫ�����ʱ��
	  [IN] t   �������¼���ʱ��
���أ���
*/
{
	CWhuSimulatorApp* pTrApp = (CWhuSimulatorApp*)AfxGetApp();
	CPacketEvent* pacEvent = (CPacketEvent*)e;
	Packet* p = pacEvent->packet;

	clock_t start,end;
	switch (e->event)
	{
	case CPacketEvent::QUEUEPROCESS:  //����Ԥ�����¼�
		{
 #ifdef TIME_ALL
			start = clock();
#endif // end of #ifdef TIME_ALL
			PacketEnQueue(p);//�����
 #ifdef TIME_ALL
			end = clock();
#endif // end of #ifdef TIME_ALL

			DEBUG4(GlobalVariable::enqueTime+=(double)(end-start)/CLOCKS_PER_SEC);

			CPacketEvent* pEvent;//�½������¼�
		
			if (_id == p->_des)
			{
				pEvent = new CPacketEvent(CPacketEvent::END,this,GetNewNextPacketTime(),p);
				//end�¼�
			}
			else
			{
				pEvent = new CPacketEvent(CPacketEvent::SEND,this,GetNewNextPacketTime(),p);
			}
#ifdef TIME_ALL
			start = clock();
#endif // end of #ifdef TIME_ALL
			CScheduler_map::InsertEvent(pEvent);
#ifdef TIME_ALL
			end = clock();
#endif // end of #ifdef TIME_ALL
			DEBUG4(GlobalVariable::eventTime+=(double)(end-start)/CLOCKS_PER_SEC);

			DEBUG4(GlobalVariable::out_time<<"����ʱ��"<<e->Time()<<"����ʱ�䣺"<<pEvent->Time() <<endl);
			

			delete e;//ɾ��Ԥ�����¼�
			break;
		}
	case CPacketEvent::SEND:		//���ķ����¼�
		{
			assert(queue->GetFront()==p);
#ifdef TIME_ALL
			start = clock();
#endif // end of #ifdef TIME_ALL
			NodeId_t nextRoute = route->GetNextRouteId(p->_des);//·����һ���ڵ�
#ifdef TIME_ALL
			end = clock();
#endif // end of #ifdef TIME_ALL
			DEBUG4(GlobalVariable::routeTime+=(double)(end-start)/CLOCKS_PER_SEC);

			p->_now_node = nextRoute;
			CPacketEvent* pEvent = new CPacketEvent(CPacketEvent::QUEUEPROCESS,
													pTrApp->graph->get_node(nextRoute),
													e->Time()+get_neighbor_delay(nextRoute),
													p);//queueprocess�¼�
			////ofstream traceroute;
			////traceroute.open("D://traceroute.txt",ios::app);
			//traceroute<<"Packet"<<p->_id<<"  "
			//	      <<"src"<<p->_src<<"  "
			//		  <<"des"<<p->_des<<"  "
			//		  <<"next_hop"<<nextRoute<<" "
			//		  <<endl;
			////traceroute<<nextRoute<<endl;
			////traceroute.close();
#ifdef TIME_ALL
			start = clock();
#endif // end of #ifdef TIME_ALL
			CScheduler_map::InsertEvent(pEvent);
#ifdef TIME_ALL
			end = clock();
#endif // end of #ifdef TIME_ALL
			
			DEBUG4(GlobalVariable::eventTime+=(double)(end-start)/CLOCKS_PER_SEC);
			DEBUG4(GlobalVariable::out_time<<"����ʱ��"<<e->Time()<<"����ʱ�䣺"<<pEvent->Time() <<endl);
#ifdef TIME_ALL
			start = clock();
#endif // end of #ifdef TIME_ALL
			PacketDeQueue();//������
#ifdef TIME_ALL
			end = clock();
#endif // end of #ifdef TIME_ALL
			DEBUG4(GlobalVariable::enqueTime+=(double)(end-start)/CLOCKS_PER_SEC);
			

			delete e;//ɾ����ǰ�¼�
			break;
		}
	case CPacketEvent::END:    //�����¼�
		{
			assert(queue->GetFront()==p);
#ifdef TIME_ALL
			start = clock();
#endif // end of #ifdef TIME_ALL
			PacketDeQueue();//������
#ifdef TIME_ALL
			end = clock();
#endif // end of #ifdef TIME_ALL
			DEBUG4(GlobalVariable::enqueTime+=(double)(end-start)/CLOCKS_PER_SEC);

			delete p;//ɾ����

			delete e;//ɾ���¼�
			break;
		}
	}
}
Handler_status_str CNode::GetHandleStatus()
{
	std::stringstream stream_value(stringstream::out);
	
	stream_value <<_id;
	return "Node "+stream_value.str();
}
void CNode::BoundRoute(CRoute* _route)
/*
������Ϊ�ڵ��·��
������[IN] _route  ��Ҫ�󶨵�·��
���أ���
*/
{ 
	route = _route;
	route->set_node(this);
}
size_t 
CNode::GetQueueLength()
{
	return queue->GetQueueLength();
}
Packet* CNode::PacketDeQueue() //��һ�����ݰ�����
{
	return queue->Deque();
}
bool CNode::PacketEnQueue(Packet* p)//��һ�����ݰ����
{
	queue->Enque(p);
	return true;
}
double CNode::GetNewNextPacketTime()
{
	return queue->GetNewNextPacketTime();
}
// default route [7/19/2010 mpiuser]
void 
CNode::set_default_route()
{
	if (!_neighbor.empty())
	{
		route->set_default_route(_neighbor.begin()->first);
	}
	else
	{
		route->set_default_route(_id);
	}

}

void
CNode::set_throughPut(double t)
{
	queue->SetThroughput(t);	
}
// new route [7/23/2010 Administrator]
double 
CNode::UpdateQueueAvgLength()//���¶��г���,������
{
	return queue->UpdateAvgLength();
}

double 
CNode::GetQueueAvgLength()//ȡ
{
	return queue->GetAvgLength();
}