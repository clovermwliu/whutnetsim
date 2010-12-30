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
���������ָ��ԴĿ�ģ�ÿ�Ĳ���R�������¼��Ĵ�����
������[IN] e   ��Ҫ�����ʱ��
	  [IN] t   �������¼���ʱ��
���أ����еĵ�һ��Ԫ��                                                                                       
��ע����ö���ͷԪ�غ�ͬʱ���¼��Ӷ�����ɾ��
*/
{
	//ȡR��
	CWhuSimulatorApp* pTrApp = (CWhuSimulatorApp*)AfxGetApp();
	CGNRanTrafficEvent* GNevent = (CGNRanTrafficEvent*)e;

	clock_t start,end;
	for (size_t pNum = 0;pNum != GNevent->packetNumR; ++pNum)
	{
		//Դ�ڵ㡢Ŀ�Ľڵ����ָ����������
		NodeId_t src = CRandom::Random_U(0,pTrApp->graph->get_node_num()-1);//����Ƽ
        //NodeId_t src = 0;
		//NodeId_t des = 3;
		NodeId_t des;
		do 
		{
			des = CRandom::Random_U(0,pTrApp->graph->get_node_num()-1);//����Ƽ
		} while (src==des);
		
		CNode* srcNode = pTrApp->graph->get_node(src);

		//�������¼�
		Packet* p = new Packet(src,des);
		p->SetStartTime(CSimulator::Now());		
#ifdef TIME_ALL
		start = clock();
#endif//end of #ifdef TIME_ALL
		srcNode->PacketEnQueue(p);//�����
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

		DEBUG4(GlobalVariable::out_time<<"����ʱ��"<<e->Time()<<"����ʱ�䣺"<<runTime<<endl);

		
	}

	//������һ�������¼�
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


	//ɾ����ǰ�������¼�;
	delete GNevent;
	//DEBUG4(GlobalVariable::out_eventlistSize<<CSimulator::Now()<<" "<<CScheduler_map::GetMapSize()<<endl);
}
Handler_status_str CGNRanTrafficStrategy::GetHandleStatus()
{
	return "traffic event:time";
}