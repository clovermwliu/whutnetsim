#pragma once

#ifndef __NODE_H__
#define __NODE_H__

#include <vector>
#include <map>
#include "Edge.h"
#include "Handler.h"
#include "Route.h"
#include "Queue.h"
#include "G_common_defs.h"
#include "Packet.h"
//#include "ShortestPath.h"
using namespace std;

class CRoute;
class CQueue;
class CNode : public CHandler
{
public:
	CNode(NodeId_t id);

	~CNode(void);

	CNode(const CNode &n);

	CNode& operator=(const CNode &);

	
public:
	Handler_status_str GetHandleStatus();//获得处理包事件的一些信息
	void Handle(CEventBase*, Time_t);//包事件的处理方法
	//邻居
	map<NodeId_t, CEdge*>::iterator get_neighbor_begin() { return _neighbor.begin();}
	map<NodeId_t, CEdge*>::iterator get_neighbor_end() { return _neighbor.end();}

	size_t get_neighbor_size() {return _neighbor.size(); }//获得邻居节点

	void add_neighbor(NodeId_t dst)						  //添加邻居节点
	{ _neighbor.insert(make_pair(dst, new CEdge(_id, dst)));}

	void clear_neighbor();	//清除所有所有邻居

	int del_neighbor(NodeId_t dst);//删除当前结点到dst的边，如果删除失败或查找失败返回-1

	double get_neighbor_cost(NodeId_t dst);//查找失败返回-1

	int set_neighbor_cost(NodeId_t dst, double w);//查找失败返回-1

	double get_neighbor_delay(NodeId_t dst);//查找失败返回-1

	int set_neighbor_delay(NodeId_t dst, double w);//查找失败返回-1

	// new route [7/23/2010 Administrator]
	bool find_neighbor(NodeId_t dst){ return (_neighbor.find(dst) == _neighbor.end())? false:true; }
	//dst为该结点的邻居时返回true，否则返回false

	//结点id
	NodeId_t get_id() { return _id; }				//返回节点ID
	void set_id(NodeId_t id) { _id = id;}			//设置节点ID

	Packet* PacketDeQueue(); //让一个数据包出队
	bool PacketEnQueue(Packet* p);//让一个数据包入队
	size_t GetQueueLength();//获得队列长度
	// new route [7/23/2010 Administrator]
	double UpdateQueueAvgLength();//更新队列长度,并返回
	double GetQueueAvgLength();//取

	double GetNewNextPacketTime();
	//NodeId_t GetNextRoute(NodeId_t des){return route->GetNextRouteId(des);}

	void BoundRoute(CRoute* _route);//给节点绑定路由 
	
	void set_default_route();
	void set_throughPut(double t);
	//debug new route
	void set_current_length_w(double w);

	void ClearRouteTable();
	//void SetDefaultRoute(){route->set_default_route();}
	// optimal route [7/22/2010 mpiuser]
	size_t get_degree() {return _neighbor.size(); }//获得邻居节点

	void ClearQueue();
	
	void ClearRoute();

	CRoute* GetRoute(){return route;}
	CQueue* GetQueue(){return queue;}
private:
	CRoute* route;	//节点选用的路由类
	CQueue* queue;	//节点上的队列指针

	//存储邻居信息，即边
	map<NodeId_t, CEdge*> _neighbor;//邻居结点id，边对象指针
	NodeId_t _id;//结点id
};
#endif