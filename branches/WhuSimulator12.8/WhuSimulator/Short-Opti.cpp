
#include "StdAfx.h"
#include "Short-Opti.h"
#include "Random.h"

CShortOpti::CShortOpti(double a, CNode *n):CRoute(n)
{
	rate = a;

}

CShortOpti::CShortOpti(CNode *n):CRoute(n)
{
	rate = 0.5;
	//shortroute = new CShortestPath();
	//optiroute = new COptimalRoute();
}

CShortOpti::CShortOpti(const CShortOpti &c):rate(c.rate), shortroute(c.shortroute), optiroute(c.optiroute),CRoute(c)
{

}

CShortOpti::~CShortOpti(void)
{
}

CRoute* 
CShortOpti::Clone()
{
	return new CShortOpti(*this);
}

NodeId_t 
CShortOpti::GetNextRouteId(NodeId_t dst)
{
	if (shortroute.GetLocalNode()==NULL)
	{
		shortroute.set_node(pn);
		//= GetLocalNode();
	}
	if (optiroute.GetLocalNode()==NULL)
	{
		optiroute.set_node(pn);
	}
	double rng = CRandom::Random_U();
	if (rng<rate)   //大于则调用连乘积的路径算法
	{
		return shortroute.GetNextRouteId(dst);
	} 
	else
	{
		return optiroute.GetNextRouteId(dst);
	}
}

void
CShortOpti::update_route_table(NodeId_t dst)//更新路由表，并返回到dst的下一跳
{

}
