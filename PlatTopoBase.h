#pragma once
#include <vector>
#include <set>
#include <map>
using namespace std;

#ifndef  __CPLATTOPOBASIC__H__ 
#define  __CPLATTOPOBASIC__H__ 
#include "node.h"
#include "common-defs.h"
#include "linkp2p.h"

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

 class CPlatTopoBase
{
public:
	CPlatTopoBase(Count_t,
				  IPAddr_t i= IPADDR_NONE,
				  const Linkp2p& = Linkp2p::Default(),
				  SystemId_t = 0);
	virtual ~CPlatTopoBase(void);
	typedef enum  {DEFAULT, CIRCLE, GRID, CIRCLES} BoxType;

public:
	virtual bool GenerateTopo() = 0;
	virtual void SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
										const Location& BoundBoxRightUpper,
										BoxType  type = DEFAULT) {};
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

protected:
	Node* GetNode(Count_t);    // Get node at specified level, index

private:
	pHook      hook;        //���ڹ��ӵĲ���
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