#pragma once

#ifndef __MONITOR_H__
#define __MONITOR_H__

#include "G_common_defs.h"
#include <fstream>

class CEventBase;
class CWhuSimulatorApp;
class CMonitor
{
public:
	CMonitor(void);
	~CMonitor(void);
	void MonitorEntry(CEventBase* e);
	void MonitorLoaderW(CEventBase* e);
	void MonitorT(CEventBase* e);
	void MonitorAverageTrasitTime(CEventBase* e);
	void MonitorAverageTrasitHop(CEventBase* e);
	void MonitorPacketDoneNums(CEventBase* e);
	void MonitorPutLoc(CEventBase* e);
private:
	Packet_t preW;//前一时刻的负载量
	Packet_t nowW;//当前时刻的负载量

	Time_t preT;//前一时刻的时间
	Time_t nowT;//当前时刻的时间

	double outresult;//需要输出的结果

private:
	CWhuSimulatorApp* pMonitorApp;

	std::ofstream outfile;
	std::ofstream result;
	std::ofstream aveHop;
	std::ofstream aveTime;
	std::ofstream aveLoc;
};
#endif