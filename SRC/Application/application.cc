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

//2010.01.04 modified by ��Цӯ

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
���������ƹ��캯���������������Copy()����ʹ�����ֹ��캯������һ���µĶ��󿽱�
*/
{
}

Application::~Application()
/*
������������������
*/
{ // Destructor
	DelL4proto();
}

void Application::Start(Time_t t)
/*
������ͨ�������µ�BasicAppEvent Start�¼����������¼����뵽ȫ���¼�������ʵ��Ӧ�õ�����
*/
{ // Schedule the Start event
  BasicAppEvent* startEvent = new BasicAppEvent(BasicAppEvent::START);
  Scheduler::Schedule(startEvent, t, this);
}

void Application::Stop(Time_t t)
/*
������ͨ�������µ�BasicAppEvent Stop�¼����������¼����뵽ȫ���¼�������ʵ��Ӧ�õ�ֹͣ
*/
{ // Schedule the Stop Event
  BasicAppEvent* stopEvent = new BasicAppEvent(BasicAppEvent::STOP);
  Scheduler::Schedule(stopEvent, t, this);
}

void Application::Handle(Event* e, Time_t)
/* 
�����������¼�
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
�����������յ�����ʱ����4��Э�������������
������[IN]p:���յ������ݰ���ָ�룬���������Ĳ��PDU
      [IN]proto:���յ����ݵĵ��Ĳ�Э���ָ��
	  [IN]seq_num:���ݰ��ķ������кţ�ʹ��TCPЭ��ʱ�õ�
��ע��������ķ����������������
*/
{ // Data has been received 
  if (p) delete p;
}

void Application::Sent(Count_t c, L4Protocol* l)
/*
�����������ݰ����ͳ�ȥ������Ӧ�ĵ�4��Э�����
������[IN]c:�Ѿ�ȷ�Ϸ��͵����ݴ�С
      [IN]l:�͵�ǰӦ�ö�Ӧ�ĵ�4��Э��ָ��
��ע����TCP��˵���յ�ȷ����Ϣʱ���ø÷���
*/
{ // Data has been sent (and Ack'ed if reliable protocol)
}

void Application::CloseRequest(L4Protocol* l4)
/*
��������Ӧ�ر����󣬵��õ��Ĳ�Э���Close()�����ر�����
*/
{ // Connection has closed
  l4->Close();  // Just close back
}

void Application::Closed(L4Protocol*)
/*
�������������ӹرպ�����
*/
{ // Connection has closed
}

void Application::ConnectionComplete(L4Protocol* p)
/*
����:������������ɹ���ɺ�����
*/
{ //. Connection request succeeded
}


void Application::ServConnectionComplete(L4Protocol* p)
/*
����������TCP������������ܵ����ӳɹ���ɵ����
*/
{ //. Connection request succeeded
}

void Application::ConnectionFailed(L4Protocol* l, bool u)
/*
������������������ʧ�����
������[IN]l:��Ӧ�ĵ��Ĳ�Э�����ָ��
      [IN]u��Ϊ��������ʧ�ܵ�ԭ��trueΪ��ֹ��falseΪʱ�䵽
*/
{ // Connection request failed
};

bool Application::ConnectionFromPeer(L4Protocol* l, IPAddr_t ip, PortId_t p)
/*
��������������TCP�յ���������ʱ���ø÷���
������[IN]l:��Ӧ�ĵ��Ĳ�Э�����ָ��
      [IN]ip:Զ��ip
	  [IN]p:���ӵĶ˿�
����ֵ��������Խ���һ���µ����ӣ��򷵻�true�����򷵻�false
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
����������Ӧ�ĵ��Ĳ�Э����󵽽��n
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


  
