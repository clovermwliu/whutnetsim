#include "StdAfx.h"
#include "Simulator.h"
#include "TrafficStrategy.h"
#include "GNRanTrafficEvent.h"
#include "GNRanTrafficStrategy.h"
#include "Handler.h"
#include "Scheduler_map.h"
#include "Monitor.h"
#include <time.h>
//std::multimap<double, CEvent*> CSimulator::EventMap;
Time_t CSimulator::T = 0;
bool CSimulator::endSimulator = false;
CSimulator::CSimulator(void)
{
	monitor = new CMonitor();
}

CSimulator::~CSimulator(void)
{
	delete monitor;
	CSimulator::endSimulator = false;
}

void CSimulator::Run()
/*
描述：模型的运行
参数：无
返回：无
*/
{
	clock_t start,end;
	DEBUG0((GlobalVariable::out_debug_event.open("c:\\debug_event.txt")));
	DEBUG6((GlobalVariable::out_eventError.open("eventError.txt",std::ofstream::app)));
	//DEBUG4(GlobalVariable::out_eventlistSize<<CSimulator::Now()<<" "<<CScheduler_map::GetEventlistSize()<<endl);
	while (!CScheduler_map::EventListEmptyT()&&!endSimulator)
	{
		//DEBUG4(GlobalVariable::out_eventlistSize<<CScheduler_map::GetEventlistSize()<<endl);
#ifdef TIME_ALL
		start = clock();
#endif//end of #ifdef TIME_ALL
		CEventBase* e = CScheduler_map::GetTopEvent();//取得事件队列中的最顶端的事件
#ifdef TIME_ALL
		end = clock();
#endif//end of #ifdef TIME_ALL
		DEBUG4(GlobalVariable::eventTime+=(double)(end-start)/CLOCKS_PER_SEC);
		
		if (!e) break; //没有事件

		if (e->handler)
		{ 
			if (e->Time() < T) //error
			{
				DEBUG6(GlobalVariable::out_eventError << "error: Now:" << T 
					<< "  eventTime:" << e->Time()
					<<"   eventUID:" <<e->uid<<"  "
					<<" eventInfo:" << e->handler->GetHandleStatus() << endl);
			}
			//lastT = T;
			T = e->Time();//更新系统事件

			monitor->MonitorEntry(e);

			DEBUG0((GlobalVariable::out_debug_event <<T<<" "<<e->handler->GetHandleStatus()<<endl));
			e->handler->Handle(e, e->Time()); //执行事件
		}
		else
		{
			//cout << "Skipping event " << e->uid
			//	 << "  at time " << e->Time()
			//	 << ", no handler " << endl;
		}

	}

	DEBUG0(GlobalVariable::out_debug_event.close());
	DEBUG0(GlobalVariable::out_debug_event.clear());
	DEBUG6(GlobalVariable::out_eventError.clear());
	DEBUG6(GlobalVariable::out_eventError.close());
}