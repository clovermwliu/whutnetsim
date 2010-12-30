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
	Handler_status_str GetHandleStatus();//��ô�����¼���һЩ��Ϣ
	void Handle(CEventBase*, Time_t);//���¼��Ĵ�����
	//�ھ�
	map<NodeId_t, CEdge*>::iterator get_neighbor_begin() { return _neighbor.begin();}
	map<NodeId_t, CEdge*>::iterator get_neighbor_end() { return _neighbor.end();}

	size_t get_neighbor_size() {return _neighbor.size(); }//����ھӽڵ�

	void add_neighbor(NodeId_t dst)						  //����ھӽڵ�
	{ _neighbor.insert(make_pair(dst, new CEdge(_id, dst)));}

	void clear_neighbor();	//������������ھ�

	int del_neighbor(NodeId_t dst);//ɾ����ǰ��㵽dst�ıߣ����ɾ��ʧ�ܻ����ʧ�ܷ���-1

	double get_neighbor_cost(NodeId_t dst);//����ʧ�ܷ���-1

	int set_neighbor_cost(NodeId_t dst, double w);//����ʧ�ܷ���-1

	double get_neighbor_delay(NodeId_t dst);//����ʧ�ܷ���-1

	int set_neighbor_delay(NodeId_t dst, double w);//����ʧ�ܷ���-1

	// new route [7/23/2010 Administrator]
	bool find_neighbor(NodeId_t dst){ return (_neighbor.find(dst) == _neighbor.end())? false:true; }
	//dstΪ�ý����ھ�ʱ����true�����򷵻�false

	//���id
	NodeId_t get_id() { return _id; }				//���ؽڵ�ID
	void set_id(NodeId_t id) { _id = id;}			//���ýڵ�ID

	Packet* PacketDeQueue(); //��һ�����ݰ�����
	bool PacketEnQueue(Packet* p);//��һ�����ݰ����
	size_t GetQueueLength();//��ö��г���
	// new route [7/23/2010 Administrator]
	double UpdateQueueAvgLength();//���¶��г���,������
	double GetQueueAvgLength();//ȡ

	double GetNewNextPacketTime();
	//NodeId_t GetNextRoute(NodeId_t des){return route->GetNextRouteId(des);}

	void BoundRoute(CRoute* _route);//���ڵ��·�� 
	
	void set_default_route();
	void set_throughPut(double t);
	//debug new route
	void set_current_length_w(double w);

	void ClearRouteTable();
	//void SetDefaultRoute(){route->set_default_route();}
	// optimal route [7/22/2010 mpiuser]
	size_t get_degree() {return _neighbor.size(); }//����ھӽڵ�

	void ClearQueue();
	
	void ClearRoute();

	CRoute* GetRoute(){return route;}
	CQueue* GetQueue(){return queue;}
private:
	CRoute* route;	//�ڵ�ѡ�õ�·����
	CQueue* queue;	//�ڵ��ϵĶ���ָ��

	//�洢�ھ���Ϣ������
	map<NodeId_t, CEdge*> _neighbor;//�ھӽ��id���߶���ָ��
	NodeId_t _id;//���id
};
#endif