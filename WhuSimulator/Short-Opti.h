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

	double rate;//ѡ�����·���ı�������Ӧ��ѡ������˻��ı�����1-rate

	CShortestPath shortroute;

	COptimalRoute optiroute;
	
	virtual void  update_route_table(NodeId_t dst = 0);//����·�ɱ�

	//void dijkstra();//���·���������㷨

	//void dijkstra_degree();//�ȵ����˻��������㷨

	//void inte_dijkstra();

	//void copy_entrys(vector<CRouteEntry> &from, 
		//vector<CRouteEntry> &to, 
		//double new_cost);
};