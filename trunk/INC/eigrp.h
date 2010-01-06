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

// Class for EIGRP routing protocol
// George F. Riley, Talal Jaafar, and Kunal Punjabi.  Georgia Tech, Spring 2004 


#ifndef __EIGRP_H__
#define __EIGRP_H__

#include <list>
#include <vector>
#include <map>
#include <string>

#include <sys/types.h>
#ifndef WIN32
#include <sys/socket.h>
#include <arpa/inet.h>
#else
#include <winsock.h>
#endif


#include "pdu.h"
#include "interface.h"
#include "l4protocol.h"
#include "protocol.h"
#include "l4demux.h"
#include "handler.h"
#include "rng.h"
#include "routing-eigrp.h"

#define EIGRP_PROTO 88
#define Infinity 999999999

typedef enum {HOLDTIMEOUT,ACTIVETIMEOUT } EIGRP_Timeout_t;

class Interface;
typedef unsigned long MsgSeq_t; // Message sequence number
typedef unsigned int  Uint_t;
typedef unsigned long Ulong_t;
typedef double        Metric_t; // For route metrics
typedef int           IF_t;     // Interface number


typedef enum { // Enumerate the multicast transmission types
    EMsgUnknown,
    EMsgReliable,
    EMsgUnreliable,
    EMsgAck
} EMCast_t;
typedef enum { // Enumerate the EIGRP Message Types
  MsgUnknown,
  MsgUpdate,
  MsgAck,
  MsgQuery,
  MsgReply,
  MsgHello 
} EMsg_t;
    
typedef enum { 
  LINK_DISTANCE_INCREASE,
  LINK_DISTANCE_DECREASE,
  LINK_STATE_CHANGE,
  RECEIVE_UPDATE,
  RECEIVE_UPDATE_DISTANCE_INCREASE,
  RECEIVE_QUERY,
  QUERY_FROM_SUCCESSOR,
  RECEIVE_LAST_REPLY 
} EIGRPInputEvent_t;

typedef enum { 
  FC_NOT_SATISFIED,
  FC_SATISFIED
} EIGRP_FC_Event_t;


// message count
typedef struct 
{
  unsigned long Hellos;
  unsigned long Updates;
  unsigned long Queries;
  unsigned long Replies;
} Mesg_Counters;



// Subclass EIGRPHeader for each of the message types

//Update Header
class UpdateHeader {
public:
  UpdateHeader() {};
  //UpdateHeader(Uint_t Version, Uint_t Opcode, Uint_t Checksum, Uint_t Flags, Uint_t Ack, Uint_t ASNumber) : 
  //EIGRPHeader(MsgUpdate, Version, Opcode, Checksum, Flags, Ack, ASNumber) {}
public:
  void setUpdateFields(Ulong_t type, Ulong_t NextHop, Ulong_t Delay, 
		  Ulong_t BW, Ulong_t MTU, Metric_t HopCount, Ulong_t Reliability, 
		  Ulong_t Load, Ulong_t Reserved, Ulong_t Prefixlength, Ulong_t dest)
    {
      typefield = type;
      //lengthfield = length;
      nexthop = NextHop;
      delay = Delay;
      bw = BW;
      mtu = MTU;
      hopcount = HopCount;
      reliability = Reliability;
      load = Load;
      reserved = Reserved;
      prefixlength = Prefixlength;
      destination = dest;
    }
public:
  Ulong_t typefield;
  Ulong_t lengthfield;
  Ulong_t nexthop;
  Ulong_t delay;
  Ulong_t bw;
  Ulong_t mtu;
  Metric_t hopcount;
  Ulong_t reliability;
  Ulong_t load;
  Ulong_t reserved;
  Ulong_t prefixlength;
  Ulong_t destination;
};

//Query Header
class QueryHeader {
public:
  QueryHeader() {};
  //QueryHeader(Uint_t Version, Uint_t Opcode, Uint_t Checksum, Uint_t Flags, Uint_t Ack, Uint_t ASNumber) : 
  //EIGRPHeader(MsgQuery, Version, Opcode, Checksum, Flags, Ack, ASNumber) {}
public:
  void setQueryFields(Ulong_t type, Metric_t HopCount, Ulong_t dest)
    {
      typefield = type;
      //lengthfield = length;
      hopcount = HopCount;
      destination = dest;
    }
public:
  Ulong_t typefield;
  Ulong_t lengthfield;
  Metric_t hopcount;
  Ulong_t destination;
};

//ReplyHeader
class ReplyHeader {
public:
  ReplyHeader() {};
  //ReplyHeader(Uint_t Version, Uint_t Opcode, Uint_t Checksum, Uint_t Flags, Uint_t Ack, Uint_t ASNumber) : 
  //EIGRPHeader(MsgReply, Version, Opcode, Checksum, Flags, Ack, ASNumber) {}
public:
  void setReplyFields(Ulong_t type, Ulong_t NextHop, Metric_t HopCount, Ulong_t dest)
    {
      typefield = type;
      //lengthfield = length;
      nexthop = NextHop;
      hopcount = HopCount;
      destination = dest;
    }
public:
  Ulong_t typefield;
  Ulong_t lengthfield; 
  Ulong_t nexthop;
  Metric_t hopcount;
  Ulong_t destination; 
};

//Hello Header
class HelloHeader {
public:
  HelloHeader() {};
  // HelloHeader(Uint_t Version, Uint_t Opcode, Uint_t Checksum, Uint_t Flags, Uint_t Ack, Uint_t ASNumber) : 
  //EIGRPHeader(MsgHello, Version, (Uint_t)Opcode, Checksum, Flags, Ack, ASNumber) {}
public:
  void setHelloFields(Uint_t type, Uint_t ip, Uint_t hold)
    {
      typefield = type;
      myip = ip;
      myholdTime = hold;
    }
public:
  Uint_t typefield;
  Uint_t myip;
  Uint_t myholdTime;
};



//typedef std::vector<UpdateHeader*> updateVec_t;
//typedef std::vector<QueryHeader*> queryVec_t;
//typedef std::vector<ReplyHeader*> replyVec_t;


// Define the packet header info for eigrp.
// This is the basic information common to all EIGRP messages.
// This is sub-classed by the more specific EIGRP messages (eg. Hello, etc).
// Since EIGRP is conceptually a Layer 4 protocol, we subclass L4PDU.
class EIGRPHeader : public L4PDU {
public:
  EIGRPHeader();
  EIGRPHeader(EMsg_t, Uint_t, Uint_t, Uint_t, Uint_t, Uint_t, Uint_t, Uint_t);
  //EIGRPHeader(const EIGRPHeader&);
  virtual ~EIGRPHeader();
  Size_t  Size() const { return 16;}
  Layer_t Layer() { return 4;}
  Proto_t Proto() { return EIGRP_PROTO;}
  PDU*    Copy() const;  
  // Serialization
  Size_t  SSize();                     // Size needed for serialization
  char*   Serialize(char*, Size_t&);   // Serialize to a buffer
  char*   Construct(char*, Size_t&);   // Construct from buffer
  // Packet tracing
  void    Trace(Tfstream&, Bitmap_t);  // Trace the contents of this pdu
public:
  MsgSeq_t        msgSeq;// Sequence number
  EMCast_t        mTxt;  // Message transmission type
  IPAddr_t        mSrc;  // Source IP address
  IPAddr_t        mDst;  // Dest IP address
  EMsg_t          mType; // Type of message

  Uint_t version:8;
  Uint_t opcode:8;
  Uint_t checksum:16;
  Uint_t flags:32;
  Uint_t ack:32;
  Uint_t asNumber:32;
  std::vector<UpdateHeader*> updateHeader;
  std::vector<QueryHeader*> queryHeader;
  std::vector<ReplyHeader*> replyHeader;
  std::vector<HelloHeader*> helloHeader;
  //updateVec_t updateHeader;
  //queryVec_t queryHeader;
  //replyVec_t replyHeader;
  //HelloHeader* helloHeader;
  
};


class EIGRPEvent;
typedef struct feasibles {
  IPAddr_t feas_succ;
  Metric_t RD;              // reported distance
  Metric_t min_RD;          // minimum distance reported by this feas_succ
  Metric_t metric;
  struct feasibles *fee;
}d1;

typedef std::list<d1> successor_list_t;


typedef struct successors {
  IPAddr_t nexthopIPAddress;
  struct successors *su;

} success;


typedef struct neighbors_ {
  IPAddr_t neigh;
  int RSflag;
  int needToReply;
  EIGRPEvent* activeEvent;
  struct neighbors_ *nb;

} neighbr;


class RouteEntry {
public:
  //default constructor
  RouteEntry() : destIPAddress(0), prefixlength(32),metric(Infinity),pointer_(NULL) { }
  //overloaded constructor
  RouteEntry(IPAddr_t n, int p,int m,struct successors* q) : destIPAddress(n), prefixlength(p), metric(m),pointer_(q) { }
  bool operator!=(const IPAddr_t& i) { return i != destIPAddress; }

public:
  IPAddr_t destIPAddress;
  Ulong_t prefixlength;
  Metric_t metric;
  struct successors *pointer_;

  //  long node;
  //  long metric;
  //  long nexthop;
};
typedef std::list<RouteEntry> routeEntryList_;


class TopologyEntry {
public:
  typedef enum { PASSIVE,ACTIVE0,ACTIVE1,ACTIVE2,ACTIVE3 } eigrp_state_t;
  
  //default constructor
  TopologyEntry() : destin(0), significant(32),state(PASSIVE),FD(Infinity), best_metric(Infinity), 
    min_metric_till_now(Infinity),current_successor(NULL),pointer(NULL) { }
  
  void setTopologyEntry(IPAddr_t dest, int signif,int st,Metric_t fd,Metric_t mm, Metric_t mmtn , 
			struct feasibles *succ, struct feasibles *point);

  bool operator!=(const IPAddr_t& i) { return i != destin; }
public:
  IPAddr_t destin;
  int significant;
  eigrp_state_t state;
  Metric_t FD;
  Metric_t best_metric;
  Metric_t min_metric_till_now;
  struct feasibles *current_successor;
  struct feasibles *pointer;
};
typedef std::list<TopologyEntry> topologyEntryList_;


class QueryOrigin_ReplyStatus_Entry {
public:
  typedef enum { PASSIVE,ACTIVE0,ACTIVE1,ACTIVE2,ACTIVE3 } eigrp_state_t;
  
  //default constructor  (Oij=1 for Passive state)
  QueryOrigin_ReplyStatus_Entry() : dest(0), Oij(1),state(PASSIVE),ptr_(NULL) { }
  void SetQORSEntry(IPAddr_t n, int o, int s,struct neighbors_ *q);
  bool operator!=(const IPAddr_t& i) { return i != dest; }
public:
  IPAddr_t dest;
  int Oij;
  eigrp_state_t state;
  struct neighbors_ *ptr_;  
};
typedef std::list<QueryOrigin_ReplyStatus_Entry> QORS_EntryList_;


class AdjacencyEntry {
public:
  //default constructor
  AdjacencyEntry() : H_(0),adjacency_(0), interface_(0),holdtime_(0),uptime_(0),srtt_(0), RTO_(0), 
    Q_(0), seq_no_(0), linkcost_(Infinity), upTimeStart_(0), holdEvent(0) {}
  
  //overloaded constructor
  AdjacencyEntry(
                 Ulong_t a,
                 IPAddr_t b,
                 Interface* c,
                 int d,
                 double e,
                 int f,
                 Ulong_t g,
                 Ulong_t h,
                 Ulong_t i,int j,
                 double k,EIGRPEvent* ev)
    : H_(a),adjacency_(b), interface_(c),holdtime_(d),
    uptime_(e),srtt_(f), RTO_(g), Q_(h), seq_no_(i), linkcost_(j),
    upTimeStart_(k),holdEvent(ev) { }
  
  bool operator!=(const IPAddr_t& i) { return i != adjacency_; }
public:
  Ulong_t H_;
  IPAddr_t adjacency_;
  Interface* interface_;
  int holdtime_;
  double uptime_;
  int srtt_;
  Ulong_t RTO_;
  Ulong_t Q_;
  Ulong_t seq_no_;
  int linkcost_;           // is this needed  ?
  double upTimeStart_;
  EIGRPEvent* holdEvent;
};
typedef std::list<AdjacencyEntry*> adjacencyEntryList_;


class ReplyBufferEntry {
public:
  //default constructor
  ReplyBufferEntry() : rd(0),finalmetric(0),destination(0),heardfrom(0) { }
  void SetReplyBufferEntry(Metric_t hpcnt, Metric_t finalmetr, IPAddr_t dest, IPAddr_t heard_frm );  
  bool operator!=(const IPAddr_t& i) { return i != destination; }
public:
  Metric_t rd;
  Metric_t finalmetric;
  IPAddr_t destination;
  IPAddr_t heardfrom;
};
typedef std::list<ReplyBufferEntry> ReplyBufferEntryList_;



//typedef std::map<MsgSeq_t, EIGRPHeader*>   MsgMap_t; // Used for message re-tx
#ifdef ADD_BACK_IN_AS_NEEDED
// Define a map by msg-sequence to message and event
class ReTxEvent;
typedef std::pair<RouteMsg, ReTxEvent*>    MsgEventPair_t;
typedef std::map<MsgSeq_t, RouteMsg>       MsgMap_t;
typedef MsgMap_t::value_type               MsgSeqPair_t;
typedef std::map<MsgSeq_t, MsgEventPair_t> MsgEventMap_t;
typedef MsgEventMap_t::value_type          MsgSeqEventPair_t;
#endif

class EIGRPEvent;
class ReTxInfo {
  // Information stored in each neighbor for re-transmission
public:
  ReTxInfo();
  ~ReTxInfo();
public:
  EIGRPHeader* e;
  Count_t      retxCount; // Number of re-tx's
  EIGRPEvent*  retxEvent; // Pending re-tx event
};

typedef std::map<MsgSeq_t, ReTxInfo*> ReTxMap_t;

class EIGRP;
class Neighbor {
public:
  typedef enum { UP, DOWN } Status_t;
  Neighbor() : myIP(IPADDR_NONE), nbIP(IPADDR_NONE), iface(nil),
      seq(0), status(DOWN), metric(100000), myEIGRP(nil) {}
  
  Status_t Status() { return status;}
  void Up()    { status = UP;}
  bool IsUp()  { return status == UP;}
  void Down()  { status = DOWN;}
  bool IsDown(){ return status == DOWN;}
  void Dump();        // Debug
  void AddReTx(EIGRPHeader*);    // Add an entry in the re-tx queue
  void RemoveReTx(MsgSeq_t);     // Remove an entry in re-tx queue (ack rx)
  ReTxInfo* GetReTx(MsgSeq_t);   // Get ReTx info for the specified sequence
public:
  IPAddr_t      myIP;    // Src IP Address used when sending to this neighbor
  IPAddr_t      nbIP;    // Dst IP of this neighbor
  Mask_t        nbMask;  // Dst IP Mask of this neighbor
  Interface*    iface;   // Interface used to communicate with this neighbor
  MsgSeq_t      seq;     // Next seq. to send to this peer
  Status_t      status;  // Status of this neighbor
  Metric_t      metric;  // Metric for routing to this neighbor
  ReTxMap_t     reTx;    // Retransmit queue for reliable transfers
  EIGRP*        myEIGRP; // Points to owning EIGRP
};

// Map of neighbors, indexed by IP Address
typedef std::map<IPAddr_t, Neighbor*> NeighborMap_t;
typedef NeighborMap_t::value_type     NeighborPair_t;




class EIGRPEvent : public Event {
public:
  typedef enum { START, STOP, HELLO, RETX, HELLO_EVENT, HOLD_EVENT, ACTIVE_EVENT, LINK_DOWN, METRIC_CHANGE} EIGRPEvent_t;
  EIGRPEvent() { }
  EIGRPEvent(Event_t ev) : Event(ev), adj(0) {}
  virtual ~EIGRPEvent() { }
public:
  Neighbor* n;  // Which neighbor (if retx)
  MsgSeq_t  s;  // Which sequence (if retx)

  // used for HoldEvent
  AdjacencyEntry* adj;
  
  // used for ActiveEvent
  struct neighbors_* n_;
  IPAddr_t dest;

  // used for linkDown()
  IPAddr_t ip;

  //used for setMetric()
  double bw;
  double delay;
  double reliability;
  double load;
};

class EIGRP;
// Retransmission timer
class D_Timer : public Handler { // Timer for retransmissions
 public: 
  D_Timer(EIGRP* a) : Handler() { pAgent = a; }
  void Schedule(double delay, Event* e);
  void Cancel(Event* e);
  void expire(Event* e);
  void Handle(Event* e, Time_t);
 protected:
  EIGRP* pAgent;
};



// Definition of EIGRP protocol class.
// EIGRP is conceptually a layer 4 protocol, but since its not
// designed to support layer 5 applications, we don't subclass L4Protocol
// here, since most of the L4Protocol api is designed to support
// applications.
class EIGRP : public Handler, public L4Demux {
public:
  typedef enum { ALIVE, DEAD } Status_t;
  typedef enum { RETX_LIMIT = 16 };
  EIGRP();
  virtual ~EIGRP();
  void  AttachNode(Node*);  // Attach this protocol to a node
  // Handler methods
  void Handle(Event*, Time_t);
  
  // Upcalls from L3 protocol, via L4Demux
  void DataIndication(Node*, Packet*, IPAddr_t, L3Protocol&,Interface*);
private:
  void McastUnreliable(EIGRPHeader*);
  void McastReliable(EIGRPHeader*, IPAddr_t = 0);
  void UcastUnreliable(EIGRPHeader* , IPAddr_t);
  void UcastReliable(EIGRPHeader*, IPAddr_t);
  Metric_t  evaluate_best_metric_and_FD( int updateFD, IPAddr_t dest=0);
  int FC_check(IPAddr_t dest, Metric_t rep_dist, Metric_t metr, int modifyFD=0);
public:
  Status_t  Status() { return status;}
  void      Alive()  { status = ALIVE;}
  bool      IsAlive(){ return status == ALIVE;}
  void      Dead()   { status = DEAD;}
  bool      IsDead() { return status == DEAD;}
  void      StartAt(Time_t);
  void      StopAt(Time_t);
  void      SetHelloInterval(Time_t t) { helloTimeout = t;}
  void      SetReTxInterval(Time_t t) { reTxTimeout = t;}
  Time_t    GetReTxTimeout() { return reTxTimeout; }
  void      SetLossProbability(Random_t);
  void      SetLinkDown(Time_t time, IPAddr_t ip);
  void      SetMetric(Time_t time, IPAddr_t ip, double BW, double delay = -1, double reliability = -1, double load = -1);
private:
  Neighbor* GetNeighbor(IPAddr_t ip, Interface*);
  IPAddr_t  GetIPAddress(Interface*);
  void      DisplayNeighborTable(); // debug
  void InitialExchange(IPAddr_t to, int init);
  void      buildNeighborList();
private:
  //Two versions of mCast, one if it is multicast except one IP (the one that sent the query)
  void      mCast(EIGRPHeader* e, EMCast_t t, IPAddr_t dontsend);
  void      mCast(EIGRPHeader*, EMCast_t); // Common for rel/unrel
  // Two versions of uCast, one if neighbor known (avoids lookup)
  void      uCast(EIGRPHeader*, EMCast_t, IPAddr_t, Neighbor*);
  void      uCast(EIGRPHeader*, EMCast_t, IPAddr_t);
  void      SendHello();
  AdjacencyEntry* FindAdj(IPAddr_t);
  AdjacencyEntry* AddAdjacency_ScheduleHold(IPAddr_t ip,Interface* if_,int holdTime,double now,int srtt);
  void displayroutingtable();
  void displayfibtable();
  void displaytopologytable();
  void displayQORStable();
  void displayadjacencytable();
  void displayreplybuffer();
  void remove_adjacency_from_RT_TT_QORS(IPAddr_t adjac, EIGRP_Timeout_t timeout_type);
  void flush_TT();
  // Completely wipe out the routing table, create RT afresh from the TT
  // sample the TT, take successors and place into RT
  // copies entries from the topology table to the routing table
  // uses ModifyRoutingTable()
  void TT_to_RT();
  void addtoQORSTable (IPAddr_t ip);
  int checkRSflags(IPAddr_t dest);
  void processReplies(int current_state,IPAddr_t dst, EIGRPHeader *reply_mg, int *sendRepl ,
		      IPAddr_t *sendReplyTo, EIGRPHeader *update_mg, int *sendUpd);
  void UpdateTopologyTable( IPAddr_t dest, int prefix , Metric_t reportedDistance, Metric_t metric, IPAddr_t feas,
			    int appendupdates=0, int appendqueries=0, int changestate=1, 
			    int inputevent=(int)RECEIVE_UPDATE, EIGRPHeader *update_mg=NULL, int *sendUpd=0,
			    EIGRPHeader *query_mg=NULL, int *sendQuer=0 );
  void ModifyRoutingTable(IPAddr_t dest,unsigned long prefix,Metric_t newmetric,IPAddr_t fromwhom);
  struct feasibles* findSuccessor(IPAddr_t dest, int modifyFD=0, int exclude=0,
				  IPAddr_t excludeFromComputationofSuccessor=0);
  void setRSflags(int rs_flag,IPAddr_t dest=0, IPAddr_t adjacency=0);
  void setNTRflag(int ntr_flag,IPAddr_t dest, IPAddr_t adjacency);
  void determineState(int input_event, int fc_event, IPAddr_t dest=0);
  void maintainConsistentState();
  void schedule_active_timer(IPAddr_t dest);
  // BW = bandwidth in kbps, delay in microseconds
  // reliability and load are expressed as a fraction of 255,   load = 255 -->  100% load
  void setMetric(IPAddr_t ip, double BW, double delay = -1, double reliability = -1, double load = -1);
  void modify_TT_link_metrics(IPAddr_t your_ip_, int metric_to_this_neighbor, EIGRPInputEvent_t INPUT_EVENT );
  void Down();
  void Reset();
  void linkDown(IPAddr_t ip);
  void HoldTimeout (int holdTimeout, AdjacencyEntry* a);
private:
  NeighborMap_t neighbors;
  Interface*    nextif;      // What is this?
  Time_t        reTxTimeout; // Retransmit time period (seconds)
  Time_t        helloTimeout;// Hello timeout period
  Status_t      status;
  EIGRPEvent*   pHelloEvent; // Non-nil if a pending Hello event
  MsgSeq_t      nextSeq;     // Next sequence to use for messages 
  bool          neighborsKnown; // False if neighbors map is not built
public:
  Node*         myNode;      // Attached node
  Random*       lossRNG;     // For debugging
  Random_t      lossProb;    // For debugging
private:
  static IPAddr_t m_nextip;  // Used to make up IP addresses if not known
public:
  D_Timer   eigrphelloTimer; // Timer for sending hellos
  D_Timer   holdTimer;       // Timing out for hello receipts, after Hold Time expires
  D_Timer   activeTimer;     // Timer for declaring route "Stuck in Active" state

  routeEntryList_ RoutingTable;
  topologyEntryList_ TopologyTable;
  adjacencyEntryList_ AdjacencyTable;
  QORS_EntryList_  QORSTable;
  ReplyBufferEntryList_ ReplyBuffer;

  EIGRPEvent* helloEvent;
  
  // Sent messages
  Mesg_Counters Sent_Mesg_Counters;
  
  // Received messages
  Mesg_Counters Recd_Mesg_Counters;

};

#endif
