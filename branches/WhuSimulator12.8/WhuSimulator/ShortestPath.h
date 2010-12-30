#pragma once

#include "Route.h"

class CShortestPath : public CRoute
{
public:	
	CShortestPath(CNode *n = NULL);
	CShortestPath(const CShortestPath &);
	~CShortestPath(void);
	
	virtual NodeId_t GetNextRouteId(NodeId_t dst);

	virtual CRoute* Clone(); 


private:
	virtual void  update_route_table(NodeId_t dst = 0);//更新路由表

	void dijkstra();

	void copy_entrys(vector<CRouteEntry> &from, 
		vector<CRouteEntry> &to, 
		double new_cost);
};
