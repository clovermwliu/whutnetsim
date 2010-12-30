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
int CNode::del_neighbor(NodeId_t dst)//删除当前结点到dst的边，如果删除失败或查找失败返回-1
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
描述：包事件的处理方法
参数：[IN] e   ：要处理的时间
	  [IN] t   ：处理事件的时间
返回：无
*/
{
	CWhuSimulatorApp* pTrApp = (CWhuSimulatorApp*)AfxGetApp();
	CPacketEvent* pacEvent = (CPacketEvent*)e;
	Packet* p = pacEvent->packet;

	clock_t start,end;
	switch (e->event)
	{
	case CPacketEvent::QUEUEPROCESS:  //包的预处理事件
		{
 #ifdef TIME_ALL
			start = clock();
#endif // end of #ifdef TIME_ALL
			PacketEnQueue(p);//包入队
 #ifdef TIME_ALL
			end = clock();
#endif // end of #ifdef TIME_ALL

			DEBUG4(GlobalVariable::enqueTime+=(double)(end-start)/CLOCKS_PER_SEC);

			CPacketEvent* pEvent;//新建发包事件
		
			if (_id == p->_des)
			{
				pEvent = new CPacketEvent(CPacketEvent::END,this,GetNewNextPacketTime(),p);
				//end事件
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

			DEBUG4(GlobalVariable::out_time<<"构造时间"<<e->Time()<<"触发时间："<<pEvent->Time() <<endl);
			

			delete e;//删除预处理事件
			break;
		}
	case CPacketEvent::SEND:		//包的发送事件
		{
			assert(queue->GetFront()==p);
#ifdef TIME_ALL
			start = clock();
#endif // end of #ifdef TIME_ALL
			NodeId_t nextRoute = route->GetNextRouteId(p->_des);//路由下一条节点
#ifdef TIME_ALL
			end = clock();
#endif // end of #ifdef TIME_ALL
			DEBUG4(GlobalVariable::routeTime+=(double)(end-start)/CLOCKS_PER_SEC);

			p->_now_node = nextRoute;
			CPacketEvent* pEvent = new CPacketEvent(CPacketEvent::QUEUEPROCESS,
													pTrApp->graph->get_node(nextRoute),
													e->Time()+get_neighbor_delay(nextRoute),
													p);//queueprocess事件
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
			DEBUG4(GlobalVariable::out_time<<"构造时间"<<e->Time()<<"触发时间："<<pEvent->Time() <<endl);
#ifdef TIME_ALL
			start = clock();
#endif // end of #ifdef TIME_ALL
			PacketDeQueue();//包出队
#ifdef TIME_ALL
			end = clock();
#endif // end of #ifdef TIME_ALL
			DEBUG4(GlobalVariable::enqueTime+=(double)(end-start)/CLOCKS_PER_SEC);
			

			delete e;//删除当前事件
			break;
		}
	case CPacketEvent::END:    //结束事件
		{
			assert(queue->GetFront()==p);
#ifdef TIME_ALL
			start = clock();
#endif // end of #ifdef TIME_ALL
			PacketDeQueue();//包出队
#ifdef TIME_ALL
			end = clock();
#endif // end of #ifdef TIME_ALL
			DEBUG4(GlobalVariable::enqueTime+=(double)(end-start)/CLOCKS_PER_SEC);

			delete p;//删除包

			delete e;//删除事件
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
描述：为节点绑定路由
参数：[IN] _route  ：要绑定的路由
返回：无
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
Packet* CNode::PacketDeQueue() //让一个数据包出队
{
	return queue->Deque();
}
bool CNode::PacketEnQueue(Packet* p)//让一个数据包入队
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
CNode::UpdateQueueAvgLength()//更新队列长度,并返回
{
	return queue->UpdateAvgLength();
}

double 
CNode::GetQueueAvgLength()//取
{
	return queue->GetAvgLength();
}