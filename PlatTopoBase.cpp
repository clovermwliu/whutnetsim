#include "PlatTopoBase.h"
#include <time.h>

Count_t    CPlatTopoBase::index=0;
CPlatTopoBase::CPlatTopoBase(Count_t  n,
							 IPAddr_t i,
							 const Linkp2p& link1 ,
							 SystemId_t id1)
:nodeCount(n),topoattribute(nil),ip(i),link(link1),sid(id1)
{
	srand((int)time(0));
}

CPlatTopoBase::~CPlatTopoBase(void)
{
	delete topoattribute;
}
Node* CPlatTopoBase::GetNode(Count_t c)
{ // Get specified  node
	if (c >= nodeCount) return nil;
	return Node::GetNode(first + c);
}
bool  CPlatTopoBase::AddLinkBetweenTwoNodes(Count_t firstNodeId, 
											  Count_t secondNodeId, 
											  const Linkp2p& link)  //������ָ���Ľ��֮�����һ����
{
	if ((firstNodeId<nodeCount)&&(secondNodeId<nodeCount))
	{
		Node* firstNode=Node::nodes[firstNodeId];
		Node* secondNode=Node::nodes[secondNodeId];
		firstNode->AddDuplexLink(secondNode, link);
		return true;
	}
	return false;
}
bool  CPlatTopoBase::SetHook(pHook f)  //�蹳��
{
	hook = f;
	return true;
}
bool  CPlatTopoBase::CallHook()  //�蹳��
{
	//hook=f;
	return true;
}
bool  CPlatTopoBase::DeleteHook()  //�蹳��
{
	hook = 0;
	return true;
}
Node* CPlatTopoBase::Begin()          //ָ���һ�����
{
	if (nodeCount==0)
	{
		return NULL;
	}
	else
	{
		index=0;
		return  Node::nodes[index++];
	}
}
Node* CPlatTopoBase::Next()           //ָ����һ����� 
{
	if (index==nodeCount)
	{
		return NULL;
	}
	else
	{
		return  Node::nodes[index++];
	}
}
bool  CPlatTopoBase::End()           //�Ƿ񵽴�ĩβ
{
	if (index==nodeCount)
		return false;                 //indexָ����Ҫ��ȡ���ݵ���һ��λ��
	else
		return true;
}
int CPlatTopoBase::random(int MAX)
{
	//
	int i=rand()%MAX;

	//int i=srand((int)time(0))%MAX;
	return i;
}