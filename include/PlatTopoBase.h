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
//更改时间：2010-1-4

#pragma once
#include <vector>
#include <set>
#include <map>
using namespace std;

#ifndef  __CPLATTOPOBASIC__H__ 
#define  __CPLATTOPOBASIC__H__ 
#include "node.h"
#include "G_common_defs.h"
#include "linkp2p.h"
#include "ErrorHandler.h"

#define  SUCCESS_PLATTOPO                              ERROR_NO_ERROR
#define  ERROR_ID_OUT_OF_NODECOUNT_FAIL                0x00050101 //要找的节点的ID超出了nodecount的范围

class TopoMask;
typedef bool (* pHook)(const Node*, Count_t argc, char **argv);//对接点分析和处理，然后将处理完的结果放在argv中

class TopoAnalysisAttribute 
{
public:
	TopoAnalysisAttribute():btchange(true),Cluster(0){}
	~TopoAnalysisAttribute(void){}
	bool         btchange;//true,分析拓扑的属性已经修改
	double       Cluster;//聚类系数
	map<int,int> distribution;//度分布
};
struct ConnectInfo 
{
	Count_t ItselfId;//本拓扑作为相连的节点的ID
	Count_t lay;    //相连的拓扑属于哪一层
	Count_t topoNum;//相连的那一层的拓扑Id
	Count_t nodeNum;//相连的那一层的拓扑中的Id号
};
class CPlatTopoBase : public CErrorHandler
{
public:
	CPlatTopoBase(Count_t,
				  IPAddr_t i= IPADDR_NONE,
				  const Linkp2p& = Linkp2p::Default(),
				  SystemId_t = 0);
	virtual ~CPlatTopoBase(void);
	typedef enum  {DEFAULT, CIRCLE, GRID, CIRCLES} BoxType;

public:
	Error_str GetLastErrorEx(){return "hello";}
	virtual bool GenerateTopo() = 0;
	virtual void SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
										const Location& BoundBoxRightUpper,
										BoxType  type = DEFAULT) {}
										//在这个函数里面会对拓扑中的结点确定位置

	virtual void StarSetLocationViaBoundBox(const Location&, const Location&,
											Angle_t = 0, Angle_t = 2 * M_PI){}
	virtual Node*  GetHub(){return nil;}                 // Get the hub node
	virtual Node*  GetLeaf(Count_t c){return nil;}

	Count_t NodeCount() { return nodeCount;}
	//bool OutputTopoInfo(vector<string> &outdata,
	//				    TopoMask mask,
	//				    Count_t start = 1,                                              //返回一个方法
	//				    Count_t end = nodeCount);//构造输出数据的格式给string,然后输出.
   
	bool OutputTopoInfoForSpecificNodes(vector<string> &outdata,
							            set<Count_t>& specNodes,
							            TopoMask mask);//构造输出数据的格式给string,然后输出.
	
	bool InputTopoInfo (const vector<string> &indata, TopoMask mask);//获得输入的数据

	//对已有拓扑增加结点
	//Link*  AddLinkBetweenTwoNodes(Count_t firstNodeId, Count_t secondNodeId);  //在两个指定的结点之间添加一条边

	bool  AddLinkBetweenTwoNodes(Count_t firstNodeId, 
								  Count_t secondNodeId, 
								  const Linkp2p& = Linkp2p::Default());  //在两个指定的结点之间添加一条边
	Link*  AddLinksFromOneSrcnode(Count_t , set<int>&);//在一个结点和其它结点之间均添加一条边
	
	Node*  AddNewNodeAndLinkToDstNode(Count_t dst_node);//增加一个结点并和linknode结点相连
    Node*  AddNewNodeAndLinkToRangedNodes(Count_t start,Count_t end);//增加一个结点并和指定范围内的结点相连
	Node*  AddNewNodeAndLinkToSpecficNodes(set<Count_t>&);//增加一个结点并和指定的很多结点相连
	

	//对已有拓扑删除边
	bool  RemoveLinkBetweenTwoNodes(Count_t ,Count_t, bool dele = false);//在两个指定的结点之间删除一条边,如没有就不删,dele为true是物理删,false是逻辑删
	
	//删除一个结点和set中的每一个结点的边,如没有就不删，dele为true是物理删,false是逻辑删，返回删除的边的条数
	int  RemoveLinkFromOneNodeAndTheSpecficNode(Count_t deleNode, 
												set<int>& specNodes,
												bool dele=false);


	//删除点                                 第一次实现真删
	int RemoveOneNode(Count_t , bool dele = false);
	
	//
	bool  SetHook(pHook f);  //设钩子
	bool  CallHook();        //调用钩子函数
	bool  DeleteHook();      //删除钩子

	Node* Begin();          //指向第一个结点
	Node* Next();           //指向下一个结点 
	bool  End();            //是否到达末尾

	int random(int Max);	//这个需要一个random类
	double random();        //产生一个0到1之间的随机数

//protected:
	Node* GetNode(Count_t);    // Get node at specified level, index


	NodeId_t GetDefaultRouteId() {return first;}//获得默认路由

public:
	vector<ConnectInfo>  brotherConnect; //同一层的拓扑之间的连接
	vector<ConnectInfo>  routerConnect;  //与路由的连接，即控制这一层的路由
	vector<ConnectInfo>  controlConnect; //这一层可以控制的下一层的拓扑
private:
	pHook      hook;        //关于钩子的操作
	NodeId_t   defaultroute;

protected:	
	NodeId_t   first;       // 第一个叶子结点,即第一个主机结点
	NodeId_t   last;        // 最后一个主机结点
	Count_t    nodeCount;   // 网络主机结点的数目、、此处有问题 
	Count_t    edgeCount;
	//NodeVec_t  nodes;	    // 存储所有主机结点的向量
    

	IPAddr_t ip;
	const Linkp2p&  link;
	SystemId_t sid;
public:
	TopoAnalysisAttribute* topoattribute;//第一次保存拓扑分析参数的时候初始化,用来保存拓扑的一些分析参数

	static Count_t    index;//想在begin，end的时候用
};
#endif