//Copyright (c) 2010, Information Security Institute of Wuhan Universtiy(ISIWhu)
//All rights reserved.
//
//PLEASE READ THIS DOCUMENT CAREFULLY BEFORE UTILIZING THE PROGRAM
//BY UTILIZING THIS PROGRAM, YOU AGREE TO BECOME BOUND BY THE TERMS OF
//THIS LICENSE.  IF YOU DO NOT AGREE TO THE TERMS OF THIS LICENSE, DO
//NOT USE THIS PROGRAM OR ANY PORTION THEREOF IN ANY FORM OR MANNER.
//
//This License allows you to:
//1. Make copies and distribute copies of the Program's source code provide that any such copy 
//   clearly displays any and all appropriate copyright notices and disclaimer of warranty as set 
//   forth in this License.
//2. Modify the original copy or copies of the Program or any portion thereof ("Modification(s)"). 
//   Modifications may be copied and distributed under the terms and conditions as set forth above. 
//   Any and all modified files must be affixed with prominent notices that you have changed the 
//   files and the date that the changes occurred.

//Termination:
//   If at anytime you are unable to comply with any portion of this License you must immediately 
//   cease use of the Program and all distribution activities involving the Program or any portion 
//   thereof.

//Statement:
//   In this program, part of the code is from the GTNetS project, The Georgia Tech Network 
//   Simulator (GTNetS) is a full-featured network simulation environment that allows researchers in 
//   computer networks to study the behavior of moderate to large scale networks, under a variety of 
//   conditions. Our work have great advance due to this project, Thanks to Dr. George F. Riley from 
//   Georgia Tech Research Corporation. Anyone who wants to study the GTNetS can come to its homepage:
//   http://www.ece.gatech.edu/research/labs/MANIACS/GTNetS/
//


//File Information:
//
//
//File Name:
//File Purpose:
//Original Author:
//Author Organization:
//Construct Data:
//Modify Author:
//Author Organization:
//Modify Data:


#ifndef __applicationwormudp_h__
#define __applicationwormudp_h__
#pragma once

#include "ApplicationWithTimer.h"
#include "udp.h"
#include "WormScanModel.h"

class ApplicationWormUDP : public ApplicationWithTimer{


private:
	//和该应用相关的变量
	Timer         timer;//处理TimerEvent事件的Timer对象

	TimerEvent*   timeoutevent;//所使用的TimerEvent事件

	UDP*		udp;//使用的UDP协议对象指针

	//和蠕虫传播相关的变量
	//这些变量都应给出默认值

	Count_t scanspersecond;//扫描率

	Size_t infectionlength;//蠕虫包大小
	PortId_t infectionport; //感染端口

	bool	infected;//	标志当前结点是否已被感染
	bool	vulnerable;//	标志当前结点是否可以被蠕虫感染（是否存在漏洞）
	
	Count_t	IPscanrange;//	扫描范围
	
	IPAddr_t	baseIP	;//目的IP地址的基IP

	//和扫描策略模型相关的变量
	bool	initialized;// 标志是否已初始化WormScanModel

public:
/////////////////////////////////////////////////////////////////////////
//基础方法
//////////////////////////////////////////////////////////////////////////
	//构造函数及析构函数
	ApplicationWormUDP();

	~ApplicationWormUDP(void);

	virtual Application* Copy() const;  // Make a copy of this application


//////////////////////////////////////////////////////////////////////////
//应用事件触发与驱动相关方法
//////////////////////////////////////////////////////////////////////////
	virtual void Timeout(TimerEvent *);
	//描述：实现，构造新的TimerEvent事件，并将该事件加入全局事件队列
	//备注：在Timer类的Handle函数里被调用

	void ScheduleNextPacket();

	virtual void StartApp();    // Called at time specified by Start

	virtual void StopApp();     // Called at time specified by Stop


//////////////////////////////////////////////////////////////////////////
//响应收到数据包方法
//////////////////////////////////////////////////////////////////////////
	// the following method is called when packets are received from the port
	virtual void Receive(Packet *, L4Protocol *, Seq_t); // Data received


//////////////////////////////////////////////////////////////////////////
//第四层协议自身相关方法
//////////////////////////////////////////////////////////////////////////
	void AttachNode(Node *n);

//////////////////////////////////////////////////////////////////////////
//蠕虫传播相关方法
//////////////////////////////////////////////////////////////////////////

	void SendWormPacket();

	bool PacketIsWorm(Packet *);   // I am going to use this to check if packet 

	void Infect();
	bool IsInfected();
	void StartInfection();

	void SetScanspersecond(Count_t scanrate) { scanspersecond = scanrate; }

	void SetInfectionlength(Size_t length) { infectionlength = length; }

	void SetInfectionport(PortId_t port) { infectionport = port; }

	void SetIPscanrange(Count_t scanrange) { IPscanrange = scanrange; }

	void SetBaseIP(IPAddr_t base) { baseIP = base; }

//////////////////////////////////////////////////////////////////////////
//扫描策略模型相关方法
//////////////////////////////////////////////////////////////////////////

	IPAddr_t GenerateNextIPAddress();

};


#endif