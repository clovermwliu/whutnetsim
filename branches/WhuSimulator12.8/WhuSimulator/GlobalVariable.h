#pragma once

#include <afx.h>
#include <string>
#include <fstream>
#include <map>

// error [7/20/2010 mpiuser]

//总
#ifndef DEBUG_MASK
#define DEBUG_MASK 1
#endif

#define DBM0 0
#define DEBUG0(x) if(1&DBM0) x //debug事件队列

#define DBM1 1
#define DEBUG1(x) if(1&DBM1) x //monitor

#define DBM2 0
#define DEBUG2(x) if(1&DBM2) x //每拍产生的中间结果

#define DBM3 0
#define DEBUG3(x) if(1&DBM3) x //内存检查

#define DBM4 1
#define DEBUG4(x) if(1&DBM4) x //插入事件、路由、队列等计时，一次实验总计时见DEBUG7
#define TIME_ALL //插入事件、路由、队列等计时

#define DBM5 0
#define DEBUG5(x) if(1&DBM5) x //事件map的大小

#define DBM6 0
#define DEBUG6(x) if(1&DBM6) x //CSimulator::Run中取事件的查错

#define DBM7 1
#define DEBUG7(x) if(1&DBM7) x //一次实验总计时

#define DBM8 0
#define DEBUG8(x) if(1&DBM8) x //事件队列查错


//事件队列数据结构选择
//#define EventMap //用一个map来存储event
#define EventMapVector //用一个map 两个vector来存储event


class GlobalVariable
{
public:
	static unsigned long long eventsAllocated;//系统中事件的数量
	static unsigned long long packetAllocated;//系统中分配的包的数量
	static unsigned long long processNum;//系统中在链路上的包的数量
	static  double packetTransitTimes;//包的传输时间之和
	static unsigned long long packetDoneNums;// 传输到的包的数量
	static unsigned long long packetHops;// 包的传输的跳数
	static std::map<int,int> putLocAnaly;

	//监测的一个变量输出
	static	std::ofstream outfile;
	//static	std::ofstream loaderW; // 输出负载
	//static	std::ofstream aveHop;  
	//static	std::ofstream aveTime;
	//static	std::ofstream aveLoc;

	//static void OutLoaderW();
	static void OutPutAveHop(std::ofstream& outPut);
	static void OutPutLocAnaly(std::ofstream& outPut);
	static void OutPutAveTime(std::ofstream& outPut);


	static double enqueTime;//统计进队时间
	static double allTime;//统计出队时间
	static double routeTime;//统计路由计算时间
	static double eventTime;//统计事件插入事件时间

	static std::ofstream out_debug_event;
	static std::ofstream out_debug_packet;
	static std::ofstream out_new;

	static std::ofstream out_eventlistSize;
	static std::ofstream out_time;//事件构造时间及触发时间

	static std::ofstream out_eventMapSize;

	static std::ofstream out_eventError;//CSimulator::Run中取事件的查错
	static std::ofstream out_test_time;//一次实验总计时

	static std::ofstream out_loc;

	//static std::string  strTmp;
	static unsigned long long allEvents;//系统中事件的数量
};
