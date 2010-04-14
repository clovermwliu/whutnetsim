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


// Georgia Tech Network Simulator - TCP Server Application class
// George F. Riley.  Georgia Tech, Summer 2002

// Define a TCP base class for all appliations that use a single TCP L4Protocol
// Saves some code duplication the "application-tcp*.cc" classes

#ifndef __application_tcp_h__
#define __application_tcp_h__

#include "G_common_defs.h"
#include "application.h"
#include "tcp.h"

class TCP;

//Doc:ClassXRef
class TCPApplication : public Application {
  //Doc:Class Class {\tt TCPApplication} defines a virtual base class that 
  //Doc:Class can be subclassed by any application that has a single {\tt TCP}
  //Doc:Class connection endpoint.
public:
  //Doc:Method
  TCPApplication(const TCP& t);
  //Doc:Class Constructor for {\tt TCPApplication}.
    //Doc:Arg1 Reference to any {\tt TCP} variation to use for the
    //Doc:Arg1 {\tt TCP} connection.

  //Doc:Method
  TCPApplication(const TCPApplication& c);
    //Doc:Desc Copy Constructor.
    //Doc:Arg1 {\tt TCPApplication} to copy.

  virtual ~TCPApplication();
  virtual Application* Copy() const = 0;  // Make a copy of this application

  //Doc:Method
  virtual void AttachNode(Node*); // Note which node attached to
    //Doc:Desc Specify which node to which this application is attached.
    //Doc:Arg1 Node pointer to attached node.

  //Doc:Method
  L4Protocol* GetL4() const { return l4Proto;}
    //Doc:Desc Get a pointer to the single {\tt TCP} object for this
    //Doc:Desc application.
    //Doc:Return Pointer to associated {\tt TCP} object.

  //Doc:Method
  void        Bind(PortId_t p);
    //Doc:Desc Bind the {\tt TCP} object to a specified port.
    //Doc:Arg1 Port number to bind.  Specify zero if an available port is to
    //Doc:Arg1 be chosen.

  //Doc:Method
  void        CloseOnEmpty(bool coe = true) { closeOnEmpty = coe;}
    //Doc:Desc Specify that the {\tt TCP} object should close the connection
    //Doc:Desc automatically when all pending data has been sent.
    //Doc:Arg1 True of close on empty desired.

  //Doc:Method
  void        SetTrace(Trace::TraceStatus);
    //Doc:Desc Set the trace status of the associated {\tt TCP} endpoint.
    //Doc:Arg1 Trace status desired.

protected:
  void        Delete();          // Delete this application
public:
  TCP*        l4Proto;
  bool        closeOnEmpty;
};

#endif