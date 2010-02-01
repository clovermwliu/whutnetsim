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
// Georgia Tech Network Simulator - TCP Base Class
// George F. Riley.  Georgia Tech, Spring 2002

// Base class for all TCP variants

#ifndef __tcp_h__
#define __tcp_h__

#include <iostream>
#include <map>

#include "pdu.h"
#include "l4protocol.h"
#include "datapdu.h"
#include "rtt-estimator.h"
//#include "timer.h"
//#include "seq.h"
//#include "notifier.h"
//#include "backplane.h"

class TCP;
//TCP相关类定义
class TCPHeader : public L4PDU {
public :
  TCPHeader();                          //默认构造函数
  TCPHeader(char*, Size_t&, Packet*);   // 从串行化中重构TCP报头
  Size_t  Size() const;
  PDU*    Copy() const;
  Proto_t Proto() { return 6;}         // 查询TCP协议号（TCP是6号）
  void    Trace(Tfstream&, Bitmap_t,   // TCP报头信息记录函数
                 Packet* = nil, const char* = nil);
  // 串行化相关
  Size_t SSize();                   // 查询串行化所需大小
  char*  Serialize(char*, Size_t&); // 串行化到缓冲区
  char*  Construct(char*, Size_t&); // 从缓冲区重构类
public:
  PortId_t sourcePort;     // 源端口
  PortId_t destPort;       // 目的端口
  Seq      sequenceNumber; // 序号
  Seq      ackNumber;      // 确认好
  Byte_t   headerLength;   // 报头长度
  Byte_t   flags;          // 标识位
  Count_t  window;         // 窗口
  Word_t   checksum;       // 校验和
  Word_t   urgentPointer;  // 紧急指针
  FId_t    fid;           // 流ID号（非TCP报头内容）
  Count_t  dataLength;    // 数据长度（非TCP报头内容，主要TRACE在用）
  Count_t  cWnd;          // Congestion window is NOT in tcp header,
                          // but included here for simulation analysis
  TCP*     peer;          // Debug
};

// Data structures needed by all TCP Variants

// UnAckData_t is Used to keep track of unacked data
// Senders use this to track data sent but not acknowledged
// Recievers use this to buffer out-of-sequence receives


typedef std::pair<Packet*, IPAddr_t> PacketIPPair_t;
typedef std::map<Seq, PacketIPPair_t> UnAckData_t;

// Timer events
//Doc:ClassXRef
class TCPEvent : public TimerEvent {
public:
  // The types of TCP timer events
  typedef enum { CONNECTION_TIMEOUT, RETRANSMIT_PACKET,
                 DELAYED_ACK, TIMED_WAIT, LAST_ACK } TCPTimeout_t;
public:
  TCPEvent(Event_t ev) : TimerEvent(ev) { };
  virtual ~TCPEvent();
};

// Class TimeSeq is used for statistics logging of seq/time data
//Doc:ClassXRef
class TimeSeq {
public:
  TimeSeq(Time_t t, Seq_t s) : time(t), seq(s) { };
  TimeSeq(const TimeSeq& c) : time(c.time), seq(c.seq) { }
public:
  Time_t time;
  Seq_t  seq;
};
typedef std::vector<TimeSeq>    TimeSeq_t;    // Time/sequence logs
typedef std::vector<TimeSeq_t*> TimeSeqVec_t; // Array by seq logging type

// Debug history log
typedef enum { NoEvent, Ind, App, Tout, Pkt } DBEvents_t;
struct DBHist {
  DBHist()
    : event(NoEvent), time(0), state(0), action(0),
      seq(0), ack(0), dLth(0) { }
  DBHist(DBEvents_t e, Time_t t, int s, int a, Seq sq, Seq ak, Count_t d)
    : event(e), time(t), state(s), action(a), seq(sq), ack(ak), dLth(d) { }
  DBEvents_t event;
  Time_t     time;
  int        state;
  int        action;
  Seq        seq;   // sequence  number
  Seq        ack;   // ack number
  Count_t    dLth;  // data length
};

typedef std::vector<DBHist> DBHVec_t;

//Doc:ClassXRef
class TCP : public L4Protocol, public TimerHandler, public NotifyHandler {
  //Doc:Class Class {\tt TCP} is the base class for all {\tt TCP} models.
  //Doc:Class Inherits from {\tt L4Protocol}.

public:
  // Logging detail types
  typedef enum { LOG_SEQ_TX, LOG_ACK_TX,
                 LOG_SEQ_RX, LOG_ACK_RX,
                 LOG_CWIN,   
                 LOG_SSTHRESH, LOG_DUPACK,
                 LOG_LAST } TimeSeqSelector_t;

  // TCP Header tracing selectors
  typedef enum { SOURCE_PORT, DEST_PORT, 
                 SEQ_NUMBER, ACK_NUMBER,
                 HEADER_LENGTH, FLAGS,
                 WINDOW, CHECKSUM,
                 URGENT, FID,
                 DATA_LENGTH, CWND } TCPTrace_t; // Tracing options

  // Define the flag bits
  typedef enum { FIN = 0x01, SYN = 0x02, RST = 0x04,
                 PSH = 0x08, ACK = 0x10, URG = 0x20 } Flags;
  typedef enum { MAX_FLAGS = 0x40 } TCPMaxFlags_t;  // Flags are 6 bits
  // Typedefs for state machine
  typedef enum { CLOSED,       // 0
                 LISTEN,       // 1
                 SYN_SENT,     // 2
                 SYN_RCVD,     // 3
                 ESTABLISHED,  // 4
                 CLOSE_WAIT,   // 5
                 LAST_ACK,     // 6
                 FIN_WAIT_1,   // 7
                 FIN_WAIT_2,   // 8
                 CLOSING,      // 9
                 TIMED_WAIT,   // 10
                 LAST_STATE } States_t;
  typedef enum { APP_LISTEN,   // 0
                 APP_CONNECT,  // 1
                 APP_SEND,     // 2
                 SEQ_RECV,     // 3
                 APP_CLOSE,    // 4
                 TIMEOUT,      // 5
                 ACK_RX,       // 6
                 SYN_RX,       // 7
                 SYN_ACK_RX,   // 8
                 FIN_RX,       // 9
                 FIN_ACK_RX,   // 10
                 RST_RX,       // 11
                 BAD_FLAGS,    // 12
                 LAST_EVENT } Events_t;
  typedef enum { NO_ACT,       // 0
                 ACK_TX,       // 1
                 ACK_TX_1,     // ACK response to syn
                 RST_TX,       // 2
                 SYN_TX,       // 3
                 SYN_ACK_TX,   // 4
                 FIN_TX,       // 5
                 FIN_ACK_TX,   // 6
                 NEW_ACK,      // 7
                 NEW_SEQ_RX,   // 8
                 RETX,         // 9
                 TX_DATA,      // 10
                 PEER_CLOSE,   // 11
                 APP_CLOSED,   // 12
                 CANCEL_TM,    // 13
                 APP_NOTIFY,   // 14 - Notify app that connection failed
				 SERV_NOTIFY,  // 15 - Notify server tcp that connection completed
                 LAST_ACTION } Actions_t;
  typedef enum { ProtoNumber =  6 } TCPProto_t; // TCP l4 Protocol number
  // Constructors
  //Doc:Method
  TCP();
    //Doc:Desc   Default constructor,  no arguments.

  //Doc:Method
  TCP(Node* n);
    //Doc:Desc Construct a {\tt TCP} endpoint attached to a particular node.
    //Doc:Arg1 Node ponter to attach this {\tt TCP} protocol to.

  //Doc:Method
  TCP(const TCP&);           // Copy Constructor       
    //Doc:Desc Copy constructor.
    //Doc:Arg1 {\tt TCP} protocol to copy.

  void CommonConstructor();  // Common constructor code
  virtual ~TCP();

  // Upcalls from layer 3
  virtual void DataIndication(Node*, Packet*, IPAddr_t, Interface*);

  // Notifier Methods
  void Notify(void*);

  // L4Protocol Methods
  NCount_t    Send(Size_t);         // Send empty data
  NCount_t    Send(char*, Size_t);  // Send with data
  NCount_t    Send(Data&);          // Send with data
  NCount_t    SendTo(Size_t, IPAddr_t, PortId_t);       // Send to target
  NCount_t    SendTo(char*, Size_t, IPAddr_t, PortId_t);// Send w/ data
  NCount_t    SendTo(Data&, IPAddr_t, PortId_t);        // Send w/ data
  Proto_t     Proto() const { return 6;}                // Return l4 protocol
  bool        Connect(IPAddr_t, PortId_t);              // Connect to remote
  bool        Close();                                  // Close connection
  L4Protocol* Copy() const = 0;                         // Create a copy
  Proto_t     ProtocolNumber();                         // Return protocol num
  bool        IsTCP();                                  // True if TCP

  // TimerHandler Methods
  void Timeout(TimerEvent*); // Called when timers expire

  // TCP Specific functins
  //Doc:Method
  bool     Listen();         // Set to accept connections
    //Doc:Desc This this endpont to the listening state to accept incoming
    //Doc:Desc connections.
    //Doc:Return True if successful.

  void     Reset();          // Reset to known, closed state
  void     ChildComplete();  // Decrement number of children
  void     Abort();          // Abort pending connection request
  // Responses to connection req from peer
  // Sets state to SYN_Received, send syn/ack     
  bool     Respond(PortId_t, IPAddr_t);
  // Rejects connection, sends reset
  void     Reject(PortId_t, IPAddr_t); 

  // Parameterizing the TCP endpoint
  //Doc:Method
  void SetRTTEstimator(const RTTEstimator&);
    //Doc:Desc Set a non--default round trip time estimator.
    //Doc:Arg1 Reference to new RTT estimator for this {\tt TCP}.

  //Doc:Method
  RTTEstimator* GetRTTEstimator();
    //Doc:Desc Get a pointer to the current RTT estimator.
    //Doc:Return Poiinter to current RTT estimator.

  //Doc:Method
  void SetSSThresh(Count_t sst) { ssThresh = sst;}
    //Doc:Desc Set the slow start threshold for this connection.
    //Doc:Arg1 Slow start threshold (bytes).

  //Doc:Method
  void SetAdvertisedWindow(Count_t w) { advWin = w;}
    //Doc:Desc Set the receiver advertised window for this connection.
    //Doc:Arg1 Advertised window (bytes) for this connection.

  //Doc:Method
  void DeleteOnComplete(bool doc = true) { deleteOnComplete = doc;}
    //Doc:Desc Specify that this connection should automatically
    //Doc:Desc delete itself when it closes.
    //Doc:Arg1 True if delete on complete desired.

  //Doc:Method
  void DeleteOnTWait(bool dotw = true) { deleteOnTWait = dotw;}
    //Doc:Desc Specify that this connection should automatically
    //Doc:Desc delete itself on the completion of the {\em Timed Wait}
    //Doc:Desc state.
    //Doc:Arg1 True if delete on timed wait desired.

  //Doc:Method
  void CloseOnEmpty(bool coe = true) { closeOnEmpty = coe;}
    //Doc:Desc Specify that this connection should initiate close 
    //Doc:Desc action when all pending data has been sent.
    //Doc:Arg1 True if close on empty desired.

  //Doc:Method
  void SetSegSize(Count_t s) { segSize = s;}
    //Doc:Desc Set the segment size for this connection.
    //Doc:Arg1 Segment size (bytes).

  //Doc:Method
  void SetTxBuffer(Count_t s) { txBuffer = s;}
    //Doc:Desc Set the size of the transmit buffer.  Note: Presently
    //Doc:Desc not enforced.
    //Doc:Arg1 Size of transmit buffer (bytes).

  void SetRxBuffer(Count_t s) { rxBuffer = s;}
    //Doc:Desc Set the size of the receive buffer.  Note: Presently
    //Doc:Desc not enforced.
    //Doc:Arg1 Size of receive buffer (bytes).

  //Doc:Method
  void SetTwTimeout(Time_t t) { twTimeout = t;}
    //Doc:Desc Set the length of the {\em Timed Wait} state.
    //Doc:Arg1 Length of timed wait state (seconds).

  //Doc:Method
  void SetConnTimeout(Time_t t) { cnTimeout = t;}
    //Doc:Desc Set the length of the connection timeout.
    //Doc:Arg1 Length of connection state (seconds).

  //Doc:Method
  void SetChildLimit(Count_t l) { childLimit = l;}
    //Doc:Desc Set a limit on the number of simulataneous child
    //Doc:Desc (spawned by a listening endpoing).
    //Doc:Arg1 Desired child limit.

  //Doc:Method
  void SetInitialCWnd(Count_t w) { cWnd = w * segSize; initialCWnd = cWnd;}
    //Doc:Desc Set a non-default value for the initial congestion window
    //Doc:Desc in units of SEGMENTS;  
    //Doc:Arg1 Desired initial congestion window in SEGMENTS.

  // Statistics management
  //Doc:Method
  Rate_t  GoodPut();           // Return goodput so far
    //Doc:Desc Return the current goodput (bits / second) for this
    //Doc:Desc connection.
    //Doc:Return Goodput in bits per second.

  //Doc:Method
  Count_t RetransmitCount();   // Return number of retransmits
    //Doc:Desc Query the number of packet retransmissions for this
    //Doc:Desc connection.
    //Doc:Return Retransmitted packet count.

  //Doc:Method
  void    EnableTimeSeq (TimeSeqSelector_t); // Enable stats collection
    //Doc:Desc The {\tt TCP} model in \GTNS\ will keep detailed sequence
    //Doc:Desc versus time history information for various counters.  This
    //Doc:Desc method enables this data collection.  This information can
    //Doc:Desc later be streamed to a data file with {\tt LogTimeSeq}.
    //Doc:Desc More than one data collection can be enabled at a time with
    //Doc:Desc multiple  calls to {\tt EnableTimeSeq}.
    //Doc:Arg1 Desired level of  history connection.  Can be one of:
    //Doc:Arg1 LOG_SEQ_TX (sequence number transmitted),
    //Doc:Arg1 LOG_ACK_TX (ack number transmitted),
    //Doc:Arg1 LOG_SEQ_RX (sequence number received),
    //Doc:Arg1 LOG_ACK_RX (ack number received),
    //Doc:Arg1 LOG_CWIN (congestion window size).

  //Doc:Method
  void    DisableTimeSeq(TimeSeqSelector_t); // Disable stats collection
    //Doc:Desc Stop history data collection for the specified type of sequence
    //Doc:Desc number.
    //Doc:Arg1 Desired history level to stop, same levels as in
    //Doc:Arg1 {\tt EnableTimeSeq}.

  //Doc:Method
  void    ResetTimeSeq(TimeSeqSelector_t);   // Remove existing stats
    //Doc:Desc Reset the collected information for the specified time/sequence
    //Doc:Desc collection.
    //Doc:Arg1 Desired history level to reset,  same levels as in
    //Doc:Arg1 {\tt EnableTimeSeq}.

  //Doc:Method
  void    LogTimeSeq(TimeSeqSelector_t, std::ostream&,
                     Seq_t div = 0, Seq_t mod = 0, 
                     char sep = ' ');
    //Doc:Desc Log previously collected time/sequence data to a file.
    //Doc:Arg1 Desired Time/Sequence type.
    //Doc:Arg2 Output stream to log the data.
    //Doc:Arg3 If non--zero, each sample is divided by this value.
    //Doc:Arg4 If non--zero, each sample is modded by this value.
    //Doc:Arg5 Separator character between time and sequence number

  // Various debug
  //Doc:Method
  Count_t CWnd() { return cWnd;}
    //Doc:Desc Query the current value of the congestion window.
    //Doc:Return Congestion window value (bytes, double).

  void    DBDump();                  // Debug dump information
  //Doc:Method
  TCP*    Peer() { return peer;}     // Debug..get peer pointer
    //Doc:Desc Get a pointer to remote peer endpoint.  This information
    //Doc:Desc is not always available, since distributed simulations may
    //Doc:Desc not have access to the peer object.
    //Doc:Return Pointer to peer {\tt TCP} object.  {\tt nil} if unknown.

  //Doc:Method
  int     State() const { return state;}
    //Doc:Desc Query the current {\em state} of this connection.
    //Doc:Return Connection state.  See {\tt tcp.h} for a list of the
    //Doc:Return state values.

  //Doc:Method
  Count_t TimeoutCount() const { return timeoutCount;}
    //Doc:Desc Query a count of timeouts for this connection.
    //Doc:Return Number of timeouts.

  //Doc:Method
  Time_t  TimeoutDelay() const { return lastTimeoutDelay;}
    //Doc:Desc Query the length of the last scheduled timeout.
    //Doc:Return Length of last scheduled timeout (seconds).

#ifdef USE_DEBUG_HISTORY
  void    DebugHistory(const DBHist&); // Add an entry to the debug hist
#else
  inline void    DebugHistory(const DBHist&) { }; // Add an entry to the debug hist
#endif

  void    PrintDBH();                  // Print the dbhist
protected:
  // Many of these should be overridden by the TCP variation implementations
  // Window Management
  virtual Count_t  UnAckDataCount(); // Return count of number of unacked bytes
  virtual Count_t  BytesInFlight();  // Return total bytes in flight
  virtual Count_t  Window();         // Return window size (integer)
  virtual Count_t  AvailableWindow();// Return unfilled portion of window
  // Below moved to individual sub-classes (Tahoe, Reno, etc) in NewAck
  // virtual void     NewCWnd(Count_t); // Adjust cWnd due to new ack

  // Manage State Machine
  // Process event        
  Actions_t ProcessEvent(Events_t);
  // Process action       
  bool      ProcessAction(Actions_t, Packet* p = NULL,
                          TCPHeader* tcp = NULL, IPAddr_t ip = IPADDR_NONE);

  // Packet Sending Functions
  void SendEmptyPacket(Byte_t, Seq_t, Seq_t);
  void SendEmptyPacketTo(Byte_t, Seq_t, Seq_t,
                         IPAddr_t dstip, PortId_t dstport);
  void SendPacket(Byte_t, Seq_t, Seq_t, Packet*);
  void SendPacketTo(Byte_t, Seq_t, Seq_t, Packet*,
                    IPAddr_t dstip, PortId_t dstport);
  // Send as much pending data as windows allow, include ack flag if requested
  // Return true if sent anything
  bool SendPendingData(bool withAck = false);

  // Ack Management
  virtual void NewAck(Seq) = 0;// Process new acknowledgement received
  virtual void CommonNewAck(Seq, bool);// NewAck common for all variants
  virtual void DupAck(const TCPHeader&, Count_t) = 0; // Duplicate ack received
  virtual void SendAck(Seq, bool = false);// Send specified ack (optional frcd)

  // New Recieved Data
  virtual void NewRx(Packet*, TCPHeader*, IPAddr_t);

  // Timer management
  void ScheduleTimer(Event_t, TCPEvent*&, Time_t);
  void CancelTimer(TCPEvent*&, bool delTimer = false);
  void CancelAllTimers();

  // Retransmissions
  virtual void ReTxTimeout() = 0; // Retransmit timeout (supplied by subclass)
  virtual void Retransmit();      // Retransmit the oldest pending packet

  // Statistics gathering
  void NoteTimeSeq(TimeSeqSelector_t, Seq_t );

public:
  // Setting defaults
  //Doc:Method
  static void DefaultSegSize (Count_t s);
    //Doc:Desc Set the default segment size.  The default
    //Doc:Desc value is 512 bytes unless a new value is assigned.
    //Doc:Arg1 Default segment size (bytes).

  //Doc:Method
  static void DefaultAdvWin  (Count_t s);
    //Doc:Desc Set the default advertised window size.
    //Doc:Arg1 Default advertised window size (bytes).The default
    //Doc:Desc value is 65k bytes unless a new value is assigned.

  //Doc:Method
  static void DefaultSSThresh(Count_t s);
    //Doc:Desc Set the default  slow--start threshold size.
    //Doc:Arg1 Default slow--size size (bytes).The default
    //Doc:Desc value is 65k bytes unless a new value is assigned.

  //Doc:Method
  static void DefaultTxBuffer(Count_t s);
    //Doc:Desc Set the default transmit buffer size.
    //Doc:Arg1 Default transmit buffer size (bytes).The default
    //Doc:Desc value is 4Gb unless a new value is assigned.

  //Doc:Method
  static void DefaultRxBuffer(Count_t s);
    //Doc:Desc Set the default receive buffer size.
    //Doc:Arg1 Default receive buffer size (bytes).The default
    //Doc:Desc value is 4Gb unless a new value is assigned.

  //Doc:Method
  static void DefaultTwTimeout(Time_t t);
    //Doc:Desc Set the default value of the the timed wait timeout.
    //Doc:Arg1 Default time wait value (seconds).The default
    //Doc:Desc value is 5 seconds unless a new value is assigned.

  //Doc:Method
  static void DefaultConnTimeout(Time_t t);
    //Doc:Desc Set the default value of the the connection timeout.
    //Doc:Arg1 Default time to wait for syn-ack (seconds).The default
    //Doc:Desc value is 6 seconds unless a new value is assigned.

  //Doc:Method
  static void DefaultDelAckTimeout(Time_t t);
    //Doc:Desc  Set the value of the delayed ack timeout.  If non-zero, delayed
    //Doc:Desc acks are used.
    //Doc:Arg1 Value of del-ack timer.
  
  //Doc:Method
  static void DefaultInitialCWnd(Count_t w);
    //Doc:Desc Sets the default value for the  initial congestion window
    //Doc:Desc size, in units of SEGMENTS.  
    //Doc:Arg1 Default initial cwnd value, units of SEGMENTS

  //Doc:Method
  static void LogFlagsText(bool b);
    //Doc:Desc Specifies that the trace file entries for this connection
    //Doc:Desc should show  the flags field in text, such as "SYN|ACK".
    //Doc:Desc If not specified, flags values are shown in 8 bit hex.
    //Doc:Arg1 True if  text flags desired.

  //Doc:Method
  static void UseTimerBuckets(bool u = true);
    //Doc:Desc Specifies that all timeout timers should be rounded to
    //Doc:Desc integral {\em bucket} values.  If specified, timers are rounded
    //Doc:Desc to units of 10ms, which is slightly more efficient in the
    //Doc:Desc simulator, and is a bit more realistic.  If not specified,
    //Doc:Desc timers are nano-second accurate.
    //Doc:Arg1 True if timer buckets desired.

  //Doc:Method
  static void DefaultConnCount(Count_t c);
    //Doc:Desc Specify the default number of connection retries.
    //Doc:Arg1 Default connection retry. The default
    //Doc:Desc value is 3 unless a new value is assigned.

  //Doc:Method
  static void Default(const TCP&); // Set default TCP variation
    //Doc:Desc Specify the default {\tt TCP} variation to use for all
    //Doc:Desc new {\tt TCP} endpoints.  Default value is {\tt TCPTahoe} 
    //Doc:Desc unless a new value is specified.
    //Doc:Arg1 Reference to any {\tt TCP} variant to set as default.

  //Doc:Method
  static TCP& Default();           // Get default TCP variation
    //Doc:Desc Return a reference to the default {\tt TCP} variation.
    //Doc:Return Reference to default {\tt TCP} variant.

  // Backplane related methods
  // Register data items with the dynamic simulation backplane
#ifndef WIN32
  static void  RegisterBackplane(DSHandle_t);
#endif
  static int   TCPExportQuery(char*, int, void*);
  static int   TCPDefaultCallback(char*, void*);
  static int   TCPExportCallback(char*, char*, int, void*);
  static int   TCPImportCallback(char*, int, char*, void*);
protected: // Members
  // History data
  UnAckData_t   bufferedData;    // Received, but out-of-sequence
  Data*         pendingData;     // Data sent by application, but not sent
  Seq           firstPendingSeq; // First sequence number in pendingData

  // Sequence Information, sender side
public: // public for debugging, should be protected
  Seq           nextTxSeq;       // Next sequence to send
  Seq           highTxMark;      // TX High water mark, for counting retx
  Seq           highestRxAck;    // largest ack received
  Seq           lastRxAck;       // For dupack testing

protected:
  Seq           fastRecoveryMark;// Mark for fast recovery
  Count_t       dupAckCount;     // Number of dup acks in a row
  bool          fastRecovery;    // True if fast recovery in progress
  bool          needAck;         // True if need ACK bit transmit
  bool          noTimer;         // True if skip resched of re-tx timer
 
  // Sequence Information, receiver side
  Seq           nextRxSeq;       // Next expected sequence
  Seq           nextAckSeq;      // Set non-zero when using delayed acks

  // Window Management
  Count_t       segSize;         // SegmentSize
  Count_t       rxWin;           // Window as received from peer
  Count_t       advWin;          // Window to advertise to peer
  Count_t       cWnd;            // Congestion window
  Count_t       ssThresh;        // Slow Start Threshold
  Count_t       initialCWnd;     // Initial (and reset) value for cWnd

  // Round Trip Time estimation
  RTTEstimator* rtt;             // Points to any subclass of RTTEstimator

  // Timers
  Timer         timer;           // Timer for various TCP timer events
  TCPEvent*     connTimeout;     // Pending connection timeout event
  TCPEvent*     retxTimeout;     // Retransmit timeout
  TCPEvent*     delAckTimeout;   // Delayed ack timeout
  TCPEvent*     timedWaitTimeout;// Timeout for timed-wait state
  TCPEvent*     lastAckTimeout;  // Timeout for last-ack state
  Time_t        twTimeout;       // Timeout period for timed-wait state
  Time_t        cnTimeout;       // Timeout period for connection retry
  Time_t        daTimeout;       // Timeout period for delayed acks

  // Counters
  Count_t       retryCount;      // Limit the re-tx retries
  Count_t       connCount;       // Connection retry count

  // State
  int           state;           // Current state
  bool          closeOnEmpty;    // True if send FIN pkt on empty data
  bool          deleteOnComplete;// True if should delete object on close
  bool          deleteOnTWait;   // True if delete after timed wait
  bool          pendingClose;    // True if close pending
  bool          closeNotified;   // True if close has been notified
  bool          closeReqNotified;// True if close request has been notified

  // Buffer limits
  Count_t       txBuffer;        // Size of tx buffer
  Count_t       rxBuffer;        // Size of rx buffer
  
  // Statistics
  Count_t       totalAck;        // Total bytes acked
  Time_t        openTime;        // Time connection was opened
  Time_t        lastRxTime;      // Time of last pkt receipt
  Time_t        lastAckTx;       // Time last ack sent
  Time_t        synTime;         // Time SYN sent
  Count_t       retransmitCount; // Count of retransmits
  Count_t       timeoutCount;    // Count of timeouts
  Count_t       pktsSent;        // Count of packets sent
  Count_t       pktsReceived;    // Count of packets received
  Count_t       bytesSent;       // Total bytes sent
  Count_t       bytesReceived;   // Total bytes received
  TimeSeqVec_t* timeSeqStats;    // Pointer to Array of time/seq stats
  TCP*          parent;          // Parent (server) tcp (if exists)
  TCP*          peer;            // Debug pointer to peer
  Count_t       nChildren;       // Number of child TCP's (if server)
  Count_t       childLimit;      // Maximum number of allowed children
  Count_t       nFin;            // Number fin packets rx, just testing
  Time_t        lastTimeoutDelay;// Timeout delay of last scheduled retx timer
  Time_t        lastMeasuredRTT; // Time of last rtt measurement
  DBHVec_t      dbVec;           // Debug history vector
  DBHVec_t::size_type dbInd;     // next available entry in dbVec
public:
  bool          isServer;        // debug..remove later
  bool          nRequested;      // notification is requested already
public:
  // Static members
  static Count_t       defaultSegSize;  // Segment size
  static Count_t       defaultAdvWin;   // Advertised window
  static Count_t       defaultSSThresh; // SlowStart Threshold
  static Count_t       defaultTxBuffer; // Maximum number of pending tx bytes
  static Count_t       defaultRxBuffer; // Maximum number of pending rx bytes
  static Time_t        defaultTwTimeout;// Default timed-wait timeout
  static Time_t        defaultConnTimeout;  // Default connection timeout
  static Time_t        defaultDelAckTimeout;// Default del-ack timer
  static Count_t       defaultInitialCWnd; // Default initial CWnd value
  static bool          logFlagsText;    // True if text mode flag logging
  static FId_t         nextFlowId;      // Next unique tcp flow id
  static bool          useTimerBuckets; // True if using timer buckets
  static Count_t       defaultConnCount;// Number of connection retries
  static TCPHeader*    bpHeader;        // Imported header from backplane
  static Count_t       totalTimeouts;   // Number of timeouts for all conns
  static Count_t       totalAborts;     // Number if ICNP aborts, all conns
private:
  static TCP*          defaultTCP;      // Default TCP variant
 };

#endif


