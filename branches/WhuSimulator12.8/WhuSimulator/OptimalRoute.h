#pragma once

#include "Route.h"

class COptimalRoute : public CRoute
{
public:
	COptimalRoute(CNode *n = NULL);
	COptimalRoute(const COptimalRoute &);
	~COptimalRoute(void);

	virtual NodeId_t GetNextRouteId(NodeId_t dst);
	virtual CRoute* Clone(); 

private:
	virtual void  update_route_table(NodeId_t dst = 0);//更新路由表

	void dijkstra_degree();

	void copy_entrys(vector<CRouteEntry> &from, 
		vector<CRouteEntry> &to, 
		double new_cost);
};
