#pragma once

#ifndef __QUEUE_H__
#define __QUEUE_H__

//#include <vector>
#include <queue>
using namespace std;

#include "Packet.h"
#include "G_common_defs.h"
#include "Simulator.h"

class CQueue
{
public:
	CQueue(void);
	~CQueue(void);

	//���л�������
	Packet* GetFront();
	Packet* Deque(); //������
	void Enque(Packet* p) { packets.push(p); }	//�����
	size_t GetQueueLength() { return packets.size(); } //��ö��еĳ���

	//��λʱ�䴦�����ݰ��ĸ�����������
	double GetThroughput(){return throughPut;} //���������
	void SetThroughput(double t) { throughPut = t; }//����������

	//��ƽ�����г���ʱ����ǰ������ռ��Ȩֵ
	double get_current_length_w() { return current_length_w; }
	void set_current_length_w(double w) { current_length_w = w; }

	double GetNewNextPacketTime(); //���ԤԼʱ��

	double GetAvgLength();
	double UpdateAvgLength();//����ƽ�����г��Ȳ�����

	void Clear();

private:
	queue<Packet*> packets;
	Time_t waitTime;//�Ѿ��Ŷӵİ������ʱ��
	double throughPut;//����
	double current_length_w;//��ƽ�����г���ʱ����ǰ������ռ��Ȩֵ
	double avg_length;//ƽ�����г��ȣ����ϸ�����
};
#endif