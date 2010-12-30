#include "StdAfx.h"
#include "Monitor.h"
#include "Graph.h"
#include "TrafficEvent.h"
#include "EndEvent.h"
#include "PacketEvent.h"

//Packet_t CMonitor::preW = 0;
//Packet_t CMonitor::nowW = 0;
//Time_t   CMonitor::preT = 0;
//Time_t   CMonitor::nowT = 0;
class CGraph;
CMonitor::CMonitor(void):preW(0),nowW(0),preT(0),nowT(0)
{
	pMonitorApp = (CWhuSimulatorApp*)AfxGetApp();
	//outfile.open("detaLoader.txt",std::ofstream::app);
	//result.open("result.txt",std::ofstream::app);
	//aveHop.open("aveHop.txt",std::ofstream::app);
	//aveTime.open("aveTime.txt",std::ofstream::app);
	outfile.open("d:\\detaLoader.txt",std::ofstream::app);
	result.open("d:\\result.txt",std::ofstream::app);
	aveHop.open("d:\\aveHop.txt",std::ofstream::app);
	aveTime.open("d:\\aveTime.txt",std::ofstream::app);
}

CMonitor::~CMonitor(void)
{
	outfile.close();
	result.close(); 
	aveHop.close();
	aveTime.close();
}
void CMonitor::MonitorEntry(CEventBase* e)
{
	if (1)
	{
		MonitorLoaderW(e);
	}
	if (0)
	{
		MonitorT(e);
	}
	if (1)
	{
		MonitorAverageTrasitTime(e);
	}
	if (1)
	{
		MonitorAverageTrasitHop(e);
	}
	if (1)
	{
		MonitorPacketDoneNums(e);
	}
	if (0)
	{
		MonitorPutLoc(e);
	}
}
void CMonitor::MonitorPutLoc(CEventBase* e)
{
	if (e->event == CPacketEvent::QUEUEPROCESS)
	{
		CPacketEvent* pacEvent = (CPacketEvent*)e;
		Packet* p = pacEvent->packet;

		size_t nowNode = p->_now_node;

		size_t putLoc = pMonitorApp->graph->get_node(nowNode)->GetQueueLength();
		++(GlobalVariable::putLocAnaly[putLoc]);
	}
	if (e->event == CEndEvent::ENDEVENT)
	{
		static int i = 0;
		std::string names;
		std::stringstream namestream;
		namestream<<"LocAnalysis"<<i<<".txt";
		namestream>>names;
		i++;
		
		std::ofstream aveLoc;
		aveLoc.open(names.c_str());

		int allLocNum = 0;
		for (std::map<int,int>::iterator iter = (GlobalVariable::putLocAnaly).begin();
			iter != (GlobalVariable::putLocAnaly).end();iter++)
		{
			allLocNum += iter->second;
		}
		int loc = 0;
		for (std::map<int,int>::iterator iter = (GlobalVariable::putLocAnaly).begin();
			iter != (GlobalVariable::putLocAnaly).end();iter++)
		{
			while (loc<iter->first)
			{
				aveLoc<<0<<endl;
				loc++;
			}
			aveLoc<<(double)iter->second/allLocNum<<endl;
			loc++;
		}
		aveLoc.close();
	}
}
void CMonitor::MonitorPacketDoneNums(CEventBase* e)
{
	if (e->event == CPacketEvent::END)
	{
		GlobalVariable::packetDoneNums++;
	}
}
void CMonitor::MonitorAverageTrasitHop(CEventBase* e)
{
	if (e->event == CPacketEvent::SEND)
	{
		CPacketEvent* pacEvent = (CPacketEvent*)e;
		Packet* p = pacEvent->packet;
		p->PacketHopOne();
	}
	if (e->event == CPacketEvent::END)
	{
		CPacketEvent* pacEvent = (CPacketEvent*)e;
		Packet* p = pacEvent->packet;
		GlobalVariable::packetHops += p->GetPacketHops();
	}
	if (e->event == CEndEvent::ENDEVENT)
	{
		aveHop<<GlobalVariable::packetHops<<"  "
			  <<GlobalVariable::packetDoneNums<<"  "
			  <<(double)GlobalVariable::packetHops/GlobalVariable::packetDoneNums<<endl;
	}
}
void CMonitor::MonitorAverageTrasitTime(CEventBase* e)
{
	if (e->event == CPacketEvent::END)
	{
		CPacketEvent* pacEvent = (CPacketEvent*)e;
		Packet* p = pacEvent->packet;
		GlobalVariable::packetTransitTimes += e->Time()-p->GetStartTime();
	}
	if (e->event == CEndEvent::ENDEVENT)
	{
		aveTime<<GlobalVariable::packetTransitTimes/GlobalVariable::packetDoneNums<<endl;
	}
}
void CMonitor::MonitorLoaderW(CEventBase* e)
{
	static int t = 0;
	if (e->event == CTrafficEvent::TRAFFICEVENT)
	{
		t++;
		if (t%10 == 0)
		{
			t = 0;
			CGraph* graph = pMonitorApp->graph;
			Packet_t pNum = 0;
			for (size_t i = 0;i!=graph->get_node_num();i++)
			{
				CNode* node = graph->get_node(i);
				pNum +=node->GetQueueLength();
			}
			//  [8/2/2010 Administrator]
			//Packet_t pNum = Packet::packetNum;
			preW = nowW; //统计两个时刻的负载
			nowW = pNum;
			preT = nowT; //更新时间
			nowT = e->Time();

			DEBUG1(outfile<<"Packet总数量："<<Packet::packetNum<<" "
				<<"QueueProcess中包的数量"<<GlobalVariable::processNum<<" 负载"
				<<nowW<<" "<<preW<<" "<<nowT<<" "<<preT<<" 流量R:"<<e->GetPacketNumROnce()<<"======");
			outresult = ((double)nowW - preW)/((nowT - preT)*(e->GetPacketNumROnce()));
			DEBUG1(outfile<<outresult<<endl);
		}
	}
	if (e->event == CEndEvent::ENDEVENT)
	{
		result<<outresult<<endl;
		preW=0; nowW=0; preT=0;	nowT=0;t = 0;
		DEBUG1(outfile<<endl);
	}
}
void CMonitor::MonitorT(CEventBase* e)
{
	preT = nowT;
	nowT = e->Time();
}