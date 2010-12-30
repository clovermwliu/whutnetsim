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
//ͼ��
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
	CNode* get_node(NodeId_t id);//��ýڵ�ָ��
	//��������
	// clear [8/2/2010 Administrator]
	void clear(int clearType);//���ͼ
	void ClearRouteTable();
	void SetDefaultRoute();
	
	//ͼ�Ĵ������ѯ
	void add_edge(NodeId_t sou, NodeId_t des);//����
	void add_edge(CEdge* edge);//����
	int add_node(NodeId_t node_id);//��ӽڵ�
	int del_node(NodeId_t node_id);
	int del_edge(NodeId_t sou, NodeId_t dst);//ɾ����
	int del_edge(CEdge* edge);//ɾ����

	size_t get_node_num(){ _node_num = _node_info.size(); return _node_num;}//�����Ŀ
	size_t get_edge_num();//��ñߵ���Ŀ

	map<NodeId_t, CNode*>::iterator get_node_info_begin() { return _node_info.begin();}
	map<NodeId_t, CNode*>::iterator get_node_info_end() { return _node_info.end();}

	//�ߡ��ھ�
	int clear_node_neighbor(NodeId_t node_id);//���ĳ���ڵ�������ھӽڵ�

	//Ȩֵ
	double get_edge_cost(NodeId_t sou, NodeId_t des);//����sou��des��Ȩֵ
	int set_edge_cost(NodeId_t sou, NodeId_t des, double w);
	double get_edge_delay(NodeId_t sou, NodeId_t des);//����sou��des��Ȩֵ
	int set_edge_delay(NodeId_t sou, NodeId_t des, double w);
	void set_ran_cost_delay(double low, double high);//cost = delay,���
	
	//ͼ������
	int delgraph();//����ɾ�����еı����
	int LoadTopo(CString TopoPath);//��ָ�����ļ��м�������
	// ini [7/19/2010 mpiuser]
	void NodeBoundRoute(CRoute* route);
	void SetThroughPut(double a);
	//debug new route
	void SetQueueW(double w);

	// new route [7/23/2010 Administrator]
	Handler_status_str GetHandleStatus() { return "Graph"; }//��ô�����¼���һЩ��Ϣ
	void Handle(CEventBase*, Time_t);//���¼��Ĵ�����

	int get_hops(NodeId_t i, NodeId_t d) { if (Did.empty()) UpdateDid(); return Did[i]->at(d); }
	void ClearDid();
	//  [8/2/2010 Administrator]
	void ClearMid();
	// new [8/2/2010 Administrator]
	double get_degreeMul(NodeId_t i, NodeId_t d) { if (Mid.empty()) UpdateMid(); return Mid[i]->at(d); }

	//  [8/2/2010 Administrator]
	bool IsConnected();//BFS�����жϵ�ǰͼ�Ƿ���ͨ�������ͨ����true�����򷵻�false
	void ClearNodeRoute();//����Ƽ
private:
	void delete_node_info();//ɾ���ڵ���Ϣ
	int GetNumfromStr(CString key);
	CString GetOneWord(CString * pStrLine);//��һ���ַ��л��һ����

	// new route [7/23/2010 Administrator]
	void UpdateDid();//�������н��֮�����������
    //  [8/2/2010 Administrator]
	void UpdateMid();//�������н��֮��·��������С���˻�

private:
	//ͼ�Ĵ洢��Ϣ
	map<NodeId_t, CNode*> _node_info;//�����Ϣ

	size_t _node_num; //�ܽ����
	size_t _edge_num; //�ܱ����Ŀ

	// new route [7/23/2010 Administrator]
	//// debug new route ע�� ���˸ı�ʱ��Ӧ���¼������Ϣ
	vector<vector<int> *> Did;//i��d��Ϊ���i�����d����������//ע�����������id��0��node_num-1���Ҳ����
	// new [8/2/2010 Administrator]
	vector<vector<double> *> Mid;//i��d��Ϊ���i�����d��·��������С���˻�//ע�����������id��0��node_num-1���Ҳ����
};
