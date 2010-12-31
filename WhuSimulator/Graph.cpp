#include "StdAfx.h"
#include "Graph.h"
//#include "Test.h"


CGraph::CGraph(void)
{
	DEBUG3(GlobalVariable::out_new<<"New Graph"<<endl);
}

CGraph::CGraph(const CGraph &g):
_node_num(g._node_num),
_edge_num(g._edge_num)
{
	map<NodeId_t, CNode*>::const_iterator it = g._node_info.begin();
	while(it != g._node_info.end())
	{
		_node_info.insert(make_pair(it->first, new CNode(*(it->second))));
		it++;
	}

}

CGraph& CGraph::operator=(const CGraph &g)
{
	_node_num=g._node_num;

	map<NodeId_t, CNode*>::const_iterator it = g._node_info.begin();
	while(it != g._node_info.end())
	{
		_node_info.insert(make_pair(it->first, new CNode(*(it->second))));
		it++;
	}

	return *this;

}

CGraph::~CGraph(void)
{
	delete_node_info();
}
bool 
CGraph::IsConnected()
{
	queue<NodeId_t> q;
	vector<bool> visited;
	for (size_t i = 0; i < get_node_num(); ++i)
	{
		visited.push_back(false);
	}
	visited[0] = true;
	q.push(0);
	CNode* this_node;
	while (!q.empty())
	{
		this_node = get_node(q.front());
		q.pop();
		map<NodeId_t, CEdge*>::iterator it = this_node->get_neighbor_begin();
		for (; it != this_node->get_neighbor_end(); ++it)
		{
			if (!visited[it->first])
			{
				visited[it->first] = true;
				q.push(it->first);
			}
		}
	}
	for (size_t i = 0; i < visited.size(); ++i)
	{
		if (!visited[i])
		{
			return false;
		}
	}	
	return true;
}

void
CGraph::clear(int clearType)
//���ͼ��tTopoΪTRUE��ʾ���������Ϣ�������ˣ�FALSE��ʾ����������
/*
0: ȫ�����(����·��)
1: ��������
2: �������˺�·��
*/
{
	switch (clearType)
	{
	case 0:
			{
				delgraph();

				_node_num = 0;
				_edge_num = 0;
				break;
			}
	case 1:
		   {
			   map<NodeId_t, CNode*>::iterator it = _node_info.begin();
			   while(it != _node_info.end())
			   {
				   it->second->ClearQueue();
				   delete it->second->GetRoute();
				   ++it;
			   }
			   break;
		   }
	case 2:
		   {
			   map<NodeId_t, CNode*>::iterator it = _node_info.begin();
			   while(it != _node_info.end())
			   {
				   it->second->ClearQueue();
				   ++it;
			   }
			   break;
		   }
	default:break;
	}
}

void 
CGraph::add_edge(NodeId_t sou, NodeId_t des)////////////////
{
	map<NodeId_t, CNode*>::iterator it_sou = _node_info.find(sou);
	map<NodeId_t, CNode*>::iterator it_des = _node_info.find(des);
	if (it_sou == _node_info.end())//�ý�㲻���� 
	{
		_node_info.insert(make_pair(sou, new CNode(sou)));
	}

	_node_info[sou]->add_neighbor(des);

	if (it_des == _node_info.end())//�ý�㲻���� 
	{
		_node_info.insert(make_pair(des, new CNode(des)));
	}
}

void CGraph::add_edge(CEdge* edge)//����
{
	add_edge(edge->get_sou_id(), edge->get_des_id());
}

int 
CGraph::add_node(NodeId_t node_id)////////////
{
	map<NodeId_t, CNode*>::iterator it = _node_info.find(node_id);
	if (it == _node_info.end())
	{
		_node_info.insert(make_pair(node_id, new CNode(node_id)));
		return 1;//������
	}
	return -1;//�Ѵ���
}
int CGraph::del_node(NodeId_t node_id)
{
	map<NodeId_t, CNode*>::iterator node = _node_info.find(node_id);
	if (node == _node_info.end())
	{
		return -1;
	}else
	{
		return 0;
	}
}
size_t
CGraph::get_edge_num()/////////////////////
{
	_edge_num = 0;
	map<NodeId_t, CNode*>::iterator it = _node_info.begin();
	while(it != _node_info.end())
	{
		_edge_num += it->second->get_neighbor_size();
		it++;
	}
	return _edge_num;
}


int
CGraph::delgraph()/////////////////
{
	delete_node_info();

	_node_num = 0;
	_edge_num = 0;
	return 0;
}

void
CGraph::delete_node_info()//////////////////
{
	map<NodeId_t, CNode*>::iterator it = _node_info.begin();
	while(it != _node_info.end())
	{
		delete it->second;
		++it;
	}
	_node_info.clear();
}
void CGraph::ClearRouteTable()
{
	map<NodeId_t, CNode*>::iterator it = _node_info.begin();
	while(it != _node_info.end())
	{
		it->second->ClearRouteTable();
		++it;
	}
	//_node_info.clear();
}
void CGraph::SetDefaultRoute()
{
	map<NodeId_t, CNode*>::iterator it = _node_info.begin();
	while(it != _node_info.end())
	{
		it->second->set_default_route();
		++it;
	}
	_node_info.clear();
}
double 
CGraph::get_edge_cost(NodeId_t sou, NodeId_t des)//����sou��des��cost////////////////////
{
	if (_node_info.find(sou) == _node_info.end())
	{
		return -1;
	}
	return _node_info[sou]->get_neighbor_cost(des);
}


int 
CGraph::set_edge_cost(NodeId_t sou, NodeId_t des, double w)//////////////
{
	if (_node_info.find(sou) == _node_info.end())
	{
		return -1;
	}
	_node_info[sou]->set_neighbor_cost(des, w);
	return 1;

}

double 
CGraph::get_edge_delay(NodeId_t sou, NodeId_t des)//����sou��des��delay////////////////////
{
	if (_node_info.find(sou) == _node_info.end())
	{
		return -1;
	}
	return _node_info[sou]->get_neighbor_delay(des);
}


int 
CGraph::set_edge_delay(NodeId_t sou, NodeId_t des, double w)//////////////
{
	if (_node_info.find(sou) == _node_info.end())
	{
		return -1;
	}
	_node_info[sou]->set_neighbor_delay(des, w);
	return 1;

}

int 
CGraph::clear_node_neighbor(NodeId_t node_id)////////////////
{
	if (_node_info.find(node_id) == _node_info.end())
	{
		return -1;
	}
	_node_info[node_id]->clear_neighbor();
	return 1;

}

int 
CGraph::del_edge(NodeId_t sou, NodeId_t dst)///////////
{
	if (_node_info.find(sou) == _node_info.end())
	{
		return -1;
	}
	_node_info[sou]->del_neighbor(dst);
	return 1;
}
int CGraph::del_edge(CEdge* edge)//ɾ����
{
	return del_edge(edge->get_sou_id(), edge->get_des_id());
}
int CGraph::LoadTopo(CString TopoPath)
{
	CStdioFile topoFile;
	if ( !topoFile.Open(TopoPath, CStdioFile::modeRead | CStdioFile::shareDenyWrite ))
		return -1;
	CString strLine;
	CString strSou, strDes;
	int sou, des;
	while (topoFile.ReadString(strLine))//��ȡһ��
	{			
		do 
		{
			strSou = GetOneWord(&strLine);//ȥ�ո�
			strDes = GetOneWord(&strLine);
			sou = GetNumfromStr(strSou);//CString to int
			des = GetNumfromStr(strDes);
			add_edge(sou, des);
		} while(!strLine.IsEmpty());			
	}
	topoFile.Close();
	return 0;
}
#define PARTITIONMARK " "
CString CGraph::GetOneWord(CString * pStrLine)
/*
Description:��PARTITIONMARKΪ�������,��һ��������ѡ��һ��PARTITIONMARK��ߵ��ַ����������������ָ���Ϊ��һ��PARTITIONMARK�ұߵ��ַ���
Parameter: pStrLine [In&Out] ָ��һ������CString��ָ��
Return Value:ȡ������һ����
Remark:ͬʱ�޸���һ������

pExample: #define PARTITIONMARK " "
CString a("OK 123 ###");
CString b;
b=GetOneKeyWord(&a);//a="123 ###",b="OK"
b=GetOneKeyWord(&a);//a="###",b="123"
b=GetOneKeyWord(&a);//a="",b="###"
*/
{
	CString KeyWord;
	CString strTemp;
	int pos=pStrLine->Find(PARTITIONMARK);//��pStrLine��ָ�������ַ������ҵ���һ��PARTITIONMARK���ڵ�λ�ã�����pos
	int len=pStrLine->GetLength();//ȡ�����ַ����ĳ��ȸ���len
	if (pos==-1)//pos=-1��ʾ��pStrLine��ָ�������ַ����в�����PARTITIONMARK
	{ 
		KeyWord=(*pStrLine);//�ؼ��ʵ�����һ�У������������ӵ�b="###"
		(*pStrLine)="";//��������Ϊ�գ������������ӵ�a=""
	}
	else
	{
		KeyWord=pStrLine->Left(pos);//��pos��ߵ��ַ�������keyword
		strTemp=pStrLine->Right(len-pos-1);//ȡpos�ұߵ��ַ�����strTemp
		(*pStrLine)=strTemp;//��������ΪstrTemp
	}
	return KeyWord;//
}
int CGraph::GetNumfromStr(CString key)
{
	int len=key.GetLength();
	int result=0;
	
	for (int i = 0; i < len; ++i)
		result += ((int)key.GetAt(i)-0x30)*(int)pow((double)10, (double)(len-1-i));
	return result;
}
CNode* CGraph::get_node(NodeId_t id)
{
	CNode* node;

	if (_node_info.find(id) == _node_info.end())
	{
		return NULL;
	}
	node = _node_info[id];
	return node;
}
// debug new route
void 
CGraph::NodeBoundRoute(CRoute* route)
{
	map<NodeId_t, CNode*>::iterator it = _node_info.begin();
	for (; it != _node_info.end(); ++it)
	{
		it->second->BoundRoute(route->Clone());
		it->second->set_default_route();
	}
	switch(route->get_route_type())
	{
	case CRoute::INTEGRATE:
		UpdateDid();
		break;
		// new [8/2/2010 Administrator]
	case CRoute::DEGREEQUEUE:
		UpdateMid();
	default:
		break;
	}


}

void 
CGraph::SetThroughPut(double a)
{
	map<NodeId_t, CNode*>::iterator it = _node_info.begin();
	for (; it != _node_info.end(); ++it)
	{
		it->second->set_throughPut(a*it->second->get_neighbor_size());
		DEBUG8(GlobalVariable::out_loc<<it->second->get_neighbor_size()<<endl);
	}
}

void 
CGraph::set_ran_cost_delay(double low, double high)//cost = delay,���
{
	map<NodeId_t, CNode*>::iterator it = _node_info.begin();
	for (; it != _node_info.end(); ++it)
	{
		map<NodeId_t, CEdge*>::iterator n_it = it->second->get_neighbor_begin();
		for (; n_it != it->second->get_neighbor_end(); ++n_it)
		{
			double r = CRandom::Random_U(low, high);
			n_it->second->set_cost(r);
			n_it->second->set_delay(r);
		}
	}	

}

// new route [7/23/2010 Administrator]
void 
CGraph::Handle(CEventBase* e, Time_t t)
/*
���������¼��Ĵ�����
������[IN] e   ��Ҫ�����ʱ��
[IN] t   �������¼���ʱ��
���أ���
*/
{
	//�������н���ƽ�����г��ȣ���������н���·�ɱ�
	map<NodeId_t, CNode*>::iterator it = _node_info.begin();
	for (; it != _node_info.end(); ++it)
	{
		it->second->UpdateQueueAvgLength();
		it->second->ClearRouteTable();
	}	

	////������һ�����¶��г��ȵ��¼�
	CUpdateAvgQueueLengthEvent * GNevent = (CUpdateAvgQueueLengthEvent *)e;
	CScheduler_map::InsertEvent(new CUpdateAvgQueueLengthEvent(CUpdateAvgQueueLengthEvent::UpdateAvgQueueLengthEvent,
								this,
								CSimulator::Now()+GNevent->get_t(),
								GNevent->get_t()));

	//ɾ����ǰ�������¼�;
	delete GNevent;
}

void
CGraph::ClearDid()
{
	for (int i = 0; i < (int)Did.size(); ++i)
	{
		Did[i]->clear();
		delete Did[i];
	}
	Did.clear();
}

// new [8/2/2010 Administrator]
void
CGraph::ClearMid()
{
	for (int i = 0; i < (int)Mid.size(); ++i)
	{
		Mid[i]->clear();
		delete Mid[i];
	}
	Mid.clear();
}


#define MAX_HOPS 10000000
void
CGraph::UpdateDid()//floyd�㷨
{
	//��ʼ��Did
	ClearDid();
	for (int i = 0; i < (int)get_node_num(); ++i)
	{
		vector<int> *v = new vector<int>;
		Did.push_back(v);
		for (int j = 0; j < (int)get_node_num(); ++j)
		{
			if (_node_info[i]->find_neighbor(j))
				Did[i]->push_back(1);
			else
				Did[i]->push_back(MAX_HOPS);
		}
		Did[i]->at(i) = 0;
	}
	//����
	for (int k = 0; k < (int)get_node_num(); ++k)
	{
		for (int i = 0; i < (int)get_node_num(); ++i)
		{
			for (int j = 0; j < (int)get_node_num(); ++j)
			{
				if (Did[i]->at(j) > (Did[i]->at(k) + Did[k]->at(j)))
				{
					Did[i]->at(j) = Did[i]->at(k) + Did[k]->at(j);
				}
			}
		}
	}
}

// new --[8/2/2010 Administrator]
#define MAX_DEGREE 10000000
void
CGraph::UpdateMid()//floyd�㷨
{
	//��ʼ��Mid
	ClearMid();
	for (int i = 0; i < (int)get_node_num(); ++i)
	{
		vector<double> *v = new vector<double>;
		Mid.push_back(v);
		double di = _node_info[i]->get_degree();
		for (int j = 0; j < (int)get_node_num(); ++j)
		{
			if (_node_info[i]->find_neighbor(j))
				Mid[i]->push_back(di*_node_info[j]->get_degree());
			else
				Mid[i]->push_back(MAX_DEGREE);
		}
		Mid[i]->at(i) = di;
	}
	//����
	for (int k = 0; k < (int)get_node_num(); ++k)
	{
		for (int i = 0; i < (int)get_node_num(); ++i)
		{
			for (int j = 0; j < (int)get_node_num(); ++j)
			{
				double tmp = Mid[i]->at(k) * Mid[k]->at(j)/Mid[k]->at(k);
				if (Mid[i]->at(j) > tmp)
				{
					Mid[i]->at(j) = tmp;
				}
			}
		}
	}
}

//debug new route
void 
CGraph::SetQueueW(double w)
{
	map<NodeId_t, CNode*>::iterator it = _node_info.begin();
	for (; it != _node_info.end(); ++it)
	{
		it->second->set_current_length_w(w);
	}
}

void
CGraph::ClearNodeRoute()//����Ƽ
{
	map<NodeId_t, CNode*>::iterator it = _node_info.begin();
	for (; it != _node_info.end(); ++it)
	{
		it->second->ClearRoute();
	}
}
