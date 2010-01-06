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
// George F. Riley.  Georgia Tech, Summer 2002

// Define a TCP Server Application Class
// Just sends back the amount of data specified in datapdu, "responseSize"

#ifndef __tcp_appserver_h__
#define __tcp_appserver_h__

#include "G_common-defs.h"
#include "application-tcp.h"
#include "statistics.h"
#include "tcp.h"
#include "rng.h"

//Doc:ClassXRef
class TCPServer : public TCPApplication {
  //Doc:Class Class {\tt TCPServer} creates a simple model of a
  //Doc:Class request/response
  //Doc:Class {\tt TCP} server.  The server binds to a specified port, and
  //Doc:Class listens for connection requests from {\tt TCP} peers.  The 
  //Doc:Class data received from the peers specifies how much data to send
  //Doc:Class or receive.

public:
  // Constructor
  //Doc:Method
  TCPServer(const TCP& = TCP::Default());   // TCP Type
    //Doc:Desc Constructor for {\tt TCPServer} objects.  Optionally
    //Doc:Desc specify a reference to any {\tt TCP} variant to use
    //Doc:Desc for the listening {\tt TCP} endpoint.
    //Doc:Arg1 A reference to any {\tt TCP} variant to use for
    //Doc:Arg1 the listening endpoint.

  //Doc:Method
  TCPServer(const TCPServer& c);               // Copy constructor
    //Doc:Desc Copy constructor.
    //Doc:Arg1 {\tt TCPServer} object to copy.

  // Upcalls from L4 protocol
  virtual void Receive(Packet*,L4Protocol*,Seq_t);   // Data received
  virtual void Sent(Count_t, L4Protocol*);     // Data has been sent
  virtual void CloseRequest(L4Protocol*);      // Close request from peer
  virtual void Closed(L4Protocol*);            // Connection has closed

  // Specific to TCPServer app
  //Doc:Method
  virtual void BindAndListen(Node*, PortId_t); // Bind to node/port
    //Doc:Desc Bind the application to the specified port and start 
    //Doc:Desc listening for connections.
    //Doc:Arg1 Node pointer to the node this application is associated with.
    //Doc:Arg2 Port number to bind to.

  //Doc:Method
  virtual void BindAndListen(PortId_t); // Bind to node/port
    //Doc:Desc Bind the application to the specified port and start 
    //Doc:Desc listening for connections.  Node is already known.
    //Doc:Arg1 Port number to bind to.

  virtual Application* Copy() const;           // Make a copy of application
  //Doc:Method
  void SetStatistics(Statistics* s) { stats = s;}
    //Doc:Desc Set a statistics object to use for statistics collection.
    //Doc:Desc This application will record the response time of each
    //Doc:Desc message sent to the statistics object.
    //Doc:Arg1 Pointer to a statistics object.

public:
  //Doc:Member
  Count_t       totSent;      // Total bytes sent
    //Doc:Desc Count of the total number of bytes sent by this application.

  //Doc:Member
  Count_t       totAck;       // Total bytes acknowledged
    //Doc:Desc Count of total bytes acknowledged by this application.

  //Doc:Member
  Count_t       totRx;        // Total bytes received
    //Doc:Desc Count of total bytes received by this application.
  Statistics*   stats;        // Stats object for logging response time

  //Doc:Member
  Time_t        msgStart;     // Time message request started
    //Doc:Desc Time the last message started transmission or receipt.

  //Doc:Member
  bool          timeKnown;    // True if above time is known
    //Doc:Desc True if the start time of the message is known.
};

#endif

