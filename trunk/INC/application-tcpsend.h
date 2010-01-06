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

#ifndef __tcp_application_h__
#define __tcp_application_h__

#include "G_common-defs.h"
#include "application-tcp.h"
#include "tcp.h"
#include "rng.h"

// Define the start/stop events
class AppTCPEvent : public Event {
public:
  typedef enum { SEND_DATA = 100 } AppTCPEvent_t;
  AppTCPEvent() { }
  AppTCPEvent(Event_t ev) : Event(ev) { }
};

//Doc:ClassXRef
class TCPSend : public TCPApplication {
  //Doc:Class Class {\tt TCPSend} creates an application that sends a
  //Doc:Class random amount of data to a {\tt TCP} server.  The application
  //Doc:Class will optionally sleep for a random amount of time and
  //Doc:Class send some more data, up to a user specified limit on the
  //Doc:Class number of sending iterations.

public:
  // Constructor
  // TCP Type, peerIp, peerPort, RandomSize, Random Sleep
  //Doc:Method
  TCPSend(IPAddr_t, PortId_t,
          const Random&,
          const TCP& = TCP::Default(), 
          const Random& sleepTime = Constant(0),
          Count_t loopCount = 1);
    //Doc:Desc Constructor for the {\tt TCPSend} application.
    //Doc:Arg1 A reference to any {\tt TCP} variant to use for the {\tt TCP}
    //Doc:Arg1 endpoint.
    //Doc:Arg2 \IPA\ of the {\tt TCPServer} to connect to.
    //Doc:Arg3 Port number of the of the {\tt TCPServer} to connect to.
    //Doc:Arg4 Random variable specifying amount of data to send each
    //Doc:Arg4 iteration.
    //Doc:Arg4 Use a {\tt Constant} random variable if a fixed size is desired.
    //Doc:Arg5 Random variable specifying the sleep time between sending
    //Doc:Arg5 iterations.
    //Doc:Arg6 Limit on the number of times the data is sent.

  //Doc:Method
  TCPSend(const TCPSend& c);     // Copy constructor
    //Doc:Desc Copy Constructor.
    //Doc:Arg1 {\tt TCPSend} object to copy.

  // Destructor
  ~TCPSend();

  void Handle(Event*, Time_t);
  // Upcalls from L4 protocol
  virtual void Receive(Packet*,L4Protocol*, Seq_t = 0);// Data received
  virtual void Sent(Count_t, L4Protocol*);     // Data has been sent
  virtual void CloseRequest(L4Protocol*);      // Close request from peer
  virtual void Closed(L4Protocol*);            // Connection has closed
  virtual void ConnectionComplete(L4Protocol*);// Connection request succeeded
  virtual void ConnectionFailed(L4Protocol*,bool);// Connection request failed
  // Called from base application class
  virtual void StartApp();           // Called at time specified by Start
  virtual void StopApp();            // Called at time specified by Stop
  virtual Application* Copy() const; // Make a copy of the application
  // Debug
  void DBDump(); 
public:
  IPAddr_t      peerIP;       // PeerIP
  PortId_t      peerPort;     // Peer port
  Random*       sizeRV;       // Random variable for size
  Random*       sleepRV;      // Random variable for sleep time
  Count_t       lCount;       // Loop Count
  Count_t       rCount;       // Number of times repeated
  AppTCPEvent*  pendingEvent; // Pending SendData event
  Count_t       sent;         // Bytes set this loop
  Count_t       sentAck;      // Bytes acknowledged this loop
  bool          connected;    // True if already connected
  // all below is debug
  bool          ended;        // True if all data sent
  IPAddr_t      srcip;
  IPAddr_t      dstip;
  PortId_t      srcPort;
  PortId_t      dstPort;
protected:
  virtual void SendData();    // Send the (next) data block
public:
  //Doc:Member
  static Count_t totalStarted;// Number of TCP applications started
    //Doc:Desc Count of total number of {\tt TCPSend} applications started.

  //Doc:Member
  static Count_t totalEnded;  // Number of TCP applications ended
    //Doc:Desc Count of the total number of {\tt TCPSend} applications  ended.
};

#endif

