#include "StdAfx.h"
#include "Queue.h"
#include <assert.h>


CQueue::CQueue(void):waitTime(0), current_length_w(0.7), avg_length(0),throughPut(4)
{ 
	DEBUG3(GlobalVariable::out_new<<"New Queue"<<endl);
}
CQueue::~CQueue(void)
{
	while (!packets.empty())
	{
		Packet* p = Deque(); //包出队
		delete p;
	}
}

double 
CQueue::GetNewNextPacketTime()
/*
描述：QUEUEPROCESS事件变为SEND事件时，SEND的时间计算
参数：无
返回：SEND的时间
*/
{
	if (CSimulator::Now() > waitTime)//更新
	{
		waitTime = CSimulator::Now();
	}
	ASSERT(throughPut);
	waitTime += 1/throughPut;
	return waitTime;
}

double 
CQueue::GetAvgLength()
/*
描述：包事件的处理方法
参数：无
返回：队列的平均长度
*/
{
	return avg_length;
}
void CQueue::Clear()
{
	waitTime = 0;
	//current_length_w = 0.7;
	avg_length = 0;
	//throughPut = 4;

	while (!packets.empty())
	{
		Packet* p = Deque(); //包 出队
		delete p;
	}
	assert(GetQueueLength()==0);
}
Packet* CQueue::Deque() //包出队
{ 
	Packet *p = packets.front(); 
	packets.pop();
	return p;
}
Packet* CQueue::GetFront()
{
	Packet *p = packets.front(); 
	return p;
}
double 
CQueue::UpdateAvgLength()
/*
描述：包事件的处理方法
参数：无
返回：队列的平均长度
*/
{
	avg_length = (1-current_length_w)*avg_length + packets.size()*current_length_w;
	return avg_length;
}