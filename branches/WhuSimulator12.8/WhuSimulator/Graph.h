#pragma once
#include <map>
#include <vector>
#include "Edge.h"
#include "Node.h"
#include "ErrorHandler.h"
#include "G_common_defs.h"
#include <math.h>
#include "Random.h"
using namespace std;

// new route [7/23/2010 Administrator]
#include "Handler.h"
#include "UpdateAvgQueueLengthEvent.h"
#include "Scheduler_map.h"

class CNode;
//图类
class CRoute;
class CGraph : public CHandler
{
public:
	CGraph(void);
	CGraph(const CGraph &g);
	CGraph& operator=(const CGraph &);
	~CGraph(void);
public:
	//Error_str GetLastErrorEx(){return 1;};
	CNode* get_node(NodeId_t id);//获得节点指针
	//辅助方法
	// clear [8/2/2010 Administrator]
	void clear(int clearType);//清除图
	void ClearRouteTable();
	void SetDefaultRoute();
	
	//图的创建与查询
	void add_edge(NodeId_t sou, NodeId_t des);//单向
	void add_edge(CEdge* edge);//单向
	int add_node(NodeId_t node_id);//添加节点
	int del_node(NodeId_t node_id);
	int del_edge(NodeId_t sou, NodeId_t dst);//删除边
	int del_edge(CEdge* edge);//删除边

	size_t get_node_num(){ _node_num = _node_info.size(); return _node_num;}//结点数目
	size_t get_edge_num();//获得边的数目

	map<NodeId_t, CNode*>::iterator get_node_info_begin() { return _node_info.begin();}
	map<NodeId_t, CNode*>::iterator get_node_info_end() { return _node_info.end();}

	//边、邻居
	int clear_node_neighbor(NodeId_t node_id);//清楚某个节点的所有邻居节点

	//权值
	double get_edge_cost(NodeId_t sou, NodeId_t des);//返回sou到des的权值
	int set_edge_cost(NodeId_t sou, NodeId_t des, double w);
	double get_edge_delay(NodeId_t sou, NodeId_t des);//返回sou到des的权值
	int set_edge_delay(NodeId_t sou, NodeId_t des, double w);
	void set_ran_cost_delay(double low, double high);//cost = delay,随机
	
	//图的销毁
	int delgraph();//包括删除所有的边与点
	int LoadTopo(CString TopoPath);//从指定的文件中加载拓扑
	// ini [7/19/2010 mpiuser]
	void NodeBoundRoute(CRoute* route);
	void SetThroughPut(double a);
	//debug new route
	void SetQueueW(double w);

	// new route [7/23/2010 Administrator]
	Handler_status_str GetHandleStatus() { return "Graph"; }//获得处理包事件的一些信息
	void Handle(CEventBase*, Time_t);//包事件的处理方法

	int get_hops(NodeId_t i, NodeId_t d) { if (Did.empty()) UpdateDid(); return Did[i]->at(d); }
	void ClearDid();
	//  [8/2/2010 Administrator]
	void ClearMid();
	// new [8/2/2010 Administrator]
	double get_degreeMul(NodeId_t i, NodeId_t d) { if (Mid.empty()) UpdateMid(); return Mid[i]->at(d); }

	//  [8/2/2010 Administrator]
	bool IsConnected();//BFS方法判断当前图是否连通，如果连通返回true，否则返回false
	void ClearNodeRoute();//关怡萍
private:
	void delete_node_info();//删除节点信息
	int GetNumfromStr(CString key);
	CString GetOneWord(CString * pStrLine);//从一串字符中获得一个词

	// new route [7/23/2010 Administrator]
	void UpdateDid();//更新所有结点之间的最少跳数
    //  [8/2/2010 Administrator]
	void UpdateMid();//更新所有结点之间路径结点度最小连乘积

private:
	//图的存储信息
	map<NodeId_t, CNode*> _node_info;//结点信息

	size_t _node_num; //总结点数
	size_t _edge_num; //总变的数目

	// new route [7/23/2010 Administrator]
	//// debug new route 注意 拓扑改变时，应重新计算该信息
	vector<vector<int> *> Did;//i行d列为结点i到结点d的最少跳数//注意这里假设结点id从0到node_num-1并且不间断
	// new [8/2/2010 Administrator]
	vector<vector<double> *> Mid;//i行d列为结点i到结点d的路径结点度最小连乘积//注意这里假设结点id从0到node_num-1并且不间断
};
