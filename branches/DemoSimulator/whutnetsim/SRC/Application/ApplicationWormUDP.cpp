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
/////基础方法
//////////////////////////////////////////////////////////////////////////

ApplicationWormUDP::ApplicationWormUDP()
//构造函数，设置一些参数的默认值
{
	//一些参数的默认值
	scanspersecond = 10;//扫描率

	infectionlength = 376;//蠕虫包大小
	infectionport = 1434;//感染端口

	infected = false;//	标志当前结点是否已被感染
	vulnerable = true;//	标志当前结点是否可以被蠕虫感染（是否存在漏洞）

	IPscanrange = 65535;//	扫描范围

	baseIP = IPAddr("192.168.0.0");//目的IP地址的基IP

	initialized = false;

	udp = NULL;

	node = NULL;

	timeoutevent = NULL;

	copyOnConnect=false;

	if (!pModel)
	{
		//设置默认扫描策略模型
	}

}

ApplicationWormUDP::~ApplicationWormUDP(void)
{

}

Application* 
ApplicationWormUDP::Copy() const
/*
描述：返回当前应用对象的拷贝
*/
{
	return new ApplicationWormUDP(*this);
}

//////////////////////////////////////////////////////////////////////////
//应用事件触发与驱动相关方法
//////////////////////////////////////////////////////////////////////////

void 
ApplicationWormUDP::ScheduleNextPacket()
/*
描述：构造新的TimerEvent事件，并将其加入全局事件队列
*/
{
	if (!timeoutevent) timeoutevent = new TimerEvent();
	timer.Schedule(timeoutevent, 1.0 / (double)scanspersecond, this);
}

void
ApplicationWormUDP::Timeout(TimerEvent *ev)
/*
描述：实现基类TimerHandler的Timeout方法，发送蠕虫数据包，并调用ScheduleNextPacket方法构造新的TimerEvent事件，将其加入全局事件队列
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
描述：从Application类继承的方法，启动该应用
备注：构造新的TimerEvent事件，并将其加入全局事件队列
*/
{
	if (infected)
		StartInfection();
}

void 
ApplicationWormUDP::StopApp()     // Called at time specified by Stop
/*
描述：从Application类继承的方法，停止该应用
备注：调用Timer类的Cancel函数将当前的TimerEvent事件从全局事件队列中去除
*/
{
	  if (timeoutevent) 
		  timer.Cancel(timeoutevent);
}


//////////////////////////////////////////////////////////////////////////
//响应收到数据包方法
//////////////////////////////////////////////////////////////////////////

void 
ApplicationWormUDP::Receive(Packet *p, L4Protocol *proto, Seq_t)
/*
描述：接受到蠕虫包时，即被感染，调用StartInfection方法构造新的TimerEvent事件，并将其加入全局事件队列
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
//第四层协议自身相关方法
//////////////////////////////////////////////////////////////////////////

void 
ApplicationWormUDP::AttachNode(Node* n) 
/*
描述：构造新的UDP对象，并绑定结点n到UDP协议对象，同时将该蠕虫应用绑定到UDP协议对象
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
//蠕虫传播相关方法
//////////////////////////////////////////////////////////////////////////
void 
ApplicationWormUDP::SendWormPacket()
/*
描述：调用选择下一拍扫描IP模块，然后调用UDP协议的SendTo方法，发送蠕虫包到目的IP
*/
{
	//需要调用模型生成NextIP
	IPAddr_t target = GenerateNextIPAddress(); 

	//DEBUG0((cout<<"sending worm packet to "<<IPAddr::ToDotted(target)<<endl));
	udp->SendTo(infectionlength, target, infectionport);
}


bool
ApplicationWormUDP::PacketIsWorm(Packet *)   // I am going to use this to check if packet 
/*
描述：判断接收到的数据包p是否为蠕虫包
备注：未实现，直接返回true
*/
{
	return true;
}

void 
ApplicationWormUDP::Infect()
/*
描述：当该结点被感染后，调用该方法将infected状态置为true
*/
{
	infected = true;
}


bool
ApplicationWormUDP::IsInfected()
/*
描述：返回当前结点是否被感染的状态
*/
{
	return infected;
}

void
ApplicationWormUDP::StartInfection()
/*
描述：该结点被感染后，调用ScheduleNextPacket方法构造新的TimerEvent事件，并将其加入全局事件队列
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
//扫描策略模型相关方法
//////////////////////////////////////////////////////////////////////////

IPAddr_t 
ApplicationWormUDP::GenerateNextIPAddress()
{
	if (!initialized)//如果还没有初始化扫描策略模型
	{
		//调用扫描策略模型的初始化函数进行初始化

	}
	IPAddr_t *IP = new IPAddr_t;

	pModel->GetModelValue((void**)(&IP));

	IPAddr_t thisIP = *IP;

	delete IP;

	return thisIP;
}
