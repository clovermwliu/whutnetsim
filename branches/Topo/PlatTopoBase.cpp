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
											  const Linkp2p& link)  //在两个指定的结点之间添加一条边
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
bool  CPlatTopoBase::SetHook(pHook f)  //设钩子
{
	hook = f;
	return true;
}
bool  CPlatTopoBase::CallHook()  //设钩子
{
	//hook=f;
	return true;
}
bool  CPlatTopoBase::DeleteHook()  //设钩子
{
	hook = 0;
	return true;
}
Node* CPlatTopoBase::Begin()          //指向第一个结点
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
Node* CPlatTopoBase::Next()           //指向下一个结点 
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
bool  CPlatTopoBase::End()           //是否到达末尾
{
	if (index==nodeCount)
		return false;                 //index指向需要读取数据的下一个位置
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