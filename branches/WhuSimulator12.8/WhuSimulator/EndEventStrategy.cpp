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

	//����¼�����
	CScheduler_map::Clearlist();
	//CScheduler_map::ResetUid();
	//��ʼ������ʱ��
	CSimulator::T = 0;

	//����ڵ��ϵİ�
	CWhuSimulatorApp* pRunApp;
	pRunApp = (CWhuSimulatorApp*)AfxGetApp();

	pRunApp->graph->clear(2);
	

	GlobalVariable::packetDoneNums = 0;
	GlobalVariable::packetTransitTimes = 0;
	GlobalVariable::packetHops = 0;
	GlobalVariable::putLocAnaly.clear();
	delete e;
}