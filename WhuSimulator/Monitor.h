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
	Packet_t preW;//ǰһʱ�̵ĸ�����
	Packet_t nowW;//��ǰʱ�̵ĸ�����

	Time_t preT;//ǰһʱ�̵�ʱ��
	Time_t nowT;//��ǰʱ�̵�ʱ��

	double outresult;//��Ҫ����Ľ��

private:
	CWhuSimulatorApp* pMonitorApp;

	std::ofstream outfile;
	std::ofstream result;
	std::ofstream aveHop;
	std::ofstream aveTime;
	std::ofstream aveLoc;
};
#endif