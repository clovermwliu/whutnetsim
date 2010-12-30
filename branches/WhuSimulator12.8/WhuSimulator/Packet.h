
#ifndef __PACKET_H__
#define __PACKET_H__

// trace [7/16/2010 mpiuser]
#include <fstream>
#include <string>
using namespace std;
#include "Simulator.h"
#include "WhuSimulator.h"
#include "G_common_defs.h"

class Packet 
{
public:
	size_t _des;//数据包的目的路由器
	size_t _src;//数据包的源节点ID
	size_t _now_node;//数据包当前所在节点ID

	// trace [7/16/2010 mpiuser]
	Packet_t _id;
	Packet(size_t src, size_t des);
	~Packet();
	//Packet(size_t des, size_t src) : _des(des), _src(src), _now_node(des){ }
	// control [8/3/2010 Administrator]
	void SetStartTime(Time_t t);
	Time_t GetStartTime();

	void SetPacketHops(Packet_t p){packetHops = p;}
	void PacketHopOne(){ ++packetHops;}
	Packet_t GetPacketHops(){return packetHops;}

private:
	static Packet_t sID; 
	// trace [7/16/2010 mpiuser]
public:
	void TracePacket();
	static void OpenTracePacket(string file_name);
	static void CloseTracePacket();


public:
	static ofstream out_trace;
	static bool bOpenTrace;
	static Packet_t packetNum;

private:
	static CWhuSimulatorApp *pApp;
	Time_t startTime;
	Packet_t	packetHops;
};
#endif