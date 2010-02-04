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

// Georgia Tech Network Simulator - Layer 4 Protocol base class
// George F. Riley.  Georgia Tech, Spring 2002

// Define the layer 4 protocol interface

#ifdef HAVE_QT
#include "GUI_Defs.h"
#endif

#include "l4protocol.h"
#include "l3protocol.h"
//#include "portdemux.h"
#include "queue.h"
#include "application.h"
//#include "event.h"
#include "interface.h"

using namespace std;

L4Protocol::L4Protocol()
    : localNode(nil), peerIP(IPADDR_NONE),
      localPort(NO_PORT), peerPort(NO_PORT),
      localApplication(nil), notification(nil),
      l3Proto(IPV4::Instance()), ttl(IPV4::DefaultTTL),
      fid(0), tos(0), iFace(nil), 
      extraRxDelay(0), extraTxDelay(0),
			deleteAppOnComplete(true)
#ifdef HAVE_QT
      ,color(nil)
#endif
{
}

L4Protocol::L4Protocol(Node* n)
    : localNode(n), peerIP(IPADDR_NONE),
      localPort(NO_PORT), peerPort(NO_PORT),
      localApplication(nil), notification(nil),
      l3Proto(IPV4::Instance()), ttl(IPV4::DefaultTTL),
      fid(0), tos(0), iFace(nil), 
      extraRxDelay(0), extraTxDelay(0),
			deleteAppOnComplete(true)
#ifdef HAVE_QT
      , color(nil)
#endif
{
}

// Copy constructor
L4Protocol::L4Protocol(const L4Protocol& c)
    : Protocol(c), 
      localNode(c.localNode), peerIP(c.peerIP),
      localPort(c.localPort), peerPort(c.peerPort),
      localApplication(nil), notification(c.notification),
      l3Proto(c.l3Proto), ttl(c.ttl),
      fid(c.fid), tos(c.tos), iFace(c.iFace), 
      extraRxDelay(c.extraRxDelay), extraTxDelay(c.extraTxDelay),
			deleteAppOnComplete(true)
#ifdef HAVE_QT
      , color(c.color)
#endif
{
}

 
L4Protocol::~L4Protocol()
{
#ifdef HAVE_QT
  if (color) delete color;
#endif
  //由应用对象释放第四层对象，应用对象由simulator的析构函数释放 2010-1-29
  //if (localApplication)
  //  {
		//	if (deleteAppOnComplete)
	 //     delete localApplication;
  //  }
}

// Event Handler
void L4Protocol::Handle(Event* e, Time_t)
{
  switch(e->event)
    {
      case DELAYED_RX:
        {
          L4DelayedRxEvent* rx = (L4DelayedRxEvent*)e;
          rx->l4Proto->DataIndication(rx->node, rx->packet, 
                                      rx->ipAddr, rx->fromIF);
          DEBUG0((cout << "Processing delayed tx" << endl));
        }
        break;
      case DELAYED_TX:
        {
          L4DelayedTxEvent* tx = (L4DelayedTxEvent*)e;
          tx->l3Proto->DataRequest(tx->node, tx->packet, &tx->reqInfo);
          if (tx->notifySent && localApplication)
            {
              localApplication->Sent(tx->msgLength, this);
            }
          DEBUG0((cout << "Processing delayed tx" << endl));
        }
        break;
    }
  delete e;  // Done with this event
}

bool L4Protocol::Bind(PortId_t p)
{ // Bind to specific port
  if (!localNode) return false; // Can't bind if no node
  if (p == NO_PORT)
    { // If binding to zero
      Bind();
      return localPort != NO_PORT;
    }
  if (localNode->Bind(ProtocolNumber(), p, this))
    {
      localPort = p;
      return true;
    }
  return false;
}

bool L4Protocol::Bind()
{ // Bind to available port
  if (!localNode) return NO_PORT; // Can't bind if no node
  localPort = localNode->Bind(ProtocolNumber(), this);
  return localPort != NO_PORT;
}

bool L4Protocol::Unbind(Proto_t proto, PortId_t port) // Remove port binding
{
  if (localNode) return localNode->Unbind(proto, port, this);
  return true; 
}

void L4Protocol::SetInterface()
{ // Find the output interface for packets from this flow
  // Query the l4 protocol for the output i/f for the peer ip
  iFace = l3Proto->PeekInterface(localNode, &peerIP);
}

#ifdef HAVE_QT
void L4Protocol::SetColor(const MyColor& c)
{
  if (!color)
    { // No existing color, create it
      color = new MyColor(c);
    }
  else
    {
      *color = c;
    }
}
#endif

void    L4Protocol::AddNotification(NotifyHandler* n)
{
  DEBUG0((cout << "L4Proto " << this << " adding notif " << n << endl));
  notification = n;
}

Packet* L4Protocol::NewPacket()
{
  Packet* p = nil;
  
#ifdef HAVE_QT
  if (IsColored()) p = new ColoredPacket(*color);
#endif
  if (!p) p = new Packet();
  p->notification = notification;
  DEBUG0((cout << "l4proto " << this
          << " newPacket, notif " << notification << endl));
  return p;
}

void L4Protocol::AddExtraRxDelay(Time_t d)
{
  extraRxDelay = d;
}

  
void L4Protocol::AddExtraTxDelay(Time_t d)
{
  extraTxDelay = d;
}

  
void L4Protocol::RequestNotification(NotifyHandler* h, void* v)
{
  if (!iFace) SetInterface();  // Get the interface pointer
  if (!iFace) return;          // Packets cannot be routed
  iFace->AddNotify(h, v);
}

void L4Protocol::CancelNotification(NotifyHandler* h)
{
  if (!iFace) return;          // Packets cannot be routed
  DEBUG0((cout << "iFace " << iFace
          << " cancel notif " << h << endl));
  iFace->CancelNotify(h);
}

bool L4Protocol::BufferAvailable(Size_t s, Packet* p)
{ // True if output link buffer avail.
  if (!iFace) SetInterface();
  if (!iFace) return true; // Cannot be found

  Queue* q = iFace->GetQueue();
  if (!q) return true;     // Should not happen
  DEBUG0((cout << "L4proto, BA, limit " << q->GetLimit() 
          << " length " << q->Length() 
          << " avail " << q->GetLimit() - q->Length()
          << endl));
  
  return q->Check(s,p);    // True if sufficient buffer space available
}




