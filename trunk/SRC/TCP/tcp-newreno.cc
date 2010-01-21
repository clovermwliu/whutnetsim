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



// Georgia Tech Network Simulator - TCP-NewReno Class
// George F. Riley.  Georgia Tech, Spring 2002

#include <iostream>

//#define DEBUG_MASK 0x03
#include "G_debug.h"
#include "tcp-newreno.h"
//#include "simulator.h"

using namespace std;

// Constructors
TCPNewReno::TCPNewReno() : TCP(), recover(0)
{
}

TCPNewReno::TCPNewReno(Node* n) : TCP(n), recover(0)
{
}

TCPNewReno::TCPNewReno(const TCPNewReno& c) : TCP(c), recover(0)
{
}

// L4Protocol Methods
L4Protocol* TCPNewReno::Copy() const
{
  return new TCPNewReno(*this); // Create a copy of this protocol
}

// TCP Overridden methods
void TCPNewReno::NewAck(Seq seq)
{ // NewAck is called only for "New" (non-duplicate) acks
  bool skipTimer = false;
  if (fastRecovery)
    { // If in fast recovery and have a new data ack, check for
      // full or partial ack, per rfc3782
      bool fullAck = (seq >= recover);
      if (fullAck)
        { // Reset cWnd and exit fastRecovery
          cWnd = min(ssThresh, UnAckDataCount() + segSize);
          fastRecovery = false;
          partialAckCount = 0;
          DEBUG0((cout << "NR::Newack, time " << Simulator::Now() 
                  << " fullack " << (Seq_t)seq 
                  << "(" << (Seq_t)seq/segSize << ")"
                  << " new cWnd " << cWnd << endl));
        }
      else
        { // Partial ack, deflate cWnd per rfc3782
          cWnd -= (seq - highestRxAck);
          if ((Count_t)(seq - highestRxAck) >= segSize) cWnd += segSize;
          highestRxAck = seq;
          // Following is to not re-schedule the
          // retx timer except on the first partialAck.
          noTimer = (partialAckCount > 0);
          skipTimer = noTimer;
          partialAckCount++;
          Retransmit();
          DEBUG0((cout << "NR::Newack, time " << Simulator::Now()
                  << " Partial ack, " << (Seq_t)seq 
                  << "(" << (Seq_t)seq/segSize << ")"
                  << " new cWnd " << cWnd << endl));
        }
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
  CommonNewAck(seq, skipTimer);       // Complete newAck processing
  noTimer = false;
}

void TCPNewReno::DupAck(const TCPHeader& t, Count_t c)
{ // Dup ack received
  DEBUG0((cout << "NewReno::DupAck " << t.ackNumber
          << " time " << Simulator::Now()
          << " count " << c
          << " hrxa " << highestRxAck 
          << " ntxs " << nextTxSeq
          << " cwnd " << cWnd
          << endl));
  NoteTimeSeq(LOG_DUPACK, (Seq_t)c);
  if (fastRecovery)
    {
      cWnd += segSize;   // Inflate the cWnd for every additional DupAck
      SendPendingData(); // Try to send data if permitted by cWnd size
    }
  else if (c == 3 && t.ackNumber > recover)
    { // Count of three indicates triple dupack has been received
      // and covers at least "recover" bytes
      DEBUG0((cout << "NR TDA, entering fast recovery, recover "
              << nextTxSeq << endl));
      fastRecovery = true; // Enter the fast recovery mode
      partialAckCount = 0;
      ssThresh = Window() / 2; // Per RFC2581
      ssThresh = max(ssThresh, 2 * segSize);
      NoteTimeSeq(LOG_SSTHRESH, (Seq_t)ssThresh); // Log if ssthresh logging
      cWnd = ssThresh + 3*segSize;    // Reset cWnd per rfc2581
      DEBUG0((cout << "NewReno TDA, time " << Simulator::Now()
              << " ackno " << t.ackNumber
              << " cWnd " << cWnd
              << " new ssthresh " << ssThresh << endl));
      recover = nextTxSeq;
      Retransmit();        // Retransmit the packet  
    }
  NoteTimeSeq(LOG_CWIN, (Seq_t)cWnd); // Log the change if cwin logging enabled
}

void TCPNewReno::ReTxTimeout()
{ // Retransmit timeout
  //cout << "Reno Timeout at time " << Simulator::Now() << endl;
  DEBUG0((cout << "Hello from TCPNewReno::Timeout" 
           << ", time " << Simulator::Now()
          << endl));
  ssThresh = Window() / 2; // Per RFC2581
  ssThresh = max(ssThresh, 2 * segSize);
  NoteTimeSeq(LOG_SSTHRESH, (Seq_t)ssThresh); // Log if ssthresh logging
  DEBUG0((cout << "NewReno TxTo, time " << Simulator::Now()
          << " window " << Window()
          << " new ssthresh " << ssThresh << endl));
  cWnd = initialCWnd;       // Collapse congestion window (re-enter slowstart)
  nextTxSeq = highestRxAck; // Start from highest Ack
  fastRecovery = false;
  partialAckCount = 0;
  rtt->IncreaseMultiplier();// Double timeout value for next retx timer
  Retransmit();             // Retransmit the packet
}


