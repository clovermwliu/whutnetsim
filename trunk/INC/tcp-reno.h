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

// Implements the Reno version of TCP
// On any timeout, collapse congestion window to one segment
// Reduce slowstart threshold to 1/2 current cwnd, and resend
// the lowest sequence number in the retx list
//
// On any triple duplicate ack, resend the seq number requested,
// collapse cwnd and slowstart threshold
// to 1/2 the current cwnd.

#ifndef __tcp_reno_h__
#define __tcp_reno_h__

#include "tcp.h"
#include "node.h"
//#include "application.h"

//Doc:ClassXRef
class TCPReno : public TCP {
  //Doc:Class Class {\tt TCPReno} defines the behavior of the {\em Reno}
  //Doc:Class variation of {\em TCP}.  The base class functions
  //Doc:Class {\tt DupAck}, {\tt ReTxTimeout} and {\tt NewAck}
  //Doc:Class  are overridden here, but
  //Doc:Class all other base class {\tt TCP} functions are used.

public:
  TCPReno();
  TCPReno(Node* n);
  TCPReno(const TCPReno&); // Copy constructor
  L4Protocol* Copy() const;  // Create a copy of this protocol
public:
  // Overridden TCP methods
  //Doc:Method
  void NewAck(Seq);
    //Doc:Desc Process newly acknowledged data.
    //Doc:Arg1 Sequence number for new ack.

  //Doc:Method
  virtual void DupAck(const TCPHeader&, Count_t); // Dup ack received
    //Doc:Desc Process a duplicate ack.
    //Doc:Arg1 TCP Header that caused the duplicate ack.
    //Doc:Arg2 Count of duplicates.

  //Doc:Method
  virtual void ReTxTimeout();   // Retransmit timeout
    //Doc:Desc Process a retransmit timeout.

private:
  bool fastRecMode;		// Flag indicating fast recovery mode
  Seq fastRecMark;		// High Tx Mark for New Reno
};

#endif


