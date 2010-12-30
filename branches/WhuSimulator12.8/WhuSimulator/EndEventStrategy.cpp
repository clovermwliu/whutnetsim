#include "StdAfx.h"
#include "EndEventStrategy.h"
#include "Simulator.h"
#include "Scheduler_map.h"
#include "WhuSimulator.h"

CEndEventStrategy::CEndEventStrategy(void)
{
}

CEndEventStrategy::~CEndEventStrategy(void)
{

}

void CEndEventStrategy::Handle(CEventBase* e, Time_t t)
{
	CSimulator::SetEndSimulator(true);

	//清除事件队列
	CScheduler_map::Clearlist();
	//CScheduler_map::ResetUid();
	//初始化仿真时间
	CSimulator::T = 0;

	//清除节点上的包
	CWhuSimulatorApp* pRunApp;
	pRunApp = (CWhuSimulatorApp*)AfxGetApp();

	pRunApp->graph->clear(2);
	

	GlobalVariable::packetDoneNums = 0;
	GlobalVariable::packetTransitTimes = 0;
	GlobalVariable::packetHops = 0;
	GlobalVariable::putLocAnaly.clear();
	delete e;
}