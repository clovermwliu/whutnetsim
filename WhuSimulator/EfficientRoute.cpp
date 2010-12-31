#include "StdAfx.h"
#include "EfficientRoute.h"
#include <math.h>

CEfficientPath::CEfficientPath(double a, CNode *n):CRoute(n),beta(a)
{
}

CEfficientPath::CEfficientPath(const CEfficientPath &c):CRoute(c),beta(c.beta)
{

}

CEfficientPath::~CEfficientPath(void)
{
}

CRoute* 
CEfficientPath::Clone()
{
	return new CEfficientPath(*this);
}

NodeId_t 
CEfficientPath::GetNextRouteId(NodeId_t dst)
{
	if (_route_table.find(dst) == _route_table.end())//找不到，则计算
	{
		update_route_table();
	}
	if (_route_table.find(dst) != _route_table.end())
	{
		switch (type)
		{
		case FIRST: return select_first(dst);
			break;
		case RANDOM: return select_random(dst);
			break;
		case SPECIFIC: return select_specific(dst);
			break;
		default: return select_first(dst);
			break;
		}
	}
	else
	{
		// default_route [7/19/2010 mpiuser]
		return _default_route;
	}
}

void 
CEfficientPath::copy_entrys(vector<CRouteEntry> &from, 
						   vector<CRouteEntry> &to, 
						   double new_cost)
						   //from里面的内容追加到to中，并且CRouteEntry里的cost改为new_cost
{
	for (int i = 0; i < (int)from.size(); ++i)
	{
		CRouteEntry r(from[i].get_next_hop(), new_cost);
		to.push_back(r);
	}
}

void
CEfficientPath::update_route_table(NodeId_t dst)//更新路由表，并返回到dst的下一跳
{
	dijkstra();
}

void 
CEfficientPath::dijkstra()
{
	//重新计算路由表
	_route_table.clear();

	//vector<NodeId_t> n_tmp;//next_hops
	multimap<double, NodeId_t> active;//cost，目标结点
	multimap<double, NodeId_t>::iterator active_it;		
	double cost = pow((double)pn->get_degree(), beta);
	active.insert(make_pair(cost, pn->get_id()));

	vector<CRouteEntry> r_tmp(1, CRouteEntry(pn->get_id()));
	_route_table.insert(make_pair(pn->get_id(), r_tmp));

	CNode *this_node = NULL;
	double this_cost;

	while(!active.empty())
	{
		active_it = active.begin();

		this_node = pApp->graph->get_node(active_it->second);//源结点
		this_cost = active_it->first;//cost

		active_it = active.erase(active_it);
		//对this_node的各邻居结点
		map<NodeId_t, CEdge*>::iterator it = this_node->get_neighbor_begin();

		for (; it != this_node->get_neighbor_end(); ++it)
		{
			NodeId_t nv_id = it->first;//邻居结点id

			double new_cost = this_cost + this_node->get_neighbor_cost(nv_id);

			map<NodeId_t, vector<CRouteEntry>>::iterator nv_it = _route_table.find(nv_id);

			//不存在或需要更新cost，cost比较大
			if (nv_it == _route_table.end()||nv_it->second.at(0).get_cost() > new_cost)
			{
				vector<CRouteEntry> rs;

				if (this_node->get_id() == pn->get_id())
				{
					rs.push_back(CRouteEntry(nv_id, new_cost));
				}	
				else
				{
					copy_entrys(_route_table[this_node->get_id()], rs, new_cost);
				}
				//_route_table.erase(nv_it);
				// route [7/19/2010 mpiuser]
				if (nv_it != _route_table.end())
					_route_table.erase(nv_it);

				//新加
				_route_table.insert(make_pair(nv_id, rs));
				active.insert(make_pair(new_cost, nv_id));
			}
			else if (nv_it->second.at(0).get_cost() == new_cost) //一样大
			{
				copy_entrys(_route_table[this_node->get_id()], nv_it->second, new_cost);
			}	
		}
	}

}
