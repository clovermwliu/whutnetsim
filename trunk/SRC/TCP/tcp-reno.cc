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



// Georgia Tech Network Simulator - TCP-Reno Class
// George F. Riley.  Georgia Tech, Spring 2002

#include <iostream>

//#define DEBUG_MASK 0x80
#include "G_debug.h"
#include "tcp-reno.h"
//#include "simulator.h"

using namespace std;

// Constructors
TCPReno::TCPReno() : TCP() 
{
}

TCPReno::TCPReno(Node* n) : TCP(n) 
{
}

TCPReno::TCPReno(const TCPReno& c) : TCP(c)
{
}

// L4Protocol Methods
L4Protocol* TCPReno::Copy() const
{
  return new TCPReno(*this); // Create a copy of this protocol
}

// TCP Overridden methods
void TCPReno::NewAck(Seq seq)
{ // NewAck is called only for "New" (non-duplicate) acks
  DEBUG(7,(cout << "TCPReno NewCWnd enter"
           << " time " << Simulator::Now()
           << " cwin " << cWnd
           << " ssThresh " << ssThresh << endl));
  if (fastRecovery)
    { // If in fast recovery and have a new data ack, reset cWnd
      // to  the ssthresh calculated when we entered fast recovery
      // and exit fast recovery mode
      cWnd = ssThresh;
      fastRecovery = false;
    }
  else if (cWnd < ssThresh)
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
  DEBUG(7,(cout << "TCPReno NewCWnd exit "
           << " time " << Simulator::Now()
           << " cwin " << cWnd
           << " ssThresh " << ssThresh << endl));
}

void TCPReno::DupAck(const TCPHeader& t, Count_t c)
{ // Dup ack received
  //cout << "Reno DupAck " << t.ackNumber << " at time " << Simulator::Now() << endl;
  DEBUG(7,(cout << "TCPReno::DupAck " << t.ackNumber
           << ", count " << c
           << ", time " << Simulator::Now()
           << endl));
  NoteTimeSeq(LOG_DUPACK, (Seq_t)c);
  if (fastRecovery)
    {
      cWnd += segSize;   // Inflate the cWin for every additional DupAck
      DEBUG(7,(cout << "TCPReno::DupAck, new cWnd " << cWnd << endl));
      SendPendingData(); // Try to send data if permitted by cWin size
    }
  else if (c == 3)
    { // Count of three indicates triple dupack has been received
      DEBUG(7, (cout << "Reno TDA enter, time " << Simulator::Now()
                << " ackno " << t.ackNumber
                << " inflight " << Window()
                << " cWnd " << cWnd
                << " ssthresh " << ssThresh << endl));
      fastRecovery = true; // Enter the fast recovery mode
      ssThresh = Window() / 2; // Per RFC2581
      ssThresh = max(ssThresh, 2 * segSize);
      NoteTimeSeq(LOG_SSTHRESH, (Seq_t)ssThresh); // Log if ssthresh logging
      cWnd = ssThresh + 3*segSize;    // Reset cWnd per rfc2581
      DEBUG(7, (cout << "Reno TDA exit , time " << Simulator::Now()
                << " ackno " << t.ackNumber
                << " inflight " << Window()
                << " cWnd " << cWnd
                << " ssthresh " << ssThresh << endl));
      Retransmit();        // Retransmit the packet  
    }
  NoteTimeSeq(LOG_CWIN, (Seq_t)cWnd); // Log the change if cwnd logging
}

void TCPReno::ReTxTimeout()
{ // Retransmit timeout
  //cout << "Reno Timeout at time " << Simulator::Now() << endl;
  DEBUG0((cout << "Hello from TCPReno::Timeout" 
           << ", time " << Simulator::Now()
          << endl));
  ssThresh = Window() / 2; // Per RFC2581
  ssThresh = max(ssThresh, 2 * segSize);
  DEBUG(7,(cout << "Reno TxTo, time " << Simulator::Now()
          << " inflight " << BytesInFlight()
          << " new ssthresh " << ssThresh << endl));
  NoteTimeSeq(LOG_SSTHRESH, (Seq_t)ssThresh); // Log if ssthresh logging
  //cWnd = initialCWnd;     // Collapse congestion window (re-enter slowstart)
  cWnd = segSize;           // Per RFC2581, only one segment after timeout
  NoteTimeSeq(LOG_CWIN, (Seq_t)cWnd); // Log the change if cwnd logging
  nextTxSeq = highestRxAck; // Start from highest Ack
  fastRecovery = false;
  rtt->IncreaseMultiplier();// Double timeout value for next retx timer
  Retransmit();             // Retransmit the packet
}

