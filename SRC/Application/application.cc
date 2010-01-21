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

#include <iostream>

#include "application.h"
#include "simulator.h"
#include "event.h"

using namespace std;

// Application Methods

// Copy constructor
Application::Application(const Application& a)
  : deleteOnComplete(a.deleteOnComplete), copyOnConnect(a.copyOnConnect)
{
}

Application::~Application()
{ // Destructor
}

void Application::Start(Time_t t)
{ // Schedule the Start event
  ApplicationEvent* startEvent = new ApplicationEvent(ApplicationEvent::START);
  Scheduler::Schedule(startEvent, t, this);
}

void Application::Stop(Time_t t)
{ // Schedule the Stop Event
  ApplicationEvent* stopEvent = new ApplicationEvent(ApplicationEvent::STOP);
  Scheduler::Schedule(stopEvent, t, this);
}

void Application::Handle(Event* e, Time_t)
{
  ApplicationEvent* ev = (ApplicationEvent*)e;
  switch(ev->event) {
    case ApplicationEvent::START :
      StartApp(); // Call the application startup code
      break;
    case ApplicationEvent::STOP :
      StopApp();  // Call the application shutdown code
      break;
  };
  delete ev;
}

// Subclasses should over-ride all the below as required
void Application::Receive(Packet* p,L4Protocol*,Seq_t)
{ // Data has been received 
  if (p) delete p;
}

void Application::Sent(Count_t, L4Protocol*)
{ // Data has been sent (and Ack'ed if reliable protocol)
}

void Application::CloseRequest(L4Protocol* l4)
{ // Connection has closed
  l4->Close();  // Just close back
}

void Application::Closed(L4Protocol*)
{ // Connection has closed
}

void Application::ConnectionComplete(L4Protocol* p)
{ //. Connection request succeeded
}

void Application::ServConnectionComplete(L4Protocol* p)
{ //. Connection request succeeded
}

void Application::ConnectionFailed(L4Protocol*, bool)
{ // Connection request failed
};

bool Application::ConnectionFromPeer(L4Protocol*, IPAddr_t, PortId_t)
{
  return true; // Accepted by default
}

void Application::AttachNode(Node*)
{ // Overridden by subclass if needed
}

  
