//Copyright (c) 2010, Information Security Institute of Wuhan Universtiy(ISIWhu)
//Project Homepage:http://code.google.com/p/whutnetsim/
//corresponding author's email: guochi@mail.whu.edu.cn


//All rights reserved
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
//Construct Date:
//Modify Author:
//Author Organization:
//Modify Date:



// Georgia Tech Network Simulator - TCP Server Application class
// George F. Riley.  Georgia Tech, Spring 2002

// Define a TCP Server Application Class
// Just sends back the amount of data specified in datapdu, "responseSize"

// Uncomment below to enable debug
//#define DEBUG_MASK 0x01
#include "G_debug.h"
#include "application-tcpserver.h"
#include "tcp.h"
#include "simulator.h"
#include "G_globalstats.h"

using namespace std;

// Constructors
// TCP Type
TCPServer::TCPServer(const TCP& t)
  : TCPApplication(t),
    totSent(0), totAck(0), totRx(0),
    stats(nil), msgStart(0.0), timeKnown(false)
{
  TCP* tcp = (TCP*)GetL4();
  tcp->isServer = true;
  tcp->Listen(); // Listen
}

TCPServer::TCPServer(const TCPServer& c)
  : TCPApplication(c),
    totSent(c.totSent), totAck(c.totAck), totRx(c.totRx),
    stats(c.stats), msgStart(c.msgStart), timeKnown(c.timeKnown)
{ // Copy constructor
}


  // Upcalls from L4 protocol
void TCPServer::Receive(Packet* p, L4Protocol*,Seq_t)
{ // Data received
  Data* d = (Data*)p->PopPDU();  // Get the data pdu
  totRx += d->Size();            // Log bytes received
  DEBUG0((cout << "AppTCPServer " << this << " RxBytes " << d->Size()
          << " totRx " << totRx << " msgsize " << d->msgSize << endl));
  if (!timeKnown)
    { // Note start time
      msgStart = p->time;
      timeKnown = true;
    }
  if (totRx >= d->msgSize)
    {
      totRx -= d->msgSize;       // Subtract for next message
      totSent = d->responseSize; // Number of bytes to sent     
      totAck = 0;                // Clear ack count     
      if (closeOnEmpty)
        {
          l4Proto->CloseOnEmpty(true);
        }
      DEBUG0((cout << "AppTCPServer " << this << " responding with "
              << totSent << " bytes " << endl));
      if (stats)
        { // Record response time
          stats->Record(Simulator::Now() - msgStart);
        }
      timeKnown = false;
      if (totRx)
        { // If got partial of next
          msgStart = p->time;
          timeKnown = true;
        }
      if (totSent)
        {
          DEBUG0((cout << "AppTS " << this 
                  << " Rq " << d->msgSize
                  << " Rx " << d->responseSize
                  << " peer " << ((TCP*)l4Proto)->Peer() << endl));
          l4Proto->Send(totSent);// Send the response   
        }
    }
  delete p;                      // Delete the packet
}

void TCPServer::Sent(Count_t c, L4Protocol*)
{ // Data has been sent
  DEBUG0((cout << "TCPServer " << this 
          << " sent c " << c << " totAck " << totAck
          << " totSent " << totSent << endl));
  totAck += c; // Note received
  if (totAck >= totSent)
    { // All sent, see if we close on empty
      if (closeOnEmpty)
        {
          l4Proto->Close();
        }
    }
}

void TCPServer::CloseRequest(L4Protocol*)
{ // Close request from peer
  DEBUG0((cout << "TCPServer::CloseRequest() " << endl));
  l4Proto->Close(); // Close in response
  Delete();// Delete this application
}

void TCPServer::Closed(L4Protocol*)
{ // Connection has closed
  // Should we delete the TCP object here?
  DEBUG0((cout << "TCPServer::Closed() " << this 
          << " doc " << deleteOnComplete << endl));
  Delete();// Delete this application
}

void TCPServer::BindAndListen(Node* n, PortId_t port)
{ // Note attached node and listen on specified port
  l4Proto->Attach(n);
  l4Proto->Bind(port);
  ((TCP*)l4Proto)->Listen();
}

void TCPServer::BindAndListen(PortId_t port)
{ // Listen on specified port
  Node* n = l4Proto->localNode;
  if (!n)
    {
      cout << "HuH? BindAndListen with no attached node" << endl;
      exit(1);
    }
  BindAndListen(n, port);
}

Application* TCPServer::Copy() const
{ // Make a copy of this application
  return new TCPServer(*this);
}

