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
	NodeId_t _next_hop;//��һ��id
	double 	_cost;//��ǰ��㵽Ŀ�Ľ�㾭_next_hopʱ����
};

class CRoute : public CErrorHandler
{
public:	
	CRoute(CNode *n = NULL);
	CRoute(const CRoute &);
	~CRoute(void);

	Error_str GetLastErrorEx(){return "hello";}

	typedef enum{ FIRST,RANDOM,SPECIFIC } RouteSelect_t;
	//�ж���·����ѡʱ��ѡ�񷽷���ѡ��һ���������ָ������ѡ

	// new [8/2/2010 Administrator]
	typedef enum{ SHORTESTPATH, INTEGRATE, OPTIMAL, DEGREEQUEUE, OTHERS } RouteType_t;
	//·�ɷ�������

	virtual NodeId_t GetNextRouteId(NodeId_t dst) = 0;//��·�ɱ�����鲻�������¼���

	void clear_route_table();

	virtual CRoute* Clone() = 0; 

	// default route [7/19/2010 mpiuser]
	void set_default_route(NodeId_t r) { _default_route = r; }
	void set_select_strategy(RouteSelect_t t) { type = t;}
	void set_node(CNode *n) { pn = n; }

	size_t GetRouteTableSize(){return _route_table.size();}

	// new route [7/23/2010 Administrator]
	virtual RouteType_t get_route_type() { return OTHERS;}

	CNode* GetLocalNode();//����Ƽ

protected:
	map<NodeId_t, vector<CRouteEntry> > _route_table;//·�ɱ�Ŀ�Ľڵ�id����һ����Ϣ

	// default route [7/19/2010 mpiuser]
	NodeId_t _default_route;

	// new route [7/23/2010 Administrator]
	virtual void  update_route_table(NodeId_t dst = 0) = 0;//����·�ɱ�

	CNode *pn;

	CWhuSimulatorApp *pApp;

	RouteSelect_t type;

	//�ж���·����ѡʱ��ѡ�񷽷�
	NodeId_t select_first(NodeId_t dst);
	NodeId_t select_random(NodeId_t dst);
	virtual NodeId_t select_specific(NodeId_t dst);
};

#endif