#include "StdAfx.h"
#include "GNRanTrafficStrategy.h"
#include "Random.h"
#include "PacketEvent.h"
#include "WhuSimulator.h"
#include "GNRanTrafficEvent.h"
#include "Simulator.h"
#include "Scheduler_map.h"

////int ntime = 0;
// error [7/20/2010 mpiuser]
//CGNRanTrafficStrategy::CGNRanTrafficStrategy(void)
//{
//}

CGNRanTrafficStrategy::~CGNRanTrafficStrategy(void)
{
}
void CGNRanTrafficStrategy::Handle(CEventBase* e, Time_t t)
/*
描述：随机指定源目的，每拍产生R个流量事件的处理方法
参数：[IN] e   ：要处理的时间
	  [IN] t   ：处理事件的时间
返回：队列的第一个元素                                                                                       
备注：获得队列头元素后，同时把事件从队列中删除
*/
{
	//取R个
	CWhuSimulatorApp* pTrApp = (CWhuSimulatorApp*)AfxGetApp();
	CGNRanTrafficEvent* GNevent = (CGNRanTrafficEvent*)e;

	clock_t start,end;
	for (size_t pNum = 0;pNum != GNevent->packetNumR; ++pNum)
	{
		//源节点、目的节点随机指定，产生包
		NodeId_t src = CRandom::Random_U(0,pTrApp->graph->get_node_num()-1);//关怡萍
        //NodeId_t src = 0;
		//NodeId_t des = 3;
		NodeId_t des;
		do 
		{
			des = CRandom::Random_U(0,pTrApp->graph->get_node_num()-1);//关怡萍
		} while (src==des);
		
		CNode* srcNode = pTrApp->graph->get_node(src);

		//产生包事件
		Packet* p = new Packet(src,des);
		p->SetStartTime(CSimulator::Now());		
#ifdef TIME_ALL
		start = clock();
#endif//end of #ifdef TIME_ALL
		srcNode->PacketEnQueue(p);//包入队
#ifdef TIME_ALL
		end = clock();
#endif//end of #ifdef TIME_ALL
		DEBUG4(GlobalVariable::enqueTime+=(double)(end-start)/CLOCKS_PER_SEC);

#ifdef TIME_ALL
		start = clock();
#endif//end of #ifdef TIME_ALL
		double runTime = srcNode->GetNewNextPacketTime();

		CScheduler_map::InsertEvent(new CPacketEvent(CPacketEvent::SEND,
									srcNode,
									runTime,
									p));
#ifdef TIME_ALL 
		end = clock();
#endif//end of #ifdef TIME_ALL
		DEBUG4(GlobalVariable::eventTime+=(double)(end-start)/CLOCKS_PER_SEC);

		DEBUG4(GlobalVariable::out_time<<"构造时间"<<e->Time()<<"触发时间："<<runTime<<endl);

		
	}

	//生成下一个流量事件
#ifdef TIME_ALL
	start = clock();
#endif//end of #ifdef TIME_ALL
	CScheduler_map::InsertEvent(new CGNRanTrafficEvent(CGNRanTrafficEvent::TRAFFICEVENT,
								this,
								CSimulator::Now()+1,GNevent->packetNumR));
#ifdef TIME_ALL
	end = clock();
#endif//end of #ifdef TIME_ALL
	DEBUG4(GlobalVariable::eventTime+=(double)(end-start)/CLOCKS_PER_SEC);


	//删除当前的流量事件;
	delete GNevent;
	//DEBUG4(GlobalVariable::out_eventlistSize<<CSimulator::Now()<<" "<<CScheduler_map::GetMapSize()<<endl);
}
Handler_status_str CGNRanTrafficStrategy::GetHandleStatus()
{
	return "traffic event:time";
}