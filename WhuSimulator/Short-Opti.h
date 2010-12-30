#pragma once

#include "Route.h"
#include "ShortestPath.h"
#include "OptimalRoute.h"

class CShortOpti : public CRoute
{
public:
	CShortOpti(double,CNode *n = NULL);
	CShortOpti(CNode *n = NULL);
	CShortOpti(const CShortOpti &);
	~CShortOpti(void);

	virtual NodeId_t GetNextRouteId(NodeId_t dst);

	virtual CRoute* Clone(); 


private:

	double rate;//选择最短路径的比例，相应地选择度连乘积的比例是1-rate

	CShortestPath shortroute;

	COptimalRoute optiroute;
	
	virtual void  update_route_table(NodeId_t dst = 0);//更新路由表

	//void dijkstra();//最短路径的搜索算法

	//void dijkstra_degree();//度的连乘积的搜索算法

	//void inte_dijkstra();

	//void copy_entrys(vector<CRouteEntry> &from, 
		//vector<CRouteEntry> &to, 
		//double new_cost);
};