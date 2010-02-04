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


// Georgia Tech Network Simulator - UDP Port Demultiplexer
// George F. Riley.  Georgia Tech, Spring 2002

// Picks the UDP instance by port number

#include <iostream>

#include "G_debug.h"
#include "udpdemux.h"
#include "node.h"
#include "protograph.h"
#include "udp.h"
#include "droppdu.h"
#include "icmp.h"
#include "simulator.h"

using namespace std;

// Define the pointer for the single tcpdemux instance
UDPDemux* UDPDemux::instance = NULL;

UDPDemux::UDPDemux()
{
  if (instance == NULL)
    { // First instance
      instance = this;
      ProtocolGraph::CreateCommon();
      DEBUG0((cout << "Inserting udpdemux, l4, proto " 
              << UDP::ProtoNumber << endl));
      ProtocolGraph::common->Insert(4, UDP::ProtoNumber, this); // Register
    }
}

void UDPDemux::DataIndication(Node* n, Packet* p, IPAddr_t ip, 
                              L3Protocol& l3, Interface* i)
{
  // We process callbacks here instead of the TCP objects, since we
  // may want a callback even if there is no matching port
  if (!n->CallCallbacks(Layer(), Proto(), PacketCallbacks::RX, p, i)) return;
  UDPHeader* pdu = (UDPHeader*)p->PeekPDU(); // Get the UDP pdu header (peek)
  UDP* udp = (UDP*)n->LookupByPort(UDP::ProtoNumber, pdu->destPort);
  DEBUG0((cout << "UDPDemux::DataInd, udp instance " << udp 
          << " dstPort " << pdu->destPort << endl));
  if (!udp)
    { // No port, discard and log
      DropPDU d("L3-NP");
      n->TracePDU(&l3, &d);
      Stats::pktsDropped++;
      // Notify ICMP of port unreachable
      IPV4Header* iphdr = (IPV4Header*)p->PeekPDU(-1);
	  //v1.01
      //ICMP::DestinationUnreachable(n, p, iphdr, pdu, ICMP::PORT_UNREACHABLE);
      delete p;
    }
  else
    {
      if (udp->extraRxDelay != 0)
        {// Need to schedule an event for later
          Scheduler::Schedule(
              new L4DelayedRxEvent(udp, n, p, ip, i),
              udp->extraRxDelay, udp);
          
        }
      else
        {
          udp->DataIndication(n, p, ip, i); // Pass to the UDP instance
        }
    }
}

// Static methods
UDPDemux* UDPDemux::Instance()
{
  return instance;
}

void UDPDemux::Initialize()
{
  if (!instance) new UDPDemux();
}
