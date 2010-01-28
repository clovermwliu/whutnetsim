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
//����ʱ�䣺2010-1-11
#include "PFP.h"

CPFP::CPFP( Count_t count,  
		   double  _sigma,
		   double  _p,double _q,
		   IPAddr_t i,     
		   SystemId_t id,
		   const Linkp2p& link)
:CPlatTopoBase(count,i,link,id),sigma(_sigma),p(_p),q(_q)
/*
������PFP���������         
������[IN] count �����˵Ľڵ���Ŀ
	  [IN] _sigma����Χ[0,1]
	  [IN] _p    �������ڵ㣬���ӷ�ʽѡ��ʱ�ĸ���
	  [IN] _q    ����Χ[0,1-_p]
	  [IN] i     �����˽ڵ�Ļ�IP
	  [IN] id    ���ֲ�ʽϵͳ��ʶ�� 
	  [IN] link  �����˵Ľڵ������ӷ�ʽ
����ֵ����                                                                                       
��ע�� 
*/
{
	allChangeDegrees = 0;
}
bool CPFP::GenerateTopo()
/*
��������������        
��������                                                 
����ֵ���Ƿ����ɳɹ�                                                                                       
��ע����ʼ��5���ڵ�Ϊ�����ڵ㣬����Ľڵ�������������ʾ������ӷ���
*/
{
	int degree;
	first = Node::nextId;
	for (Count_t t = 0;t != nodeCount;t++)
	{
		Node* addNode;
		if (t < 5)                    //��ʼ��5���ڵ�Ϊ�����ڵ�
		{
			addNode = new Node();
			degree = 1; 
			degrees.push_back(degree);
            double  dealDegree = ChangeDegree(degree);
			changeDegrees.push_back(dealDegree);
			allChangeDegrees += dealDegree;
			continue;
		}
		
		double ranP = random();//���ݸ���ѡ�����ӷ�ʽ
		if (ranP < p)
		{
			NodeId_t chooseid;
			ChooseOneIdOuter(chooseid);
			addNode = new Node();
			degrees.push_back(0);
			changeDegrees.push_back(0);
			addNode->AddDuplexLink(Node::nodes[first + chooseid],link);

			NodeId_t innerchooseid;
			ChooseOneIdInner(innerchooseid,chooseid);
			Node::nodes[first + chooseid]->AddDuplexLink(Node::nodes[first + innerchooseid],link);
			
			degrees[chooseid]+=2;
			degrees[innerchooseid]++;

			degree = 1;
		}
		else if (ranP <(p+q))
		{
			NodeId_t chooseid;
			ChooseOneIdOuter(chooseid);
			addNode = new Node();
			degrees.push_back(0);
			changeDegrees.push_back(0);
			addNode->AddDuplexLink(Node::nodes[first + chooseid],link);
			

			NodeId_t innerchooseid1,innerchooseid2;
			ChooseTwoIdInner(innerchooseid1,innerchooseid2,chooseid);
			Node::nodes[first + chooseid]->AddDuplexLink(Node::nodes[first + innerchooseid1],link);
			Node::nodes[first + chooseid]->AddDuplexLink(Node::nodes[first + innerchooseid2],link);
           
			degrees[chooseid]+=3;
			degrees[innerchooseid1]++;
			degrees[innerchooseid2]++;

			degree = 1;
		}
		else
		{
			NodeId_t chooseid1,chooseid2;
			ChooseTwoIdOuter(chooseid1,chooseid2);
			addNode = new Node();
			degrees.push_back(0);
			changeDegrees.push_back(0);
			addNode->AddDuplexLink(Node::nodes[first + chooseid1],link);
			addNode->AddDuplexLink(Node::nodes[first + chooseid2],link);
		
			NodeId_t innerchooseid1;
			ChooseOneIdInner(innerchooseid1,chooseid1);
			Node::nodes[first + chooseid1]->AddDuplexLink(Node::nodes[first + innerchooseid1],link);
			
			degrees[chooseid1]+=2;
			degrees[chooseid2]++;
			degrees[innerchooseid1]++;
			
			degree = 2;
		}
		addNode->SetIPAddr(ip++);

		//�����ڵ�Ķ�ѹ���ջ
		degrees[t]=degree;
		double  dealDegree = ChangeDegree(degree);
		changeDegrees[t] = dealDegree;
		allChangeDegrees += dealDegree;
	}
	SetLastError(SUCCESS_PLATTOPO);
	return true;
}
void  CPFP::ChooseOneIdOuter(NodeId_t &chooseid)
/*
�������������Ľڵ�ѡ��һ�����нڵ�����          
������ [OUT]chooseid ��ѡ��������ڵ��ID                                               
����ֵ����                                                                                       
��ע�� 
*/
{
	double ranChooseId;      //ѡ��ڵ�������
	ranChooseId = random();
	ranChooseId *= allChangeDegrees;	

	double allChooseId; 
	allChooseId = 0;
	vector<double>::size_type i;
	for (i = 0;i!=changeDegrees.size();i++)
	{
		allChooseId += changeDegrees[i];
		if (ranChooseId <= allChooseId)
			break;
	}
	chooseid = i;
}
void  CPFP::ChooseTwoIdOuter(NodeId_t &chooseid1,NodeId_t &chooseid2)
/*
�������������Ľڵ�ѡ�����������нڵ�����          
������[OUT]chooseid1 ��ѡ��ĵ�һ�������ڵ��ID          
	  [OUT]chooseid2 ��ѡ��ĵڶ��������ڵ��ID  
����ֵ����                                                                                       
��ע�� 
*/
{
	ChooseOneIdOuter(chooseid1);
	ChooseOneIdOuter(chooseid2);
	while (chooseid1 == chooseid2)
	{
         ChooseOneIdOuter(chooseid2);
	}
}
void CPFP::ChooseOneIdInner(NodeId_t &chooseid,NodeId_t ForTheId)
/*
��������һ���ڲ����нڵ�ѡ������һ���ڵ�          
������[IN]ForTheId   ����ѡ���ڲ����ӽڵ��ID
	  [OUT]chooseid  ���ҵ����ڲ�ID 
����ֵ����                                                                                       
��ע�� ��ѡ�Ľڵ㲻��Ҫѡ��ڵ���ھӽڵ�
*/
{
	//���ھӽڵ�Ķ�ȥ��
	double  cpAllChangeDegreesc = allChangeDegrees;
	
	NodeWeightVec_t nwv; 
	vector<Count_t> neighsId;
	Node::nodes[first + ForTheId]->Neighbors(nwv);
	Count_t  neighNum = nwv.size(); 
	for (Count_t  numsize = 0;numsize != nwv.size();numsize++)
	{
		NodeIfWeight& nodeNeigh = nwv[numsize];
		Count_t  NodeId=nodeNeigh.node->Id();
		neighsId.push_back(NodeId-first);
		cpAllChangeDegreesc -= changeDegrees[NodeId-first];
	}

	//ѡ��ڵ�

	do 
	{
		double ranChooseId;      //ѡ��ڵ�������
		ranChooseId = random();
		ranChooseId *= allChangeDegrees;	

		double allChooseId; 
		allChooseId = 0;
		vector<double>::size_type i;
		for (i = 0;i!=changeDegrees.size();i++)
		{
			allChooseId += changeDegrees[i];
			vector<Count_t>::iterator iter;
			iter = find(neighsId.begin(),neighsId.end(),(Count_t)i);
			if (iter!=neighsId.end())
				continue;
			if (ranChooseId <= allChooseId)
				break;
		}
		chooseid = i;
	} while(chooseid==changeDegrees.size());
	
}
void  CPFP::ChooseTwoIdInner(NodeId_t &chooseid1,NodeId_t &chooseid2,NodeId_t ForTheId)
/*
��������һ���ڲ����нڵ�ѡ�����������ڵ�          
������[IN]ForTheId   ����ѡ���ڲ����ӽڵ��ID
	  [OUT]chooseid1 ���ҵ��ĵ�һ���ڲ�ID 
	  [OUT]chooseid2 ���ҵ��ĵڶ����ڲ�ID 
����ֵ����                                                                                       
��ע�� ��ѡ�Ľڵ㲻��Ҫѡ��ڵ���ھӽڵ�
*/
{
	ChooseOneIdInner(chooseid1,ForTheId);
	ChooseOneIdInner(chooseid2,ForTheId);
	while (chooseid1 == chooseid2)
	{
		ChooseOneIdInner(chooseid2,ForTheId);
	}
}
double  CPFP::ChangeDegree(int degree)
/*
����������ת��Ϊѡ��ڵ�ʱ��Ӧ�ķ�ʽ��ֵ
������[IN]degree ����ת���Ķ�               
���أ�ת�����ֵ                                                                  
��ע��ת��������pow(Ki,1 + sigma*log10(Ki))��
*/
{
	double frac = 1 + sigma*log10((double)degree);
	double result = pow((double)degree,frac);
	return result;
}
void CPFP::SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
							      const Location& BoundBoxRightUpper,
							      BoxType  type)
/*
������ͨ����λ�������ڵ���������
������[in]BoundBoxLeftDown   �������½ǵ�λ��
      [in]BoundBoxRightUpper �������Ͻǵ�λ��
	  [in]type               ��������λ�õ�����
����ֵ����
*/
{
	const NodeVec_t& nodes = Node::GetNodes();

	Meters_t  Xbound = BoundBoxRightUpper.X() - BoundBoxLeftDown.X();
	Meters_t  Ybound = BoundBoxRightUpper.Y() - BoundBoxLeftDown.Y();
	//�������λ��
	for (NodeId_t nodeNum=0;nodeNum!=nodeCount;nodeNum++)
	{
		int x =  random(Xbound);
		int y =  random(Ybound);
		nodes[first + nodeNum]->SetLocation(BoundBoxLeftDown.X()+x,BoundBoxLeftDown.Y()+y);
	}
	SetLastError(SUCCESS_PLATTOPO);
}
int  CPFP::ChooseNode()
/*
������          
������                                                 
����ֵ����                                                                                       
��ע�� 
*/
{
	int retsultNode;
	retsultNode = 0;
	return retsultNode;
}
CPFP::~CPFP(void)
/*
������          
������                                                 
����ֵ����                                                                                       
��ע�� 
*/
{
}
