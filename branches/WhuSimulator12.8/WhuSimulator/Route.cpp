#include "StdAfx.h"
#include "Route.h"

CRoute::CRoute(CNode *n):pn(n), type(FIRST)
/*
描述：路由类构造函数
参数：[IN] n:路由绑定的节点
返回：无
*/
{
	pApp = (CWhuSimulatorApp*)AfxGetApp();
	DEBUG3(GlobalVariable::out_new<<"New Route"<<endl);
}

CRoute::~CRoute(void)
{
	clear_route_table();
}
void CRoute::clear_route_table() 
{ 
	map<NodeId_t, vector<CRouteEntry> >::iterator route_it = _route_table.begin();
	while(route_it != _route_table.end())
	{
		(route_it->second).clear();
		route_it++;
	}
	_route_table.clear(); 
}
NodeId_t 
CRoute::select_first(NodeId_t dst)
{
	ASSERT(_route_table.find(dst) != _route_table.end());
	return _route_table[dst].at(0).get_next_hop();
}

NodeId_t 
CRoute::select_random(NodeId_t dst)
{
	ASSERT(_route_table.find(dst) != _route_table.end());
	int num = (int)_route_table[dst].size();
	return _route_table[dst].at(CRandom::Random_U(0, num-1)).get_next_hop();
}

NodeId_t 
CRoute::select_specific(NodeId_t dst)
{
	ASSERT(_route_table.find(dst) != _route_table.end());
	return _route_table[dst].at(0).get_next_hop();
}

CRoute::CRoute(const CRoute & c)
				:_default_route(c._default_route),
				pn(c.pn),
				pApp(c.pApp),
				type(c.type)
{

}

CNode* CRoute::GetLocalNode()
{
	return pn;
}