// new route [7/23/2010 Administrator]
#pragma once
#include "Route.h"

class CIntegrateRoute : public CRoute
{
public:
	virtual RouteType_t get_route_type() { return INTEGRATE;}

	CIntegrateRoute(double aa, double bb, CNode *n = NULL);
	CIntegrateRoute(CNode *n = NULL);
	CIntegrateRoute(const CIntegrateRoute &);
	~CIntegrateRoute(void);

	virtual NodeId_t GetNextRouteId(NodeId_t dst);

	virtual CRoute* Clone();

	void set_a_b(double aa, double bb) { a = aa; b = bb; }
	double get_a() { return a; }
	double get_b() { return b; }

private:
	virtual void  update_route_table(NodeId_t dst = 0);//更新路由表，只更新到目的结点dst的路由项
	//map<NodeId_t, vector<CRouteEntry> > _route_table;
	//该路由算法路由表中存储的是 目的结点id，选择邻居_next_hop为下一跳的概率是_cost

	double a;//系数
	double b;//系数

	NodeId_t select_neighbor(NodeId_t dst);//选择下一跳
};
