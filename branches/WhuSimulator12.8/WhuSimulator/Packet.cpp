// trace [7/16/2010 mpiuser]
#include "StdAfx.h"
#include "Packet.h"
#include "GlobalVariable.h"

ofstream Packet::out_trace;
bool Packet::bOpenTrace = false;
Packet_t Packet::sID = 0;
Packet_t Packet::packetNum = 0;

CWhuSimulatorApp* Packet::pApp = (CWhuSimulatorApp*)AfxGetApp();

Packet::Packet(size_t src, size_t des) 
: _des(des), _src(src), _now_node(_src), _id(sID++),startTime(0),packetHops(0)
{
	GlobalVariable::packetAllocated++;
	++packetNum;
}
Packet::~Packet()
{
	GlobalVariable::packetAllocated--;
	--packetNum;
}
void 
Packet::SetStartTime(Time_t t)
{ 
	startTime=t;
	//TracePacket();//����Ƽ
}

Time_t 
Packet::GetStartTime()
{return startTime;}
void 
Packet::TracePacket()
{
	out_trace << CSimulator::Now() << " "//��ǰʱ��
		<< _now_node << " "//���ݰ���ǰ���ڽ��
		<< pApp->graph->get_node(_now_node)->GetQueueLength() << " "//��ǰ���ڶ��г���
		<< _id << " " //���ݰ�id
		<< _src << " " //���ݰ�Դ
		<< _des << endl; //���ݰ�Ŀ��
}

void 
Packet::OpenTracePacket(string file_name)
{
	if (!bOpenTrace)
	{
		out_trace.open(file_name.c_str());
		bOpenTrace = true;
	}
}

void 
Packet::CloseTracePacket()
{
	if (bOpenTrace)
	{
		out_trace.clear();
		out_trace.close();
		bOpenTrace = false;
	}
}



