// trace [7/16/2010 mpiuser]
#include "StdAfx.h"
#include "Test.h"
#include "ShortestPath.h"
#include "EfficientRoute.h"
#include "GNRanTrafficEvent.h"
#include "GNRanTrafficStrategy.h"
#include "Scheduler_map.h"
#include "FileConfig.h"
#include "GenericConfigItem.h"
#include "EndEvent.h"
#include "EndEventStrategy.h"
#include "Route.h"
#include "IntegrateRoute.h"
#include "UpdateAvgQueueLengthEvent.h"
#include "OptimalRoute.h"
#include "DegreeQueueRoute.h"
#include "Short-Opti.h"

#define R 600
#define THROUGHPUT_A 1
#define LOW 1
#define HIGH 10
using namespace std;


int TestMain(const string str_ini_file)//按照rate改变
{
	CWhuSimulatorApp* pRunApp;
	pRunApp = (CWhuSimulatorApp*)AfxGetApp();
	//Packet::OpenTracePacket("d://trace.txt");
	CFileConfig iniFile(str_ini_file);
	if ( iniFile.LoadFile() != SUCCESS_NO_ERROR )
	{
		//配置文件读取出错处理
		SetLastError(ERROR_TASK_FILENOTEXSIT);
		return 0;
	}

	//队列吞吐
	CGenericConfigItem<double> _throughPut_A(iniFile,"Throughput","throughput_a");
	double throughPut = _throughPut_A.MyValue();
	pRunApp->graph->SetThroughPut(throughPut);

	///链路cost、delay
	CGenericConfigItem<double> _costDelayRanL(iniFile,"EdgeCostDelay","Low");
	double Low = _costDelayRanL.MyValue();
	CGenericConfigItem<double> _costDelayRanH(iniFile,"EdgeCostDelay","High");
	double High = _costDelayRanH.MyValue();
	pRunApp->graph->set_ran_cost_delay(Low, High);

	// dat [7/23/2010 Administrator]
	//批处理
	CGenericConfigItem<int> _initR(iniFile,"Traffic","initR");
	int initR = _initR.MyValue();
	CGenericConfigItem<double> _step(iniFile,"Traffic","step");
	double step = _step.MyValue();
	CGenericConfigItem<int> _times(iniFile,"Traffic","times");
	int times = _times.MyValue();

	DEBUG3(GlobalVariable::out_debug_packet.open("c:\\debug_packet.txt"));
	DEBUG4(GlobalVariable::out_eventlistSize.open("c:\\eventlistSize.txt"));

	DEBUG4(GlobalVariable::out_time.open("c:\\windowSize.txt"));

	DEBUG5(GlobalVariable::out_eventMapSize.open("c:\\eventMapSize.txt"));

	DEBUG7(GlobalVariable::out_test_time.open("testTime.txt",std::ofstream::app));	

	//读取路由rate信息
	CGenericConfigItem<double> _routeB(iniFile,"Route","rate");
	double rate = _routeB.MyValue();
	CGenericConfigItem<double> _routebeta(iniFile,"Route","beta");
	double beta= _routebeta.MyValue();
	CGenericConfigItem<int> _routestr(iniFile,"Route","strategy");
	int type= _routestr.MyValue();
	ofstream outfile;
	for (int i = 0;i<times;i++)
	{
		clock_t start = clock();			

		int packetNum = initR;

		//绑定路由
		CRoute* r;
		if (type==5)
		{
			rate += i*step;
			outfile.open("d:\\detaLoader.txt",std::ofstream::app);
			outfile<<"**************rate = "<<rate<<endl;
			outfile.close();
			r = new CShortOpti(rate); 
			pRunApp->graph->NodeBoundRoute(r);
		} 
		else
		{
			beta += i*step;
			outfile.open("d:\\detaLoader.txt",std::ofstream::app);
			outfile<<"**************beta = "<<beta<<endl;
			outfile.close();
			r = new CEfficientPath(beta); 
			pRunApp->graph->NodeBoundRoute(r);
		}

		//初始化事件队列
		CEventBase* GNevent = new CGNRanTrafficEvent(packetNum);
		// handler [8/3/2010 Administrator]
		CHandler* pstart = new CGNRanTrafficStrategy();//流量事件一直用这个处理指针
		GNevent->handler = pstart;
		GNevent->Time(CSimulator::Now());
		CScheduler_map::InsertEvent(GNevent);

		//插入结束事件
		CGenericConfigItem<int> _endTime(iniFile,"EndTime","endTime");
		int endTime = _endTime.MyValue();
		CEndEvent* Endevent = new CEndEvent();
		CHandler* pend = new CEndEventStrategy();
		Endevent->handler = pend;
		Endevent->Time(endTime);
		CScheduler_map::InsertEndEvent(Endevent);

		//粒度
		CGenericConfigItem<int> _initGran(iniFile,"MapInfo","GranP");
		int GranP = _initGran.MyValue();
		CScheduler_map::SetVecLaterSize(endTime, GranP);

		CSimulator* s = new CSimulator();
		s->Run();
		pRunApp->graph->ClearNodeRoute();
		delete s;
		delete r;

		delete pstart;
		delete pend;

		DEBUG4(GlobalVariable::out_eventlistSize<<"--------出、入队时间，路由时间，插入事件事件,总时间----"<<endl);
		DEBUG4(GlobalVariable::out_eventlistSize<<GlobalVariable::enqueTime<<"  "<<GlobalVariable::routeTime<<"  "<<GlobalVariable::eventTime);

		clock_t end = clock();	
		DEBUG7(GlobalVariable::out_test_time<<"第" << i+1 << "次实验计时："<<(double)(end-start)/CLOCKS_PER_SEC <<endl);
	}

	DEBUG4(GlobalVariable::out_eventlistSize<<endl<<GlobalVariable::allEvents);

	DEBUG4(GlobalVariable::out_eventlistSize.close());
	DEBUG4(GlobalVariable::out_time.close());

	//图中信息删除
	// clear [8/2/2010 Administrator]
	pRunApp->graph->clear(0);

	DEBUG3(GlobalVariable::out_debug_packet.close());
	DEBUG3(GlobalVariable::out_new.close());
	DEBUG5(GlobalVariable::out_eventMapSize.close());
	DEBUG8(GlobalVariable::out_loc.close());
	//Packet::CloseTracePacket();
	return 1;
}

//
//int TestMain(const string str_ini_file)//按照packet改变
//{
//	CWhuSimulatorApp* pRunApp;
//	pRunApp = (CWhuSimulatorApp*)AfxGetApp();
//    //Packet::OpenTracePacket("d://trace.txt");
//	CFileConfig iniFile(str_ini_file);
//	if ( iniFile.LoadFile() != SUCCESS_NO_ERROR )
//	{
//		//配置文件读取出错处理
//		SetLastError(ERROR_TASK_FILENOTEXSIT);
//		return 0;
//	}
//
//	//绑定路由
//	CGenericConfigItem<int> _routeStrategy(iniFile,"Route","strategy");
//	int routeStrategy = _routeStrategy.MyValue();
//	CRoute* r;
//	switch (routeStrategy)
//	{
//	case 1: r = new CShortestPath();break;//最短
//	case 2: r = new COptimalRoute();break;//度连乘积	
//	case 3: //CIntegrateRoute综合
//		{
//			CGenericConfigItem<double> _routeA(iniFile,"Route","a");
//			double a = _routeA.MyValue();
//			CGenericConfigItem<double> _routeB(iniFile,"Route","b");
//			double b = _routeB.MyValue();
//			r = new CIntegrateRoute(a, b); 
//		}
//		break;
//		// new [8/2/2010 Administrator]
//	case 4: //CDegreeQueueRoute队列
//		{
//			CGenericConfigItem<double> _routeA(iniFile,"Route","a");
//			double a = _routeA.MyValue();
//			CGenericConfigItem<double> _routeB(iniFile,"Route","b");
//			double b = _routeB.MyValue();
//			r = new CDegreeQueueRoute(a, b); 
//		}
//		break;
//	case 5://最短路径和度的连乘积的综合,以概率a进行选择
//		{
//			CGenericConfigItem<double> _routeB(iniFile,"Route","rate");
//			double a = _routeB.MyValue();
//			r = new CShortOpti(a); 
//		}
//	case 6://有效路径
//		{
//			CGenericConfigItem<double> _routeB(iniFile,"Route","beta");
//			double a = _routeB.MyValue();
//			r = new CEfficientPath(a); 
//		}
//		break;
//	default:break;
//	}
//	DEBUG8(GlobalVariable::out_loc.open("c:\\loc.txt",std::ofstream::app));
//	pRunApp->graph->NodeBoundRoute(r);
//	//队列吞吐
//	CGenericConfigItem<double> _throughPut_A(iniFile,"Throughput","throughput_a");
//	double throughPut = _throughPut_A.MyValue();
//	pRunApp->graph->SetThroughPut(throughPut);
//	//平均队列长度
//	// new [8/2/2010 Administrator]
//	if (routeStrategy == 3 || routeStrategy == 4)
//	{
//		CGenericConfigItem<double> _wq(iniFile,"Route","wq");
//		double wq = _wq.MyValue();
//		pRunApp->graph->SetQueueW(wq);
//	}
//	///链路cost、delay
//	CGenericConfigItem<double> _costDelayRanL(iniFile,"EdgeCostDelay","Low");
//	double Low = _costDelayRanL.MyValue();
//	CGenericConfigItem<double> _costDelayRanH(iniFile,"EdgeCostDelay","High");
//	double High = _costDelayRanH.MyValue();
//	pRunApp->graph->set_ran_cost_delay(Low, High);
//	
//	// dat [7/23/2010 Administrator]
//	//批处理
//	CGenericConfigItem<int> _initR(iniFile,"Traffic","initR");
//	int initR = _initR.MyValue();
//	CGenericConfigItem<int> _step(iniFile,"Traffic","step");
//	int step = _step.MyValue();
//	CGenericConfigItem<int> _times(iniFile,"Traffic","times");
//	int times = _times.MyValue();
//
//	DEBUG3(GlobalVariable::out_debug_packet.open("c:\\debug_packet.txt"));
//	DEBUG4(GlobalVariable::out_eventlistSize.open("c:\\eventlistSize.txt"));
//
//	DEBUG4(GlobalVariable::out_time.open("c:\\windowSize.txt"));
//
//	DEBUG5(GlobalVariable::out_eventMapSize.open("c:\\eventMapSize.txt"));
//
//	DEBUG7(GlobalVariable::out_test_time.open("testTime.txt",std::ofstream::app));	
//	
//    ofstream outfile;
//	for (int i = 0;i<times;i++)
//	{
//		clock_t start = clock();		
//		int packetNum = initR+i*step;
//
//		outfile.open("d:\\detaLoader.txt",std::ofstream::app);
//		outfile<<"**************packet= "<<packetNum<<endl;
//		outfile.close();
//
//		//初始化事件队列
//		CEventBase* GNevent = new CGNRanTrafficEvent(packetNum);
//		// handler [8/3/2010 Administrator]
//		CHandler* pstart = new CGNRanTrafficStrategy();//流量事件一直用这个处理指针
//		GNevent->handler = pstart;
//		GNevent->Time(CSimulator::Now());
//		CScheduler_map::InsertEvent(GNevent);
//		
//		//插入结束事件
//		CGenericConfigItem<int> _endTime(iniFile,"EndTime","endTime");
//		int endTime = _endTime.MyValue();
//		CEndEvent* Endevent = new CEndEvent();
//		CHandler* pend = new CEndEventStrategy();
//		Endevent->handler = pend;
//		Endevent->Time(endTime);
//		CScheduler_map::InsertEndEvent(Endevent);
//
//		//粒度
//		CGenericConfigItem<int> _initGran(iniFile,"MapInfo","GranP");
//		int GranP = _initGran.MyValue();
//		CScheduler_map::SetVecLaterSize(endTime, GranP);
//
//		//debug new route
//		//增加更新结点平均队列长度事件
//		if (routeStrategy == 3 || routeStrategy == 4)
//		{
//			CGenericConfigItem<double> _routeInterval(iniFile,"Route","UpdateAvgQueueLengthInterval");
//			double routeInterval= _routeInterval.MyValue();
//
//			CEventBase* updateQueueEvent = new CUpdateAvgQueueLengthEvent(routeInterval);
//			updateQueueEvent->handler = pRunApp->graph;
//			updateQueueEvent->Time(CSimulator::Now());
//			CScheduler_map::InsertEvent(updateQueueEvent);			
//		}
//
//		CSimulator* s = new CSimulator();
//		s->Run();
//		delete s;
//
//		delete pstart;
//		delete pend;
//
//		DEBUG4(GlobalVariable::out_eventlistSize<<"--------出、入队时间，路由时间，插入事件事件,总时间----"<<endl);
//		DEBUG4(GlobalVariable::out_eventlistSize<<GlobalVariable::enqueTime<<"  "<<GlobalVariable::routeTime<<"  "<<GlobalVariable::eventTime);
//		
//		clock_t end = clock();	
//		DEBUG7(GlobalVariable::out_test_time<<"第" << i+1 << "次实验计时："<<(double)(end-start)/CLOCKS_PER_SEC <<endl);
//	}
//
//	DEBUG4(GlobalVariable::out_eventlistSize<<endl<<GlobalVariable::allEvents);
//
//	DEBUG4(GlobalVariable::out_eventlistSize.close());
//	DEBUG4(GlobalVariable::out_time.close());
//
//	//图中信息删除
//	// clear [8/2/2010 Administrator]
//	pRunApp->graph->clear(0);
//
//	DEBUG3(GlobalVariable::out_debug_packet.close());
//	DEBUG3(GlobalVariable::out_new.close());
//	DEBUG5(GlobalVariable::out_eventMapSize.close());
//	DEBUG8(GlobalVariable::out_loc.close());
//	//Packet::CloseTracePacket();
//	return 1;
//}