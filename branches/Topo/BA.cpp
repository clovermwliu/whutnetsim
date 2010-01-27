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
//����ʱ�䣺2010-1-8
#include "BA.h"

//#include <qnamespace.h>
CBA::CBA(Count_t count,                                        //���ɵ����˵Ľڵ�ĸ���
		 int onceAddEdge_m,                                    //ÿ����ӵĵ�����ٸ�������
		 IPAddr_t i,                                           //�����нڵ㸳�Ļ�IP
		 SystemId_t id,const Linkp2p& link)                    //�ڵ��,��֮������ӷ�ʽ
		 :CPlatTopoBase(count,i,link,id),onceAddEdge(onceAddEdge_m)
{
	allNodeDegrees = 0;
}

bool CBA::GenerateTopo()
{
	first = Node::nextId;
	for (Count_t i = 0; i < nodeCount; i++)
	{
		if (i <=onceAddEdge)                                   //��������еĽڵ���С�ڵ�����Ҫ��ӵıߵĸ���,
		{                                                      //������ӵĽڵ������е����еĽڵ�����
			Node* addNode = new Node(sid); 
			addNode->SetIPAddr(ip++);

			degrees.push_back(1); 
			allNodeDegrees++;
			//                              
			//for (Count_t connect=0;connect<i;connect++)
			//{
			//	addNode->AddDuplexLink(Node::nodes[connect]);  //����ӵĽڵ������е����еĽڵ�����
			//	degrees[connect]++; 
			//}
			//allNodeDegrees +=i*2;                               //allNodeDegrees�ܶ��������������ߵ�����
		}
		else 
		{
			AddOneNode(i);                                      //��������еĽڵ���������Ҫ��ӵıߵĸ���,����AddOneNode
			allNodeDegrees +=onceAddEdge+1;                     //allNodeDegrees�ܶ��������������ߵ�����,������onceAddEdge����
		}
	}
	return true;
}

bool CBA::AddOneNode(Count_t nodeNum)
{
	Node* addNode = new Node(sid);                              //�½�һ���ڵ�           
    addNode->SetIPAddr(ip++);

	vector<Count_t> addEdge;                                    //������¼Ϊ�����ڵ�ѡ������ӽڵ�
	vector<Count_t>::iterator iter;
	for (Count_t i = 0;i < onceAddEdge;)
	{
		int ran =  random(allNodeDegrees)+1;                      //����һ�������,[1,allNodeDegrees]
		int findthenode = 0;                                      //findthenode����Ѱ�����������Ľڵ�
		for (vector<Count_t>::size_type  findnum = 0;
			findnum != degrees.size(); findnum++)
		{
			findthenode += degrees[findnum];                    //�ӵ�һ���ڵ㿪ʼ,�ԶȽ����ۼ�,ֱ�����ڲ����������ran
			if (findthenode >=ran)
			{
				iter = find(addEdge.begin(), addEdge.end(), findnum);//�Ƿ��Ѿ�������ڵ�����

				if (iter == addEdge.end())                        //û�к�����ڵ�����,��
				{
					addEdge.push_back(findnum);                   //��addEdge�м�¼,��ʾ�����ڵ��Ѿ�������ڵ�����
					Node *old=Node::nodes[first+findnum];
					degrees[findnum]++;
					allNodeDegrees++;
					addNode->AddDuplexLink(old,Linkp2p::Default());
					i++;
					break;
				}
			}
		}	
	}
	degrees.push_back(onceAddEdge+1);
	return true;
}
void CBA::QuickSort(RecType& R, 
					NodeVec_t& changeOther,
					RecType::size_type s, 
					RecType::size_type t)
{
	RecType::size_type i = s,j = t;
	iVeca tmp;
	iChangeVeca changeothertmp;
	if (s < t)
	{
		tmp = R[s];
		changeothertmp = changeOther[s];

		while (i != j)
		{
			while(j > i && R[j] < tmp)
				j--;
			if (i < j)
			{
				R[i] = R[j];
				changeOther[i] = changeOther[j];
				i++;
			}
			while (i < j && R[i] > tmp)
				i++;
			if (i < j)
			{
				R[j] = R[i];
				changeOther[j] = changeOther[i];
				j--;
			}

			if (i != s)
			{
				R[i] = tmp;
				changeOther[i] = changeothertmp;
			}
			if (i > s)
			{
				QuickSort(R,changeOther,s,i-1);
			}
			if (i < t)
			{
				QuickSort(R,changeOther,i+1,t);
			} 
		}
	}
}
void CBA::SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
								 const Location& BoundBoxRightUpper,
								 BoxType  type)
{
	const NodeVec_t& nodes = Node::GetNodes();

	//���նȴ�С�����������,�Ӻ���ǰ����,���ɶȴӴ�С��ʼ����
	multimap<int,Node*> imulti;
	for (NodeVec_t::size_type  copy = 0;copy !=nodeCount;copy++)
	{
		Node* newNode = nodes[first+copy];
		imulti.insert(make_pair(degrees[copy],newNode));   //pair<��,�ڵ�ָ��>
	}

	////�������λ��
	//for (NodeVec_t::size_type nodeNum=0;nodeNum!=nodes.size();nodeNum++)
	//{
	//	int x =  random(10000);
	//	int y =  random(10000);
	//	nodes[nodeNum]->SetLocation(x,y);
	//}

	//���նȵĴ�С����
	Meters_t x0 = (BoundBoxRightUpper.X()+BoundBoxLeftDown.X())/2;   //��һ����Բ������λ��
	Meters_t y0 = (BoundBoxRightUpper.Y()+BoundBoxLeftDown.Y())/2;

	Meters_t x,y;
	miter begin,end;
	int count_node=0;
	for (begin = imulti.rbegin();begin != imulti.rend();begin++)
	{
		Node* node=begin->second;                                    //��ýڵ�
		count_node++;
		if (node->HasLocation())
		{
		}
		else
		{
			int ranangle = random(360);
			x = x0 +r0*cos((M_PI/180)*ranangle);                    //û��λ����������ɶ�,����Բ��Ϊ��ԲԲ��,�뾶Ϊro��Բ��
			y = y0 +r0*sin((M_PI/180)*ranangle);
			node->SetLocation(x,y);
		}

		NodeWeightVec_t nwv;
		node->Neighbors(nwv);                                    //����ھӽڵ�����
		Count_t  neighNum = nwv.size();
		for (Count_t  numsize = 0;numsize != nwv.size();numsize++)
		{
			NodeIfWeight& nodeNeigh = nwv[numsize];              //����ھӽڵ�
			if (nodeNeigh.node->HasLocation())                   //����Ѿ���λ��,�����������һ���ھ�              
				continue;

			int ranangle = random(360);                          //û��λ��,��������ɶ�,����Բ��Ϊ��node,
			r  rnode;                                            //�뾶���Ե���,��ʱ�����ǰ30%���ڵ�뾶Ϊr1,����Ϊr2
			if(count_node < degrees.size()*0.3) rnode = r1;
			else rnode = r2;
			Meters_t   lx  = node->GetLocation().X() +rnode*cos((M_PI/180)*ranangle);
			Meters_t   ly  = node->GetLocation().Y() +rnode*sin((M_PI/180)*ranangle);

			nodeNeigh.node->SetLocation(Location(lx,ly));
		}
	}
}
bool CBA::AddEdges(Count_t nodeNum)
{
	return true;
}
CBA::~CBA(void)
{

}
