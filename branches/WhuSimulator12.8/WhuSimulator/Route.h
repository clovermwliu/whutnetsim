#pragma once

#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "ErrorHandler.h"
#include "G_common_defs.h"
#include "Graph.h"
#include "Node.h"
#include "WhuSimulator.h"
#include "Random.h"

#include <map>
#include <vector>

class CNode;
class CWhuSimulatorApp;

class CRouteEntry
{
public:
	CRouteEntry(NodeId_t next_hop, 
		double cost=0):_next_hop(next_hop),_cost(cost) {}

	CRouteEntry(void) {}

	~CRouteEntry(void) {}

	NodeId_t get_next_hop() { return _next_hop; }
	double get_cost() { return _cost; }

private:
	NodeId_t _next_hop;//下一跳id
	double 	_cost;//当前结点到目的结点经_next_hop时代价
};

class CRoute : public CErrorHandler
{
public:	
	CRoute(CNode *n = NULL);
	CRoute(const CRoute &);
	~CRoute(void);

	Error_str GetLastErrorEx(){return "hello";}

	typedef enum{ FIRST,RANDOM,SPECIFIC } RouteSelect_t;
	//有多条路径备选时，选择方法。选第一条，随机，指定方法选

	// new [8/2/2010 Administrator]
	typedef enum{ SHORTESTPATH, INTEGRATE, OPTIMAL, DEGREEQUEUE, OTHERS } RouteType_t;
	//路由方法类型

	virtual NodeId_t GetNextRouteId(NodeId_t dst) = 0;//查路由表，如果查不到则重新计算

	void clear_route_table();

	virtual CRoute* Clone() = 0; 

	// default route [7/19/2010 mpiuser]
	void set_default_route(NodeId_t r) { _default_route = r; }
	void set_select_strategy(RouteSelect_t t) { type = t;}
	void set_node(CNode *n) { pn = n; }

	size_t GetRouteTableSize(){return _route_table.size();}

	// new route [7/23/2010 Administrator]
	virtual RouteType_t get_route_type() { return OTHERS;}

	CNode* GetLocalNode();//关怡萍

protected:
	map<NodeId_t, vector<CRouteEntry> > _route_table;//路由表，目的节点id，下一跳信息

	// default route [7/19/2010 mpiuser]
	NodeId_t _default_route;

	// new route [7/23/2010 Administrator]
	virtual void  update_route_table(NodeId_t dst = 0) = 0;//更新路由表

	CNode *pn;

	CWhuSimulatorApp *pApp;

	RouteSelect_t type;

	//有多条路径备选时，选择方法
	NodeId_t select_first(NodeId_t dst);
	NodeId_t select_random(NodeId_t dst);
	virtual NodeId_t select_specific(NodeId_t dst);
};

#endif