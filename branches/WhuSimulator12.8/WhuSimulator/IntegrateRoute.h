// new route [7/23/2010 Administrator]
#pragma once
#include "Route.h"

class CIntegrateRoute : public CRoute
{
public:
	virtual RouteType_t get_route_type() { return INTEGRATE;}

	CIntegrateRoute(double aa, double bb, CNode *n = NULL);
	CIntegrateRoute(CNode *n = NULL);
	CIntegrateRoute(const CIntegrateRoute &);
	~CIntegrateRoute(void);

	virtual NodeId_t GetNextRouteId(NodeId_t dst);

	virtual CRoute* Clone();

	void set_a_b(double aa, double bb) { a = aa; b = bb; }
	double get_a() { return a; }
	double get_b() { return b; }

private:
	virtual void  update_route_table(NodeId_t dst = 0);//����·�ɱ�ֻ���µ�Ŀ�Ľ��dst��·����
	//map<NodeId_t, vector<CRouteEntry> > _route_table;
	//��·���㷨·�ɱ��д洢���� Ŀ�Ľ��id��ѡ���ھ�_next_hopΪ��һ���ĸ�����_cost

	double a;//ϵ��
	double b;//ϵ��

	NodeId_t select_neighbor(NodeId_t dst);//ѡ����һ��
};
