#pragma once

#include "Route.h"

class CEfficientPath : public CRoute
{
public:	
	CEfficientPath(double a, CNode *n = NULL );
	CEfficientPath(const CEfficientPath &);
	~CEfficientPath(void);

	virtual NodeId_t GetNextRouteId(NodeId_t dst);

	virtual CRoute* Clone(); 


private:

	double beta;

	virtual void  update_route_table(NodeId_t dst = 0);//更新路由表

	void dijkstra();

	void copy_entrys(vector<CRouteEntry> &from, 
		vector<CRouteEntry> &to, 
		double new_cost);
};