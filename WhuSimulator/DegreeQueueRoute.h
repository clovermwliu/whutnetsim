// new [8/2/2010 Administrator]
#pragma once
#include "route.h"

class CDegreeQueueRoute :
	public CRoute
{
public:
	virtual RouteType_t get_route_type() { return DEGREEQUEUE;}

	CDegreeQueueRoute(double aa, double bb, CNode *n = NULL):a(aa), b(bb), CRoute(n){}
	CDegreeQueueRoute(CNode *n = NULL):a(-2.5), b(2.5), CRoute(n){}
	CDegreeQueueRoute(const CDegreeQueueRoute & c):a(c.a), b(c.b), CRoute(c){}
	~CDegreeQueueRoute(void) {}

	virtual NodeId_t GetNextRouteId(NodeId_t dst);

	virtual CRoute* Clone() { return new CDegreeQueueRoute(*this); }

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
