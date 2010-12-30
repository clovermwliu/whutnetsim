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

	//队列基本操作
	Packet* GetFront();
	Packet* Deque(); //包出队
	void Enque(Packet* p) { packets.push(p); }	//包入队
	size_t GetQueueLength() { return packets.size(); } //获得队列的长度

	//单位时间处理数据包的个数，即吞吐
	double GetThroughput(){return throughPut;} //获得吞吐量
	void SetThroughput(double t) { throughPut = t; }//设置吞吐量

	//算平均队列长度时，当前长度所占的权值
	double get_current_length_w() { return current_length_w; }
	void set_current_length_w(double w) { current_length_w = w; }

	double GetNewNextPacketTime(); //获得预约时间

	double GetAvgLength();
	double UpdateAvgLength();//更新平均队列长度并返回

	void Clear();

private:
	queue<Packet*> packets;
	Time_t waitTime;//已经排队的包的最后时间
	double throughPut;//吞吐
	double current_length_w;//算平均队列长度时，当前长度所占的权值
	double avg_length;//平均队列长度，不断更新中
};
#endif