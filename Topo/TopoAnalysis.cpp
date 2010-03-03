//Copyright (c) 2010, Information Security Institute of Wuhan Universtiy(ISIWhu)
//Project Homepage:http://code.google.com/p/whutnetsim/
//corresponding author's email: guochi@mail.whu.edu.cn


//All rights reserved
//
//PLEASE READ THIS DOCUMENT CAREFULLY BEFORE UTILIZING THE PROGRAM
//BY UTILIZING THIS PROGRAM, YOU AGREE TO BECOME BOUND BY THE TERMS OF
//THIS LICENSE.  IF YOU DO NOT AGREE TO THE TERMS OF THIS LICENSE, DO
//NOT USE THIS PROGRAM OR ANY PORTION THEREOF IN ANY FORM OR MANNER.
//
//This License allows you to:
//1. Make copies and distribute copies of the Program's source code provide that any such copy 
//   clearly displays any and all appropriate copyright notices and disclaimer of warranty as set 
//   forth in this License.
//2. Modify the original copy or copies of the Program or any portion thereof ("Modification(s)"). 
//   Modifications may be copied and distributed under the terms and conditions as set forth above. 
//   Any and all modified files must be affixed with prominent notices that you have changed the 
//   files and the date that the changes occurred.

//Termination:
//   If at anytime you are unable to comply with any portion of this License you must immediately 
//   cease use of the Program and all distribution activities involving the Program or any portion 
//   thereof.

//Statement:
//   In this program, part of the code is from the GTNetS project, The Georgia Tech Network 
//   Simulator (GTNetS) is a full-featured network simulation environment that allows researchers in 
//   computer networks to study the behavior of moderate to large scale networks, under a variety of 
//   conditions. Our work have great advance due to this project, Thanks to Dr. George F. Riley from 
//   Georgia Tech Research Corporation. Anyone who wants to study the GTNetS can come to its homepage:
//   http://www.ece.gatech.edu/research/labs/MANIACS/GTNetS/
//


//File Information:
//
//
//File Name:
//File Purpose:
//Original Author:
//Author Organization:
//Construct Date:
//Modify Author:
//Author Organization:
//Modify Date:

//�����ˣ�����
//����ʱ�䣺2010-3-3
#include "TopoAnalysis.h"
#include "node.h"

typedef map<int,int>::value_type degreeValtype;
CTopoAnalysis::CTopoAnalysis(CPlatTopoBase* base):platTopo(base)
{
	

}

CTopoAnalysis::~CTopoAnalysis(void)
{
}
void CTopoAnalysis::AnalysisDegree()
{	
	int num = 0;
	if (!(platTopo->topoattribute))
	{
		platTopo->topoattribute=new TopoAnalysisAttribute();
	}
	
	Node* iternode;
    iternode = platTopo->Begin();
	while(iternode)                                                         //�е����⣬End��begin�ı��
	{
		NodeWeightVec_t nwv;
		iternode->Neighbors(nwv);
		num = nwv.size();
		if ( platTopo->topoattribute->distribution.count(num) )
		{ 
			platTopo->topoattribute->distribution[num]++;
		} 
		else
		{
			platTopo->topoattribute->distribution.insert(degreeValtype(num,1));
		}
	    iternode = platTopo->Next();
	} 	
}

void CTopoAnalysis::ComBetweenness()
{
	_size = platTopo->NodeCount();

	_p_list = new des_node[_size];

	// ��ʼ����ά���� int **_p_spc;
	_p_spc = (int**)new int[_size];
	for (size_t i = 0; i < _size; ++i)
	{
		_p_spc[i] = new int[_size];
		for (size_t j = 0; j < _size; ++j)
		{
			_p_spc[i][j] = 0;
		}
	}

	//��ʼ��_hit����
	_hit = new int[_size];
	for (size_t i = 0; i < _size; ++i)
	{
		_hit[i] = -1;
	}

	shortestPathNum = 0;
}
int CTopoAnalysis::print_btns()
{
	std::cout << std::fixed;
	std::cout.precision(0);

	std::string file_name = "out.dat";
	std::ofstream out_f(file_name.c_str());		
	if (!out_f)
	{
		std::cerr << "error: unable to open input file: flow.dat" << std::endl;
		return -1;
	}

	for (size_t i = 0; i < _size; ++i)
	{
		std::cout.precision(0);
		if (platTopo->GetNode(i)->get_btns() != 0)
		{
			out_f << i << "�����˵��·����Ŀ" << platTopo->GetNode(i)->get_btns() << std::endl;
		}
		else
		{
			out_f << i << "	" << platTopo->GetNode(i)->get_btns()+1 << std::endl;//���������1
		}	

		//std::cout << "�ڵ�" << node_id << "	";
		//std::cout.precision(2);
		//std::cout << "�ڵ����: " << _p_graph->find(node_id)->get_btns()/*+_p_graph->find(node_id)->get_degree()*/ << std::endl;
	}
	out_f <<"���·������Ŀ��"<< shortestPathNum << std::endl;
	return 1;
}
int CTopoAnalysis::cal_btns()
{
	for (size_t i = 0; i < _size; ++i)
	{
		search_sps(i);
		shortestPathNum += GetOneStartPathNum();
	}

	//ÿ����Ľ�����Ҫ����2��

	return 1;
}

/*
*	����int search_sps(int start)����ڵ�start��ͼ�������ڵ�����·��
*	����˵��: int start��_id_table�����һ������ֵ
*/
int CTopoAnalysis::search_sps(size_t start)
{
	int pos = 0, length = 1;

	//������������
	zero_array();

	_hit[start] = 0;
	_p_list[0]._index = start;
	_p_spc[start][start] = 1;

	for (size_t i = 0; i < _size; ++i)
	{
		_p_list[i]._pre_id.clear();
	}

	while (pos < length)
	{
		size_t x1 = _p_list[pos]._index;	   //pos��ʾ�ڵ�_p_list�����е�λ��
		Node *one_node = platTopo->GetNode(x1);//index��ʾ����������˵ĵڼ����ڵ�

		//ѭ������ڵ�x1��ÿ����
		NodeWeightVec_t nwv;
		one_node->Neighbors(nwv);
		for (NodeWeightVec_t::iterator iter = nwv.begin();iter!=nwv.end();iter++)
		{
			NodeIfWeight one_edge = *iter;
	        one_edge.weight = 1;
			int new_cost = _p_list[pos]._step + one_edge.weight;

			size_t i = one_edge.node->Id() - platTopo->GetFirst();  // i������_p_list�е�pos���ڵ��һ����ָ��ڵ������

			if (_hit[i] >= 0)   // ����Ϊi�Ľڵ��Ѿ������ڱ��������Ŀɴ��㼯����
			{
				int j = _hit[i];  // j��������Ϊi�Ľڵ���_p_list�����е�λ��

				if (new_cost < _p_list[j]._step)
				{
					_p_list[j]._step = new_cost;
					_p_list[j]._pre_id.clear();
					//_p_list[j]._pre_id.push_back(pos);
					for (size_t k = 0; k < _size; ++k)
					{
						_p_spc[i][k] = 0;
					}
				}
				if (new_cost == _p_list[j]._step)
				{
					_p_list[j]._pre_id.push_back(pos);
					for (size_t k = 0; k < _size; ++k)
					{
						_p_spc[i][k] += _p_spc[x1][k];
					}
					_p_spc[i][i] += _p_spc[x1][x1];
				}
			}
			else    // ����Ϊi�Ľڵ㻹δ�����ڱ��������Ŀɴ��㼯����
			{
				_p_list[length]._index = i;
				_p_list[length]._step = new_cost;
				_p_list[length]._pre_id.push_back(pos);

				_hit[i] = length;

				for (size_t k = 0; k < _size; ++k)
				{
					_p_spc[i][k] += _p_spc[x1][k];
				}
				_p_spc[i][i] += _p_spc[x1][x1];

				++length;
			}
		}	
		++pos;
	}

	//����Դ��Ŀ�Ľڵ��ID

	for (size_t i =  0; i < _size; ++i)
	{
		for (size_t j = 0; j < _size; ++j)
		{
			if (j != start && i !=start && j != i && _p_spc[i][j] != 0)
			{
				//���ӽڵ����
				Node *one_node = platTopo->GetNode(j);
				one_node->add_btns(_p_spc[i][j]);
			}
		}
	}
	return 1;
}

int CTopoAnalysis::zero_array()
{
	//_p_spc[][]��������
	for (size_t i = 0; i < _size; ++i)
	{
		_hit[i] = -1;
		for (size_t j = 0; j < _size; ++j)
		{
			_p_spc[i][j] = 0;
		}
	}

	return 1;
}

int CTopoAnalysis::GetOneStartPathNum()
{
	int *shortPath;

	shortPath = new int[_size];
	shortPath[0] = 1;
	int allPathNum = 0;
	for (size_t i = 1;i<_size;i++)
	{
		shortPath[i] = 0;
		std::vector<size_t>& neigh = _p_list[i]._pre_id;
		int theShort = 1;
		for (std::vector<size_t>::iterator iter = neigh.begin();iter!=neigh.end();iter++)
		{
			size_t neighId = *iter;
			if (theShort > shortPath[neighId])
			{
				theShort = shortPath[neighId];
			}
		}
		for (std::vector<size_t>::iterator iter = neigh.begin();iter!=neigh.end();iter++)
		{
			size_t neighId = *iter;
			if (theShort == shortPath[neighId])
			{
				shortPath[i] +=shortPath[neighId];
			}
		}
		allPathNum += shortPath[i];
	}
	return allPathNum;
}
