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



// Georgia Tech Network Simulator - TCP-Tahoe Class
// George F. Riley.  Georgia Tech, Spring 2002

#include <iostream>

//#define DEBUG_MASK 0x07
#include "G_debug.h"
#include "tcp-tahoe.h"
#include "simulator.h"
#include "G_globalstats.h"

using namespace std;

// Constructors
TCPTahoe::TCPTahoe() : TCP() 
{
}

TCPTahoe::TCPTahoe(Node* n) : TCP(n) 
{
}

TCPTahoe::TCPTahoe(const TCPTahoe& c) : TCP(c)
{
}

TCPTahoe::~TCPTahoe()
{
  DEBUG0((cout << "Deleting tcptahoe " << this << endl));
}

// L4Protocol Methods
L4Protocol* TCPTahoe::Copy() const
{
  return new TCPTahoe(*this); // Create a copy of this protocol
}

// TCP Overridden methods
void TCPTahoe::NewAck(Seq seq)
{ // New acknowledgement up to sequence number "seq"
  // Adjust congestion window in response to new ack's received
  DEBUG(3,(cout << "TCP " << this << " NewAck "
           << " seq " << seq
           << " cwnd " << cWnd
           << " ssThresh " << ssThresh << endl));
  if (cWnd < ssThresh)
    { // Slow start mode, add one segSize to cWnd
      DEBUG(6,(cout << "NewCWnd SlowStart, cWnd " << cWnd 
               << " sst " << ssThresh << endl));
      cWnd += segSize;
    }
  else
    { // Congestion avoidance mode, adjust by (ackBytes*segSize) / cWnd
      // Changed by GFR to match RFC2581
      Mult_t adder =  ((Mult_t)segSize * segSize) / cWnd;
      if (adder < 1.0) adder = 1.0;
      cWnd += (Count_t)adder;
    }
  NoteTimeSeq(LOG_CWIN, (Seq_t)cWnd); // Log the change if cwin logging enabled
  CommonNewAck(seq, false);           // Complete newAck processing
}

  
void TCPTahoe::DupAck(const TCPHeader& t, Count_t c)
{ // Dup ack received
  DEBUG(1,(cout << "TCPTahoe::DupAck " << t.ackNumber
           << ", count " << c
           << ", time " << Simulator::Now()
           << endl));
  NoteTimeSeq(LOG_DUPACK, (Seq_t)c);
  if (c == 3)
    { // Count of three indicates triple duplicate ack
      ssThresh = Window() / 2; // Per RFC2581
      ssThresh = max(ssThresh, 2 * segSize);
      DEBUG0((cout << "Tahoe TDA, time " << Simulator::Now()
              << " seq " << t.ackNumber
              << " in flight " << BytesInFlight()
              << " new ssthresh " << ssThresh << endl));
      NoteTimeSeq(LOG_SSTHRESH, (Seq_t)ssThresh); // Log if ssthresh logging

      cWnd = initialCWnd * segSize;// Collapse cwnd (re-enter slowstart)
      NoteTimeSeq(LOG_CWIN, (Seq_t)cWnd);
      DEBUG(1,(cout << "Retransmitting" << endl));
      DEBUG(2,(cout << "TCP " << this << " retx, tripledup ack" 
               << t.ackNumber << endl));
      // For Tahoe, we also reset nextTxSeq
      nextTxSeq = highestRxAck;
      SendPendingData();
      //Retransmit();        // Retransmit the packet
      Stats::tcpDupAckRetx++;
    }
}

void TCPTahoe::ReTxTimeout()
{ // Retransmit timeout
  DEBUG0((cout << "TCPTahoe::Timeout" 
           << ", time " << Simulator::Now()
          << endl));
  ssThresh = Window() / 2; // Per RFC2581
  ssThresh = max(ssThresh, 2 * segSize);
  DEBUG0((cout << "Tahoe TxTo, time " << Simulator::Now()
          << " in flight " << BytesInFlight()
          << " new ssthresh " << ssThresh << endl));
  NoteTimeSeq(LOG_SSTHRESH, (Seq_t)ssThresh); // Log if ssthresh logging
  // Set cWnd to segSize on timeout,  per rfc2581
  cWnd = segSize;           // Collapse congestion window (re-enter slowstart)
  NoteTimeSeq(LOG_CWIN, (Seq_t)cWnd);
  nextTxSeq = highestRxAck; // Start from highest Ack
  rtt->IncreaseMultiplier(); // Double timeout value for next retx timer
  DEBUG0((cout << "RTT Timeout is " << rtt->RetransmitTimeout() << endl));
  Retransmit();             // Retransmit the packet
}


