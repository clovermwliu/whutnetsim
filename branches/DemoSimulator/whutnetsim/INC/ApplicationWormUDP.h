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
	//�͸�Ӧ����صı���
	Timer         timer;//����TimerEvent�¼���Timer����

	TimerEvent*   timeoutevent;//��ʹ�õ�TimerEvent�¼�

	UDP*		udp;//ʹ�õ�UDPЭ�����ָ��

	//����洫����صı���
	//��Щ������Ӧ����Ĭ��ֵ

	Count_t scanspersecond;//ɨ����

	Size_t infectionlength;//������С
	PortId_t infectionport; //��Ⱦ�˿�

	bool	infected;//	��־��ǰ����Ƿ��ѱ���Ⱦ
	bool	vulnerable;//	��־��ǰ����Ƿ���Ա�����Ⱦ���Ƿ����©����
	
	Count_t	IPscanrange;//	ɨ�跶Χ
	
	IPAddr_t	baseIP	;//Ŀ��IP��ַ�Ļ�IP

	//��ɨ�����ģ����صı���
	bool	initialized;// ��־�Ƿ��ѳ�ʼ��WormScanModel

public:
/////////////////////////////////////////////////////////////////////////
//��������
//////////////////////////////////////////////////////////////////////////
	//���캯������������
	ApplicationWormUDP();

	~ApplicationWormUDP(void);

	virtual Application* Copy() const;  // Make a copy of this application


//////////////////////////////////////////////////////////////////////////
//Ӧ���¼�������������ط���
//////////////////////////////////////////////////////////////////////////
	virtual void Timeout(TimerEvent *);
	//������ʵ�֣������µ�TimerEvent�¼����������¼�����ȫ���¼�����
	//��ע����Timer���Handle�����ﱻ����

	void ScheduleNextPacket();

	virtual void StartApp();    // Called at time specified by Start

	virtual void StopApp();     // Called at time specified by Stop


//////////////////////////////////////////////////////////////////////////
//��Ӧ�յ����ݰ�����
//////////////////////////////////////////////////////////////////////////
	// the following method is called when packets are received from the port
	virtual void Receive(Packet *, L4Protocol *, Seq_t); // Data received


//////////////////////////////////////////////////////////////////////////
//���Ĳ�Э��������ط���
//////////////////////////////////////////////////////////////////////////
	void AttachNode(Node *n);

//////////////////////////////////////////////////////////////////////////
//��洫����ط���
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
//ɨ�����ģ����ط���
//////////////////////////////////////////////////////////////////////////

	IPAddr_t GenerateNextIPAddress();

};


#endif