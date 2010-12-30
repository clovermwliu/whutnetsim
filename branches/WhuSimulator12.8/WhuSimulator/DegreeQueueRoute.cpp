// new [8/2/2010 Administrator]
#include "StdAfx.h"
#include "DegreeQueueRoute.h"
#include "Random.h"


NodeId_t 
CDegreeQueueRoute::GetNextRouteId(NodeId_t dst)
{
	if(dst == pn->get_id() || pn->find_neighbor(dst))
		return dst;

	if (_route_table.find(dst) == _route_table.end())//�Ҳ����������
	{
		update_route_table(dst);
	}

	return select_neighbor(dst);
}

//���о�
void  
CDegreeQueueRoute::update_route_table(NodeId_t dst)//����·�ɱ�
{
	double sum = 0;
	double tmp1, tmp2;
	//�����ý����ھӽ��,sum
	map<NodeId_t, CEdge*>::iterator it = pn->get_neighbor_begin();
	for (; it != pn->get_neighbor_end(); ++it)
	{
		//�ھӽ��it->first
		tmp1 = pow(pApp->graph->get_node(it->first)->GetQueueAvgLength()+1, a);
		double ta = pApp->graph->get_degreeMul(pn->get_id(), dst);
		double tb = pApp->graph->get_degreeMul(it->first, dst);
		tmp2 = exp(b*(pApp->graph->get_degreeMul(pn->get_id(), dst) - pApp->graph->get_degreeMul(it->first, dst) - 1));
		sum += tmp1*tmp2;
	}
	//����ѡ����ھӽ��ĸ��ʣ���������·�ɱ���
	if (_route_table.find(dst) != _route_table.end())
	{
		_route_table.erase(_route_table.find(dst));
	}
	//����
	vector<CRouteEntry> r;
	it = pn->get_neighbor_begin();
	for (; it != pn->get_neighbor_end(); ++it)
	{
		////�ھӽ��it->first	
		tmp1 = pow(pApp->graph->get_node(it->first)->GetQueueAvgLength()+1, a);
		tmp2 = exp(b*(pApp->graph->get_degreeMul(pn->get_id(), dst) - pApp->graph->get_degreeMul(it->first, dst) - 1));
		r.push_back(CRouteEntry(it->first, tmp1*tmp2/sum));
	}
	_route_table.insert(make_pair(dst, r));
}

NodeId_t 
CDegreeQueueRoute::select_neighbor(NodeId_t dst)//ѡ����һ��
{
	double ran = CRandom::Random_U();
	map<NodeId_t, vector<CRouteEntry>>::iterator it = _route_table.find(dst);
	double sum = 0;
	for (int i = 0; i < (int)it->second.size(); ++i)
	{
		sum += it->second.at(i).get_cost();
		if (ran <= sum)
		{
			return it->second.at(i).get_next_hop();
		}
	}
	return _default_route;
}
