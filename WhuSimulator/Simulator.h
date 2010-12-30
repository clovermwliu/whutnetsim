#pragma once

#ifndef __SIMULAROR_H__
#define __SIMULAROR_H__

#include <map>
#include "EventBase.h"
#include "ErrorHandler.h"
class CMonitor;

class CSimulator : public CErrorHandler
{
public:
	Error_str GetLastErrorEx(){return "hello";}
	CSimulator(void);
	~CSimulator(void);
	//void InitEventList();//初始化事件队列
	void Run();//运行
	//Time_t lastT;

	static void SetEndSimulator(bool t){endSimulator = t;}

	static Time_t Now(){return T;}//返回仿真事件的时间
	static Time_t T;//当前仿真时间

	static bool endSimulator;//结束仿真
private:
	CMonitor* monitor;
};
#endif