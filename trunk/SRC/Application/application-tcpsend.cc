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



// Georgia Tech Network Simulator - TCP Bulk Data Transfer Application class
// George F. Riley.  Georgia Tech, Spring 2002

// Define a TCP Bulk Data class

// Uncomment below to enable debug level 0
//#define DEBUG_MASK 0x1
#include "G_debug.h"
#include "application-tcpsend.h"
#include "tcp.h"
#include "simulator.h"
#include "G_globalstats.h"

using namespace std;

// Static members
Count_t TCPSend::totalStarted = 0;// Number of TCP applications started
Count_t TCPSend::totalEnded = 0;  // Number of TCP applications ended

// Constructors
// TCP Type, peerIp, peerPort, RandomSize, Random Sleep
TCPSend::TCPSend(IPAddr_t ip, PortId_t p, 
                 const Random& szRv, 
                 const TCP& t, 
                 const Random& sleepTime,
                 Count_t loopCount)
  : TCPApplication(t), peerIP(ip), peerPort(p),
    sizeRV(szRv.Copy()), sleepRV(sleepTime.Copy()), 
    lCount(loopCount), rCount(0), pendingEvent(0),
    sent(0), sentAck(0),
    connected(false), ended(false)
{
}

TCPSend::TCPSend(const TCPSend& c)
  : TCPApplication(c),
    peerIP(c.peerIP), peerPort(c.peerPort),
    sizeRV(c.sizeRV->Copy()), sleepRV(c.sleepRV->Copy()),
    lCount(c.lCount), rCount(c.rCount),
    pendingEvent(0), sent(0), sentAck(0),
    connected(c.connected), ended(c.ended)
{ // Copy constructor
}

TCPSend::~TCPSend()
{
  delete sizeRV;
  delete sleepRV;
}

// Handler Methods
void TCPSend::Handle(Event* e, Time_t t)
{
  AppTCPEvent* ev = (AppTCPEvent*)e;
  switch (ev->event) {
    case AppTCPEvent::SEND_DATA :
      DEBUG0((cout << "Send Data Event" << endl));
      SendData();
      delete e;
      return;
    default :
      break;
  }
  Application::Handle(e, t);  // May be application event
}

  // Upcalls from L4 protocol
void TCPSend::Receive(Packet*,L4Protocol*,Seq_t)
{ // Data received (ignored)
}

void TCPSend::Sent(Count_t c, L4Protocol*)
{ // Data has been sent
  if (!c) return;
  sentAck += c; // Note received
  DEBUG0((cout << "Application::Sent c " << c << " sentAck " << sentAck
          << " sent " << sent << endl));
  if (sentAck >= sent)
    { // All sent, schedule sleep event if more loops
      DEBUG(0,(cout << "All sent, acked " << sentAck 
               << " send " << sent
               << " rCount " << rCount 
               << " lCount " << lCount << endl));
      if ( ++rCount < lCount)
        { // Need wakeup event
          AppTCPEvent* ev = new AppTCPEvent(AppTCPEvent::SEND_DATA);
          pendingEvent = ev;
          // Schedule again    
          Time_t st = sleepRV->Value(); // Sleep time
          DEBUG0((cout << "Sleeping for " << st << " secs" << endl));
          cout << "Sleeping for " << st << " secs" << endl;
          Scheduler::Schedule(ev, st, this);
        }
      else
        { // No (more) repeats, just close
          DEBUG0((cout << "Closing" << endl));
          l4Proto->Close();
          totalEnded++;
          ended = true;
          DEBUG0((cout << "TCP App ended " << totalEnded
                  << " of " << totalStarted 
                  << " mem " << Simulator::instance->ReportMemoryUsageMB()
                  << "Mb" 
                  << " evlSize " << Scheduler::Instance()->EventListSize() 
                  << " pkts " << Stats::pktsAllocated - Stats::pktsDeleted
                  << endl));
        }
    }
}

void TCPSend::CloseRequest(L4Protocol*)
{ // Close request from peer
  cout << "TCPSend::CloseRequest() " << endl;
  l4Proto->Close(); // Close in response
  Delete();  // Delete the application object 
}

void TCPSend::Closed(L4Protocol*)
{ // Connection has closed
  // Should we delete the TCP object here?
  DEBUG0((cout << "TCPSend::Closed() " << endl));
  Delete();// Delete the application object    
}

void TCPSend::ConnectionComplete(L4Protocol*)
{ // Connection request succeeded
  connected = true;
  DEBUG0((cout << "TCPSend::ConnectionComplete tcp " << l4Proto << endl));
  SendData(); // Connection successful, send the data
}

void TCPSend::ConnectionFailed(L4Protocol* l4, bool aborted)
{ // Connection request failed
  // Possibly print a debug msg
  IPAddr_t peer = l4->peerIP;
  if (aborted)
    {
      cout << "TCPSend::ConnectionAborted, peer " 
           << (string)IPAddr(peer) << endl;
    }
  else
    {
      cout << "TCPSend::ConnectionFailed, peer " 
           << (string)IPAddr(peer) << endl;
    }
  totalEnded++; // Count as ended
}

// Application Methods
void TCPSend::StartApp()    // Called at time specified by Start
{
  StopApp();                       // Insure no pending event
  SendData();                      // Send the data block
  totalStarted++;                  // Count started apps
  DEBUG0((cout << "TCP App started " << totalStarted 
          << " mem " << Simulator::instance->ReportMemoryUsageMB() 
          << "Mb" << " evlSize " << Scheduler::Instance()->EventListSize() 
          << " time " << Simulator::Now()
          << endl));
}

void TCPSend::StopApp()     // Called at time specified by Stop
{
  if (pendingEvent)
    { // Cancel the pending transmit event
      Scheduler::Cancel(pendingEvent);
      delete pendingEvent;
      pendingEvent = NULL;
    }
}

Application* TCPSend::Copy() const
{ // Make a copy of this application
  return new TCPSend(*this);
}

void TCPSend::DBDump()
{
  if (!ended)
    {
      cout << "TCPSend " << this 
           << " ended " << ended
           << " sip " << (string)IPAddr(srcip)
           << " spt " << srcPort
           << " rip " << (string)IPAddr(dstip)
           << " rpt " << dstPort
           << " sent " << sent
           << " sentAck " << sentAck
           << endl;
    }
}

// Private Methods

void TCPSend::SendData()
{
  if (!connected)
    {
      DEBUG0((cout << "TCPSend::SendData not connected, tcp " << l4Proto << endl));
      l4Proto->Connect(peerIP, peerPort);
      return;
    }
  sent = (Count_t)sizeRV->Value();  // Number of bytes to send
  if (sent == 0) sent++;   // Never 0
  sentAck = 0;                 // Reset sent count
  Data d(sent);                // Message size
  d.msgSize = sent;            // Message size
  d.responseSize = 0;          // Response size
  l4Proto->Send(d);            // Send the data
}

