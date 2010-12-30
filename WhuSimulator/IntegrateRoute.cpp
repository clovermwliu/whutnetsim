// new route [7/23/2010 Administrator]
#include "StdAfx.h"
#include "IntegrateRoute.h"
// new route [7/23/2010 Administrator]
#include <math.h>
#include "Random.h"


CIntegrateRoute::CIntegrateRoute(double aa, double bb, CNode *n):a(aa), b(bb), CRoute(n)
{

}
CIntegrateRoute::CIntegrateRoute(CNode *n):a(-2.5), b(2.5), CRoute(n)
{
}

//debug new route
CIntegrateRoute::CIntegrateRoute(const CIntegrateRoute &c):a(c.a), b(c.b), CRoute(c)
{

}
CIntegrateRoute::~CIntegrateRoute(void)
{
}

CRoute* 
CIntegrateRoute::Clone()
{
	return new CIntegrateRoute(*this);
}


NodeId_t 
CIntegrateRoute::GetNextRouteId(NodeId_t dst)
{
	//debug new route
	if(dst == pn->get_id() || pn->find_neighbor(dst))
		return dst;

	if (_route_table.find(dst) == _route_table.end())//找不到，则计算
	{
		update_route_table(dst);
	}

	return select_neighbor(dst);
}

//debug new route
NodeId_t 
CIntegrateRoute::select_neighbor(NodeId_t dst)//选择下一跳
{
	double ran = CRandom::Random_U();
	map<NodeId_t, vector<CRouteEntry>>::iterator it = _route_table.find(dst);
	double sum = 0;
	for (int i = 0; i < (int)it->second.size(); ++i)
	{
		sum += it->second.at(i).get_cost();
		if (ran <= sum)
		{
			return it->second.at(i).get_next_hop();
		}
	}
	return _default_route;
}


//debug new route
void  
CIntegrateRoute::update_route_table(NodeId_t dst)//更新路由表
{
	double sum = 0;
	double tmp1, tmp2;
	//遍历该结点的邻居结点,sum
	map<NodeId_t, CEdge*>::iterator it = pn->get_neighbor_begin();
	for (; it != pn->get_neighbor_end(); ++it)
	{
		//邻居结点it->first
		tmp1 = pow(pApp->graph->get_node(it->first)->GetQueueAvgLength()+1, a);
		double ta = pApp->graph->get_hops(pn->get_id(), dst);
		double tb = pApp->graph->get_hops(it->first, dst);
		tmp2 = exp(b*(pApp->graph->get_hops(pn->get_id(), dst) - pApp->graph->get_hops(it->first, dst) - 1));
		sum += tmp1*tmp2;
	}
	//计算选择各邻居结点的概率，并保存在路由表中
	if (_route_table.find(dst) != _route_table.end())
	{
		_route_table.erase(_route_table.find(dst));
	}
	//计算
	vector<CRouteEntry> r;
	it = pn->get_neighbor_begin();
	for (; it != pn->get_neighbor_end(); ++it)
	{
		////邻居结点it->first	
		tmp1 = pow(pApp->graph->get_node(it->first)->GetQueueAvgLength()+1, a);
		tmp2 = exp(b*(pApp->graph->get_hops(pn->get_id(), dst) - pApp->graph->get_hops(it->first, dst) - 1));
		r.push_back(CRouteEntry(it->first, tmp1*tmp2/sum));
	}
	_route_table.insert(make_pair(dst, r));
}


