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

// Define a TCP base class for all appliations that use a single TCP L4Protocol
// Saves some code duplication the "application-tcp*.cc" classes

//#define DEBUG_MASK 0x01
#include "G_debug.h"
#include "application-tcp.h"
#include "l4protocol.h"
#include "tcp.h"
#include "simulator.h"

using namespace std;

// Constructor
TCPApplication::TCPApplication(const TCP& t)
  : l4Proto((TCP*)t.Copy()), closeOnEmpty(false)
{ 
  l4Proto->AttachApplication(this);
}

// Copy constructor
TCPApplication::TCPApplication(const TCPApplication& c)
  : Application(c), l4Proto((TCP*)(c.l4Proto->Copy())),
    closeOnEmpty(c.closeOnEmpty)
{
  l4Proto->AttachApplication(this);
}

// Destructor
TCPApplication::~TCPApplication()
{
#ifdef DONT_DELETE_THIS
  if (l4Proto)
    { // Don't delete if bound to a port.  Will self delete later
      // when timeout period is done
      if (l4Proto->localPort == NO_PORT)
        delete l4Proto;
    }
#endif
}

void TCPApplication::Bind(PortId_t p)
{
  GetL4()->Bind(p);
}

void TCPApplication::SetTrace(Trace::TraceStatus ts)
{ // Set the trace status of layer 4 protocol
  if (l4Proto) l4Proto->SetTrace(ts);
}

void TCPApplication::AttachNode(Node* n)
{ // Note attached node
  if (l4Proto) l4Proto->Attach(n);
}

// Private methods
void TCPApplication::Delete()
{ // Delete this application and remove connection to the TCP protocol object
  if (deleteOnComplete)
    {
      GetL4()->AttachApplication(nil);
      Simulator::instance->DeleteObject(this);
      DEBUG0((cout << "TCPApplication " << this
              << " deleting self" << endl));
    }
}
