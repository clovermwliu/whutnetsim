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

//更改人：李玉
//更改时间：2010-1-11
#include "PFP.h"

CPFP::CPFP( Count_t count,  
		   double  _sigma,
		   double  _p,double _q,
		   IPAddr_t i,     
		   SystemId_t id,
		   const Linkp2p& link)
:CPlatTopoBase(count,i,link,id),sigma(_sigma),p(_p),q(_q)
/*
描述：PFP网络的生成         
参数：[IN] count ：拓扑的节点数目
	  [IN] _sigma：范围[0,1]
	  [IN] _p    ：新增节点，连接方式选择时的概率
	  [IN] _q    ：范围[0,1-_p]
	  [IN] i     ：拓扑节点的基IP
	  [IN] id    ：分布式系统标识符 
	  [IN] link  ：拓扑的节点间的连接方式
返回值：空                                                                                       
备注： 
*/
{
	allChangeDegrees = 0;
}
bool CPFP::GenerateTopo()
/*
描述：生成拓扑        
参数：无                                                 
返回值：是否生成成功                                                                                       
备注：初始的5个节点为孤立节点，后面的节点以随机产生概率决定连接方法
*/
{
	int degree;
	first = Node::nextId;
	for (Count_t t = 0;t != nodeCount;t++)
	{
		Node* addNode;
		if (t < 5)                    //初始的5个节点为孤立节点
		{
			addNode = new Node();
			degree = 1; 
			degrees.push_back(degree);
            double  dealDegree = ChangeDegree(degree);
			changeDegrees.push_back(dealDegree);
			allChangeDegrees += dealDegree;
			continue;
		}
		
		double ranP = random();//根据概率选择连接方式
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

		//新增节点的度压入堆栈
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
描述：给新增的节点选择一个已有节点相连          
参数： [OUT]chooseid ：选择的相连节点的ID                                               
返回值：空                                                                                       
备注： 
*/
{
	double ranChooseId;      //选择节点的随机数
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
描述：给新增的节点选择两个个已有节点相连          
参数：[OUT]chooseid1 ：选择的第一个相连节点的ID          
	  [OUT]chooseid2 ：选择的第二个相连节点的ID  
返回值：空                                                                                       
备注： 
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
描述：给一个内部已有节点选择另外一个节点          
参数：[IN]ForTheId   ：需选择内部连接节点的ID
	  [OUT]chooseid  ：找到的内部ID 
返回值：空                                                                                       
备注： 新选的节点不是要选择节点的邻居节点
*/
{
	//将邻居节点的度去掉
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

	//选择节点

	do 
	{
		double ranChooseId;      //选择节点的随机数
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
描述：给一个内部已有节点选择另外两个节点          
参数：[IN]ForTheId   ：需选择内部连接节点的ID
	  [OUT]chooseid1 ：找到的第一个内部ID 
	  [OUT]chooseid2 ：找到的第二个内部ID 
返回值：空                                                                                       
备注： 新选的节点不是要选择节点的邻居节点
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
描述：将度转换为选择节点时对应的方式的值
参数：[IN]degree ：需转换的度               
返回：转换后的值                                                                  
备注：转换方法：pow(Ki,1 + sigma*log10(Ki))；
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
描述：通过绑定位置来给节点设置坐标
参数：[in]BoundBoxLeftDown   ——左下角的位置
      [in]BoundBoxRightUpper ——右上角的位置
	  [in]type               ——设置位置的类型
返回值：无
*/
{
	const NodeVec_t& nodes = Node::GetNodes();

	Meters_t  Xbound = BoundBoxRightUpper.X() - BoundBoxLeftDown.X();
	Meters_t  Ybound = BoundBoxRightUpper.Y() - BoundBoxLeftDown.Y();
	//随机生成位置
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
描述：          
参数：                                                 
返回值：空                                                                                       
备注： 
*/
{
	int retsultNode;
	retsultNode = 0;
	return retsultNode;
}
CPFP::~CPFP(void)
/*
描述：          
参数：                                                 
返回值：空                                                                                       
备注： 
*/
{
}
