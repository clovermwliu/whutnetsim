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
// Georgia Tech Network Simulator - TCP Base Class
// George F. Riley.  Georgia Tech, Spring 2002

// Base class for all TCP variants

#ifdef HAVE_QT
#include <qcolor.h>
#endif

// Uncomment below to enable debug
//#define DEBUG_MASK 0x01
//#define DEBUG_MASK 0x2
//#define DEBUG_MASK 0x4
//#define DEBUG_MASK 0x7
//#define DEBUG_MASK 0x010
//#define DEBUG_MASK 0x20
//#define DEBUG_MASK 0x40
//#define DEBUG_MASK 0x80
//#define DEBUG_MASK 0x7f
// Enable debug level 8
//#define DEBUG_MASK 0x100
//#define DEBUG_MASK 0x200
//#define DEBUG_MASK 0x3ff
// Enable debug level 7
//#define DEBUG_MASK 0x80

#include "G_debug.h"
#include "tcp.h"
#include "hex.h"
#include "application.h"
//#include "simulator.h"
//#include "tcpdemux.h"
#include "timerbuckets.h"
#include "tcp-tahoe.h"
//#include "packet-callbacks.h"

using namespace std;

// TCP Static Members
Count_t       TCP::defaultSegSize = 512;    // Segment size
Count_t       TCP::defaultAdvWin = 0xffff;  // Advertised window
Count_t       TCP::defaultSSThresh = 0xffff;// SlowStart Threshold
Count_t       TCP::defaultTxBuffer = MAX_COUNT;
Count_t       TCP::defaultRxBuffer = MAX_COUNT;
Time_t        TCP::defaultTwTimeout = 5.0;  // Five seconds default timeout
Time_t        TCP::defaultConnTimeout = 6.0;// Six seconds default timeout
Time_t        TCP::defaultDelAckTimeout = 0.0;// Default to not used
Count_t       TCP::defaultInitialCWnd = 1;  // Default initial cWnd (Segments)
bool          TCP::logFlagsText = false;    // True if text mode flag logging
FId_t         TCP::nextFlowId;              // Next unique tcp flow id
bool          TCP::useTimerBuckets = false; // True if using timer buckets
Count_t       TCP::defaultConnCount = 3;    // Number of connection retries
TCP*          TCP::defaultTCP = nil;        // Default TCP variant
TCPHeader*    TCP::bpHeader = nil;          // Imported header from backplane
Count_t       TCP::totalTimeouts = 0;       // Total number of timeout events
Count_t       TCP::totalAborts = 0;         // Total number of ICMP aborts

// TCP Event Methods
TCPEvent::~TCPEvent()
{
}

// size of debug history buffer
#define DBH_SIZE 50

//TCP State Machine Definition
class SA { // State/Action pair
public:
  SA() : state(TCP::LAST_STATE), action(TCP::LAST_ACTION) { }
  SA(TCP::States_t s, TCP::Actions_t a) : state(s), action(a) { }
public:
  TCP::States_t  state;
  TCP::Actions_t action;
};

typedef vector<SA>  StateActionVec_t;
typedef vector<StateActionVec_t> StateActions_t;  // One per current state
typedef vector<TCP::Events_t>    EventVec_t;      // For flag events lookup

class TCPStateMachine {
public:
  typedef enum { MAX_FLAGS = 0x40 } TCPMaxFlags_t;  // Flags are 6 bits
  TCPStateMachine();
  SA            Lookup(TCP::States_t, TCP::Events_t);  
  TCP::Events_t FlagsEvent(Byte_t); // Lookup event from flags
public:
  StateActions_t aT; // Action table
  EventVec_t     eV; // Flags event lookup  
};

TCPStateMachine::TCPStateMachine() 
  : aT(TCP::LAST_STATE, StateActionVec_t(TCP::LAST_EVENT)),
    eV(MAX_FLAGS)
{ // Create the state table
  // Closed state
  aT[TCP::CLOSED][TCP::APP_LISTEN]  = SA(TCP::LISTEN,   TCP::NO_ACT);
  aT[TCP::CLOSED][TCP::APP_CONNECT] = SA(TCP::SYN_SENT, TCP::SYN_TX);
  aT[TCP::CLOSED][TCP::APP_SEND]    = SA(TCP::CLOSED,   TCP::RST_TX);
  aT[TCP::CLOSED][TCP::SEQ_RECV]    = SA(TCP::CLOSED,   TCP::NO_ACT);
  aT[TCP::CLOSED][TCP::APP_CLOSE]   = SA(TCP::CLOSED,   TCP::NO_ACT);
  aT[TCP::CLOSED][TCP::TIMEOUT]     = SA(TCP::CLOSED,   TCP::RST_TX);
  aT[TCP::CLOSED][TCP::ACK_RX]      = SA(TCP::CLOSED,   TCP::RST_TX);
  aT[TCP::CLOSED][TCP::SYN_RX]      = SA(TCP::CLOSED,   TCP::RST_TX);
  aT[TCP::CLOSED][TCP::SYN_ACK_RX]  = SA(TCP::CLOSED,   TCP::RST_TX);
  aT[TCP::CLOSED][TCP::FIN_RX]      = SA(TCP::CLOSED,   TCP::RST_TX);
  aT[TCP::CLOSED][TCP::FIN_ACK_RX]  = SA(TCP::CLOSED,   TCP::RST_TX);
  aT[TCP::CLOSED][TCP::RST_RX]      = SA(TCP::CLOSED,   TCP::CANCEL_TM);
  aT[TCP::CLOSED][TCP::BAD_FLAGS]   = SA(TCP::CLOSED,   TCP::RST_TX);

  // Listen State
  // For the listen state, anything other than CONNECT or SEND
  // is simply ignored....this likely indicates the child TCP
  // has finished and issued unbind call, but the remote end
  // has not yet  closed.
  aT[TCP::LISTEN][TCP::APP_LISTEN]  = SA(TCP::LISTEN,   TCP::NO_ACT);
  aT[TCP::LISTEN][TCP::APP_CONNECT] = SA(TCP::SYN_SENT, TCP::SYN_TX);
  aT[TCP::LISTEN][TCP::APP_SEND]    = SA(TCP::SYN_SENT, TCP::SYN_TX);
  aT[TCP::LISTEN][TCP::SEQ_RECV]    = SA(TCP::LISTEN,   TCP::NO_ACT);
  aT[TCP::LISTEN][TCP::APP_CLOSE]   = SA(TCP::CLOSED,   TCP::NO_ACT);
  aT[TCP::LISTEN][TCP::TIMEOUT]     = SA(TCP::LISTEN,   TCP::NO_ACT);
  aT[TCP::LISTEN][TCP::ACK_RX]      = SA(TCP::LISTEN,   TCP::NO_ACT);
  aT[TCP::LISTEN][TCP::SYN_RX]      = SA(TCP::LISTEN,   TCP::SYN_ACK_TX);
  aT[TCP::LISTEN][TCP::SYN_ACK_RX]  = SA(TCP::LISTEN,   TCP::NO_ACT);
  aT[TCP::LISTEN][TCP::FIN_RX]      = SA(TCP::LISTEN,   TCP::NO_ACT);
  aT[TCP::LISTEN][TCP::FIN_ACK_RX]  = SA(TCP::LISTEN,   TCP::NO_ACT);
  aT[TCP::LISTEN][TCP::RST_RX]      = SA(TCP::LISTEN,   TCP::NO_ACT);
  aT[TCP::LISTEN][TCP::BAD_FLAGS]   = SA(TCP::LISTEN,   TCP::NO_ACT);

  // Syn Sent State
  aT[TCP::SYN_SENT][TCP::APP_LISTEN]  = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::SYN_SENT][TCP::APP_CONNECT] = SA(TCP::SYN_SENT,    TCP::SYN_TX);
  aT[TCP::SYN_SENT][TCP::APP_SEND]    = SA(TCP::SYN_SENT,    TCP::NO_ACT);
  aT[TCP::SYN_SENT][TCP::SEQ_RECV]    = SA(TCP::ESTABLISHED, TCP::NEW_SEQ_RX);
  aT[TCP::SYN_SENT][TCP::APP_CLOSE]   = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::SYN_SENT][TCP::TIMEOUT]     = SA(TCP::CLOSED,      TCP::NO_ACT);
  aT[TCP::SYN_SENT][TCP::ACK_RX]      = SA(TCP::SYN_SENT,    TCP::NO_ACT);
  aT[TCP::SYN_SENT][TCP::SYN_RX]      = SA(TCP::SYN_RCVD,    TCP::SYN_ACK_TX);
  aT[TCP::SYN_SENT][TCP::SYN_ACK_RX]  = SA(TCP::ESTABLISHED, TCP::ACK_TX_1);
  aT[TCP::SYN_SENT][TCP::FIN_RX]      = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::SYN_SENT][TCP::FIN_ACK_RX]  = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::SYN_SENT][TCP::RST_RX]      = SA(TCP::CLOSED,      TCP::APP_NOTIFY);
  aT[TCP::SYN_SENT][TCP::BAD_FLAGS]   = SA(TCP::CLOSED,      TCP::RST_TX);

  // Syn Recvd State
  aT[TCP::SYN_RCVD][TCP::APP_LISTEN]  = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::SYN_RCVD][TCP::APP_CONNECT] = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::SYN_RCVD][TCP::APP_SEND]    = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::SYN_RCVD][TCP::SEQ_RECV]    = SA(TCP::ESTABLISHED, TCP::NEW_SEQ_RX);
  aT[TCP::SYN_RCVD][TCP::APP_CLOSE]   = SA(TCP::FIN_WAIT_1,  TCP::FIN_TX);
  aT[TCP::SYN_RCVD][TCP::TIMEOUT]     = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::SYN_RCVD][TCP::ACK_RX]      = SA(TCP::ESTABLISHED, TCP::SERV_NOTIFY);
  aT[TCP::SYN_RCVD][TCP::SYN_RX]      = SA(TCP::SYN_RCVD,    TCP::SYN_ACK_TX);
  aT[TCP::SYN_RCVD][TCP::SYN_ACK_RX]  = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::SYN_RCVD][TCP::FIN_RX]      = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::SYN_RCVD][TCP::FIN_ACK_RX]  = SA(TCP::CLOSE_WAIT,  TCP::PEER_CLOSE);
  aT[TCP::SYN_RCVD][TCP::RST_RX]      = SA(TCP::CLOSED,      TCP::CANCEL_TM);
  aT[TCP::SYN_RCVD][TCP::BAD_FLAGS]   = SA(TCP::CLOSED,      TCP::RST_TX);

  // Established State
  aT[TCP::ESTABLISHED][TCP::APP_LISTEN] = SA(TCP::CLOSED,     TCP::RST_TX);
  aT[TCP::ESTABLISHED][TCP::APP_CONNECT]= SA(TCP::CLOSED,     TCP::RST_TX);
  aT[TCP::ESTABLISHED][TCP::APP_SEND]   = SA(TCP::ESTABLISHED,TCP::TX_DATA);
  aT[TCP::ESTABLISHED][TCP::SEQ_RECV]   = SA(TCP::ESTABLISHED,TCP::NEW_SEQ_RX);
  aT[TCP::ESTABLISHED][TCP::APP_CLOSE]  = SA(TCP::FIN_WAIT_1, TCP::FIN_TX);
  aT[TCP::ESTABLISHED][TCP::TIMEOUT]    = SA(TCP::ESTABLISHED,TCP::RETX);
  aT[TCP::ESTABLISHED][TCP::ACK_RX]     = SA(TCP::ESTABLISHED,TCP::NEW_ACK);
  aT[TCP::ESTABLISHED][TCP::SYN_RX]     = SA(TCP::SYN_RCVD,   TCP::SYN_ACK_TX);
  aT[TCP::ESTABLISHED][TCP::SYN_ACK_RX] = SA(TCP::ESTABLISHED,TCP::NO_ACT);
  aT[TCP::ESTABLISHED][TCP::FIN_RX]     = SA(TCP::CLOSE_WAIT, TCP::PEER_CLOSE);
  aT[TCP::ESTABLISHED][TCP::FIN_ACK_RX] = SA(TCP::CLOSE_WAIT, TCP::PEER_CLOSE);
  aT[TCP::ESTABLISHED][TCP::RST_RX]     = SA(TCP::CLOSED,     TCP::CANCEL_TM);
  aT[TCP::ESTABLISHED][TCP::BAD_FLAGS]  = SA(TCP::CLOSED,     TCP::RST_TX);

  // Close Wait State
  aT[TCP::CLOSE_WAIT][TCP::APP_LISTEN]  = SA(TCP::CLOSED,     TCP::RST_TX);
  aT[TCP::CLOSE_WAIT][TCP::APP_CONNECT] = SA(TCP::SYN_SENT,   TCP::SYN_TX);
  aT[TCP::CLOSE_WAIT][TCP::APP_SEND]    = SA(TCP::CLOSE_WAIT, TCP::TX_DATA);
  aT[TCP::CLOSE_WAIT][TCP::SEQ_RECV]    = SA(TCP::CLOSE_WAIT, TCP::NEW_SEQ_RX);
  aT[TCP::CLOSE_WAIT][TCP::APP_CLOSE]   = SA(TCP::LAST_ACK,   TCP::FIN_ACK_TX);
  aT[TCP::CLOSE_WAIT][TCP::TIMEOUT]     = SA(TCP::CLOSE_WAIT, TCP::NO_ACT);
  aT[TCP::CLOSE_WAIT][TCP::ACK_RX]      = SA(TCP::CLOSE_WAIT, TCP::NO_ACT);
  aT[TCP::CLOSE_WAIT][TCP::SYN_RX]      = SA(TCP::CLOSED,     TCP::RST_TX);
  aT[TCP::CLOSE_WAIT][TCP::SYN_ACK_RX]  = SA(TCP::CLOSED,     TCP::RST_TX);
  aT[TCP::CLOSE_WAIT][TCP::FIN_RX]      = SA(TCP::CLOSE_WAIT, TCP::ACK_TX);
  aT[TCP::CLOSE_WAIT][TCP::FIN_ACK_RX]  = SA(TCP::CLOSE_WAIT, TCP::ACK_TX);
  aT[TCP::CLOSE_WAIT][TCP::RST_RX]      = SA(TCP::CLOSED,     TCP::CANCEL_TM);
  aT[TCP::CLOSE_WAIT][TCP::BAD_FLAGS]   = SA(TCP::CLOSED,     TCP::RST_TX);

  // Close Last Ack State
  aT[TCP::LAST_ACK][TCP::APP_LISTEN]  = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::LAST_ACK][TCP::APP_CONNECT] = SA(TCP::SYN_SENT,    TCP::SYN_TX);
  aT[TCP::LAST_ACK][TCP::APP_SEND]    = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::LAST_ACK][TCP::SEQ_RECV]    = SA(TCP::LAST_ACK,    TCP::NEW_SEQ_RX);
  aT[TCP::LAST_ACK][TCP::APP_CLOSE]   = SA(TCP::CLOSED,      TCP::NO_ACT);
  aT[TCP::LAST_ACK][TCP::TIMEOUT]     = SA(TCP::CLOSED,      TCP::NO_ACT);
  aT[TCP::LAST_ACK][TCP::ACK_RX]      = SA(TCP::CLOSED,      TCP::APP_CLOSED);
  aT[TCP::LAST_ACK][TCP::SYN_RX]      = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::LAST_ACK][TCP::SYN_ACK_RX]  = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::LAST_ACK][TCP::FIN_RX]      = SA(TCP::LAST_ACK,    TCP::FIN_ACK_TX);
  aT[TCP::LAST_ACK][TCP::FIN_ACK_RX]  = SA(TCP::CLOSED,      TCP::NO_ACT);
  aT[TCP::LAST_ACK][TCP::RST_RX]      = SA(TCP::CLOSED,      TCP::CANCEL_TM);
  aT[TCP::LAST_ACK][TCP::BAD_FLAGS]   = SA(TCP::CLOSED,      TCP::RST_TX);

  // FIN_WAIT_1 state
  aT[TCP::FIN_WAIT_1][TCP::APP_LISTEN]  = SA(TCP::CLOSED,     TCP::RST_TX);
  aT[TCP::FIN_WAIT_1][TCP::APP_CONNECT] = SA(TCP::CLOSED,     TCP::RST_TX);
  aT[TCP::FIN_WAIT_1][TCP::APP_SEND]    = SA(TCP::CLOSED,     TCP::RST_TX);
  aT[TCP::FIN_WAIT_1][TCP::SEQ_RECV]    = SA(TCP::FIN_WAIT_1, TCP::NEW_SEQ_RX);
  aT[TCP::FIN_WAIT_1][TCP::APP_CLOSE]   = SA(TCP::FIN_WAIT_1, TCP::NO_ACT);
  aT[TCP::FIN_WAIT_1][TCP::TIMEOUT]     = SA(TCP::FIN_WAIT_1, TCP::NO_ACT);
  aT[TCP::FIN_WAIT_1][TCP::ACK_RX]      = SA(TCP::FIN_WAIT_2, TCP::NEW_ACK);
  aT[TCP::FIN_WAIT_1][TCP::SYN_RX]      = SA(TCP::CLOSED,     TCP::RST_TX);
  aT[TCP::FIN_WAIT_1][TCP::SYN_ACK_RX]  = SA(TCP::CLOSED,     TCP::RST_TX);
  aT[TCP::FIN_WAIT_1][TCP::FIN_RX]      = SA(TCP::CLOSING,    TCP::ACK_TX);
  aT[TCP::FIN_WAIT_1][TCP::FIN_ACK_RX]  = SA(TCP::TIMED_WAIT, TCP::ACK_TX);
  aT[TCP::FIN_WAIT_1][TCP::RST_RX]      = SA(TCP::CLOSED,     TCP::CANCEL_TM);
  aT[TCP::FIN_WAIT_1][TCP::BAD_FLAGS]   = SA(TCP::CLOSED,     TCP::RST_TX);

  // FIN_WAIT_2 state
  aT[TCP::FIN_WAIT_2][TCP::APP_LISTEN]  = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::FIN_WAIT_2][TCP::APP_CONNECT] = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::FIN_WAIT_2][TCP::APP_SEND]    = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::FIN_WAIT_2][TCP::SEQ_RECV]    = SA(TCP::FIN_WAIT_2,  TCP::NO_ACT);
  aT[TCP::FIN_WAIT_2][TCP::APP_CLOSE]   = SA(TCP::FIN_WAIT_2,  TCP::NO_ACT);
  aT[TCP::FIN_WAIT_2][TCP::TIMEOUT]     = SA(TCP::FIN_WAIT_2,  TCP::NO_ACT);
  aT[TCP::FIN_WAIT_2][TCP::ACK_RX]      = SA(TCP::FIN_WAIT_2,  TCP::NEW_ACK);
  aT[TCP::FIN_WAIT_2][TCP::SYN_RX]      = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::FIN_WAIT_2][TCP::SYN_ACK_RX]  = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::FIN_WAIT_2][TCP::FIN_RX]      = SA(TCP::TIMED_WAIT,  TCP::ACK_TX);
  aT[TCP::FIN_WAIT_2][TCP::FIN_ACK_RX]  = SA(TCP::TIMED_WAIT,  TCP::ACK_TX);
  aT[TCP::FIN_WAIT_2][TCP::RST_RX]      = SA(TCP::CLOSED,      TCP::CANCEL_TM);
  aT[TCP::FIN_WAIT_2][TCP::BAD_FLAGS]   = SA(TCP::CLOSED,      TCP::RST_TX);

  // CLOSING state
  aT[TCP::CLOSING][TCP::APP_LISTEN]  = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::CLOSING][TCP::APP_CONNECT] = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::CLOSING][TCP::APP_SEND]    = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::CLOSING][TCP::SEQ_RECV]    = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::CLOSING][TCP::APP_CLOSE]   = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::CLOSING][TCP::TIMEOUT]     = SA(TCP::CLOSING,     TCP::NO_ACT);
  aT[TCP::CLOSING][TCP::ACK_RX]      = SA(TCP::TIMED_WAIT,  TCP::NO_ACT);
  aT[TCP::CLOSING][TCP::SYN_RX]      = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::CLOSING][TCP::SYN_ACK_RX]  = SA(TCP::CLOSED,      TCP::RST_TX);
  aT[TCP::CLOSING][TCP::FIN_RX]      = SA(TCP::CLOSED,      TCP::ACK_TX);
  aT[TCP::CLOSING][TCP::FIN_ACK_RX]  = SA(TCP::CLOSED,      TCP::ACK_TX);
  aT[TCP::CLOSING][TCP::RST_RX]      = SA(TCP::CLOSED,      TCP::CANCEL_TM);
  aT[TCP::CLOSING][TCP::BAD_FLAGS]   = SA(TCP::CLOSED,      TCP::RST_TX);

  // TIMED_WAIT state
  aT[TCP::TIMED_WAIT][TCP::APP_LISTEN]  = SA(TCP::TIMED_WAIT, TCP::NO_ACT);
  aT[TCP::TIMED_WAIT][TCP::APP_CONNECT] = SA(TCP::TIMED_WAIT, TCP::NO_ACT);
  aT[TCP::TIMED_WAIT][TCP::APP_SEND]    = SA(TCP::TIMED_WAIT, TCP::NO_ACT);
  aT[TCP::TIMED_WAIT][TCP::SEQ_RECV]    = SA(TCP::TIMED_WAIT, TCP::NO_ACT);
  aT[TCP::TIMED_WAIT][TCP::APP_CLOSE]   = SA(TCP::TIMED_WAIT, TCP::NO_ACT);
  aT[TCP::TIMED_WAIT][TCP::TIMEOUT]     = SA(TCP::TIMED_WAIT, TCP::NO_ACT);
  aT[TCP::TIMED_WAIT][TCP::ACK_RX]      = SA(TCP::TIMED_WAIT, TCP::NO_ACT);
  aT[TCP::TIMED_WAIT][TCP::SYN_RX]      = SA(TCP::TIMED_WAIT, TCP::NO_ACT);
  aT[TCP::TIMED_WAIT][TCP::SYN_ACK_RX]  = SA(TCP::TIMED_WAIT, TCP::NO_ACT);
  aT[TCP::TIMED_WAIT][TCP::FIN_RX]      = SA(TCP::TIMED_WAIT, TCP::NO_ACT);
  aT[TCP::TIMED_WAIT][TCP::FIN_ACK_RX]  = SA(TCP::TIMED_WAIT, TCP::NO_ACT);
  aT[TCP::TIMED_WAIT][TCP::RST_RX]      = SA(TCP::TIMED_WAIT, TCP::NO_ACT);
  aT[TCP::TIMED_WAIT][TCP::BAD_FLAGS]   = SA(TCP::TIMED_WAIT, TCP::NO_ACT);

  // Create the flags lookup table
  eV[ 0x00] = TCP::SEQ_RECV;  // No flags
  eV[ 0x01] = TCP::FIN_RX;    // Fin
  eV[ 0x02] = TCP::SYN_RX;    // Syn
  eV[ 0x03] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x04] = TCP::RST_RX;    // Rst
  eV[ 0x05] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x06] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x07] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x08] = TCP::SEQ_RECV;  // Psh flag is not used
  eV[ 0x09] = TCP::FIN_RX;    // Fin
  eV[ 0x0a] = TCP::SYN_RX;    // Syn
  eV[ 0x0b] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x0c] = TCP::RST_RX;    // Rst
  eV[ 0x0d] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x0e] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x0f] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x10] = TCP::ACK_RX;    // Ack
  eV[ 0x11] = TCP::FIN_ACK_RX;// Fin/Ack
  eV[ 0x12] = TCP::SYN_ACK_RX;// Syn/Ack
  eV[ 0x13] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x14] = TCP::RST_RX;    // Rst
  eV[ 0x15] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x16] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x17] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x18] = TCP::ACK_RX;    // Ack
  eV[ 0x19] = TCP::FIN_ACK_RX;// Fin/Ack
  eV[ 0x1a] = TCP::SYN_ACK_RX;// Syn/Ack
  eV[ 0x1b] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x1c] = TCP::RST_RX;    // Rst
  eV[ 0x1d] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x1e] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x1f] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x20] = TCP::SEQ_RECV;  // No flags (Urgent not presently used)
  eV[ 0x21] = TCP::FIN_RX;    // Fin
  eV[ 0x22] = TCP::SYN_RX;    // Syn
  eV[ 0x23] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x24] = TCP::RST_RX;    // Rst
  eV[ 0x25] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x26] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x27] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x28] = TCP::SEQ_RECV;  // Psh flag is not used
  eV[ 0x29] = TCP::FIN_RX;    // Fin
  eV[ 0x2a] = TCP::SYN_RX;    // Syn
  eV[ 0x2b] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x2c] = TCP::RST_RX;    // Rst
  eV[ 0x2d] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x2e] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x2f] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x30] = TCP::ACK_RX;    // Ack (Urgent not used)
  eV[ 0x31] = TCP::FIN_ACK_RX;// Fin/Ack
  eV[ 0x32] = TCP::SYN_ACK_RX;// Syn/Ack
  eV[ 0x33] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x34] = TCP::RST_RX;    // Rst
  eV[ 0x35] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x36] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x37] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x38] = TCP::ACK_RX;    // Ack
  eV[ 0x39] = TCP::FIN_ACK_RX;// Fin/Ack
  eV[ 0x3a] = TCP::SYN_ACK_RX;// Syn/Ack
  eV[ 0x3b] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x3c] = TCP::RST_RX;    // Rst
  eV[ 0x3d] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x3e] = TCP::BAD_FLAGS; // Illegal
  eV[ 0x3f] = TCP::BAD_FLAGS; // Illegal
}

SA TCPStateMachine::Lookup(TCP::States_t s, TCP::Events_t e)
{
  return aT[s][e];
}

TCP::Events_t TCPStateMachine::FlagsEvent(Byte_t f)
{ // Lookup event from flags
  if (f >= MAX_FLAGS) return TCP::BAD_FLAGS;
  return eV[f]; // Look up flags event
}

// Define the one and only state machine object
TCPStateMachine StateMachine;

// TCPHeader methods

TCPHeader::TCPHeader()
  : sourcePort(0), destPort(0),
    sequenceNumber(0), ackNumber(0),
    headerLength(0), flags(0),
    window(0), checksum(0), urgentPointer(0),
    fid(0), dataLength(0), cWnd(0), peer(nil)
{
}

// Construct from serialized buffer
TCPHeader::TCPHeader(char* b, Size_t& sz, Packet* p)
{
  DEBUG0((cout << "Constructing tcp, initial size " << sz << endl));
  Size_t s = 0;
  b = Serializable::GetSize(b, sz, s);
  DEBUG0((cout << "l4pdu size is " << s << " (" << Hex8(s) << ") " << endl));
  sz -= s;
  b = Construct(b, s);
  DEBUG0((cout << "Constructing tcp, final  size " << sz << endl));
  p->PushPDUBottom(this);  // Add to packet
  if (sz)
    { // Remaining size, must be data
      new Data(b, sz, p);
    }
}

Size_t TCPHeader::Size() const
{
  return 20;
}

PDU*   TCPHeader::Copy() const
{
  return new TCPHeader(*this);
}

void   TCPHeader::Trace(Tfstream& tos, Bitmap_t b, Packet* p, const char* s)
{ // Trace the contents of this pdu
  tos << " ";
  if (s)
    tos << s;
  tos << "L4-TCP";
  if (Detail(TCP::SOURCE_PORT, b))     tos << " " << sourcePort;
  if (Detail(TCP::DEST_PORT, b))       tos << " " << destPort;
  if (Detail(TCP::SEQ_NUMBER, b))      tos << " " << sequenceNumber;
  if (Detail(TCP::ACK_NUMBER, b))      tos << " " << ackNumber;
  if (Detail(TCP::HEADER_LENGTH, b))   tos << " " << headerLength;
  if (Detail(TCP::FLAGS, b))
    {
      tos << " ";
      bool first = true;
      Byte_t f = flags;
      if (TCP::logFlagsText)
        {
          if (f & TCP::FIN) {if(!first)tos << '|'; tos << "FIN";first=false;}
          if (f & TCP::SYN) {if(!first)tos << '|'; tos << "SYN";first=false;}
          if (f & TCP::RST) {if(!first)tos << '|'; tos << "RST";first=false;}
          if (f & TCP::PSH) {if(!first)tos << '|'; tos << "PSH";first=false;}
          if (f & TCP::ACK) {if(!first)tos << '|'; tos << "ACK";first=false;}
          if (f & TCP::URG) {if(!first)tos << '|'; tos << "URG";first=false;}
          if (first) tos << 0; // No flags
        }
      else
        {
          tos << Hex2(flags);
        }
    }
  if (Detail(TCP::WINDOW, b))          tos << " " << window;
  if (Detail(TCP::CHECKSUM, b))        tos << " " << Hex4(checksum);
  if (Detail(TCP::URGENT, b))          tos << " " << urgentPointer;
  if (Detail(TCP::FID, b))             tos << " " << fid;
  if (Detail(TCP::DATA_LENGTH, b))     tos << " " << dataLength;
  if (Detail(TCP::CWND, b))            tos << " " << cWnd;
}

// Serialization
Size_t TCPHeader::SSize()
{ // Size needed for serialization
  return 
    sizeof(sourcePort) +
    sizeof(destPort) +
    sizeof(sequenceNumber) +
    sizeof(ackNumber) +
    sizeof(headerLength) +
    sizeof(flags) +
    sizeof(window) +
    sizeof(checksum) +
    sizeof(urgentPointer) +
    sizeof(fid) +
    sizeof(dataLength) +
    sizeof(cWnd) +
    sizeof(peer);
}

char*  TCPHeader::Serialize(char* b, Size_t& sz)
{ // Serialize to a buffer
  Seq_t seq = sequenceNumber;
  Seq_t ack = ackNumber;
  b = SerializeToBuffer(b, sz, sourcePort);
  b = SerializeToBuffer(b, sz, destPort);
  b = SerializeToBuffer(b, sz, seq);
  b = SerializeToBuffer(b, sz, ack);
  b = SerializeToBuffer(b, sz, headerLength);
  b = SerializeToBuffer(b, sz, flags);
  b = SerializeToBuffer(b, sz, window);
  b = SerializeToBuffer(b, sz, checksum);
  b = SerializeToBuffer(b, sz, urgentPointer);
  b = SerializeToBuffer(b, sz, fid);
  b = SerializeToBuffer(b, sz, dataLength);
  b = SerializeToBuffer(b, sz, cWnd);
  b = SerializeToBuffer(b, sz, (unsigned long&)peer);
  return b;
}

char*  TCPHeader::Construct(char* b, Size_t& sz)
{ // Construct from buffer
  Seq_t seq = 0;
  Seq_t ack = 0;
  b = ConstructFromBuffer(b, sz, sourcePort);
  b = ConstructFromBuffer(b, sz, destPort);
  b = ConstructFromBuffer(b, sz, seq);
  b = ConstructFromBuffer(b, sz, ack);
  b = ConstructFromBuffer(b, sz, headerLength);
  b = ConstructFromBuffer(b, sz, flags);
  b = ConstructFromBuffer(b, sz, window);
  b = ConstructFromBuffer(b, sz, checksum);
  b = ConstructFromBuffer(b, sz, urgentPointer);
  b = ConstructFromBuffer(b, sz, fid);
  b = ConstructFromBuffer(b, sz, dataLength);
  b = ConstructFromBuffer(b, sz, cWnd);
  b = ConstructFromBuffer(b, sz, (unsigned long&)peer);
  peer = nil; // Peer pointer from remote is of course useless
  sequenceNumber = seq;
  ackNumber = ack;
  DEBUG0((cout << "TCPH::Construct "
          << " sp " << sourcePort
          << " dp " << destPort
          << " flags " << Hex2(flags)
          << endl));
  return b;
}

// TCP methods
TCP::TCP() :
  L4Protocol(), pendingData(nil), timeSeqStats(nil)
{
  CommonConstructor();
  DBG((Stats::tcpAllocated++));
}

TCP::TCP(Node* n) :
  L4Protocol(n), pendingData(nil), timeSeqStats(nil)
{
  DEBUG0((cout << "Constructing TCP " << this << endl));
  CommonConstructor();
  DBG((Stats::tcpAllocated++));
}

TCP::TCP(const TCP& t) :
  L4Protocol(t), pendingData(nil), dupAckCount(0),
  fastRecovery(false), needAck(false), noTimer(false), nextRxSeq(0),
  segSize(t.segSize), rxWin(t.rxWin), advWin(t.advWin),
  cWnd(t.cWnd), ssThresh(t.ssThresh), initialCWnd(t.initialCWnd),
  rtt(t.rtt->Copy()), 
  connTimeout(nil), retxTimeout(nil),
  delAckTimeout(nil), timedWaitTimeout(nil),
  lastAckTimeout(nil),
  twTimeout(t.twTimeout), cnTimeout(t.cnTimeout), daTimeout(t.daTimeout),
  retryCount(t.retryCount),
  connCount(t.connCount),
  state(t.state), closeOnEmpty(t.closeOnEmpty),
  deleteOnComplete(t.deleteOnComplete),
  deleteOnTWait(t.deleteOnTWait),
  pendingClose(t.pendingClose),
  closeNotified(t.closeNotified),
  closeReqNotified(t.closeReqNotified),
  txBuffer(t.txBuffer), rxBuffer(t.rxBuffer),
  totalAck(0), openTime(0.0), lastRxTime(0.0),
  lastAckTx(0.0), synTime(0.0),
  retransmitCount(t.retransmitCount), timeoutCount(t.timeoutCount),
  pktsSent(0), pktsReceived(0),
  bytesSent(0), bytesReceived(0),
  timeSeqStats(nil), parent(nil), peer(nil),
  nChildren(0), childLimit(t.childLimit),
  nFin(t.nFin),
  lastTimeoutDelay(t.lastTimeoutDelay),
  lastMeasuredRTT(t.lastMeasuredRTT),
  dbInd(t.dbInd),
  isServer(t.isServer),
  nRequested(t.nRequested)
{
  DEBUG0((cout << "Constructing (Copy) TCP " << this 
          << " notifier " << (NotifyHandler*)this
          << endl));
  if (t.timeSeqStats)
    { // stats exist, need to reallocate
      timeSeqStats = new TimeSeqVec_t(LOG_LAST, 0);
      TimeSeqVec_t& tsv = *timeSeqStats;
      TimeSeqVec_t& ctsv = *(t.timeSeqStats);
      for (TimeSeqVec_t::size_type i = 0; i < tsv.size(); ++i)
        {
          if (ctsv[i])
            { // stats exist, need to alloc our own copy
              tsv[i] = new TimeSeq_t();
            }
        }
    }
  DBG((Stats::tcpAllocated++));
}


void TCP::CommonConstructor()
{
  DEBUG0((cout << "Constructing TCP " << this 
          << " notifier " << (NotifyHandler*)this
          << endl));
  // Set default values for members
  nextTxSeq = 0;
  highTxMark = 0;
  highestRxAck = 0;
  dupAckCount = 0;
  fastRecovery = false;
  needAck = false;
  noTimer = false;
  nextRxSeq = 0;
  segSize  = defaultSegSize;
  rxWin = defaultAdvWin;
  advWin = defaultAdvWin;
  cWnd = defaultInitialCWnd * segSize;
  ssThresh  = defaultSSThresh;
  initialCWnd = defaultInitialCWnd;
  rtt = RTTEstimator::Default()->Copy();
  connTimeout = nil;
  retxTimeout =  nil;
  delAckTimeout = nil;
  timedWaitTimeout = nil;
  lastAckTimeout = nil;
  twTimeout = defaultTwTimeout;
  cnTimeout = defaultConnTimeout;
  daTimeout = defaultDelAckTimeout;
  retryCount = 0;
  connCount = 0;
  state = CLOSED;
  closeOnEmpty = false;
  deleteOnComplete = false;
  deleteOnTWait = false;
  pendingClose = false;
  closeNotified = false;
  closeReqNotified = false;
  txBuffer = defaultTxBuffer;
  rxBuffer  = defaultRxBuffer;
  totalAck = 0;
  openTime = 0.0;
  lastRxTime = 0.0;
  lastAckTx = 0.0;
  synTime = 0.0;
  retransmitCount = 0;
  timeoutCount = 0;
  pktsSent = 0;
  pktsReceived = 0;
  bytesSent = 0;
  bytesReceived = 0;
  parent = nil;
  peer = nil;
  nChildren = 0;
  childLimit = MAX_COUNT;
  nFin = 0;
  isServer = false;
  nRequested = false;
  lastTimeoutDelay = 0.0;
  lastMeasuredRTT = 0.0;
  dbInd = 0;

  // Turn off some of the default tracing options
  DetailOff(HEADER_LENGTH);
  DetailOff(WINDOW);
  DetailOff(CHECKSUM);
  DetailOff(URGENT);
  DetailOff(CWND);
}

TCP::~TCP()
{
  DEBUG0((cout << "Deleting TCP " << this 
          << " notifier " << (NotifyHandler*)this
          << endl));
  DBG((Stats::tcpDeleted++));
  if (rtt) delete rtt;
  if (pendingData) delete pendingData;
  // Delete any existing seq/time stats
  if (timeSeqStats)
    {
      TimeSeqVec_t& tsv = *timeSeqStats;
      for (TimeSeqVec_t::size_type i = 0; i < tsv.size(); ++i)
        {
          if (tsv[i])
            { // stats exist
              tsv[i]->clear();
              delete tsv[i];
            }
        }
    }
  if (parent) parent->ChildComplete();       // Notify parent of completion
  CancelAllTimers();                         // Insure no timers active
  CancelNotification(this);
  if (localPort != NO_PORT)
    { // Need to unbind
      DEBUG0((cout << "TCP " << this << " unbinding lport " << localPort
              << " rport " << peerPort << " rip " << (string)IPAddr(peerIP)
              << endl));
      if (peerPort != NO_PORT)
        { // Use 4-tuple
          localNode->Unbind(Proto(), localPort, IPADDR_NONE,
                            peerPort, peerIP, this);
        }
      else
        {
          Unbind(Proto(), localPort);
        }
    }
}

void TCP::DataIndication(Node* n, Packet* p, IPAddr_t ip, Interface*)
{ // From lower layer
  TCPHeader* tcp = (TCPHeader*)p->PeekPDU(); // Get the TCP header
  lastRxTime = Simulator::Now();            // Log last pkt time
  pktsReceived++;  // Manage rx count statistics
  if (Trace::Enabled()) 
    {
      n->TracePDU(this, tcp, p, "+");
    }

  // Check if ACK bit set, and if so notify rtt estimator
  if (tcp->flags & ACK)
    { // Notify rtt of possible rtt sample
      Time_t m = rtt->AckSeq(tcp->ackNumber);
      if (m != 0.0) lastMeasuredRTT = m;
      // Log if enabled
      NoteTimeSeq(LOG_ACK_RX, tcp->ackNumber);
      peer = tcp->peer;
      // Also insure that connection timer is canceled,
      // as this may be the final part of 3-way handshake
      CancelTimer(connTimeout, true);
    }
  // Check syn/ack and update rtt if so
  if (((tcp->flags & (SYN | ACK)) == SYN | ACK) && state == SYN_SENT)
    {
      rtt->Measurement(Simulator::Now() - synTime);
    }
    
  // Determine type of event and process
  TCP::Events_t event = StateMachine.FlagsEvent(tcp->flags);
  // debug follows
  Data* d = (Data*)p->PeekPDU(1);
  Count_t dlth = 0;
  if (d) dlth = d->Size();
  DebugHistory(DBHist(Ind, Simulator::Now(), state,
                      StateMachine.Lookup((States_t)state, event).action,
                      tcp->sequenceNumber, tcp->ackNumber, dlth));
  DEBUG(8,(cout << "TCP " << this << " got seq " << tcp->sequenceNumber 
           << " bytes " << tcp->dataLength
           << " nextack " << (Count_t)tcp->sequenceNumber + tcp->dataLength
           << " ack " << tcp->ackNumber 
           << " flags " << Hex2(tcp->flags)
           << " state " << state << endl));
#ifdef VERBOSE_DEBUG
  if (state == CLOSE_WAIT && event == SEQ_RECV)
    {
      cout << "TCP " << this << " HuH?  NewSeq in CW" << endl;
    }
  if (event == SYN_RX)
    {
      cout << "Got SYN ";
      cout << "lip " << (string)IPAddr(localNode->GetIPAddr())
           << " lport " << localPort
           << " rip " << (string)ip
           << " rport " << tcp->sourcePort << endl;
    }
#endif
  if (event == SYN_RX && state == CLOSED)
    { // debug..what's going on here?
      cout << "TCP " << this << " HuH?  Syn in Closed " << endl;
      cout << "lip " << (string)IPAddr(localNode->GetIPAddr())
           << " lport " << localPort
           << " rip " << (string)IPAddr(ip)
           << " rport " << tcp->sourcePort << endl;
    }
  DEBUG0((cout << "TCP " << this << " Processing event " << event << endl));
  States_t  saveState = (States_t)state; // For testing transition to TimedWait
  Actions_t action = ProcessEvent(event);
  // debug.
  if (saveState == LISTEN && state != LISTEN)
    {
      cout << "TCP " << this
           << " HuH?  Transition from LISTEN to " << state
           << " event " << event << endl;
      exit(1);
    }
  // debug..count fin's
#ifdef VERVOSE_DEBUG
  if (tcp->flags & FIN)
    {
      nFin++;
      //if (state == ESTABLISHED && nFin > 1)
      if ( nFin > 1)
        {
          cout << "TCP " << this 
               << " nFin " << nFin
               << " event " << event
               << " state " << state
               << " oldState " << saveState << endl;
        }
    }
#endif
  if (state != saveState && state == TIMED_WAIT)
    { // This is a small hack...but works
      Application* a = GetApplication();
      if (!closeNotified)
        {
          DEBUG(9,(cout << "TCP " << this << " calling Closed from DI"
                   << " oldstate " << saveState
                   << " event " << event << endl));
          closeNotified = true;
          if (a) a->Closed(this); // Notify application of final close      
        }
    }
  if (state == CLOSED && saveState != CLOSED)
    {
      CancelAllTimers();
    }
  if (state == TIMED_WAIT)
    {
      CancelAllTimers(); // Insure no re-tx timers pending
      DEBUG0((cout << "TCP " << this << " got event " << event
              << " action " << action << " in TW State" << endl));
      ScheduleTimer(TCPEvent::TIMED_WAIT, timedWaitTimeout, twTimeout);
    }
  DEBUG0((cout << "TCP " << this << " Processing action " << action << endl));
  ProcessAction(action, p, tcp, ip);
  // Is this necessary?
  if (state == LAST_ACK && !lastAckTimeout)
    {
      DEBUG0((cout << "TCP " << this << " scheduling LATO1" << endl));
      ScheduleTimer(TCPEvent::LAST_ACK,
                    lastAckTimeout,
                    rtt->RetransmitTimeout());
    }

}

// Notifier Methods
void TCP::Notify(void*)
{  
  nRequested = false;
  SendPendingData();
}


// L4Protocol Methods
NCount_t TCP::Send(Size_t s)
{ // Send empty data
  return Send((char*)0, s);
}

NCount_t TCP::Send(char* d, Size_t s)
{ // Send with data
  if(state == ESTABLISHED || state == SYN_SENT || state == CLOSE_WAIT)
    { // Ok to buffer some data to send
      DEBUG0((cout << "TCP " << this << " sending " << s << " bytes to "
              << (string)IPAddr(peerIP) << endl));
      if (!pendingData)
        {
          pendingData = new Data(0);   // Create if non-existent
          firstPendingSeq = nextTxSeq; // Note seq of first
        }
      pendingData->Add(s,d);
      Actions_t action = ProcessEvent(APP_SEND);
      ProcessAction(action);
      return s;
    }
  cout << "TCP " << this << " send wrong state " << state << endl;
  return -1;
}

NCount_t TCP::Send(Data& d)
{ // Send with data
  DEBUG0((cout << "TCP::Send w/ Data, tcp " << this << " state " << state 
          << " size " << d.Size() << endl));
  if(state == ESTABLISHED || state == SYN_SENT || state == CLOSE_WAIT)
    { // Ok to buffer some data to send
      if (!pendingData)
        { // Create if non-existent
          pendingData = (Data*)d.Copy();
          DEBUG(1,(cout << "TCP " << this
                   << " create pending, sz " << d.Size()<<endl));
          firstPendingSeq = nextTxSeq; // Note seq of first
        }
      else
        { // Existing pending, just add this one
          DEBUG0((cout << "TCP " << this
                  << " adding pending, sz " << d.Size()<<endl));
          pendingData->Add(d.Size(), d.Contents());
        }
      Count_t sz = d.Size();
      Actions_t action = ProcessEvent(APP_SEND);
      ProcessAction(action);
      DEBUG0((cout << "TCP::Send sz " << sz << endl));
      return sz;
    }
  cout << "TCP " << this << " Send wrong state " << state << endl;
  return -1;
}

NCount_t TCP::SendTo(Size_t, IPAddr_t, PortId_t)
{ // Send to target, not allowed with TCP
  return -1;
}

NCount_t TCP::SendTo(char*, Size_t, IPAddr_t, PortId_t)
{ // Send to target not allowed with TCP
  return -1;
}

NCount_t TCP::SendTo(Data&, IPAddr_t, PortId_t)
{ // Send to target not allowed with TCP
  return -1;
}

bool     TCP::Connect(IPAddr_t ip, PortId_t p)
{
  // Debug follows
  DebugHistory(DBHist(App, Simulator::Now(), state, 1111,
                      0, 0, 0));

  DEBUG0((cout << "TCP::Connect to " << (string)IPAddr(ip)
          << " port " << p << endl));
  if (localPort == NO_PORT)
    { // Provide a default local binding
      Bind();
    }
  States_t origState = (States_t)state; // debug
  connCount = defaultConnCount;
  Actions_t action = ProcessEvent(TCP::APP_CONNECT);
  peerIP = ip;
  peerPort = p;
  openTime = Simulator::Now();
  // debug
  if (action != SYN_TX)
    {
      cout << "HuH?  Bad connect action " << action
           << " orig state " << origState << endl;
    }
  return ProcessAction(action);
}

bool     TCP::Close()
{
  if (state == CLOSED) return false; // Already closed
  DEBUG(0,(cout << "TCP " << this << " Close, state " << state << endl));
  Actions_t action = ProcessEvent(TCP::APP_CLOSE);
  DEBUG(0,(cout << "TCP " << this << " Close, state after " << state << endl));
  DEBUG(4,(cout << "TCP " << this 
           << " close at time " << Simulator::Now() 
           << " state " << state << endl));
  if (pendingData)
    { // Pending data exist, see if we need to defer
      DEBUG(0,(cout << "TCP " << this 
               << " close at time " << Simulator::Now() 
               << " state " << state 
               << " pdsize " << pendingData->Size() << endl));
      if (action == FIN_TX && pendingData->Size())
        { // Close from application, but still have pending data
          closeOnEmpty = true;
          return true;
        }
    }
  bool r = ProcessAction(action);
  if (state == LAST_ACK && !lastAckTimeout)
    {
      DEBUG0((cout << "TCP " << this << " scheduling LATO1" << endl));
      ScheduleTimer(TCPEvent::LAST_ACK,
                    lastAckTimeout,
                    rtt->RetransmitTimeout());
    }
  return r;
}

Proto_t TCP::ProtocolNumber()
{ // Return the l4 protocol number
  return ProtoNumber;
}

bool    TCP::IsTCP()
{
  return true;
}

// TimerHandler Methods
void TCP::Timeout(TimerEvent* pEv)
{
  TCPEvent* te = (TCPEvent*)pEv;
  DebugHistory(DBHist(Tout, Simulator::Now(), state, 0,
                      nextTxSeq, highestRxAck, 0));


  DEBUG(9,(cout << "TCP::Timeout this " << this << " type " << te->event
          << " state " << state 
          << " time " << Simulator::Now() << endl));
  Application* a = GetApplication();
  Actions_t    action;
  switch(te->event) {
    case TCPEvent::CONNECTION_TIMEOUT:
      DEBUG(9,(cout << "TCP " << this << " processing conn timeout, state "
               << state << endl));
      delete te;
      connTimeout = nil;
      te = nil;
      if (state == SYN_SENT)
        { // This is the active side of connection attempt, try again
          if (--connCount)
            { // Try again
              Actions_t action = ProcessEvent(TCP::APP_CONNECT);
              ProcessAction(action);
            }
          else
            {
              action = ProcessEvent(TCP::TIMEOUT);
              ProcessAction(action);
              if (a) a->ConnectionFailed(this, false);
              timeoutCount++;  // Count for statistics
              totalTimeouts++;
            }
        }
      else
        { // This is the passive (listener).  
          DEBUG0((cout << "TCP " << this << " passive conn timeout" << endl));
          // Notify application of missing ACK on 3-way handshake
          if (a) a->ConnectionFailed(this, false);
        }
      break;
    case TCPEvent::RETRANSMIT_PACKET:
      retxTimeout = nil; // No longer pending
      timeoutCount++;    // Count for statistics
      totalTimeouts++;
      ReTxTimeout();     // Process the timeout
      Stats::tcpRetxTimeouts++;
      break;
    case TCPEvent::DELAYED_ACK:
      // Time to send the pending ack
      if (nextAckSeq == 0)
        {
          cout << "HuH?  DelAck timeout with no pending ack" << endl;
        }
      else
        {
          SendAck(nextAckSeq, true);
          nextAckSeq = 0; // No longer pending
        }
      delAckTimeout = nil; // No longer pending
      break;
    case TCPEvent::TIMED_WAIT:
      // Ok to delete this tcp endpoint now
      DEBUG0((cout << "TCP " << this 
              << " TimedWait timer popped, deleting" << endl));
      timedWaitTimeout = nil;
      CancelAllTimers();
      CancelNotification(this);
      // If we have an application, delete it as well
      if (a && a->deleteOnComplete)
        {
          DEBUG(8,(cout << "TimedWait timer popped, deleting app" << endl));
          Simulator::instance->DeleteObject(a);
        }
      if (deleteOnTWait)
        {
          DEBUG(8,(cout << "TimedWait timer popped, deleting tcp" << endl));
          Simulator::instance->DeleteObject(this);
        }
      // Unbind the port
      // Moved to TCP destructor, so removed below
      //localNode->Unbind(ProtocolNumber(),
      //                  Port(), IPADDR_NONE,
      //                  peerPort, peerIP, this);
      break;
    case TCPEvent::LAST_ACK :
      // Go ahead and close connection, assuming peer has gone
      delete te;
      lastAckTimeout = nil;
      te = nil;
      DEBUG0((cout << "TCP " << this 
              << " got LAST_ACK Timeout, state " << state
              << " cn " << closeNotified 
              << " app " << a << endl));
      if (state == LAST_ACK)
        {
          action = ProcessEvent(TCP::TIMEOUT);
          ProcessAction(action);
        }
      if (!closeNotified)
        {
          closeNotified = true;
          DEBUG(9,(cout << "TCP " << this << " calling Closed from TO"
                   << " state " << state << endl));
          if (a) a->Closed(this);
        }
      DEBUG0((cout << "TCP " << this << " processing LATO, time " 
              << Simulator::Now() << endl));
      break;
  }
  DBG((Stats::tcpTimeouts++));
  if (te) delete te;
  // ? Is this necessary?
  if (state == LAST_ACK && !lastAckTimeout)
    {
      DEBUG0((cout << "TCP " << this << " scheduling LATO1" << endl));
      ScheduleTimer(TCPEvent::LAST_ACK,
                    lastAckTimeout,
                    rtt->RetransmitTimeout());
    }
}

// TCP Specific functions
bool TCP::Listen()
{ // Set endpoint to accept connection requests
  Actions_t action = ProcessEvent(TCP::APP_LISTEN);
  return ProcessAction(action);
}

void TCP::Reset()
{ // Reset to initial state
  bufferedData.clear();
  firstPendingSeq = 0;
  if (pendingData) delete pendingData;
  pendingData = nil;
  nextTxSeq = 0;
  highTxMark = 0;
  highestRxAck = 0;
  dupAckCount = 0;
  fastRecovery = false;
  needAck = false;
  noTimer = false;
  nextRxSeq = 0;
  rxWin = defaultAdvWin;
  advWin = defaultAdvWin;
  cWnd = defaultInitialCWnd * segSize;
  ssThresh  = defaultSSThresh;
  initialCWnd = defaultInitialCWnd;
  rtt->Reset();
  CancelAllTimers();
  twTimeout = defaultTwTimeout;
  retryCount = 0;
  connCount = 0;
  state = CLOSED;
  pendingClose = false;
  closeNotified = false;
  closeReqNotified = false;
  totalAck = 0;
  openTime = 0.0;
  lastRxTime = 0.0;
  lastAckTx = 0.0;
  synTime = 0.0;
  retransmitCount = 0;
  timeoutCount = 0;
  pktsSent = 0;
  pktsReceived = 0;
  peer = nil;
  nChildren = 0;
  nFin = 0;
  lastTimeoutDelay = 0.0;
  lastMeasuredRTT = 0.0;
  //dbInd  = 0;
  //dbVec.clear();
  DEBUG0((cout << "TCP " << this 
          << " reset, nextRxSeq " << nextRxSeq << endl));
}

void TCP::ChildComplete()
{
  if (!nChildren)
    {
      cout << "HuH?  TCP " << this << " child complete w/ no children" << endl;
      return;
    }
  nChildren--;
}

void TCP::Abort()
{ // Called by ICMP when  unreachable response is received
  DEBUG0((cout << "TCP " << this << " abort " << endl));
  if (state == SYN_SENT)
    { // The ICMP Abort is only of interest for connections awaiting syn-ack
      Actions_t    action;
      action = ProcessEvent(TCP::TIMEOUT);
      ProcessAction(action);
      Application* a = GetApplication();
      if (a) a->ConnectionFailed(this, true);
      totalAborts++;  // Count for statistics
    }
}

  
bool TCP::Respond(PortId_t rport, IPAddr_t raddr)
{ // This tcp is a copy of a listening tcp. The SYN packet was received
  // by the listener, and it created a copy of itself.  This copy
  // will bind to the local port/ip, remote port/ip and respond to all
  // future packets.
  state = SYN_RCVD;
  peerPort = rport;
  peerIP = raddr;
  // No need to check return from bind, since will always be true
  localNode->Bind(ProtoNumber, localPort, IPADDR_NONE, rport, raddr, this);
  DEBUG0((cout << "TCP " << this << " bound to localPort " << localPort
          << " laddr " << (string)IPAddr(IPADDR_NONE)
          << " rPort " << rport 
          << " raddr " << (string)IPAddr(raddr) << endl));
  // Send the syn/ack packet
  return ProcessAction(SYN_ACK_TX);
}

void  TCP::Reject(PortId_t rport, IPAddr_t raddr)
{ // Application not accepting more connections
  DEBUG0((cout << "TCP " << this 
          << " rejecting incomming connection request" << endl));
  cout << "TCP " << this 
       << " rejecting incomming connection request" << endl;
  SendEmptyPacketTo(RST, 0, 0, raddr, rport);
}

Count_t TCP::UnAckDataCount()
{ // Return count of unacknowledged data bytes
  return nextTxSeq - highestRxAck;
  //return highTxMark - highestRxAck;
}

Count_t TCP::BytesInFlight()
{ // Return count of unacknowledged data bytes
  return highTxMark - highestRxAck;
}

Count_t  TCP::Window() 
{ // Return window size (integer)
  return min(rxWin, cWnd);
}

Count_t  TCP::AvailableWindow()
{ // Return unfilled portion of window
  Count_t unack = UnAckDataCount(); // Number of outstanding bytes
  Count_t win = Window();
  DEBUG(8,(cout << "Time " << Simulator::Now()
           << " TCP " << this << " AVW, unack " << unack << " win " << win 
           << " rxWin " << rxWin << " cWnd " << (Count_t)cWnd << endl));
  if (win < unack) return 0;  // No space available
  return (win - unack);       // Amount of window space available
}

#ifdef MOVED_TO_SUBCLASSES
void TCP::NewCWnd(Count_t ackBytes)
{ // Adjust congestion window in response to new ack's received
  DEBUG(7,(cout << "TCP NewCWnd "
           << " time " << Simulator::Now()
           << " ackBytes " << ackBytes
           << " cwin " << cWnd
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
      Count_t adder =  (segSize * segSize) / cWnd;
      if (!adder) adder = 1;
      cWnd += adder;
    }
  NoteTimeSeq(LOG_CWIN, (Seq_t)cWnd); // Log the change if cwin logging enabled
}
#endif

// Parameterizing the TCP endpoint
void TCP::SetRTTEstimator(const RTTEstimator& newrtt)
{
  if (rtt) delete rtt;
  rtt = newrtt.Copy();
}

RTTEstimator* TCP::GetRTTEstimator()
{
  return rtt;
}

//  Statistics
Rate_t TCP::GoodPut()
{ // Calculate goodput as total bytes ack'ed since open
  Time_t now = Simulator::Now();
  if (now == openTime) return 0;
  return (Rate_t)(totalAck * 8)/(now - openTime);
}
  
Count_t TCP::RetransmitCount()
{
  return retransmitCount;
}

void TCP::EnableTimeSeq (TimeSeqSelector_t s)
{ // Enable stats collection
  if (!timeSeqStats) timeSeqStats = new TimeSeqVec_t(LOG_LAST, 0);
  TimeSeqVec_t& tsv = *timeSeqStats;
  if (!tsv[s])
    { // Enable this statistic collection
      tsv[s] = new TimeSeq_t();
    }
}

void TCP::DisableTimeSeq(TimeSeqSelector_t s)
{ // Disable stats collection
  if (!timeSeqStats) timeSeqStats = new TimeSeqVec_t(LOG_LAST, 0);
  TimeSeqVec_t& tsv = *timeSeqStats;
  if (tsv[s])
    { // Exists, clear and delete
      tsv[s]->clear();
      delete tsv[s];
    }
}

void TCP::ResetTimeSeq(TimeSeqSelector_t s)
{ // Disable stats collection
  if (!timeSeqStats) timeSeqStats = new TimeSeqVec_t(LOG_LAST, 0);
  TimeSeqVec_t& tsv = *timeSeqStats;
  if (tsv[s])
    { // Exists, clear
      tsv[s]->clear();
    }
}

void TCP::LogTimeSeq(TimeSeqSelector_t s, ostream& os, 
                     Seq_t div, Seq_t mod, char sep)
{
  if (!timeSeqStats) timeSeqStats = new TimeSeqVec_t(LOG_LAST, 0);
  TimeSeqVec_t& tsv = *timeSeqStats;
  DEBUG0((cout << "TCP::LogTimeSeq, this " << this
          << " tsv[s] " << tsv[s] << endl));
  if (tsv[s])
    {
      TimeSeq_t* ts = tsv[s];
      DEBUG0((cout << "TCP::LogTimeSeq, this " << this
              << " tsv[s].size() " << ts->size() << endl));
      for (TimeSeq_t::size_type i = 0; i < ts->size(); ++i)
        {
          os << (*ts)[i].time << sep;
          Seq_t seq = (*ts)[i].seq;
          if (div) seq /= div;
          if (mod) seq %= mod;
          os << seq << endl;
        }
    }  
}

void TCP::DBDump()
{
  cout << "DBDump TCP " << this 
       << " bsize " << bufferedData.size()
       << " ntxs " << nextTxSeq
       << " nrxs " << nextRxSeq
       << " hrxa " << highestRxAck
       << " state " << state
       << " lrx " << lastRxTime
       << " pendcl " << pendingClose
       << " nFin " << nFin
       << " fid " << fid
       << " lato " << lastAckTimeout
       << endl;
}

#ifdef USE_DEBUG_HISTORY
void TCP::DebugHistory(const DBHist& db)
{
  if (dbVec.size() < DBH_SIZE)
    {
      dbVec.push_back(db);
      dbInd = dbVec.size();
    }
  else
    {
      dbVec[dbInd++] = db;
    }
  if (dbInd == DBH_SIZE) dbInd = 0;
}
#endif

void TCP::PrintDBH()
{
  DBHVec_t::size_type dbStart = dbInd;
  if (!dbVec.size()) return; //empty
  while(1)
    {
      DBHist& db = dbVec[dbInd];
      cout << "Time " << db.time
           << " ev "  << db.event
           << " st "  << db.state
           << " ac "  << db.action
           << " sq "  << db.seq
           << " ak "  << db.ack
           << " dl "  << db.dLth << endl;
      if (!dbInd)
        { // See if wrap or done
          if (dbVec.size() < DBH_SIZE) return; // Done
          dbInd = dbVec.size() - 1;
        }
      else
        {
          dbInd--;
        }
      if (dbInd == dbStart) return;
    }
}

// Manage the state machine
TCP::Actions_t TCP::ProcessEvent(Events_t e)      // Process event
{
  States_t saveState = (States_t)state;
  DEBUG(1,(cout << "TCP " << this << " processing event " << e <<endl));
  SA stateAction = StateMachine.Lookup((States_t)state, e);
  // debug
  if (stateAction.action == RST_TX)
    {
      cout << "TCP " << this << " sending RST from state "
           << saveState << " event " << e << endl;
      cout << "lip " << (string)IPAddr(localNode->GetIPAddr())
           << " lport " << localPort
           << " rip " << (string)IPAddr(peerIP)
           << " rport " << peerPort << endl;
    }
  bool needCloseNotify = (stateAction.state == CLOSED && state != CLOSED && e != TIMEOUT);
  state = stateAction.state;
  // This is a small hack, but reduces the overall number of actions needed
  if (state == TIMED_WAIT)
    {
      if (!timedWaitTimeout)
        { // No pending timed wait timer, schedule
          DEBUG(8,(cout << "TCP " << this << " entered tw state" << endl));
          ScheduleTimer(TCPEvent::TIMED_WAIT, timedWaitTimeout, twTimeout);
        }
    }
  if (needCloseNotify && !closeNotified)
    {
      DEBUG(8,(cout << "TCP " << this << " transition to CLOSED from " << state
              << " event " << e << " closeNot " << closeNotified 
              << " action " << stateAction.action << endl));
      if (deleteOnComplete && !deleteOnTWait)
        { // Need to delete this object
          localNode->Unbind(ProtocolNumber(),
                            Port(), IPADDR_NONE,
                            peerPort, peerIP,
                            this);
          CancelAllTimers();
          CancelNotification(this);
          Simulator::instance->DeleteObject(this);
          DEBUG(8,(cout << "TCP " << this 
                   << " deleting self in APP_CLOSED" << endl));
        }
      Application* a = GetApplication();
      closeNotified = true;
      DEBUG(9,(cout << "TCP " << this << " calling Closed from PE"
               << " origState " << saveState
               << " event " << e << endl));
      if (a) a->Closed(this);
      DEBUG(9,(cout << "TCP " << this << " transition to CLOSED from "
               << state << " event " << e
               << " set CloseNotif " << endl));
    }
  return stateAction.action;
}

bool TCP::ProcessAction(Actions_t a, Packet* p, TCPHeader* tcp, IPAddr_t ip)
{ // Process action in response to events
  Time_t rttTO;
  switch(a) {
    case NO_ACT:
      break;
    case ACK_TX:
      SendEmptyPacket(ACK, nextTxSeq, nextRxSeq);
      if (tcp)
        { // Save the receiver advertised window
          rxWin = tcp->window;
        }
      break;
    case ACK_TX_1:
      // Ack in response to syn/ack
      if (tcp)
        { // Save the receiver advertised window
          rxWin = tcp->window;
        }
          CancelTimer(connTimeout, true);
          DEBUG0((cout << "TCP " << this << " connectionEst, localA "
                  << localApplication  << endl));
          // Notify application
          if (localApplication)
            {
              localApplication->ConnectionComplete(this);
            }
      if (!pendingData || pendingData->Size() == 0)
        { // Application did not send anything, need to send ack
          SendEmptyPacket(ACK, nextTxSeq, nextRxSeq);
        }
      break;
    case RST_TX:
      DEBUG0((cout << "Sending rst" << endl));
      if (tcp)
        { // Use source ip/port
          SendEmptyPacketTo(RST, 0, 0, ip, tcp->sourcePort);
        }
      else
        { // No peer info, just use ip
          SendEmptyPacketTo(RST, 0, 0, ip, peerPort);
        }
      break;
    case SYN_TX: 
      // Schedule a Timeout for "no reply" processing
      DEBUG0((cout << "TCP " << this << " sending SYN" << endl));
      ScheduleTimer(TCPEvent::CONNECTION_TIMEOUT,
                    connTimeout,
                    cnTimeout);
      // Log time sent so we can get an initial rtt estimate
      synTime = Simulator::Now();
      SendEmptyPacket(SYN, 0, 0);
      break;
    case SYN_ACK_TX:
      // Need a little special-casing here.
      // If tcp header passed in, then make a copy and ask the
      // copy to respond, because this is the listening TCP.
      // Otherwise, it is the copied tcp from above,
      // then just send the Syn/Ack
      if (tcp && (state == LISTEN))
        {
          // If there is an attached application, copy the application
          // (which will copy this tcp as well)
          TCP* t;
          Application* myApp = GetApplication();
          bool accept = true;
          if (myApp)
            { // Yes, we have an application
              Application* a = nil;
              if  (myApp->CopyOnConnect())
                { // Need a new app for each connection
                  a = myApp->Copy();
                  DEBUG(8,(cout << "TCP Server found app, newapp doc "
                           << a->deleteOnComplete << endl));
                  t = (TCP*)a->GetL4();
                  if (!t)
                    { // App is not reporting l4, just copy self
                      t = (TCP*)Copy(); // Make a copy
                    }
                  t->AttachApplication(a);
#ifdef HAVE_QT
                  if (p->IsColored())
                    { // Syn packet has a color, assign color to this endpoint
                      if (!t->IsColored())
                        { // No color already assigned
                          t->SetColor(QColor(p->R(), p->G(), p->B()));
                        }
                    }
#endif    
                  DEBUG(8,(cout << "TCP Server found app, newapp tcp doc "
                           << t->deleteOnComplete 
                           << " coe " << t->closeOnEmpty << endl));
                }
              else
                { // Reuse app, new tcp
                  a = myApp;
                  t = (TCP*)Copy(); // Make a copy of self
                  t->AttachApplication(a);
									t->deleteAppOnComplete = false;
                }
              // Notify app of connection request  
              accept = a->ConnectionFromPeer(t, ip, tcp ? tcp->sourcePort : 0);
            }
          else
            {
              t = (TCP*)Copy();   // Make a copy of this agent
            }
          t->isServer = true; // debug
          t->DeleteOnTWait(); // Automatically delete object on timed wait
          DEBUG0((cout << "TCP " << this
                  << " Creating TCP copy of listening tcp " << t 
                  << " sourcePort " << tcp->sourcePort <<endl));
          t->peer = tcp->peer; // debug
          t->parent = this;  // Note parent
          // For debug, assign same FID to listener as requester,
          // if requester non-zero and listener 0
          if (tcp->fid != 0 && t->FlowId() == 0) t->FlowId(tcp->fid);
          bool r;
          if (accept)
            {
              DEBUG0((cout << "TCP " << this << " responding" << endl));
              r = t->Respond(tcp->sourcePort, ip);
              nChildren++;       // Count number of children
            }
          else
            { // Application not accepting more connections, reject
              t->Reject(tcp->sourcePort, ip);
              r = false;
            }
          delete p;
          return r;
        }
      // This is the copy, so just respond
      SendEmptyPacket(SYN | ACK, 0, 0);
      // We also need a timeout in case of Syn-Flood attacker
      rttTO = rtt->RetransmitTimeout();
      DEBUG0((cout << "Scheduling ACK TO " << rttTO << endl));
      ScheduleTimer(TCPEvent::CONNECTION_TIMEOUT,
                    connTimeout, rttTO);
      break;
    case FIN_TX:
      DEBUG0((cout << "TCP " << this << " sending empty FIN pkt" << endl));
      SendEmptyPacket(FIN, nextTxSeq, nextRxSeq);
      break;
    case FIN_ACK_TX:
      DEBUG(4,(cout << "TCP " << this << " sending FIN/ACK" << endl));
      SendEmptyPacket(FIN | ACK, nextTxSeq, nextRxSeq);
      if (state == LAST_ACK)
        {
          DEBUG0((cout << "TCP " << this << " scheduling LATO" << endl));
          ScheduleTimer(TCPEvent::LAST_ACK,
                        lastAckTimeout,
                        rtt->RetransmitTimeout());
        }
      break;
    case NEW_ACK:
      DEBUG0((cout << "TCP " << this << " got ack " << tcp->ackNumber
              << " highestRxAck " << highestRxAck
              << " ntxs " << nextTxSeq
              << " time " << Simulator::Now() << endl));
      
      if (tcp->ackNumber < highestRxAck)
        {
          delete p;
          return true; // Old ack, no action
        }
      if (tcp->ackNumber == highestRxAck && tcp->ackNumber < nextTxSeq)
        {
          DupAck(*tcp, ++dupAckCount); // Duplicate ack received
          DEBUG(8,(cout << "TCP " << this << " dupackcount " << dupAckCount
                   << " ack " << tcp->ackNumber << "ntxs " << nextTxSeq
                   << endl));
          return true;
        }
      if (tcp->ackNumber > highestRxAck) dupAckCount = 0; // Not duplicates
      DEBUG0((cout << "TCP " << this
               << " clearing dupack count, acknum " << tcp->ackNumber
               << " hrxa " << highestRxAck
               << " ntxs " << nextTxSeq << endl));
      
      NewAck(tcp->ackNumber);
      NewRx(p, tcp, ip); // In case any associated data
      p = nil;
      break;
    case NEW_SEQ_RX :
      // New data has been received
      NewRx(p, tcp, ip);
      return true; // ! Return here instead of break, since we don't delete p
    case RETX:
      break;
    case TX_DATA:
      SendPendingData(); // Send as much as windows allow
      break;
    case PEER_CLOSE :
      {
        // First we have to be sure the FIN packet was not received
        // out of sequence.  If so, note pending close and process
        // new sequence rx
        if (tcp && tcp->sequenceNumber != nextRxSeq)
          { // process close later
            pendingClose = true;
            DEBUG0((cout << "TCP " << this << " setting pendingClose" 
                    << " rxseq " << tcp->sequenceNumber 
                    << " nextRxSeq " << nextRxSeq << endl));
            NewRx(p, tcp, ip);
            return true;
          }
        // Now we need to see if any data came with the FIN
        // if so, call NewRx
        Data* d = nil;
        Count_t s = 0;
        if (p) d = (Data*)p->PeekPDU(1);
        if (d) s = d->Size();
        if (s)
          {
            NewRx(p, tcp, ip);
            p = nil; // Don't delete later
          }
        States_t saveState = (States_t)state; // Used to see if app responds
        DEBUG(4,(cout << "TCP " << this 
                 << " peer close, state " << state << endl));
        Application* a = GetApplication();
        if (a)
          { // Application exists, notify
            if (!closeReqNotified)
              {
                DEBUG0((cout << "TCP " << this 
                        << " calling AppCloseRequest" << endl));
                a->CloseRequest(this);
              }
            closeReqNotified = true;
          }
        else
          {
            DEBUG0((cout << "TCP " << this 
                   << " PEER_CLOSE w/ no app" << endl));
            Close(); // Just close our end as well
          }
        DEBUG(4,(cout << "TCP " << this 
                 << " peer close, state after " << state << endl));
        if (state == saveState)
          { // Need to ack, the application will close later
            SendEmptyPacket(ACK, nextTxSeq, nextRxSeq);
            // Also need to re-tx the ack if we 
          }
        if (state == LAST_ACK)
          {
            DEBUG0((cout << "TCP " << this << " scheduling LATO1" << endl));
            ScheduleTimer(TCPEvent::LAST_ACK,
                          lastAckTimeout,
                          rtt->RetransmitTimeout());
          }
      }
      break;
    case APP_CLOSED :
      // Notify application of completed close
      if (!closeNotified && state != SYN_SENT) // This is a real HACK..fix this
        {
          Application* a = GetApplication();
          closeNotified = true;
          if (a) a->Closed(this); // Notify application of final close
          if (deleteOnComplete && !deleteOnTWait)
            { // Need to delete this object
              localNode->Unbind(ProtocolNumber(),
                                Port(), IPADDR_NONE,
                                peerPort, peerIP,
                                this);
              CancelAllTimers();
              CancelNotification(this);
              Simulator::instance->DeleteObject(this);
              DEBUG(8,(cout << "TCP " << this 
                       << " deleting self in APP_CLOSED" << endl));
            }
        }
      break;
    case APP_NOTIFY :
      {
        Application* a = GetApplication();
        // NOtify app that connection failed
        if (a) a->ConnectionFailed(this, false);
      }
      // Note, fall through to CANCEL_TM here
    case CANCEL_TM :
      CancelAllTimers();
      break;
	case SERV_NOTIFY:
   		{

       		/* Added for the case where the TCP server application requires
          	a notification about when the connection it accepted moved to
          	Established state*/

       		if (localApplication)
           	localApplication->ServConnectionComplete(this);
      	 	break;
   		}
    default: // Some compilers warn about missing enum values
      break;
    }
  if (p) delete p;
  return true;
}

// Packet Sending Functions
void TCP::SendEmptyPacket(Byte_t flags, Seq_t seq, Seq_t ack)
{
  SendEmptyPacketTo(flags, seq, ack, peerIP, peerPort);
}

void TCP::SendEmptyPacketTo(Byte_t flags, Seq_t seq, Seq_t ack,
                          IPAddr_t dstip, PortId_t dstport)
{
  SendPacketTo(flags, seq, ack, NewPacket(), dstip, dstport);
}

void TCP::SendPacket(Byte_t flags, Seq_t seq, Seq_t ack, Packet* p)
{
  SendPacketTo(flags, seq, ack, p, peerIP, peerPort);
}

void TCP::SendPacketTo(Byte_t flags, Seq_t seq, Seq_t ack, Packet* p,
                       IPAddr_t dstip, PortId_t dstport)
{
  if (!localNode)
    {
      cout << "TCP " << this << " sending packet with no local node" << endl;
      return;
    }
  Count_t dataLength = 0;
  Data* d = (Data*)p->PeekPDU();  // Pointer to data (if any)
  // Append the data (if any)
  if (d)
    {
      dataLength = d->Size();
      // Also if associated data we need a retransmit timer
      Time_t rto = rtt->RetransmitTimeout();
      DEBUG(3,(cout << "TCP " << this << " scheduling retx timeout, time"
              << Simulator::Now() + rto 
               << " future " << rto << endl));
      if (retxTimeout == nil || !noTimer)
        { // Skip this only if already active and "noTimer" set
          ScheduleTimer(TCPEvent::RETRANSMIT_PACKET, 
                        retxTimeout,
                        rto);
        }
      lastTimeoutDelay = rto;
      // Notify the rtt estimator we are sending this packet
      rtt->SentSeq(seq, dataLength);
      // Count possible retransmits
      if (Seq(seq) < highTxMark) retransmitCount++;
      DEBUG(1,(cout << "TCP " << this << " sending seq " << seq
               << " ack " << ack
               << " lth " << dataLength
               << " flags " << Hex2(flags) 
               << " time " << Simulator::Now() << endl));
    }
  else
    {
      DEBUG(1,(cout << "TCP " << this << " sending ack " << ack << endl));
      DEBUG(8,(cout << "TCP " << this << " sending ack " << ack << endl));
    }
  // Build the TCP Header
  TCPHeader* h = new TCPHeader();
  h->peer = this;
  h->sourcePort = localPort;
  h->destPort = dstport;
  h->sequenceNumber = seq;
  h->ackNumber = ack;
  // Set FIN flag if in FIN states
  if (state == FIN_WAIT_1 || state == FIN_WAIT_2)
    {
      if (pendingData && !pendingData->Size())
        { // Set FIN bit if no more pending
          flags |= FIN;
        }
    }
  if (needAck)
    {
      flags |= ACK;
    }
  needAck = false;
  h->flags = flags;
  if (flags & ACK) lastAckTx = Simulator::Now();
#ifdef VERVOSE_DEBUG
  // debug
  if (d)
    {
      if (isServer && d->Size() < segSize && (flags & FIN) == 0)
        cout << "HuH? Server sending short packet w/o FIN" 
             << " size " << d->Size() << endl;
    }
#endif
  DEBUG0((cout << "TCP " << this 
          << " sending pkt, flags " << Hex2(flags) << endl));
  // Check and add header checksum if requested (code later)
  h->window = advWin;
  // The next three are not part of "real" tcp headers, but are
  // useful in the simulation
  h->fid = fid;
  h->dataLength = dataLength;
  h->cWnd = CWnd();
  // Add this header to the packet
  p->PushPDU(h); 

  // Call the l4 TX callback before trace, since the callback
  // might delete the packet
  if (!localNode->CallCallbacks(Layer(), Proto(), PacketCallbacks::TX, p))
    return;// Callback deleted the packet
   
  // Trace this PDU
  if (Trace::Enabled())
    {
      Trace::Instance()->NewNode(localNode);
      localNode->TracePDU(this, h, p, "-");
    }

  // Log the ACK TX if ACK bit set
  if (flags & ACK)
    {
      NoteTimeSeq(LOG_ACK_TX, seq);
    }

  // Log in debug history
  Count_t dLen = 0;
  if (pendingData) dLen = pendingData->Size();
  DebugHistory(DBHist(Pkt, Simulator::Now(), state, dLen,
                      h->sequenceNumber, h->ackNumber, dataLength));

  // Pass to layer 3 protocol
  // Create the Layer 3 request info
  // Test for IPV6 and use if specified (code later)
  IPV4ReqInfo r(dstip, IPADDR_NONE, ttl, ProtoNumber, tos); // Info for l3
  DEBUG0((cout << "TCP " << this
          << " IPV4Req peer is " << (string)IPAddr(dstip) << endl));
  // Forward to layer 3
  if (extraTxDelay != 0)
    { // Need to schedule an event for later
      Scheduler::Schedule(
          new L4DelayedTxEvent(l3Proto, localNode,
                               p, r, 0, false),
          extraTxDelay, this);
    }
  else
    {
      l3Proto->DataRequest(localNode, p, &r);
    }
  
  // Maintain transmit count statistics
  pktsSent++;
}

bool TCP::SendPendingData(bool withAck)
{
  if (!pendingData) return(false); // No data exists
  Count_t nSent = 0; // Count number packets sent
  while(pendingData->SizeFromSeq(firstPendingSeq, nextTxSeq))
    { // Send as long as data remains
      Count_t w = AvailableWindow();// Get available window size
      DEBUG(8,(cout << "Time " << Simulator::Now()
               << " TCP " << this << " SendPendingData"
               << " w " << w 
               << " rxwin " << rxWin
               << " cwnd " << cWnd
               << " segsize " << segSize
               << " nextTxSeq " << nextTxSeq
               << " highestRxAck" << highestRxAck 
               << " pd->Size " << pendingData->Size()
               << " pd->SFS " << pendingData->SizeFromSeq(firstPendingSeq, nextTxSeq)
               << endl));
      if (w < segSize && pendingData->Size() > w) break; // No more
      // Insure buffer space available before continuing
      if (!BufferAvailable(segSize + 100)) // Fudge a bit to allow for hdrs
        { // No Buffer space available on output link
          // request a notification when available
          DEBUG(8,(cout << "SendPending, NBS" << endl));
	  if(!nRequested)
	    {
	      RequestNotification(this, nil); // Notify when space available
	      nRequested = true;
	    }
          DEBUG(8,(cout << "TCP Req Notif " << this
                   << " notifier " << (NotifyHandler*)this
                   << endl));
          break;
        }
      Count_t s = min(w, segSize);  // Send no more than window
      if (Trace::Enabled())
        {
          Trace::Instance()->AppendEOL(); // Each on separate line
        }
      // CopyFromSequ will return < s bytes if < s data available
      Data* d = pendingData->CopyFromSeq(s, firstPendingSeq, nextTxSeq);
      DEBUG(8,(cout << "TCP " << this << " sendPendingData"
              << " txseq " << nextTxSeq
              << " s " << s 
              << " datasize " << d->Size() << endl));
      Byte_t flags = 0;
      // Compute amount still remaining to see if we need FIN flag
      Count_t remainingData = pendingData->SizeFromSeq(
                                          firstPendingSeq,
                                          nextTxSeq + Seq(d->Size()));
      DEBUG(2,(cout << "TCP " << this << "remData " << remainingData
               << " coe " << closeOnEmpty << endl));
      if (closeOnEmpty && (remainingData == 0) && bytesSent != 0)
        { // Need to include FIN
          DEBUG0((cout << "TCP " << this << " setting FIN_WAIT_1" << endl));
          flags |= FIN;
          state = FIN_WAIT_1;
        }
      // Log the sequence sent if enabled
      DEBUG0((cout << "TCP " << this << " logging tx seq " << nextTxSeq 
              << " time " << Simulator::Now() << endl));
      NoteTimeSeq(LOG_SEQ_TX, nextTxSeq);
      // Create and send the packet
      Size_t sz = d->Size();                      // Size of packet
      Packet* p = NewPacket();
      p->PushPDU(d);                              // Add the data pdu
      SendPacket(flags, nextTxSeq, nextRxSeq, p); // No flags
      nSent++;                                    // Count sent this loop
      nextTxSeq += sz;                            // Advance next tx sequence
      highTxMark = max(nextTxSeq, highTxMark);    // Note the high water marh
      bytesSent += sz;                            // Statistics 
    }
  // If we sent anything be sure there is a re-tx timer pending
  DEBUG0((cout << "TCP " << this << " SPD firstPend " << firstPendingSeq
          << " nextTxSeq " << nextTxSeq
          << " sizefromseq " 
          << pendingData->SizeFromSeq(firstPendingSeq, nextTxSeq)
          << " retxTo " << retxTimeout << endl));
  
  if (nSent)
    { // At least one packet sent, be sure we have pending re-tx timer
      if (!retxTimeout)
        { // Need to schedule re-tx
          Time_t rto = rtt->RetransmitTimeout();
          DEBUG(3,(cout << "TCP " << this << " scheduling retx timeout1, time"
                   << Simulator::Now() + rto 
                   << " future " << rto << endl));
          ScheduleTimer(TCPEvent::RETRANSMIT_PACKET, 
                        retxTimeout,
                        rto);
          lastTimeoutDelay = rto;
        }
    }
  return (nSent > 0);
}

// Ack management
void TCP::CommonNewAck(Seq ack, bool skipTimer)
{ // CommonNewAck is called only for "New" (non-duplicate) acks
  // and MUST be called by any subclass, from the NewAck function
  // Always cancel any pending re-tx timer on new acknowledgement
  DEBUG(1,(cout << "TCP " << this << "Cancelling retx timer " << endl));
  if (!skipTimer)CancelTimer(retxTimeout, true);
  Count_t numberAck = ack - highestRxAck; // Number bytes ack'ed
  DEBUG(5,(cout << "TCP " << this << " NewAck " << ack 
           << " numberAck " << numberAck << endl));
  totalAck += numberAck;      // Count for statistics
  highestRxAck = ack;         // Note the highest recieved Ack
  if (ack > nextTxSeq) nextTxSeq = ack; // If advanced
  //NewCWnd(numberAck);         // Adjust congestion window
  // See if all pending ack'ed; if so we can delete the data
  if (pendingData)
    { // Data exists, see of can be deleted
      if (pendingData->SizeFromSeq(firstPendingSeq, highestRxAck) == 0)
        { // All pending acked, can be deleted
          pendingData->Clear();
          delete pendingData;
          pendingData = nil;
          // Insure no re-tx timer
          CancelTimer(retxTimeout, true);
          // Check if FIN needed
          if (closeOnEmpty) { } // Need to check/code this
        }
    }
  // Notify application of data sent
  Application* a = GetApplication();
  if (numberAck)
    { // Notify application of data acknowledged
      if (a) a->Sent(numberAck, this);
    }
  // Try to send more data
  SendPendingData();
  // See if we need to post a re-tx timer
  if (!retxTimeout && !skipTimer &&
      (UnAckDataCount() || state == FIN_WAIT_1 || state == FIN_WAIT_2))
    {
      ScheduleTimer(TCPEvent::RETRANSMIT_PACKET, 
                    retxTimeout,
                    rtt->RetransmitTimeout());
    }
}

void TCP::SendAck(Seq ack, bool forced)
{
  bool sendIt = true;
  // Forced is true on a del-ack timeout
  if (!forced)
    { // See if we need to delay this ack
      if (daTimeout != 0.0)
        { // Using delayed ack, see if already pending
          if (delAckTimeout)
            { // Cancel this one and send the packet
              DEBUG0((cout << "Canceling delAckTimeout" << endl));
              CancelTimer(delAckTimeout, true);
            }
          else
            { // Delay this ack
              DEBUG0((cout << "Delaying ack by " << daTimeout
                      << " seconds" << endl));
              ScheduleTimer(TCPEvent::DELAYED_ACK, delAckTimeout, daTimeout);
              nextAckSeq = ack;
              sendIt = false;
            }
        }
    }
  DEBUG0((cout << "TCP " << this << " sending ack (nrxs), " << ack
          << " latx " << lastAckTx
          << " nextTxSeq " << nextTxSeq << endl));
  if (!SendPendingData(true))
    {  // Nothing sent, send empty
      if (lastAckTx == Simulator::Now())
        {
          DEBUG(7,(cout << "HuH? Resending ack?" << endl));
          return;
        }
      if (sendIt)SendEmptyPacket(ACK, nextTxSeq, ack);
      DEBUG(6,(cout << "TCP " << this 
               << " SendAck, npd, pd " << pendingData << endl));
    }
  else
    {
      DEBUG(7,(cout << "TCP " << this 
               << " SendAck, pendingdata found" << endl));
    }
}

// New Received Data
void TCP::NewRx(Packet* p, TCPHeader* h, IPAddr_t ip)
{
  // Packet still has TCP header in the PDU stack, in case we buffer it
  int origState = state;
  bool noDelete = false;
  Data* d = (Data*)p->PeekPDU(1);
  Count_t s = 0;         // Size of associated data
  if (d) s = d->Size();  // Non-zero size of data exists
  DEBUG(1,(cout << "TCP " << this << " NewRx, d " << d 
           << " size " << s << endl));
  if (!d || !s)
    {// Nothing to do if no associated data or empty
      delete p;          // Delete the packet
      return;            
    }
  DEBUG0((cout << "TCP " << this << " NewRx, seq " << h->sequenceNumber 
          << " nrxs " << nextRxSeq << " size " << s
          << endl));
  
  // Log sequence received if enabled
  NoteTimeSeq(LOG_SEQ_RX, h->sequenceNumber);
  // Three possibilities
  // 1) Received seq is expected, deliver this and any buffered data
  // 2) Received seq is < expected, just re-ack previous
  // 3) Received seq is > expected, just re-ack previous and buffer data
  needAck = true;  // Note we need the ACK bit on next transmitted pkt
  if (h->sequenceNumber == nextRxSeq)
    { // If seq is expected seq
      // 1) Update nextRxSeq
      // 2) Deliver to application this packet
      // 3) See if any buffered can be delivered
      // 4) Send the ack
      nextRxSeq += s;           // Advance next expected sequence
      bytesReceived += s;       // Statistics
      DEBUG0((cout << "Case 1, advanced nrxs to " << nextRxSeq << endl));
      
      Application* a = GetApplication();
      if (a)
        { // Application exists
          p->PopPDU();          // Remove TCP header before calling app
          a->Receive(p, this,h->sequenceNumber);  // Advise application of data
          noDelete = true;      // Don't delete later
          if (closeNotified)
            {
              cout << "TCP " << this << " HuH?  Got data after closeNotif" 
                   << endl;
            }
        }
      DEBUG0((cout << "TCP " << this << " adv rxseq by " << s << endl));

      // Look for buffered data
      UnAckData_t::iterator i;
      // Note that the bufferedData list DOES contain the tcp header
      while (!bufferedData.empty())
        { // Check the buffered data for delivery
          DEBUG0((cout << "TCP " << this
                  << " bufferedData.size() " << bufferedData.size() 
                  << " time " << Simulator::Now() << endl));
          i = bufferedData.begin();
          Packet* p1 = i->second.first;
          Data*   d1 = 0;
          Seq     s1 = 0;
          if (i->first > nextRxSeq) break;  // Not next expected
          // Get the TCP Header
          TCPHeader* h = dynamic_cast<TCPHeader*>(p1->PopPDU());
          // Check non-null here...
          Byte_t flags = h->flags;                 // Flags (used below)
          if (i->first < nextRxSeq)
            { // remove already delivered data
              // Two cases here.
              // 1) seq + length <= nextRxSeq, just discard
              // 2) seq + length > nextRxSeq, can deliver partial
              d1 = (Data*)p1->PeekPDU();    // Get the data
              if (d1) s1 = d1->Size();
              if ((i->first + s1) < nextRxSeq)
                { // Just remove from list
                  //bufferedData.erase(i);
                  delete p1;
                  p1 = 0; // Nothing to deliver
                }
              else
                { // Remove partial data to prepare for delivery
                  Count_t dup = nextRxSeq - i->first;
                  d1->Remove(dup); // Remove already delivered data
                }
            }
          else
            { // At this point i->first must equal nextRxSeq
              if (i->first != nextRxSeq)
                { 
                  cout << "HuH?  NexRx failure, first " << i->first
                       << " nextRxSeq " << nextRxSeq << endl;
                  exit(0);
                }
              d1 = (Data*)p1->PeekPDU();    // Get the data
              if (d1) s1 = d1->Size();      // Note the size
            }
          bytesReceived += s1;
          if (a && p1)
            { // Application exists and packet exists
              a->Receive(p1, this, i->first);  // Advise application of data
            }
          else
            {
              delete p1;             // No application, just delete
            }
          DEBUG0((cout << "TCP " << this << " adv rxseq1 by " << s1 << endl));
          
          nextRxSeq += s1;           // Note data received
          DEBUG0((cout << "Case 1a, advancing nrxs by " << s1 
                  << " to " << nextRxSeq << endl));
          bufferedData.erase(i);     // Remove from list
          if (flags & FIN)
            DEBUG0((cout << "TCP " << this 
                    << " found FIN in buffered"
                    << endl));
        }
      if (pendingClose || (origState > ESTABLISHED))
        { // See if we can close now
          DEBUG0((cout << "TCP " << this
                  << " newRx with pending, bd.size " << bufferedData.size()
                  << endl));
          if (bufferedData.empty())
            {
              DEBUG0((cout << "TCP " << this
                      << " deferred PEER_CLOSE" << endl));
              ProcessAction(PEER_CLOSE);
              DEBUG0((cout << "TCP " << this 
                      << " back from PEER_CLOSE" << endl));
              noDelete = true; // packet deleted by ProcessAction
            }
        }
    }
  else if (h->sequenceNumber >= nextRxSeq)
    { // Need to buffer this one
      DEBUG0((cout << "Case 2, buffering " << h->sequenceNumber << endl));

      if (!p) cout << "HuH?  Buffering nil packet" << endl;
      UnAckData_t::iterator i = bufferedData.find(h->sequenceNumber);
      if (i != bufferedData.end())
        {
          delete i->second.first;             // Delete already buffered
        }
      bufferedData[h->sequenceNumber] = 
        PacketIPPair_t(p, ip);                // Save for later delivery
      noDelete = true;                        // Don't delete later
    }
  else
    { // debug
      DEBUG0((cout << "TCP " << this << " got seq " << h->sequenceNumber
              << " expected " << nextRxSeq
              << " flags " << Hex2(h->flags) << endl));
    }
  // Now send a new ack packet acknowledging all received and delivered data
  SendAck(nextRxSeq);

  // Delete packet if we did not process it (or save it)
  if (!noDelete) delete p;
}

// Timer Management
void TCP::ScheduleTimer(Event_t e, TCPEvent*& ev, Time_t t)
{
#ifdef EXTRA_CHECK
  // debug
  if (e == TCPEvent::RETRANSMIT_PACKET)
    {
      if (!pendingData)
        {
          cout << "TCP " << this
               << " HuH?   Scheduling RETX w/o pending, state " 
               << state << endl;
        }
    }
#endif
  Time_t now = Simulator::Now();
  if (ev)
    { // Check if this event already scheduled at same time, ignore if so
      if (ev->Time() == t + now) return; // Already scheduled for correct time
      CancelTimer(ev);
    }
  if (!ev)
    {
      ev = new TCPEvent(e);
    }
  DEBUG0((cout << "TCP " << this << " scheduling new timer "
          << ev->event << " time " << Simulator::Now()
          << " for " << t << " secs in future" << endl));
  if  (useTimerBuckets)
    {
      TimerBucket::Instance()->Schedule(ev, t, this);
    }
  else
    {
      timer.Schedule(ev, t, this);
    }
  Stats::tcpTimers++;
}

void TCP::CancelTimer(TCPEvent*& ev, bool delTimer)
{
  if (ev)
    {
      if (useTimerBuckets)
        {
          TimerBucket::Instance()->Cancel(ev);
          if (ev->evList)
            {
              // For bucketed timers, canceled events are deleted later
              // So just nil the pointer at this point
              ev = nil;
            }
          else
            {
              if (delTimer)
                { // Delete it
                  delete ev;
                  ev = nil;
                }
            }
        }
      else
        {
          timer.Cancel(ev);
          if (delTimer)
            { // Delete it
              delete ev;
              ev = nil;
            }
        }
    }
}

void TCP::CancelAllTimers()
{
  CancelTimer(connTimeout, true);
  CancelTimer(retxTimeout, true);
  CancelTimer(delAckTimeout, true);
  CancelTimer(timedWaitTimeout, true);
  CancelTimer(lastAckTimeout, true);
}

// Retransmissions
void TCP::Retransmit()
{ // Retransmit the oldest pending packet
  Byte_t flags = 0;
  DEBUG(1,(cout << "Hello from TCP::Retransmit" << endl));
  // Back up nextTxSeq to highestRxAck
  // No, below is wrong.  Do this only on  timeout
  //nextTxSeq = highestRxAck;
  // Get a new packet
  Packet* p = NewPacket();
  // Get a copy of the data to be retransmitted
  if (!pendingData)
    {
      if (state == FIN_WAIT_1 || state == FIN_WAIT_2)
        { // Must have lost FIN, re-send
          SendEmptyPacket(FIN, nextTxSeq, nextRxSeq);
        }
      cout << "HuH?  Retx without pending data pointer" << endl;
      return;
    }
  Data*   d = pendingData->CopyFromSeq(segSize, firstPendingSeq, highestRxAck);
  if (!d)
    {
      cout << "HuH?  Retx without pending data?" << endl;
      return;
    }
  // Calculate remaining data for COE check
  Count_t remainingData = pendingData->SizeFromSeq(
                                          firstPendingSeq,
                                          nextTxSeq + Seq(d->Size()));
  if (closeOnEmpty && (remainingData == 0))
    { // Add the FIN flag
      DEBUG0((cout << "TCP " << this << " adding FIN to retx" << endl));
      flags |= FIN;
    }
  p->PushPDU(d);
  // And send the packet
  DEBUG(1,(cout << "TCP " << this << " retxing seq " << highestRxAck
           << " size " << d->Size() << endl));
  // Note the time/seq in the log (if enabled)
  NoteTimeSeq(LOG_SEQ_TX, highestRxAck);
  SendPacket(flags, highestRxAck, nextRxSeq, p);
}

// Statistics Management
void TCP::NoteTimeSeq(TimeSeqSelector_t s, Seq_t seq )
{ // Make an entry in the Time/Sequence log at time Simulator::Now()
  if (!timeSeqStats) return;
  TimeSeqVec_t& tsv = *timeSeqStats;
  DEBUG0((cout << "TCP::NoteTimeSeq, this " << this
          << " tsv[s] " << tsv[s] << endl));
  if (tsv[s])
    { // This sequence is being collected
      tsv[s]->push_back(TimeSeq(Simulator::Now(), seq));
      DEBUG0((cout << "TCP::NoteTimeSeq, this " << this
              << " tsv[s]->size() " << tsv[s]->size() << endl));
    }
}

// Static member functions

void TCP::DefaultSegSize (Count_t s)  
{ 
  defaultSegSize = s;
  if (defaultTCP) defaultTCP->segSize = s;
}

void TCP::DefaultAdvWin  (Count_t s)  
{ 
  defaultAdvWin = s;
  if (defaultTCP) defaultTCP->rxWin = s;
  if (defaultTCP) defaultTCP->advWin = s;
}

void TCP::DefaultSSThresh(Count_t s) 
{ 
  defaultSSThresh = s;
  if (defaultTCP) defaultTCP->ssThresh = s;
}

void TCP::DefaultTxBuffer(Count_t s)  
{ 
  defaultTxBuffer = s;
  if (defaultTCP) defaultTCP->txBuffer = s;
}

void TCP::DefaultRxBuffer(Count_t s)  
{ 
  defaultRxBuffer = s;
  if (defaultTCP) defaultTCP->rxBuffer = s;
}

void TCP::DefaultTwTimeout(Time_t t)  
{ 
  defaultTwTimeout = t;
  if (defaultTCP) defaultTCP->twTimeout = t;
}

void TCP::DefaultConnTimeout(Time_t t)  
{ 
  defaultConnTimeout = t;
  if (defaultTCP) defaultTCP->cnTimeout = t;
}

void TCP::DefaultDelAckTimeout(Time_t t) 
{ 
  defaultDelAckTimeout = t;
  if (defaultTCP) defaultTCP->daTimeout = t;
}

void TCP::DefaultInitialCWnd(Count_t w) 
{ 
  defaultInitialCWnd = w;
  if (defaultTCP) defaultTCP->initialCWnd = w;
}

void TCP::LogFlagsText(bool b)        
{ 
  logFlagsText = b;
}

void TCP::UseTimerBuckets(bool u) 
{ 
  useTimerBuckets = u;
}

void TCP::DefaultConnCount(Count_t c) 
{ 
  defaultConnCount = c;
}


//  Manage default TCP variation
void TCP::Default(const TCP& t)
{ // Set default TCP variation
  if (defaultTCP) delete defaultTCP; // Delete any existing
  defaultTCP = (TCP*)t.Copy();       // Get a copy of specified TCP
}

TCP& TCP::Default()
{
  if (!defaultTCP) Default(TCPTahoe()); // Default to Tahoe if not specified
  return *defaultTCP;
}


// Backplane related methods
class BPTcp
{
public:
  typedef enum {
      SPORT,
      DPORT,
      SEQ,
      ACK,
      HLTH,
      FLAGS,
      WSIZE,
      CKSUM
  } TCPItems_t;
};

#ifndef WIN32
void TCP::RegisterBackplane(DSHandle_t)
{
PRHandle_t prh;
ITHandle_t ith;

cout << "Hello from TCP:RegisterBackplane" << endl;

  prh = RegisterProtocol("TCP", PROTO_OPTIONAL, TCPExportQuery, NULL);
  ith = RegisterItem(prh, "Source Port", ITEM_OPTIONAL, 16,
                     TCPExportCallback,  (void*)BPTcp::SPORT,
                     TCPImportCallback,  (void*)BPTcp::SPORT,
                     TCPDefaultCallback, (void*)BPTcp::SPORT);
  ith = RegisterItem(prh, "Destination Port", ITEM_REQUIRED, 16,
                     TCPExportCallback,  (void*)BPTcp::DPORT,
                     TCPImportCallback,  (void*)BPTcp::DPORT,
                     TCPDefaultCallback, (void*)BPTcp::DPORT);
  ith = RegisterItem(prh, "Sequence Number", ITEM_REQUIRED, 32,
                     TCPExportCallback,  (void*)BPTcp::SEQ,
                     TCPImportCallback,  (void*)BPTcp::SEQ,
                     TCPDefaultCallback, (void*)BPTcp::SEQ);
  ith = RegisterItem(prh, "Acknowledgement Number", ITEM_REQUIRED, 32,
                     TCPExportCallback,  (void*)BPTcp::ACK,
                     TCPImportCallback,  (void*)BPTcp::ACK,
                     TCPDefaultCallback, (void*)BPTcp::ACK);
  ith = RegisterItem(prh, "TCP Flags",              ITEM_REQUIRED, 32,
                     TCPExportCallback,  (void*)BPTcp::FLAGS,
                     TCPImportCallback,  (void*)BPTcp::FLAGS,
                     TCPDefaultCallback, (void*)BPTcp::FLAGS);
  ith = RegisterItem(prh, "Window Size", ITEM_OPTIONAL, 16,
                     TCPExportCallback,  (void*)BPTcp::WSIZE,
                     TCPImportCallback,  (void*)BPTcp::WSIZE,
                     TCPDefaultCallback, (void*)BPTcp::WSIZE);
  ith = RegisterItem(prh, "TCP Header Length", ITEM_OPTIONAL, 16,
                     TCPExportCallback,  (void*)BPTcp::HLTH,
                     TCPImportCallback,  (void*)BPTcp::HLTH,
                     TCPDefaultCallback, (void*)BPTcp::HLTH);
}

#endif // WIN32
int   TCP::TCPExportQuery(char* pSource, int, void*)
{
  Packet* pPacket = (Packet*)pSource;

  TCPHeader* pHdrTcp = nil;
  // Find the TCP header
  for (PDUVec_t::size_type i = 0; i < pPacket->top; ++i)
    {
      PDU* pdu = pPacket->PDUs[i];
      if (pdu && (pdu->Layer() == 4) & (pdu->Proto() == 6))
        { // Found it
          pHdrTcp = (TCPHeader*)pdu;
          break;
        }
    }
  return pHdrTcp != nil; // If found, need to export this protocol
}

int   TCP::TCPDefaultCallback(char*, void*)
{
  return 0; // Not neeed
}

int   TCP::TCPExportCallback(char* pSource, char* pRet, int lRet, void* p)
{
  Packet* pPacket = (Packet*)pSource;
  BPTcp::TCPItems_t e = (BPTcp::TCPItems_t)((int)p);
  unsigned long*  pul = (unsigned long*)pRet;
  unsigned short* pus = (unsigned short*)pRet;

  DEBUG0((cout << "Hello from TCPExportCallback, item " << e << endl));

  TCPHeader* pHdrTcp = nil;
  // Find the TCP header
  for (PDUVec_t::size_type i = 0; i < pPacket->top; ++i)
    {
      PDU* pdu = pPacket->PDUs[i];
      if (pdu && (pdu->Layer() == 4) & (pdu->Proto() == 6))
        { // Found it
          pHdrTcp = (TCPHeader*)pdu;
          break;
        }
    }

  if (!pHdrTcp) return 0; // No TCP header here

  switch(e) {
  case BPTcp::SPORT:
    *pus = pHdrTcp->sourcePort;
    return sizeof(*pus);

  case BPTcp::DPORT:
    *pus = pHdrTcp->destPort;
    return sizeof(*pus);

  case BPTcp::SEQ:
    *pul = pHdrTcp->sequenceNumber;
    return sizeof(*pul);

  case BPTcp::ACK:
    *pul = pHdrTcp->ackNumber;
    return sizeof(*pul);

  case BPTcp::HLTH:
    *pus = pHdrTcp->headerLength;
    return sizeof(*pus);

  case BPTcp::FLAGS:
    *pul = pHdrTcp->flags;
    return sizeof(*pul);

  case BPTcp::WSIZE:
    *pus = pHdrTcp->window;
    return sizeof(*pus);

  case BPTcp::CKSUM:
    *pus = pHdrTcp->checksum;
    return sizeof(*pus);

  }
  return 0;
}


int   TCP::TCPImportCallback(char* pSource, int lSource, char* pDest, void* p)
{
  BPTcp::TCPItems_t e = (BPTcp::TCPItems_t)((int)p);
  unsigned long*  pul = (unsigned long*)pSource;
  unsigned short* pus = (unsigned short*)pSource;

  if (!bpHeader)
    { // Need a header
      bpHeader = new TCPHeader();
    }

  DEBUG0((cout << "Hello from TCPImportCallback, item " << e << endl));
  switch(e) {
  case BPTcp::SPORT:
    bpHeader->sourcePort = *pus;
    break;

  case BPTcp::DPORT:
    bpHeader->destPort = *pus;
    break;

  case BPTcp::SEQ:
    bpHeader->sequenceNumber = *pul;
    break;

  case BPTcp::ACK:
    bpHeader->ackNumber = *pul;
    break;

  case BPTcp::FLAGS:
    bpHeader->flags = *pul;
    break;

  case BPTcp::WSIZE:
    // ns does not support rx side flow control!
    bpHeader->window = *pus;
    break;

  case BPTcp::HLTH:
    bpHeader->headerLength = *pus;
    break;

  case BPTcp::CKSUM:
    bpHeader->checksum = *pus;
    break;
  }
  return 0;
}
