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



// Georgia Tech Network Simulator - Layer 5 Application base class
// George F. Riley.  Georgia Tech, Spring 2002

// Define the layer 5 application interface

//2010.01.04 modified by 张笑盈

#include <iostream>

#include "application.h"
#include "simulator.h"
#include "event.h"

using namespace std;

// Application Methods

// Copy constructor
Application::Application(const Application& a)
  : deleteOnComplete(a.deleteOnComplete), copyOnConnect(a.copyOnConnect), node(a.node), pModel(a.pModel), l4proto(a.l4proto)
/*
描述：复制构造函数，所有派生类的Copy()方法使用这种构造函数创建一个新的对象拷贝
*/
{
}

Application::~Application()
/*
描述：析构函数，空
*/
{ // Destructor
	DelL4proto();
}

void Application::Start(Time_t t)
/*
描述：通过构造新的BasicAppEvent Start事件，并将该事件加入到全局事件队列来实现应用的启动
*/
{ // Schedule the Start event
  BasicAppEvent* startEvent = new BasicAppEvent(BasicAppEvent::START);
  Scheduler::Schedule(startEvent, t, this);
}

void Application::Stop(Time_t t)
/*
描述：通过构造新的BasicAppEvent Stop事件，并将该事件加入到全局事件队列来实现应用的停止
*/
{ // Schedule the Stop Event
  BasicAppEvent* stopEvent = new BasicAppEvent(BasicAppEvent::STOP);
  Scheduler::Schedule(stopEvent, t, this);
}

void Application::Handle(Event* e, Time_t)
/* 
描述：处理事件
*/
{
  BasicAppEvent* ev = (BasicAppEvent*)e;
  switch(ev->event) {
    case BasicAppEvent::START :
      StartApp(); // Call the application startup code
      break;
    case BasicAppEvent::STOP :
      StopApp();  // Call the application shutdown code
      break;
  };
  delete ev;
}

// Subclasses should over-ride all the below as required
void Application::Receive(Packet* p,L4Protocol*proto,Seq_t seq_num)
/*
描述：当接收到数据时，第4层协议会调用这个方法
参数：[IN]p:接收到的数据包的指针，不包含第四层的PDU
      [IN]proto:接收到数据的第四层协议的指针
	  [IN]seq_num:数据包的发送序列号，使用TCP协议时用到
备注：派生类的方法会重载这个方法
*/
{ // Data has been received 
  if (p) delete p;
}

void Application::Sent(Count_t c, L4Protocol* l)
/*
描述：当数据包发送出去后，由相应的第4层协议调用
参数：[IN]c:已经确认发送的数据大小
      [IN]l:和当前应用对应的第4层协议指针
备注：对TCP来说，收到确认信息时调用该方法
*/
{ // Data has been sent (and Ack'ed if reliable protocol)
}

void Application::CloseRequest(L4Protocol* l4)
/*
描述：响应关闭请求，调用第四层协议的Close()方法关闭连接
*/
{ // Connection has closed
  l4->Close();  // Just close back
}

void Application::Closed(L4Protocol*)
/*
描述：处理连接关闭后的情况
*/
{ // Connection has closed
}

void Application::ConnectionComplete(L4Protocol* p)
/*
描述:处理连接请求成功完成后的情况
*/
{ //. Connection request succeeded
}


void Application::ServConnectionComplete(L4Protocol* p)
/*
描述：处理被TCP服务器代理接受的连接成功完成的情况
*/
{ //. Connection request succeeded
}

void Application::ConnectionFailed(L4Protocol* l, bool u)
/*
描述：处理连接请求失败情况
参数：[IN]l:相应的第四层协议对象指针
      [IN]u：为连接请求失败的原因，true为终止，false为时间到
*/
{ // Connection request failed
};

bool Application::ConnectionFromPeer(L4Protocol* l, IPAddr_t ip, PortId_t p)
/*
描述：当监听的TCP收到连接请求时调用该方法
参数：[IN]l:相应的第四层协议对象指针
      [IN]ip:远程ip
	  [IN]p:连接的端口
返回值：如果可以接受一个新的连接，则返回true，否则返回false
*/
//Doc:Desc Called by an associated layer 4 protocol when a listening
//Doc:Desc TCP protocol has received a connection request.
//Doc:Arg1 A pointer to the listening TCP protocol object.
//Doc:Arg2 \IPA\ of remote peer.
//Doc:Arg3 Port number of report peer.
//Doc:Return Return true if able to accept a new connection, false if not.
{
	return true; // Accepted by default
}

void Application::AttachNode(Node* n)
/*
描述：绑定相应的第四层协议对象到结点n
*/
{ // Overridden by subclass if needed
	if(!pModel)
		pModel->AttachNode(n);
}



void 
Application::SetAppModel(const AppModel & model)
{
	pModel = model.Copy();
}


  
