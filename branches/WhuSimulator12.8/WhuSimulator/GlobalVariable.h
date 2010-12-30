#pragma once

#include <afx.h>
#include <string>
#include <fstream>
#include <map>

// error [7/20/2010 mpiuser]

//��
#ifndef DEBUG_MASK
#define DEBUG_MASK 1
#endif

#define DBM0 0
#define DEBUG0(x) if(1&DBM0) x //debug�¼�����

#define DBM1 1
#define DEBUG1(x) if(1&DBM1) x //monitor

#define DBM2 0
#define DEBUG2(x) if(1&DBM2) x //ÿ�Ĳ������м���

#define DBM3 0
#define DEBUG3(x) if(1&DBM3) x //�ڴ���

#define DBM4 1
#define DEBUG4(x) if(1&DBM4) x //�����¼���·�ɡ����еȼ�ʱ��һ��ʵ���ܼ�ʱ��DEBUG7
#define TIME_ALL //�����¼���·�ɡ����еȼ�ʱ

#define DBM5 0
#define DEBUG5(x) if(1&DBM5) x //�¼�map�Ĵ�С

#define DBM6 0
#define DEBUG6(x) if(1&DBM6) x //CSimulator::Run��ȡ�¼��Ĳ��

#define DBM7 1
#define DEBUG7(x) if(1&DBM7) x //һ��ʵ���ܼ�ʱ

#define DBM8 0
#define DEBUG8(x) if(1&DBM8) x //�¼����в��


//�¼��������ݽṹѡ��
//#define EventMap //��һ��map���洢event
#define EventMapVector //��һ��map ����vector���洢event


class GlobalVariable
{
public:
	static unsigned long long eventsAllocated;//ϵͳ���¼�������
	static unsigned long long packetAllocated;//ϵͳ�з���İ�������
	static unsigned long long processNum;//ϵͳ������·�ϵİ�������
	static  double packetTransitTimes;//���Ĵ���ʱ��֮��
	static unsigned long long packetDoneNums;// ���䵽�İ�������
	static unsigned long long packetHops;// ���Ĵ��������
	static std::map<int,int> putLocAnaly;

	//����һ���������
	static	std::ofstream outfile;
	//static	std::ofstream loaderW; // �������
	//static	std::ofstream aveHop;  
	//static	std::ofstream aveTime;
	//static	std::ofstream aveLoc;

	//static void OutLoaderW();
	static void OutPutAveHop(std::ofstream& outPut);
	static void OutPutLocAnaly(std::ofstream& outPut);
	static void OutPutAveTime(std::ofstream& outPut);


	static double enqueTime;//ͳ�ƽ���ʱ��
	static double allTime;//ͳ�Ƴ���ʱ��
	static double routeTime;//ͳ��·�ɼ���ʱ��
	static double eventTime;//ͳ���¼������¼�ʱ��

	static std::ofstream out_debug_event;
	static std::ofstream out_debug_packet;
	static std::ofstream out_new;

	static std::ofstream out_eventlistSize;
	static std::ofstream out_time;//�¼�����ʱ�估����ʱ��

	static std::ofstream out_eventMapSize;

	static std::ofstream out_eventError;//CSimulator::Run��ȡ�¼��Ĳ��
	static std::ofstream out_test_time;//һ��ʵ���ܼ�ʱ

	static std::ofstream out_loc;

	//static std::string  strTmp;
	static unsigned long long allEvents;//ϵͳ���¼�������
};
