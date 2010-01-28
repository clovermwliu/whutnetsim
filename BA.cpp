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
//更改时间：2010-1-28
#include "BA.h"

//#include <qnamespace.h>
CBA::CBA(Count_t count,                                        
		 int onceAddEdge_m,                                    
		 IPAddr_t i,                                           
		 SystemId_t id,const Linkp2p& link)                    
:CPlatTopoBase(count,i,link,id),onceAddEdge(onceAddEdge_m)
/*
描述：平面拓扑CBA构造函数      
参数：[IN] count          ：拓扑的节点数目
	  [IN] onceAddEdge_m  ：每次添加的点与多少个点相连
      [IN] i              ：拓扑节点的基IP
      [IN] id             ：分布式系统标识符 
	  [IN] link           ：拓扑的节点间的连接方式
返回值：空                                                                                       
备注： 
*/
{
	allNodeDegrees = 0;
}

bool CBA::GenerateTopo()
/*
描述：生成拓扑        
参数：无                                                 
返回值：是否生成成功                                                                                       
备注：初始的onceAddEdge个节点为孤立节点，degree中保存的是每个节点的度+1
*/
{
	first = Node::nextId;
	for (Count_t i = 0; i < nodeCount; i++)
	{
		if (i <=onceAddEdge)                                   //如果网络中的节点数小于等于需要添加的边的个数,
		{                                                      //则新添加的节点与已有的所有的节点相连
			Node* addNode = new Node(sid); 
			addNode->SetIPAddr(ip++);

			degrees.push_back(1); 
			allNodeDegrees++;
			//                              
			//for (Count_t connect=0;connect<i;connect++)
			//{
			//	addNode->AddDuplexLink(Node::nodes[connect]);  //新添加的节点与已有的所有的节点相连
			//	degrees[connect]++; 
			//}
			//allNodeDegrees +=i*2;                               //allNodeDegrees总度数增加了新增边的两倍
		}
		else 
		{
			AddOneNode(i);                                      //如果网络中的节点数大于需要添加的边的个数,调用AddOneNode
			allNodeDegrees +=onceAddEdge+1;                     //allNodeDegrees总度数增加了新增边的两倍,新增了onceAddEdge条边
		}
	}
	SetLastError(SUCCESS_PLATTOPO);
	return true;
}

bool CBA::AddOneNode(Count_t nodeNum)
/*
描述：添加一个节点        
参数：[IN] nodeNum ：添加的第nodeNum个节点，可以不要，后来认为。                                                
返回：是否添加成功                                                                                    
备注：如果网络中的节点数大于需要添加的边的个数,调用AddOneNode
*/
{
	Node* addNode = new Node(sid);                              //新建一个节点           
    addNode->SetIPAddr(ip++);

	vector<Count_t> addEdge;                                    //用来记录为新增节点选择的连接节点
	vector<Count_t>::iterator iter;
	for (Count_t i = 0;i < onceAddEdge;)
	{
		int ran =  random(allNodeDegrees)+1;                      //产生一个随机数,[1,allNodeDegrees]
		int findthenode = 0;                                      //findthenode用来寻找满足条件的节点
		for (vector<Count_t>::size_type  findnum = 0;
			findnum != degrees.size(); findnum++)
		{
			findthenode += degrees[findnum];                    //从第一个节点开始,对度进行累加,直到大于产生的随机数ran
			if (findthenode >=ran)
			{
				iter = find(addEdge.begin(), addEdge.end(), findnum);//是否已经和这个节点相连

				if (iter == addEdge.end())                        //没有和这个节点相连,则
				{
					addEdge.push_back(findnum);                   //在addEdge中记录,表示新增节点已经和这个节点相连
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
	SetLastError(SUCCESS_PLATTOPO);
	return true;
}
void CBA::QuickSort(RecType& R, 
					NodeVec_t& changeOther,
					RecType::size_type s, 
					RecType::size_type t)
/*
描述：不要        
*/
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
/*
描述：通过绑定位置来给节点设置坐标
参数：[in]BoundBoxLeftDown   ——左下角的位置
      [in]BoundBoxRightUpper ——右上角的位置
	  [in]type               ——设置位置的类型
返回值：无
备注：
*/
{
	const NodeVec_t& nodes = Node::GetNodes();

	//按照度从小到大进行排序,从后往前访问,即由度从大到小开始访问
	multimap<int,Node*> imulti;
	for (NodeVec_t::size_type  copy = 0;copy !=nodeCount;copy++)
	{
		Node* newNode = nodes[first+copy];
		imulti.insert(make_pair(degrees[copy],newNode));   //pair<度,节点指针>
	}

	////随机生成位置
	//for (NodeVec_t::size_type nodeNum=0;nodeNum!=nodes.size();nodeNum++)
	//{
	//	int x =  random(10000);
	//	int y =  random(10000);
	//	nodes[nodeNum]->SetLocation(x,y);
	//}

	//按照度的大小生成
	Meters_t x0 = (BoundBoxRightUpper.X()+BoundBoxLeftDown.X())/2;   //第一个大圆的中心位置
	Meters_t y0 = (BoundBoxRightUpper.Y()+BoundBoxLeftDown.Y())/2;

	Meters_t x,y;
	miter begin,end;
	int count_node=0;
	for (begin = imulti.rbegin();begin != imulti.rend();begin++)
	{
		Node* node=begin->second;                                    //获得节点
		count_node++;
		if (node->HasLocation())
		{
		}
		else
		{
			int ranangle = random(360);
			x = x0 +r0*cos((M_PI/180)*ranangle);                    //没有位置则随机生成度,放在圆心为大圆圆心,半径为ro的圆上
			y = y0 +r0*sin((M_PI/180)*ranangle);
			node->SetLocation(x,y);
		}

		NodeWeightVec_t nwv;
		node->Neighbors(nwv);                                    //获得邻居节点数组
		Count_t  neighNum = nwv.size();
		for (Count_t  numsize = 0;numsize != nwv.size();numsize++)
		{
			NodeIfWeight& nodeNeigh = nwv[numsize];              //获得邻居节点
			if (nodeNeigh.node->HasLocation())                   //如果已经有位置,则继续处理下一个邻居              
				continue;

			int ranangle = random(360);                          //没有位置,则随机生成度,放在圆心为它node,
			r  rnode;                                            //半径可以调整,此时是如果前30%个节点半径为r1,后面为r2
			if(count_node < degrees.size()*0.3) rnode = r1;
			else rnode = r2;
			Meters_t   lx  = node->GetLocation().X() +rnode*cos((M_PI/180)*ranangle);
			Meters_t   ly  = node->GetLocation().Y() +rnode*sin((M_PI/180)*ranangle);

			nodeNeigh.node->SetLocation(Location(lx,ly));
		}
	}
	SetLastError(SUCCESS_PLATTOPO);
}
bool CBA::AddEdges(Count_t nodeNum)
{
	return true;
}
CBA::~CBA(void)
{

}
