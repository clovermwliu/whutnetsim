#include <iostream>


#include "StdAfx.h"
#include "GlobalVariable.h"
using namespace std;

// Static members
unsigned long long GlobalVariable::eventsAllocated  = 0;
unsigned long long GlobalVariable::packetAllocated  = 0;
unsigned long long GlobalVariable::processNum = 0;
 double GlobalVariable::packetTransitTimes = 0;//���Ĵ���ʱ��֮��
unsigned long long GlobalVariable::packetDoneNums = 0;// ���䵽�İ�������
unsigned long long GlobalVariable::packetHops = 0;// ���Ĵ��������
map<int,int> GlobalVariable::putLocAnaly;
unsigned long long GlobalVariable::allEvents = 0;

////����һ���������
//std::ofstream GlobalVariable::outfile;
//std::ofstream GlobalVariable::result;
//std::ofstream GlobalVariable::aveHop;
//std::ofstream GlobalVariable::aveTime;
//std::ofstream GlobalVariable::aveLoc;

void GlobalVariable::OutPutAveHop(std::ofstream& outPut)
{
	outPut<<packetHops/packetDoneNums<<endl;
}
void GlobalVariable::OutPutLocAnaly(std::ofstream& outPut)
{
	int allLocNum = 0;
	for (std::map<int,int>::iterator iter = putLocAnaly.begin();iter != putLocAnaly.end();iter++)
	{
		allLocNum += iter->second;
	}
	int loc = 0;
	for (std::map<int,int>::iterator iter = putLocAnaly.begin();iter != putLocAnaly.end();iter++)
	{
		while (loc<iter->first)
		{
			outPut<<0<<endl;
			loc++;
		}
		outPut<<iter->second/allLocNum<<endl;
		loc++;
	}
}
void GlobalVariable::OutPutAveTime(std::ofstream& outPut)
{
	outPut<<packetTransitTimes/packetDoneNums<<endl;
}

double GlobalVariable::enqueTime = 0;//ͳ�ƽ���ʱ��
double GlobalVariable::allTime = 0;//������ʱ��
double GlobalVariable::routeTime = 0;//ͳ��·�ɼ���ʱ��
double GlobalVariable::eventTime = 0;//ͳ���¼������¼�ʱ��


ofstream GlobalVariable::out_debug_event;
ofstream GlobalVariable::out_debug_packet;
ofstream GlobalVariable::out_new;
std::ofstream GlobalVariable::out_eventlistSize;
std::ofstream GlobalVariable::out_time;

std::ofstream GlobalVariable::out_eventMapSize;

std::ofstream GlobalVariable::out_eventError;
std::ofstream GlobalVariable::out_test_time;

std::ofstream GlobalVariable::out_loc;