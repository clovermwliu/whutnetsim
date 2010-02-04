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
//����ʱ�䣺2010-1-4

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
#define  ERROR_ID_OUT_OF_NODECOUNT_FAIL                0x00050101 //Ҫ�ҵĽڵ��ID������nodecount�ķ�Χ

class TopoMask;
typedef bool (* pHook)(const Node*, Count_t argc, char **argv);//�Խӵ�����ʹ���Ȼ�󽫴�����Ľ������argv��

class TopoAnalysisAttribute 
{
public:
	TopoAnalysisAttribute():btchange(true),Cluster(0){}
	~TopoAnalysisAttribute(void){}
	bool         btchange;//true,�������˵������Ѿ��޸�
	double       Cluster;//����ϵ��
	map<int,int> distribution;//�ȷֲ�
};
struct ConnectInfo 
{
	Count_t ItselfId;//��������Ϊ�����Ľڵ��ID
	Count_t lay;    //����������������һ��
	Count_t topoNum;//��������һ�������Id
	Count_t nodeNum;//��������һ��������е�Id��
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
										//��������������������еĽ��ȷ��λ��

	virtual void StarSetLocationViaBoundBox(const Location&, const Location&,
											Angle_t = 0, Angle_t = 2 * M_PI){}
	virtual Node*  GetHub(){return nil;}                 // Get the hub node
	virtual Node*  GetLeaf(Count_t c){return nil;}

	Count_t NodeCount() { return nodeCount;}
	//bool OutputTopoInfo(vector<string> &outdata,
	//				    TopoMask mask,
	//				    Count_t start = 1,                                              //����һ������
	//				    Count_t end = nodeCount);//����������ݵĸ�ʽ��string,Ȼ�����.
   
	bool OutputTopoInfoForSpecificNodes(vector<string> &outdata,
							            set<Count_t>& specNodes,
							            TopoMask mask);//����������ݵĸ�ʽ��string,Ȼ�����.
	
	bool InputTopoInfo (const vector<string> &indata, TopoMask mask);//������������

	//�������������ӽ��
	//Link*  AddLinkBetweenTwoNodes(Count_t firstNodeId, Count_t secondNodeId);  //������ָ���Ľ��֮�����һ����

	bool  AddLinkBetweenTwoNodes(Count_t firstNodeId, 
								  Count_t secondNodeId, 
								  const Linkp2p& = Linkp2p::Default());  //������ָ���Ľ��֮�����һ����
	Link*  AddLinksFromOneSrcnode(Count_t , set<int>&);//��һ�������������֮������һ����
	
	Node*  AddNewNodeAndLinkToDstNode(Count_t dst_node);//����һ����㲢��linknode�������
    Node*  AddNewNodeAndLinkToRangedNodes(Count_t start,Count_t end);//����һ����㲢��ָ����Χ�ڵĽ������
	Node*  AddNewNodeAndLinkToSpecficNodes(set<Count_t>&);//����һ����㲢��ָ���ĺܶ�������
	

	//����������ɾ����
	bool  RemoveLinkBetweenTwoNodes(Count_t ,Count_t, bool dele = false);//������ָ���Ľ��֮��ɾ��һ����,��û�оͲ�ɾ,deleΪtrue������ɾ,false���߼�ɾ
	
	//ɾ��һ������set�е�ÿһ�����ı�,��û�оͲ�ɾ��deleΪtrue������ɾ,false���߼�ɾ������ɾ���ıߵ�����
	int  RemoveLinkFromOneNodeAndTheSpecficNode(Count_t deleNode, 
												set<int>& specNodes,
												bool dele=false);


	//ɾ����                                 ��һ��ʵ����ɾ
	int RemoveOneNode(Count_t , bool dele = false);
	
	//
	bool  SetHook(pHook f);  //�蹳��
	bool  CallHook();        //���ù��Ӻ���
	bool  DeleteHook();      //ɾ������

	Node* Begin();          //ָ���һ�����
	Node* Next();           //ָ����һ����� 
	bool  End();            //�Ƿ񵽴�ĩβ

	int random(int Max);	//�����Ҫһ��random��
	double random();        //����һ��0��1֮��������

//protected:
	Node* GetNode(Count_t);    // Get node at specified level, index


	NodeId_t GetDefaultRouteId() {return first;}//���Ĭ��·��

public:
	vector<ConnectInfo>  brotherConnect; //ͬһ�������֮�������
	vector<ConnectInfo>  routerConnect;  //��·�ɵ����ӣ���������һ���·��
	vector<ConnectInfo>  controlConnect; //��һ����Կ��Ƶ���һ�������
private:
	pHook      hook;        //���ڹ��ӵĲ���
	NodeId_t   defaultroute;

protected:	
	NodeId_t   first;       // ��һ��Ҷ�ӽ��,����һ���������
	NodeId_t   last;        // ���һ���������
	Count_t    nodeCount;   // ��������������Ŀ�����˴������� 
	Count_t    edgeCount;
	//NodeVec_t  nodes;	    // �洢����������������
    

	IPAddr_t ip;
	const Linkp2p&  link;
	SystemId_t sid;
public:
	TopoAnalysisAttribute* topoattribute;//��һ�α������˷���������ʱ���ʼ��,�����������˵�һЩ��������

	static Count_t    index;//����begin��end��ʱ����
};
#endif