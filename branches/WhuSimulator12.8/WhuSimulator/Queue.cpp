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
		Packet* p = Deque(); //������
		delete p;
	}
}

double 
CQueue::GetNewNextPacketTime()
/*
������QUEUEPROCESS�¼���ΪSEND�¼�ʱ��SEND��ʱ�����
��������
���أ�SEND��ʱ��
*/
{
	if (CSimulator::Now() > waitTime)//����
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
���������¼��Ĵ�����
��������
���أ����е�ƽ������
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
		Packet* p = Deque(); //�� ����
		delete p;
	}
	assert(GetQueueLength()==0);
}
Packet* CQueue::Deque() //������
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
���������¼��Ĵ�����
��������
���أ����е�ƽ������
*/
{
	avg_length = (1-current_length_w)*avg_length + packets.size()*current_length_w;
	return avg_length;
}