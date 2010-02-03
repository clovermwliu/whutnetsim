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


#include "ApplicationWormUDP.h"


//////////////////////////////////////////////////////////////////////////
/////��������
//////////////////////////////////////////////////////////////////////////

ApplicationWormUDP::ApplicationWormUDP()
//���캯��������һЩ������Ĭ��ֵ
{
	//һЩ������Ĭ��ֵ
	scanspersecond = 10;//ɨ����

	infectionlength = 376;//������С
	infectionport = 1434;//��Ⱦ�˿�

	infected = false;//	��־��ǰ����Ƿ��ѱ���Ⱦ
	vulnerable = true;//	��־��ǰ����Ƿ���Ա�����Ⱦ���Ƿ����©����

	IPscanrange = 65535;//	ɨ�跶Χ

	baseIP = IPAddr("192.168.0.0");//Ŀ��IP��ַ�Ļ�IP

	initialized = false;

	udp = NULL;

	node = NULL;

	timeoutevent = NULL;

	copyOnConnect=false;

	if (!pModel)
	{
		//����Ĭ��ɨ�����ģ��
	}

}

ApplicationWormUDP::~ApplicationWormUDP(void)
{

}

Application* 
ApplicationWormUDP::Copy() const
/*
���������ص�ǰӦ�ö���Ŀ���
*/
{
	return new ApplicationWormUDP(*this);
}

//////////////////////////////////////////////////////////////////////////
//Ӧ���¼�������������ط���
//////////////////////////////////////////////////////////////////////////

void 
ApplicationWormUDP::ScheduleNextPacket()
/*
�����������µ�TimerEvent�¼������������ȫ���¼�����
*/
{
	if (!timeoutevent) timeoutevent = new TimerEvent();
	timer.Schedule(timeoutevent, 1.0 / (double)scanspersecond, this);
}

void
ApplicationWormUDP::Timeout(TimerEvent *ev)
/*
������ʵ�ֻ���TimerHandler��Timeout����������������ݰ���������ScheduleNextPacket���������µ�TimerEvent�¼����������ȫ���¼�����
*/
{
	if (!node)
	{
		cout << "WormApplication::Timeout with no attached node "<<endl;
		return;
	}

	//DEBUG0((cout<< "WormApplicationWorm::Timeout "<<Simulator::Now()<<endl));

	//// Sending the worm packet
	SendWormPacket();
	ScheduleNextPacket();
}

void 
ApplicationWormUDP::StartApp()   // Called at time specified by Start
/*
��������Application��̳еķ�����������Ӧ��
��ע�������µ�TimerEvent�¼������������ȫ���¼�����
*/
{
	if (infected)
		StartInfection();
}

void 
ApplicationWormUDP::StopApp()     // Called at time specified by Stop
/*
��������Application��̳еķ�����ֹͣ��Ӧ��
��ע������Timer���Cancel��������ǰ��TimerEvent�¼���ȫ���¼�������ȥ��
*/
{
	  if (timeoutevent) 
		  timer.Cancel(timeoutevent);
}


//////////////////////////////////////////////////////////////////////////
//��Ӧ�յ����ݰ�����
//////////////////////////////////////////////////////////////////////////

void 
ApplicationWormUDP::Receive(Packet *p, L4Protocol *proto, Seq_t)
/*
���������ܵ�����ʱ��������Ⱦ������StartInfection���������µ�TimerEvent�¼������������ȫ���¼�����
*/
{
	DEBUG0((cout<<"Received UDP packet"<<endl));

	if (PacketIsWorm(p)) {
		if (vulnerable && !infected) {
			StartInfection();
		}
	}
	delete p;
}

//////////////////////////////////////////////////////////////////////////
//���Ĳ�Э��������ط���
//////////////////////////////////////////////////////////////////////////

void 
ApplicationWormUDP::AttachNode(Node* n) 
/*
�����������µ�UDP���󣬲��󶨽��n��UDPЭ�����ͬʱ�������Ӧ�ð󶨵�UDPЭ�����
*/
{
	Application::AttachNode(n);

	udp = new UDP(n);

	l4proto = udp;
	//l4proto = new UDP(n);

	//l4proto->Attach(n);
	udp->Attach(n);
	udp->Bind(infectionport);
	udp->AttachApplication(this);
	DEBUG0((cout << "udp " << udp << " , attach App " << this << endl));
	if(udp) 
	{ 
		udp->Attach(node);
		udp->Bind(infectionport);
	}

}

//////////////////////////////////////////////////////////////////////////
//��洫����ط���
//////////////////////////////////////////////////////////////////////////
void 
ApplicationWormUDP::SendWormPacket()
/*
����������ѡ����һ��ɨ��IPģ�飬Ȼ�����UDPЭ���SendTo����������������Ŀ��IP
*/
{
	//��Ҫ����ģ������NextIP
	IPAddr_t target = GenerateNextIPAddress(); 

	//DEBUG0((cout<<"sending worm packet to "<<IPAddr::ToDotted(target)<<endl));
	udp->SendTo(infectionlength, target, infectionport);
}


bool
ApplicationWormUDP::PacketIsWorm(Packet *)   // I am going to use this to check if packet 
/*
�������жϽ��յ������ݰ�p�Ƿ�Ϊ����
��ע��δʵ�֣�ֱ�ӷ���true
*/
{
	return true;
}

void 
ApplicationWormUDP::Infect()
/*
���������ý�㱻��Ⱦ�󣬵��ø÷�����infected״̬��Ϊtrue
*/
{
	infected = true;
}


bool
ApplicationWormUDP::IsInfected()
/*
���������ص�ǰ����Ƿ񱻸�Ⱦ��״̬
*/
{
	return infected;
}

void
ApplicationWormUDP::StartInfection()
/*
�������ý�㱻��Ⱦ�󣬵���ScheduleNextPacket���������µ�TimerEvent�¼������������ȫ���¼�����
*/
{
	DEBUG0(( cout<<"Infected machine:"<< IPAddr::ToDotted(node->GetIPAddr())<<endl ));
	infected = true;
	DEBUG0(( cout<<" Now starting timer event"<<endl));
#ifdef HAVE_QT
	// Set the node to red on animation
	node->Color(Qt::red);
#endif
	ScheduleNextPacket();
}

//////////////////////////////////////////////////////////////////////////
//ɨ�����ģ����ط���
//////////////////////////////////////////////////////////////////////////

IPAddr_t 
ApplicationWormUDP::GenerateNextIPAddress()
{
	if (!initialized)//�����û�г�ʼ��ɨ�����ģ��
	{
		//����ɨ�����ģ�͵ĳ�ʼ���������г�ʼ��

	}
	IPAddr_t *IP = new IPAddr_t;

	pModel->GetModelValue((void**)(&IP));

	IPAddr_t thisIP = *IP;

	delete IP;

	return thisIP;
}
