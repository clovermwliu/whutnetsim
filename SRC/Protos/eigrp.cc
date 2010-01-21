/*

* GTNetS provides a portable C++ class library for network simulations
*
* Copyright (C) 2003 George F. Riley
*
* No guarantees or warranties or anything are provided or implied in any way
* whatsoever.  Use this program at your own risk.  Permission to use this
* program for any purpose is given, as long as the copyright is kept intact.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

* Permission is hereby granted for any non-commercial use of this code

*/

// Class for EIGRP routing application
// George F. Riley, Talal Jaafar, and Kunal Punjabi.  Georgia Tech, Spring 2004 

#include <assert.h>
#include <iostream>

#define  DEBUG_MASK 0x01
#include "debug.h"
#include "eigrp.h"
#include "link.h"

#ifdef HAVE_QT
#include <qnamespace.h>
#include <qcolor.h>
#endif
//#undef HAVE_QT

using namespace std;

#define EIGRP_SRTT 10

#define RTO 5000          // default value in milliseconds

#define EIGRP_DEFAULT_HELLO_INTERVAL 5       // each EIGRP router must send Hellos every 5 seconds to all its Neighbors

// if no hellos are received from a neighbor during the hold time, DUAL is informed the neighbor is down
#define EIGRP_DEFAULT_HOLD_TIME   EIGRP_DEFAULT_HELLO_INTERVAL*3

// after which, the route is declared stuck-in-active (SIA) and the neighbor is flushed from neigbor table
#define EIGRP_DEFAULT_ACTIVE_TIME  180

#define INITIAL_FINITE_DISTANCE_TO_NEIGHBORS  Infinity

// for the QORS table...used when it has no adjacencies, but finite number of destinations
#define INVALID_ 999

// FC check mode

// Source Node Condition
#ifdef SNC
#undef SNC
#endif
// Distance Increase Condition
#ifdef DIC
#undef DIC
#endif
// Current Successor Condition
#ifdef CSC
#undef CSC
#endif

#define SNC
//FC check mode


// whether or not to display the Topology Table
#ifdef SHOWTOPOLOGYTABLE
#undef SHOWTOPOLOGYTABLE
#endif
// whether or not to display the Routing Table
#ifdef SHOWROUTINGTABLE
#undef SHOWROUTINGTABLE
#endif
// whether or not to display the QORS Table
#ifdef SHOWQORSTABLE
#undef SHOWQORSTABLE
#endif
// whether or not to display the Adjacency Table
#ifdef SHOWADJACENCYTABLE
#undef SHOWADJACENCYTABLE
#endif
// whether or not to display the Reply Buffer
#ifdef SHOWREPLYBUFFER
#undef SHOWREPLYBUFFER
#endif


// determines whether or not you want hellos to be seen.
#ifdef SHOWHELLOPACKETS
#undef SHOWHELLOPACKETS
#endif

// determines whether or not you want the Search for adjacency to be displayed
#ifdef SHOWADJACENCYSEARCH
#undef SHOWADJACENCYSEARCH
#endif


// determines whether incoming update packets (with RD=Infinity) should be added to the TT (and sent as updates to other routers) or not.
#ifdef REJECTUPDATESWITHINFINITEMETRIC
#undef REJECTUPDATESWITHINFINITEMETRIC
#endif



// during INITIAL TT exchange....should a router send ALL known FS or just  1 successor ? sending all may cause a problem in addupdatetop() if the second FS (for same dest) has worse metric wrt the first
#ifdef SEND_TOPOLOGY_TABLE_ON_INIT
#undef SEND_TOPOLOGY_TABLE_ON_INIT
#endif


#ifdef SEND_ROUTING_TABLE_ON_INIT
#undef SEND_ROUTING_TABLE_ON_INIT
#endif

#define SEND_ROUTING_TABLE_ON_INIT



// determines whether or not to display the current table sizes (whenever DetermineTableSizes() is called)
// for Routing, Topology, QORS, Adjacency,and Neighbor tables and the Reply Buffer and the Total Combined Size.
#ifdef DISPLAY_CURRENT_TABLE_SIZES
#undef DISPLAY_CURRENT_TABLE_SIZES
#endif

// determines whether or not to display the min-max-average table sizes, whenever DisplayTableSizes() is called.
#ifdef DISPLAY_THIS_ROUTERS_TABLE_SIZES
#undef DISPLAY_THIS_ROUTERS_TABLE_SIZES
#endif


#define SHOWTOPOLOGYTABLE
//#define SHOWROUTINGTABLE
//#define SHOWQORSTABLE
//#define SHOWADJACENCYTABLE
//#define SHOWREPLYBUFFER

//#define SHOWHELLOPACKETS
//#define SHOWADJACENCYSEARCH
#define REJECTUPDATESWITHINFINITEMETRIC


// to show the message counters whenever a hello, update, query or reply message is sent/received.
//#define SHOW_MESSAGE_COUNT


// global control over the memory model display.
// if outputs of memory model are to be displayed.
#ifndef DISPLAY_MEMORY_MODELS
#undef DISPLAY_MEMORY_MODELS
#endif
//#define DISPLAY_MEMORY_MODELS

#ifdef DISPLAY_MEMORY_MODELS
#define DISPLAY_CURRENT_TABLE_SIZES
#define DISPLAY_THIS_ROUTERS_TABLE_SIZES
#endif

// to show the convergence time at the end of each topology change....i.e every time the network converges.
#ifdef DISPLAY_CONVERGENCE_MODELS
#undef DISPLAY_CONVERGENCE_MODELS
#endif
#define DISPLAY_CONVERGENCE_MODELS


// Convergence Model
static double start_convergence=0;
static double end_convergence=0;
static unsigned long number_of_topology_changes=0;
static double average_convergence_time=0;

static double start_convergence_measurement=0;
static double end_convergence_measurement=Infinity;

typedef enum { NOT_EQUAL=0, EQUAL=1 }  Network_State_t; 
Network_State_t NetworkMessages = EQUAL;
Network_State_t CheckConvergence();

static Mesg_Counters Global_Sent_Counter;
static Mesg_Counters Global_Recd_Counter;


// green HelloColor
// red UpdateColor
// yellow AckColor

// Sent messages
inline void Increment_Sent_Hellos(long by = 1)
{
  Global_Sent_Counter.Hellos = Global_Sent_Counter.Hellos + by;
#ifdef SHOW_MESSAGE_COUNT
  cout << "Global Count:: Number of Hello Messages Sent = " << Global_Sent_Counter.Hellos << std::endl;
  cout << "Global Count:: Number of Hello Messages Received = " << Global_Recd_Counter.Hellos << std::endl;
#endif
}


inline void Increment_Sent_Updates(long by = 1)
{
  Global_Sent_Counter.Updates = Global_Sent_Counter.Updates + by;

  if( NetworkMessages == EQUAL)
    {
      // Network is in converged state
      start_convergence = Simulator::instance->Now();
      NetworkMessages = NOT_EQUAL;
      number_of_topology_changes++;
#ifdef DISPLAY_CONVERGENCE_MODELS
      cout << "ntc increased=" << number_of_topology_changes << std::endl;
      cout << "CON::start at " << start_convergence << std::endl;
#endif
    }

#ifdef SHOW_MESSAGE_COUNT
  cout << "Global Count:: Number of Update Messages Sent = " << Global_Sent_Counter.Updates << std::endl;
  cout << "Global Count:: Number of Update Messages Received = " << Global_Recd_Counter.Updates << std::endl;
#endif
}


inline void Increment_Sent_Queries(long by = 1)
{
  Global_Sent_Counter.Queries = Global_Sent_Counter.Queries + by;

  if( NetworkMessages == EQUAL)
    {
      // Network is in converged state
      start_convergence = Simulator::instance->Now();
      NetworkMessages = NOT_EQUAL;
      number_of_topology_changes++;
#ifdef DISPLAY_CONVERGENCE_MODELS
      cout << "ntc increased=" << number_of_topology_changes << std::endl;
      cout << "CON::start at " << start_convergence << std::endl;
#endif
    }

#ifdef SHOW_MESSAGE_COUNT
  cout << "Global Count:: Number of Query Messages Sent = " << Global_Sent_Counter.Queries << std::endl;
  cout << "Global Count:: Number of Query Messages Received = " << Global_Recd_Counter.Queries << std::endl;
#endif
}


inline void Increment_Sent_Replies( long by =1)
{
  Global_Sent_Counter.Replies = Global_Sent_Counter.Replies + by;

  if( NetworkMessages == EQUAL)
    {
      // Network is in converged state
      start_convergence = Simulator::instance->Now();
      NetworkMessages = NOT_EQUAL;
      number_of_topology_changes++;
#ifdef DISPLAY_CONVERGENCE_MODELS
      cout << "ntc increased=" << number_of_topology_changes << std::endl;
      cout << "CON::start at " << start_convergence << std::endl;
#endif
    }

#ifdef SHOW_MESSAGE_COUNT
  cout << "Global Count:: Number of Reply Messages Sent = " << Global_Sent_Counter.Replies << std::endl;
  cout << "Global Count:: Number of Reply Messages Received = " << Global_Recd_Counter.Replies << std::endl;
#endif
}




// Received Messages
inline void Increment_Recd_Hellos(long by = 1)
{
  Global_Recd_Counter.Hellos = Global_Recd_Counter.Hellos + by;
#ifdef SHOW_MESSAGE_COUNT
  cout << "Global Count:: Number of Hello Messages Received = " << Global_Recd_Counter.Hellos << std::endl;
  cout << "Global Count:: Number of Hello Messages Sent = " << Global_Sent_Counter.Hellos << std::endl;
#endif
}


inline void Increment_Recd_Updates(long by = 1)
{
  Global_Recd_Counter.Updates = Global_Recd_Counter.Updates + by;
#ifdef SHOW_MESSAGE_COUNT
  cout << "Global Count:: Number of Update Messages Received = " << Global_Recd_Counter.Updates << std::endl;
  cout << "Global Count:: Number of Update Messages Sent = " << Global_Sent_Counter.Updates << std::endl;
#endif

  CheckConvergence();
}


inline void Increment_Recd_Queries(long by = 1)
{
  Global_Recd_Counter.Queries = Global_Recd_Counter.Queries + by;
#ifdef SHOW_MESSAGE_COUNT
  cout << "Global Count:: Number of Query Messages Received = " << Global_Recd_Counter.Queries << std::endl;
  cout << "Global Count:: Number of Query Messages Sent = " << Global_Sent_Counter.Queries << std::endl;
#endif

  CheckConvergence();
}


inline void Increment_Recd_Replies(long by = 1)
{
  Global_Recd_Counter.Replies = Global_Recd_Counter.Replies + by;
#ifdef SHOW_MESSAGE_COUNT
  cout << "Global Count:: Number of Reply Messages Received = " << Global_Recd_Counter.Replies << std::endl;
  cout << "Global Count:: Number of Reply Messages Sent = " << Global_Sent_Counter.Replies << std::endl;
#endif

  CheckConvergence();
}


// MODIFIED*
void TopologyEntry::setTopologyEntry(IPAddr_t dest, int signif,int st,Metric_t fd,Metric_t mm, Metric_t mmtn , struct feasibles *succ, struct feasibles *point)
{ 
  destin= dest;
  significant= signif;
  state=(eigrp_state_t) st;
  FD= fd;
  best_metric= mm;
  min_metric_till_now= mmtn;
  current_successor=succ;
  pointer= point;
}



// MODIFIED*
void QueryOrigin_ReplyStatus_Entry::SetQORSEntry(IPAddr_t n, int o, int s,struct neighbors_ *q)
{
  dest=n;
  Oij=o;
  state=(eigrp_state_t)s;
  ptr_=q;
}


// MODIFIED*
void ReplyBufferEntry::SetReplyBufferEntry(Metric_t hpcnt, Metric_t finalmetr, IPAddr_t dest, IPAddr_t heard_frm )
{
  rd=hpcnt;
  finalmetric=finalmetr;
  destination=dest;
  heardfrom=heard_frm;
}


// EIGRP Header Methods
EIGRPHeader::EIGRPHeader()
  : msgSeq(0), mTxt(EMsgUnknown), mSrc(IPADDR_NONE), mDst(IPADDR_NONE),
    mType(MsgUnknown)
{
}

EIGRPHeader::EIGRPHeader(EMsg_t t,Uint_t Version, Uint_t Opcode, Uint_t Checksum, Uint_t Flags, Uint_t Seq, Uint_t Ack, Uint_t ASNumber)
  : msgSeq(Seq), mTxt(EMsgUnknown), mSrc(IPADDR_NONE), mDst(IPADDR_NONE),
    mType(t), version(Version), opcode(Opcode), checksum(Checksum),
    flags(Flags), ack(Ack), asNumber(ASNumber)
{
}

EIGRPHeader::~EIGRPHeader()
{
}

PDU* EIGRPHeader::Copy() const
{
  return new EIGRPHeader(*this);
}

Size_t  EIGRPHeader::SSize()
{ // Size needed for serialization
  return 16; // 4 items, 4 bytes each
}

char*   EIGRPHeader::Serialize(char*, Size_t&)    // Serialize to a buffer
{
  return nil; // code later
}

char*   EIGRPHeader::Construct(char*, Size_t&)    // Construct from buffer
{
  return nil; // code later
}


void    EIGRPHeader::Trace(Tfstream&, Bitmap_t)   // Trace contents of this pdu
{
  return; // code later
}

// Retx info methods

ReTxInfo::ReTxInfo()
{
}

ReTxInfo::~ReTxInfo()
{
  delete e; // Don't need the copy of the header anymore
  if (retxEvent)
    { // Need to cancel the timer
      Scheduler::Cancel(retxEvent);
      delete retxEvent;
      retxEvent = nil;
    }
}





// Neighbor Methods
void Neighbor::Dump()
{
}

void Neighbor::AddReTx(EIGRPHeader* e)
{
  DEBUG0((cout << "EIGRP " << myEIGRP->myNode->Id()
          << " neighbor " << (string)IPAddr(nbIP)
          << " AddReTx, seq " << e->msgSeq << endl));
  
  ReTxMap_t::iterator i = reTx.find(e->msgSeq);
  if (i != reTx.end())
    {
      cout << "Huh?  Removing duplicate retx msg seq in EIGRP" << endl;
      delete i->second;
      reTx.erase(i);
    }
  EIGRPEvent* ev = new EIGRPEvent(EIGRPEvent::RETX);
  ev->n = this;        // Neighbor
  ev->s = e->msgSeq;   // Sequence
  
  ReTxInfo*   retx = new ReTxInfo();
  retx->e = (EIGRPHeader*)e->Copy(); // Save a copy for later re-use
  retx->retxCount = 0; // Count number of retries
  retx->retxEvent = ev;// Save the event pointer for deletion
  // Insert the retx info  inthe queue
  DEBUG0((cout << "EIGRP " << myEIGRP->myNode->Id()
          << " neighbor " << (string)IPAddr(nbIP)
          << " adding retx seq " << e->msgSeq << endl));
  reTx.insert(ReTxMap_t::value_type(e->msgSeq, retx));
  // Schedule the retx event
  Scheduler::Schedule(ev, myEIGRP->GetReTxTimeout(), myEIGRP);
}

void Neighbor::RemoveReTx(MsgSeq_t s)
{ // An ack has been received, remove the retx entry for this
  DEBUG0((cout << "EIGRP " << myEIGRP->myNode->Id() 
          << " removing retx seq " << s << endl));
  ReTxMap_t::iterator i = reTx.find(s);
  if (i == reTx.end())
    { // Not found, just ignore (this in fact can happen during startup)
      return;
    }
  delete i->second; // This deletes the l4 header, and cancels the retx event
  reTx.erase(i);
}

ReTxInfo* Neighbor::GetReTx(MsgSeq_t s)
{ // Retrieve the retx info for the specified seq number
  DEBUG0((cout << "Entering GetReTx" << endl));
  ReTxMap_t::iterator i = reTx.find(s);
  if (i == reTx.end())
    {
      cout << "Huh?  Can't find retx msg seq in EIGRP::retx" << endl;
      return nil;
    }
  return i->second;
#ifdef MOVE_THIS
  // At this point, the retx timer has been deleted, so we need a new one
  EIGRPEvent* ev = new EIGRPEvent(EIGRPEvent::RETX);
  ev->n = this;        // Neighbor
  ev->s = s;           // Sequence
  
  ReTxInfo* retx = i->second;
  retx->retxEvent = ev;
#endif  
}

// EIGRP Methods  
EIGRP::EIGRP()
  : nextif(nil), reTxTimeout(1.0), helloTimeout(5.0), 
    status(DEAD), pHelloEvent(nil), nextSeq(0), neighborsKnown(false),
    eigrphelloTimer(this), holdTimer(this), activeTimer(this)
{
}

EIGRP::~EIGRP()
{
}

void EIGRP::AttachNode(Node* n)
{
  if (!n->IsReal()) return; // Ghost node in distributed simulation. do nothing
  myNode = n;
  n->InsertProto(4, EIGRP_PROTO, this); // Insert into protocol graph
}


void EIGRP::buildNeighborList()
{
  // Need to find neighbor map
  DEBUG0((cout << "++++++++++++++++++++++++++building neighbor map" << endl));
  
  // Talal : todo: Need to check if every neighbor has an EIGRP agent before adding it to the map
  // Get the interfaces for this node
  IFVec_t IfVec = myNode->Interfaces();
  for (IFVec_t::iterator j = IfVec.begin(); j != IfVec.end(); ++j)
    {
      IPAddrVec_t IpAddrVec;
      NodeWeightVec_t NodeWeightVec;
      myNode->NeighborsByIf(*j, IpAddrVec);
      
      Link* myLink = (*j)->GetLink();
      myLink->AllNeighbors(NodeWeightVec); 
      
      for(NodeWeightVec_t::iterator k = NodeWeightVec.begin(); k != NodeWeightVec.end(); ++k)
	{
	  //L4Demux* l4demux = (L4Demux*)(*k).node->LookupProto(4, EIGRP_PROTO);
	  //if (l4demux) // this node runs EIGRP
	  //{
	      cout << "node " << (*k).node->Id() << " runs EIGRP" << std::endl;
	      // Add neighbors
	      IPAddr_t ip = (*j)->NodePeerIP((*k).node);
	      if((string)IPAddr(ip) == "0.0.0.0")
		continue;
	      // Sanity Check (make sure that the neighbor node is not this node)
	      if( (*k).node != myNode) {
		Neighbor* n = GetNeighbor(ip, *j);
		n = NULL;
	      }
	      //}
	}
    }
}

// Handler Methods
void EIGRP::Handle(Event* ev, Time_t)
{
  DEBUG0((cout << "Entering EIGRP::Handle" << endl));
  
  IPAddr_t myIP, yourIP;
  Mask_t yourMask;

  EIGRPEvent* e = (EIGRPEvent*)ev;
  ReTxInfo* retx;
  
  switch(e->event) {

  case EIGRPEvent::START:
    if(0)cout << "EIGRP " << myNode->Id() << " handling START event " << endl;
    Alive();    // Set to alive
      
    //#ifdef FORGET
    TopologyTable.clear();
    RoutingTable.clear();
    QORSTable.clear();
    AdjacencyTable.clear();
    ReplyBuffer.clear();
    //#endif
      
      
    if(!neighborsKnown)
      buildNeighborList();
      
      
    for (NeighborMap_t::iterator i = neighbors.begin();  i != neighbors.end();  ++i)
      { 
	myIP=i->second->myIP;
	yourIP=i->first;
	yourMask = i->second->nbMask;
	
	if(0)cout << "Neighbor IP is: " << yourIP << " and Neighbor Mask is: " << yourMask << " \n\n\n\n\n";

	// in place of addUpdateTopologyEntry
	UpdateTopologyTable ( myIP, yourMask /*Same mask as neighbor*/, 0, 0, 0, 0/*appendupdates*/, 0/*appendqueries*/);

	//todo...maybe the RD should be initialized to 0, not INFINITY
	UpdateTopologyTable ( yourIP, yourMask /*Same mask as neighbor*/, 0 /*RD*/, INITIAL_FINITE_DISTANCE_TO_NEIGHBORS, yourIP, 0/*appendupdates*/, 0/*appendqueries*/);
      }
      
    SendHello();// Send the hellos
    break;

  case EIGRPEvent::STOP:
    if(0)cout << "EIGRP " << myNode->Id() << " handling STOP event " << endl;
    Dead();     // Set to dead, which stops responding to messages
    break;
      
  case EIGRPEvent::HELLO:
    if(0)cout << "EIGRP " << myNode->Id() << " handling HELLO event " << endl;
    pHelloEvent = nil;   // We are going to delete this one
    SendHello();         // Send the hello message
    break;

  case EIGRPEvent::RETX:
    if(0)cout << "EIGRP " << myNode->Id() << " handling RETX event " << endl;
    retx = e->n->GetReTx(e->s); // Get the re-tx info
    if (!retx)
      {
	cout << "HuH?  Can't find retx info on RETX event" << endl;
      }
    else
      { 
	if (++retx->retxCount < 3)
	  {
	    // Schedule the retx event
	    Scheduler::Schedule(ev, reTxTimeout, this);
	    ev = nil; // Since we re-used the event, we don't delete below
	    // And send the new packet
	    if(0)cout << "preparing retransmission packet" << std::endl;
	    Packet* p = new Packet();
	    p->PushPDU(retx->e->Copy());
	    IPV4ReqInfo r(retx->e->mDst, IPADDR_NONE, 1, 88);
	    IPV4::Instance()->DataRequest(myNode, p, &r);
	    DEBUG0((cout << "EIGRP " << myNode->Id()
		    << " retransmitting seq "
		    << e->s << endl));
	  }
	else
	  { // retransmitted too many times, just give up
	    retx->retxEvent = nil; // Deleting this event below, not here
	    e->n->RemoveReTx(e->s);
	    e->n->Down();  // And set the neighbor down
	  }
      }
    break;
    
  case EIGRPEvent::LINK_DOWN:
    linkDown(e->ip);
    break;
  case EIGRPEvent::METRIC_CHANGE:
    setMetric(e->ip, e->bw, e->delay, e->reliability, e->load);
    break;
  }
  delete ev;
}


// D_Timer Implementation

void D_Timer::Schedule(double delay, Event* e)
{
  DEBUG0((cout << "Entering Schedule" << endl));
  Scheduler::Schedule(e, delay, this);
}

void D_Timer::Cancel(Event* e)
{
  Scheduler::Cancel(e);
}

void D_Timer::expire(Event *e)
{
}


// MODIFIED*
void D_Timer::Handle(Event *e, Time_t)
{
  DEBUG0((cout << "Entering D_Timer::Handle" << endl));
  EIGRPEvent* ev = (EIGRPEvent*)e;

  switch (ev->event) 
    {
    case EIGRPEvent::HELLO_EVENT :
      cout << "\n\n\n\n\n\n\n\nHELLO  TIMEOUT \n\n\n\n\n\n\n\n";
      //pAgent->HelloTimeout(ev);
      break;

    case EIGRPEvent::HOLD_EVENT :
      cout << "\n\n\n\n\n\n\n\nHOLD  TIMEOUT \n\n\n\n\n\n\n\n";
      //pAgent->HoldTimeout( 1, ev->adj);
      // delete the event
      break;

    case EIGRPEvent::ACTIVE_EVENT:
      cout << "\n\n\n\n\n\n\n\nACTIVE TIMEOUT\n";
      //pAgent->ActiveTimeout(ev);
      break;

    }
}
  
  
void EIGRP::DataIndication(Node* n, Packet* p, IPAddr_t ip,
                           L3Protocol& l3, Interface* if_)
{ // Got a packet from L3
  DEBUG0((cout << "EIGRP " << myNode->Id() << " DataInd *************************************************" << endl));
  if(0)cout << "ip from DataIndication is: " << IPAddr::ToDotted(ip) << std::endl;
  /*
    if (lossRNG)
    {
    Random_t l = lossRNG->Value();
    if (l < lossProb)
    { // Randomly loose packet for debugging reliable Unicast
    cout << "EIGRP " << myNode->Id() 
    << " randomly loosing packet" << endl;
    delete p;
    return;
    }
    }
  */
  
  double now =  Simulator::instance->Now();
  unsigned long holdTime= EIGRP_DEFAULT_HOLD_TIME;
  int flag_update=1;

  IPAddr_t myIP, yourIP;
  Metric_t metric;
  
  IPAddr_t sendReplyTo;
  int sendRepl=0;
  int sendUpd=0;

  
  // prepare for sending out an update
  EIGRPHeader* update_Mg =  new EIGRPHeader(MsgUpdate,2,(unsigned int)MsgUpdate,0x0000EFCF,0x00000000,0,0,1000);
   

  // prepare to send out a query
  EIGRPHeader* query_Mg =  new EIGRPHeader(MsgQuery,2,(unsigned int)MsgQuery,0x0000EFCF,0x00000000,0,0,1000);


  // prepare to send out a reply
  EIGRPHeader* reply_Mg =  new EIGRPHeader(MsgReply,2,(unsigned int)MsgReply,0x0000EFCF,0x00000000,0,0,1000);


  /********* used for processReplies() *******/

  // prepare to send out a reply.....this stores the BEST REPLY
  EIGRPHeader* reply_mg =  new EIGRPHeader(MsgReply,2,(unsigned int)MsgReply,0x0000EFCF,0x00000000,0,0,1000);
  
  
  // prepare for sending out an update
  EIGRPHeader* update_mg =  new EIGRPHeader(MsgUpdate,2,(unsigned int)MsgUpdate,0x0000EFCF,0x00000000,0,0,1000);
 







  /********* used for UpdateTopologyTable() ******/

  int sendQuer=0;
  // Dont send updates at this point only to this router...it will get the entire TT soon (INITIAL EXCHANGE).
  IPAddr_t AvoidRouterForUpdates=0;
  int avoid=0;

  /******** used for UpdateTopologyTable() *******/

  int Send_Updates = 0;
  int Send_Queries = 0;
  int Send_Replies = 0;
  bool avoidAcking = false;


  EIGRPHeader* e = (EIGRPHeader*)p->PeekPDU();

  if (e->Layer() != 4 || e->Proto() != EIGRP_PROTO)
    {
      cout << "HuH?  EIGRP receive, not EIGRP header" << endl;
      return;
    }

  
  if (Status() == ALIVE)
    { // Ignore all incoming msgs if not alive
      Neighbor*  nb = GetNeighbor(ip, if_);// Find or create neighbor
      nb->status = Neighbor::UP;   // Set status up if we get a message
      
      AdjacencyEntry* adj = FindAdj(ip);
      int adj_flag=0;
    
      
      switch(e->mTxt) {
	
      case EMsgAck :
	// Got an ack, remove the retx info
	if(0)cout << "Received an Ack packet \n";
	nb->RemoveReTx(e->msgSeq);
	avoidAcking = true;
	break;

      default:
	break;
      }
      

      while(!e->updateHeader.empty() || !e->queryHeader.empty() || !e->replyHeader.empty() || !e->helloHeader.empty())
	{
	  if(0)
	    {
	      cout << "Still processing some headers\n";
	      
	      if(!e->helloHeader.empty())
		cout << "e->helloHeader.size()\n"; 
	      if(!e->updateHeader.empty())
		cout << "e->updateHeader.size()\n"; 
	      if(!e->queryHeader.empty())
		cout << "e->queryHeader.size()\n"; 
	      if(!e->replyHeader.empty())
		cout << "e->replyHeader.size()\n";
	    }
	  
	  switch(e->mType) {
	
	  case MsgHello :
	    // HELLO MESSAGE
	    if(e->opcode == 5) { //confirm it is a Hello Message
       
	      //HelloHeader* recvdHello = e->helloHeader.back();
	      //e->helloHeader.pop_back();

	      HelloHeader* recvdHello = e->helloHeader.front();
	      vector<HelloHeader*>::iterator iter = e->helloHeader.begin();
	      e->helloHeader.erase(iter);

	      if(recvdHello->typefield!=0x0001) { 
		cout << "Typefield should be 0x0001 for Hello Packets. Exiting.\n"; 
		exit(1);
	      } 

	      
	      // if it is a new adjacency,add it to the adjacency table
	      if(adj==NULL && adj_flag!=1)
		{ 
		  if(0)cout << "Received Hello packet from a NEW adjacency\n";
		  adj=AddAdjacency_ScheduleHold(ip,if_,recvdHello->myholdTime,now,EIGRP_SRTT);
		  adj_flag=1;
		  addtoQORSTable(ip);
	      
		  // Start the Initial Topology Table Exchange      
		  // send my topology table to the NEW node I got the Hello from.
		  // INIT flag is set=1
	      
		  // send only the Routing Table, not the entire TT
#if defined(SEND_TOPOLOGY_TABLE_ON_INIT)
		  // SendTopologyTableUpdate (ip,1);
#elif defined(SEND_ROUTING_TABLE_ON_INIT)
		  InitialExchange (ip,1);
#else
		  cout << "One Has to be used\n";
		  //exit(1);
#endif
		}
	      // (adj==NULL)
	  
	      else  // got hello from a known neighbor
		{
		  adj_flag=1;
	      
		  // reset the hold timer
		  // cancel the previousEvent  adj->holdEvent
		  holdTimer.Cancel(adj->holdEvent);
	      
#ifdef SHOWHELLOPACKETS
		  cout << "\nRescheduling a hold event at " << now << " holdTime=" << holdTime << std::endl;  // dont use new EIGRPEvent-it will cause a memory leak
#endif  
		  holdTimer.Schedule( holdTime, adj->holdEvent);   // use holdTime, not EIGRP_DEFAULT_HOLD_TIME
		}
	  
	      for (NeighborMap_t::iterator i = neighbors.begin();  i != neighbors.end();  ++i)
		{       
		  yourIP=i->first;
		  if(yourIP==ip) 
		    {
		      i->second->Up();
		      if(0)cout << "yourIP is " << IPAddr::ToDotted(i->first) << " and status is " << i->second->Status() << std::endl;
	    }
		}
	    }
	    // HELLO MESSAGE
	    break;



	  case MsgUpdate:
	    // UPDATE MESSAGE
	    if(e->opcode == 1)  //confirm it is an Update Message
	      {
		if(0)cout << "Received an Update Message from " << IPAddr::ToDotted(ip) << std::endl;

		// get the first update, and remove it from the vector
		//UpdateHeader* recvdUpdate = e->updateHeader.back();
		//e->updateHeader.pop_back();
		
		UpdateHeader* recvdUpdate = e->updateHeader.front();
		vector<UpdateHeader*>::iterator iter = e->updateHeader.begin();
		e->updateHeader.erase(iter);
		
	    
		if(recvdUpdate->typefield!=0x0102) { 
		  cout << "Typefield should be 0x0102 for Update Packets. Exiting.\n";
		  exit(1);
		} 
	    
		unsigned long tempo;
	    
		EIGRPInputEvent_t INPUT_EVENT = RECEIVE_UPDATE;
		int appendupdates=1;
		int appendqueries=1;
	    
	    
		// INIT flag
		if(e->flags ==1) 
		  {
		    cout << "\n\nINIT flag set  (Initial Topology Table Exchange)\n";
		    cout << "Send updates to all BUT the guy you got the message from (as the TT will be sent soon after to him.)\n";
		    appendupdates=1;
		    appendqueries=0;
		    AvoidRouterForUpdates=ip;
		    avoid=1;
		  }
	    
	    
		if (adj == NULL && adj_flag!=1)
		  {
		    // ONLY WHEN ACCEPTING UPDATE MESSAGES FROM UNHEARD-OF NEIGHBORS
		    cout << "\nGot an 'Update' packet from an unrecognised Neighbor (havent yet got a hello from it). \n";
		    cout << "Nonetheless, Adding the entry to Adjacency Table\n";
		
		    // add to adjacency table, create and reset a new hold timer
		    adj=AddAdjacency_ScheduleHold(ip,if_,holdTime,now,EIGRP_SRTT);
		    adj_flag=1;
		    addtoQORSTable(ip);
		  }
	    
		else  // adj!=NULL  known neighbor
		  {
		    adj_flag=1;
		
		    //   cancel the previousEvent    adj->holdEvent
		    holdTimer.Cancel(adj->holdEvent);
		    cout << "\nReScheduling a hold event at " << now << " holdTime=" << holdTime << std::endl;  // dont use new EIGRPEvent-it will cause a memory leak
		    holdTimer.Schedule( holdTime, adj->holdEvent);       // use holdTime, not EIGRP_DEFAULT_HOLD_TIME
		  }
	    
	    
		tempo=htonl(recvdUpdate->destination);
       		tempo=htonl(recvdUpdate->nexthop);
		flag_update=1;
	    
	    
		// obtain the linkcost to the router you got the update from
		for (NeighborMap_t::iterator i = neighbors.begin();  i != neighbors.end();  ++i)
		  {
		    myIP=i->second->myIP;
		    yourIP=i->first;
		    
		    if(yourIP==ip)
		      {
			metric= i->second->metric;
			if(0)cout << "\nmetric(linkcost) to this neighbor router " << metric << std::endl;
			i->second->Up();
			if(0)cout << "Status " << i->second->status << std::endl;
		      }
		  }
	    
		metric=metric + recvdUpdate->hopcount;
		if(recvdUpdate->hopcount >= Infinity) recvdUpdate->hopcount=Infinity;
		if(metric >= Infinity) metric=Infinity;
	    
	    
		// initialize state_ to PASSIVE
		int state_=0;
	    
		for (topologyEntryList_::iterator tr_ = TopologyTable.begin(); tr_ != TopologyTable.end(); tr_ ++)
		  {
		    if(tr_->destin == recvdUpdate->destination)
		      {
			// current state of the destination
			state_= (int)tr_->state;
		      }
		  }
	    
	    
		if(state_!=0)
		  {
		    // destination is ACTIVE
		    // update the entry in the Reply Buffer......ie if the neighbor has already replied to my query, update the reply
		
     		
		    for (ReplyBufferEntryList_::iterator trav = ReplyBuffer.begin(); trav != ReplyBuffer.end(); ++trav)
		      {
			if( (trav->destination == recvdUpdate->destination) && (trav->heardfrom == ip) )
			  {
			    // the neighbor has already replied to my query, update the reply
			    trav->rd= recvdUpdate->hopcount;
			    trav->finalmetric=metric;
			  }                  
		      }
		  }
		// if(state_!=0)
	    
		else
		  {
		    // destination is PASSIVE
		    INPUT_EVENT=RECEIVE_UPDATE;
		
		    for (topologyEntryList_::iterator tr_ = TopologyTable.begin(); tr_ != TopologyTable.end(); tr_ ++)
		      {
			if(tr_->destin == recvdUpdate->destination)
			  {
			    struct feasibles *itr;
			
			    itr=tr_->pointer;
			    while (itr->fee!=NULL)
			      {
				itr=itr->fee;
				if(itr->feas_succ==ip)
				  {
				    if(itr->metric < metric) INPUT_EVENT = RECEIVE_UPDATE_DISTANCE_INCREASE;
				  }
			      }
			    // while()
			  }
			// if()
		      }
		    // for()
		
		
		    UpdateTopologyTable( (IPAddr_t) recvdUpdate->destination, recvdUpdate->prefixlength ,recvdUpdate->hopcount, 
					 metric,(IPAddr_t) ip , appendupdates /*default=1*/, appendqueries /*default=1*/,1 /*changestate*/,
					 (int)INPUT_EVENT, update_mg, &sendUpd, query_Mg, &sendQuer );
		
		  }
		// else(destination is PASSIVE)
    
	      }
	    // UPDATE MESSAGE
	    break;
	
	
	  case MsgQuery:
	    // QUERY MESSAGE
	    if(e->opcode == 3)
	      { 
		cout << "Received a Query from " << IPAddr::ToDotted(ip) << std::endl;
	    	  
		// get the first query, and remove it from the vector
		//QueryHeader* recvdQuery = e->queryHeader.back();
		//e->queryHeader.pop_back();
		
		QueryHeader* recvdQuery = e->queryHeader.front();
		vector<QueryHeader*>::iterator iter = e->queryHeader.begin();
		e->queryHeader.erase(iter);
	    
		if(recvdQuery->typefield!=0x0103) { 
		  cout << "Typefield should be 0x0103 for Query Packets. Exiting.\n"; 
		  exit(1);
		}
	    
			    
		if (adj == NULL && adj_flag!=1) 
		  {
		    //  ONLY WHEN ACCEPTING QUERY MESSAGES FROM UNHEARD-OF NEIGHBORS
		    cout << "\nGot a 'Query' packet from an unrecognised Neighbor (havent yet got a hello from it). \n";
		    cout << "Nonetheless, Adding the entry to Adjacency Table\n";
		    //exit(1);
		
		    // add to adjacency table, create and reset a new hold timer
		    adj=AddAdjacency_ScheduleHold (ip,if_,holdTime,now,EIGRP_SRTT);
		    adj_flag=1;
		    addtoQORSTable(ip);
		  }
		else
		  {
		    //adj!=NULL  known neighbor
		    adj_flag=1;
		
		    //  cancel the previousEvent  adj->holdEvent
		    holdTimer.Cancel(adj->holdEvent);
		    cout << "\nReScheduling a hold event at " << now << " holdTime=" << holdTime << std::endl;   // dont use new EIGRPEvent-it will cause a memory leak
		    // use holdTime, not EIGRP_DEFAULT_HOLD_TIME
		    holdTimer.Schedule( holdTime, adj->holdEvent);
		  }
	    
	    
		// find the cost to the neighbor
		double metr=0;
		for (NeighborMap_t::iterator i_ = neighbors.begin();  i_ != neighbors.end();  ++i_)
		  {
		    yourIP=i_->first;
		   
		    if(yourIP==ip)
		      {
			i_->second->Up();
			metr= i_->second->metric;
		      }
		  }
	    
		if(recvdQuery->hopcount >=Infinity) recvdQuery->hopcount=Infinity;
		double met_= recvdQuery->hopcount + metr;
		if(met_ >=Infinity) met_=Infinity;
	    
	    
		// if able to find a feasible succ, send a reply, and dont change to active state
		// if not able to find a feasible succ,send queries to all neighbors (including neighbor you got query from), (if PASSIVE,goto ACTIVE3)
		if(0)cout << "Query has::dest=" << IPAddr::ToDotted(recvdQuery->destination) << ", hopcount=" << recvdQuery->hopcount << std::endl;
	    
		//displaytopologytable();
	    
		EIGRPInputEvent_t INPUT_EVENT = RECEIVE_QUERY;
		EIGRP_FC_Event_t FC_EVENT = FC_NOT_SATISFIED;
	    
		struct feasibles *itr, *current_succ, *new_succ;
		struct feasibles *temp_itr;
	    
		current_succ= findSuccessor(recvdQuery->destination,0);
	    
		for (topologyEntryList_::iterator traverse = TopologyTable.begin(); traverse != TopologyTable.end(); ++traverse)
		  {
		    if(traverse->destin==recvdQuery->destination)
		      {
			if(0)cout << "found the destination in TT   FD=" << traverse->FD << std::endl;
		    
			if((int)traverse->state>0)
			  {
			    // destination is ACTIVE
			    // current_succ=traverse->current_successor;
			
			    // update the entry in the Reply Buffer......ie if the neighbor has already replied to my query, update the reply
			    for (ReplyBufferEntryList_::iterator trav = ReplyBuffer.begin(); trav != ReplyBuffer.end(); ++trav)
			      {
				if( (trav->destination==recvdQuery->destination) && (trav->heardfrom == ip) )
				  {
				    // the neighbor has already replied to my query, update the reply
				    trav->rd=recvdQuery->hopcount;
				    trav->finalmetric=met_;
				  }                  
			      }
			  }
			// if(traverse->state>0)
		    
		    
			// sanity check 1
			if(current_succ==NULL) { cout << "oops\n"; exit(1); }
		    
		    
			// sanity check 2
			// insure its the same successor
		    
			if(traverse->current_successor->feas_succ != current_succ->feas_succ  && (int)traverse->state==0) 
			  { 
			    cout <<"Incorrect Successor in TT\n";  
			    exit(1); 
			  }
		    
			// used to find the best new candidate to the dest
			Metric_t new_best_RD=recvdQuery->hopcount;
			double new_best_metric=met_;
			int modified=0;
		    
			// update the TT with the new distance to the destination via that neighbor(Successor or not)
			// call flush_TT() if necessary
			itr = traverse->pointer;
			assert(itr!=NULL);
			while(itr->fee!=NULL)
			  {
			    itr=itr->fee;
			    if(itr->feas_succ == ip)
			      {
				if(0)cout << "Change the entry in the TT\n";
				itr->RD=recvdQuery->hopcount;
				itr->metric=met_;
				if(itr->RD < itr->min_RD) itr->min_RD=itr->RD;
			    
				modified=1;
				//todo
				// send updates ?? Dont think so....only if using CISCO's method.
			    
				// keep track of this feas_succ.
				temp_itr=itr;
			      }
			
			    // find the best new candidate to the dest (lowest metric)
			    if(itr->metric < new_best_metric)
			      {
				new_best_metric=itr->metric;
				new_best_RD=itr->RD;
			      }
			  }
		    
			if(modified==0)
			  {
			    // example of this scenario...node j in Garcia's paper...page 136.
			    // need to add the entry to the TT
			
			    UpdateTopologyTable( (IPAddr_t) traverse->destin, traverse->significant ,recvdQuery->hopcount, met_, (IPAddr_t) ip , 0 /*appendupdates*/, 0 /*appendqueries*/);
			  }
		    
		    
			setNTRflag( 1, recvdQuery->destination, ip);
		    
			// todo
			// dont update the FD or successor yet ??  Or does it matter ?
			evaluate_best_metric_and_FD (0, recvdQuery->destination);

			if(0)cout << "Check if he is the current Successor to the destination\n";
			// if yes, update the TT entry


			if(traverse->current_successor->feas_succ==ip)
			  {
			    // query from successor
			    INPUT_EVENT = QUERY_FROM_SUCCESSOR;
			    cout << "Query from Successor !! ...\n";

			    // find FC_EVENT
			    if( FC_check(recvdQuery->destination, new_best_RD, new_best_metric)==1)
			      {
				// query from successor, the FC was satisfied. (eg. node d in Garcia's example)
				FC_EVENT=FC_SATISFIED;
				// FC satisfied. Send back a reply as unicast.

				// find a new successor ONLY if in PASSIVE state
				if((int) traverse->state==0) new_succ= findSuccessor(recvdQuery->destination);
				else new_succ=traverse->current_successor;

				if(new_succ !=NULL)
				  {
				    if(0)cout << "successor for dest " << IPAddr::ToDotted(traverse->destin) << ".....";
				    if(0)cout << "= " << IPAddr::ToDotted(new_succ->feas_succ) << ", RD=" << new_succ->RD << ", min_RD=" 
					      << new_succ->min_RD<< ", metric=" << new_succ->metric << std::endl;
				    if(traverse->current_successor->feas_succ !=ip) { cout << "Something is wrong.\n";  exit(1); }


				    ReplyHeader* reply = new ReplyHeader();
				    reply->setReplyFields(0x0104,new_succ->feas_succ,new_succ->metric,
							  recvdQuery->destination);
				    reply_Mg->replyHeader.push_back(reply);
				   			
				    // send back reply....saying you are my successor.
				    // send reply as a unicast to the router I got the query from
				    Send_Replies=1;

				    // reset the NTR flag
				    setNTRflag( 0, recvdQuery->destination, ip);


				    //todo
				    // If the distance (to the dest) changes, ALSO send an update to the rest of the neighbors.

				    // IT DOESNT MATTER WHETHER THE SUCCESSOR CHANGED OR NOT.
				    // UPDATES ARE ONLY SENT IF THE METRIC TO THE DEST CHANGES (WRT THE OLD METRIC)
				    // THIS IS BECAUSE THE ROUTERS RECEIVING THE UPDATES WILL USE ME AS NEXT HOP IN EITHER CASE.


				    if(new_succ->metric != traverse->current_successor->metric)
				      {
					// The metric changed via the successor
					// i.e. new distance is different from previous distance
					// send out an update
					UpdateHeader* update = new UpdateHeader();
					update->setUpdateFields(0x0102, new_succ->feas_succ,0,0,1500, new_succ->metric,
								0,0,0,traverse->significant,recvdQuery->destination);
					update_Mg->updateHeader.push_back(update);
					// 2 is the flow id for update packets

					Send_Updates=1;

				      }
				    // if()
				    else 
				      {
					cout << "Didnt Append Updates....no change in Successor's metric\n";
				      }

				    traverse->current_successor = new_succ;
				  }
				else 
				  {
				    cout << "Couldnt find a successor (which is impossible) \n";
				    exit(1);
				  }
			      }
			    //if FC_check()==1


			    else
			      {
				// query from successor, the FC was NOT satisfied
				FC_EVENT=FC_NOT_SATISFIED;

				if((int)traverse->state==0)
				  {
				    // in PASSIVE state
				    cout << "In PASSIVE state...\n";

				    // forward query...can only do this if in passive state.
				    // for a given destination, a node can become active in only one diffusing computation at a time
				    // so it can expect at most one reply from each neighbor
				    cout << "Sending Queries to Everyone...Beginning another Diffusing Computation\n";

				    //sanity check...not necessary
				    if( (new_best_RD!=recvdQuery->hopcount)  && (new_best_metric !=met_) ) { cout << "Error. Check again !\n";  exit(1); }

				    // what if the destination doesnt exist in TT and QORS table ?                  
				    // couldnt find a feasible successor..the value of RD in the query in this case specifies the new distance via the old(current) successor.

				    QueryHeader* query = new QueryHeader();
				    query->setQueryFields(0x0103, met_, recvdQuery->destination);
				    query_Mg->queryHeader.push_back(query);
				    if(0)cout << "Query has::destination=" << recvdQuery->destination << ", metric=" << met_ << std::endl;
				    setRSflags(1,(IPAddr_t)recvdQuery->destination);
				    schedule_active_timer(recvdQuery->destination);

				    Send_Queries=1;

				    // set RD (distaince reported by successor)...same for all DIC,SNC,CSC
				    // RD set to NEW metric via the OLD(current) successor (since we couldnt find a new successor !)
				    // todo...dont know if the following 1 line is correct or not
				    if(recvdQuery->hopcount < traverse->current_successor->min_RD)  traverse->current_successor->min_RD=recvdQuery->hopcount;
				    traverse->current_successor->RD = recvdQuery->hopcount;
				    traverse->current_successor->min_RD = recvdQuery->hopcount;
				    traverse->current_successor->metric = met_;

				    temp_itr->min_RD=recvdQuery->hopcount;

				    // set FD...taken from Garcia's paper...page 133.
				    // Henceforth I cannot change the FD, successor or RD till Ive received all replies to this query
				    // todo...is the following right ?
#if defined(SNC)
				    traverse->min_metric_till_now= new_best_metric;
				    traverse->FD=traverse->min_metric_till_now;
				    evaluate_best_metric_and_FD (1, recvdQuery->destination);
#elif defined(DIC)
				    traverse->min_metric_till_now= new_best_metric;
				    traverse->FD=traverse->min_metric_till_now;
				    evaluate_best_metric_and_FD (1, recvdQuery->destination);
#elif defined(CSC)
				    //todo...test this
				    traverse->current_successor->min_RD = recvdQuery->hopcount;
				    traverse->FD= traverse->current_successor->min_RD;
#endif

				  }

			      }
			    //else()
			  }
			// if (query from successor)


			else
			  {

			    //todo
			    // when a node is passive and processes a query from a node other than its current successor, and
			    // if FC is not satisfied, it sends reply to its neighbor with the current value of RD before starting its own Query.

			    // query from a neighbor (not the successor)
			    INPUT_EVENT=RECEIVE_QUERY;
			    cout << "Ordinary Query....Not from Successor.\n";

			    // FC has to be satisfied by the current successor.
			    FC_EVENT=FC_SATISFIED;

			    // todo...is it appropriate to find a new successor ?
			    if((int) traverse->state==0) new_succ= findSuccessor(recvdQuery->destination,1);
			    else new_succ=traverse->current_successor;

			    if(new_succ !=NULL)
			      {
				if(0)cout << "successor for dest " << IPAddr::ToDotted(traverse->destin) << ".....";
				if(0)cout << "= " << IPAddr::ToDotted(new_succ->feas_succ) << ", RD=" << new_succ->RD << ", min_RD=" 
					  << new_succ->min_RD << ", metric=" << new_succ->metric << std::endl;
                              
				traverse->current_successor = new_succ;

				// wrong ?
				ReplyHeader* reply = new ReplyHeader();
				reply->setReplyFields(0x0104,new_succ->feas_succ,new_succ->metric,
						      recvdQuery->destination);
				reply_Mg->replyHeader.push_back(reply);

				// send reply as a unicast to the router I got the query from
				Send_Replies=1;

				// reset the NTR flag
				setNTRflag( 0, recvdQuery->destination, ip);
			      }
			    else 
			      {
				cout << "Couldnt find a successor (which is impossible) \n";
				exit(1);
			      }

			  }
			// else (query from successor)


			if(0)cout << "previous state of destination " << IPAddr::ToDotted(traverse->destin) << " is s=" << traverse->state << std::endl;
			determineState((int)INPUT_EVENT,(int)FC_EVENT, (IPAddr_t) recvdQuery->destination);
			if(0)cout << "New state of destination " << IPAddr::ToDotted(traverse->destin) << " is s=" << traverse->state << std::endl;
                  
		      }
		    //if(traverse->destin ==destination)
		  }
		// for()

		displayQORStable();
		evaluate_best_metric_and_FD(1);

		flush_TT();
		TT_to_RT();
		//displaytopologytable();
	      }
	    // QUERY MESSAGE
	    break;

	
	  case MsgReply:
	    // REPLY MESSAGE
	    if(e->opcode == 4)
	      { 
		if(0)cout << "Received a Reply from " << IPAddr::ToDotted(ip) << std::endl;

		// get the first reply, and remove it from the vector
		//ReplyHeader* recvdReply = e->replyHeader.back();
		//e->replyHeader.pop_back();
		
		ReplyHeader* recvdReply = e->replyHeader.front();
		vector<ReplyHeader*>::iterator iter = e->replyHeader.begin();
		e->replyHeader.erase(iter);

		if(recvdReply->typefield!=0x0104) { cout << "Typefield should be 0x0104 for Reply Packets. Exiting.\n"; exit(1);}


		// sanity check
		// if the destination is in PASSIVE state, it could not have received a Reply packet.
		int current_state;
		for (topologyEntryList_::iterator traverse = TopologyTable.begin(); traverse != TopologyTable.end(); ++traverse)
		  {
		    if(traverse->destin==recvdReply->destination)
		      {
			current_state=(int)traverse->state;
			if(current_state==0)
			  {
			    cout << "Destination is in PASSIVE state, it could not have received a Reply packet.\n";
			    exit(1);
			  }
		      }
		  }


		if (adj == NULL && adj_flag!=1)
		  {
		    // ONLY WHEN ACCEPTING REPLY MESSAGES FROM UNHEARD-OF NEIGHBORS
		    cout << "\nGot a 'Reply' packet from an unrecognised Neighbor (havent yet got a hello from it). \n";
		    cout << "Nonetheless, Adding the entry to Adjacency Table\n";
		    //exit(1);

		    // add to adjacency table, create and reset a new hold timer
		    adj=AddAdjacency_ScheduleHold (ip,if_,holdTime,now,EIGRP_SRTT);
		    adj_flag=1;
		    addtoQORSTable(ip);
		  }
		else
		  {
		    //adj!=NULL  known neighbor
		    adj_flag=1;
              
		    //  cancel the previousEvent  adj->holdEvent
		    holdTimer.Cancel(adj->holdEvent);
		    cout << "\nReScheduling a hold event at " << now << " holdTime=" << holdTime << holdTime << std::endl;   // dont use new EIGRPEvent-it will cause a memory leak
		    // use holdTime, not EIGRP_DEFAULT_HOLD_TIME
		    holdTimer.Schedule( holdTime, adj->holdEvent);
		  }
          

		// find the cost to the neighbor
		Metric_t metr=0;
		for (NeighborMap_t::iterator i_ = neighbors.begin();  i_ != neighbors.end();  ++i_)
		  { 
		    yourIP=i_->first;
	
		    if(yourIP==ip) 
		      {
			i_->second->Up();
			metr= i_->second->metric;
			cout << "cost to neighbor " << metr << std::endl;
		      }
		  }

		cout << "Buffering the reply packet till I get Replies from all my neighbors, or their active timers expire...\n";
		cout << "Till then,I cannot modify the current successor, FD or the RD\n";

		double finalmetric= recvdReply->hopcount + metr;
		if(finalmetric>=Infinity) finalmetric=Infinity;

		ReplyBufferEntry R;
		R.SetReplyBufferEntry(recvdReply->hopcount, finalmetric, recvdReply->destination, ip );
		ReplyBuffer.push_back(R);


		// cancel the last scheduled Active timer for that dest-adjacency pair
		struct neighbors_ *itr;
		for (QORS_EntryList_::iterator traverse = QORSTable.begin(); traverse != QORSTable.end(); ++traverse)
		  {
		    if(traverse->dest==recvdReply->destination)
		      {
			itr=traverse->ptr_;
			if(itr->neigh==ip)        
			  {
			    // cancel the scheduled active timer
			    activeTimer.Cancel(itr->activeEvent);
			    itr=itr->nb;
			  }
		      } // if()

		  } // for()


		// now Reset the RS flag in the QORS table to 0
		setRSflags( 0 /*rs_flag*/, recvdReply->destination , ip /*adjacency*/ );
		int check = checkRSflags(recvdReply->destination);
     
		if(check==0)
		  {
		    cout << "LAST REPLY received.\n";

		    // possible that I will receive multiple reply packets (for different destinations) bundled in the same packet 
		    // (from the same router)
		    // If last reply is received for all those destinations, I MAY need to reply to one router(if NTR bit set)
		    // use just ONE packet.


		    // determine the BEST reply, among all.
		    // pass pointers to the reply message, update message
		    // and pointers for whether reply/update messages need to be sent
		    // and pointers for where replies,if any, have to be sent.
		    processReplies(current_state,recvdReply->destination, reply_mg , &sendRepl, &sendReplyTo, update_mg, &sendUpd);
		  } 
		// if()
          
		else
		  {
		    cout << "Awaiting more replies.\n";
		    // do nothing.
		  }
	    
	      } // REPLY MESSAGE
	    break;
	    
	  case MsgUnknown:
	  case MsgAck:
	    break;
	    
	  }
	}
    }


  
  // if its an update packet with INIT bit set (Initial topology table exchange)
  if(e->opcode==1)
    {
      //displaytopologytable();
      // CHECK the INIT flag
      if(e->flags==1)
        {
          // Send back your Topology Table
	  cout << "\n\nINIT flag set. This EIGRP sends back his Topology (actually Routing) Table.\n";
	  cout << "But I dont set my INIT flag\n";

          // send only the Routing Table, not the entire TT
#if defined(SEND_TOPOLOGY_TABLE_ON_INIT)
	  // SendTopologyTableUpdate (ip,0);
#elif defined(SEND_ROUTING_TABLE_ON_INIT)
          InitialExchange (ip,0);
#else
          cout << "One Has to be used\n";
          //exit(1);
#endif
        }
    }



  if(Send_Updates==1)
    {
      // send Updates to every one except the one I got the query from
      McastReliable(update_Mg,ip);
      
      int k_=0;
      // increment sent message counter for Updates
      for (NeighborMap_t::iterator i_ = neighbors.begin();  i_ != neighbors.end();  ++i_)
        {

          if (i_->second->status == Neighbor::UP)  
            { 
              k_++;
            }
        }
      k_--;
      Sent_Mesg_Counters.Updates += k_;
      Increment_Sent_Updates(k_);
      
      if(0)cout << "Sent_Mesg_Counters.Updates = " << Sent_Mesg_Counters.Updates << std::endl;
      if(0)cout << "Sent Update Message as a Reliable Multicast to everybody except the one I got the Query from(" << IPAddr::ToDotted(ip) << ")\n";
      Send_Updates=0;
    }



  if( Send_Queries==1 || sendQuer==1 )
    {
      // send mcast to everyone.
      McastReliable(query_Mg);

      int k_=0;
      // increment sent message counter for Queries
      for (NeighborMap_t::iterator i_ = neighbors.begin();  i_ != neighbors.end();  ++i_)
        {
          if (i_->second->status == Neighbor::UP)  
            { 
              k_++;
            }
        }

      Sent_Mesg_Counters.Queries += k_;
      Increment_Sent_Queries(k_);
      if(0)cout << "Sent_Mesg_Counters.Queries = " << Sent_Mesg_Counters.Queries << std::endl;
      if(0)cout << "Recd_Mesg_Counters.Queries = " << Recd_Mesg_Counters.Queries << std::endl;

      if(0)cout << "Going Active, starting diffusing computation\n";
      if(0)cout << "Sent Queries as Reliable Multicast to everybody \n";
      Send_Queries=0;
      sendQuer=0;
    }



  if(Send_Replies==1)
    { 
      // send reply as a unicast to the router I got the query from
      UcastReliable(reply_Mg, ip);              // 4 is the flow id for reply packets
      Sent_Mesg_Counters.Replies += 1;
      Increment_Sent_Replies();
      if(0)cout << "Sent_Mesg_Counters.Replies = " << Sent_Mesg_Counters.Replies << std::endl;
      if(0)cout << "Recd_Mesg_Counters.Replies = " << Recd_Mesg_Counters.Replies << std::endl;
      if(0)cout << "Sent Reply as Reliable Unicast to " << IPAddr::ToDotted(ip) << std::endl;
  
      cout << "Clearing the reply packet.\n";
      cout << "*************************************\n";
      Send_Replies=0;
    }



  if(sendRepl==1)
    {
      // send reply as a unicast to the router I got the query from
      UcastReliable(reply_mg,sendReplyTo);              // 4 is the flow id for reply packets
      Sent_Mesg_Counters.Replies += 1;
      Increment_Sent_Replies();
      if(0)cout << "Sent_Mesg_Counters.Replies = " << Sent_Mesg_Counters.Replies << std::endl;
      if(0)cout << "Sent Reply as Reliable Unicast to " << IPAddr::ToDotted(sendReplyTo) << std::endl;

      cout << "Clearing the reply packet.\n";
      cout << "*************************************\n";
      sendRepl=0;
    }
  


  if(sendUpd==1)
    {
      // 2 is the flow id for update packets
      if(avoid==0)
        {
          McastReliable(update_mg);
          int k_=0;
          
          // increment sent message counter for Updates
          for (NeighborMap_t::iterator i_ = neighbors.begin();  i_ != neighbors.end();  ++i_)
            {
              
              if (i_->second->status == Neighbor::UP)  
                {
                  k_++;
                }
            }
          
          cout << "Sent Update Message as a Reliable Multicast to everybody \n";
          Sent_Mesg_Counters.Updates += k_;
          Increment_Sent_Updates(k_);
          if(0)cout << "Sent_Mesg_Counters.Updates = " << Sent_Mesg_Counters.Updates << std::endl;
        }
      
      else if(avoid==1)
        {
          // dont send to this one router
          McastReliable(update_mg, AvoidRouterForUpdates);
          
          int k_=0;
          // increment sent message counter for Updates
          
          for (NeighborMap_t::iterator i_ = neighbors.begin();  i_ != neighbors.end();  ++i_)
            {
              
              if (i_->second->status == Neighbor::UP)  
                {
                  k_++;
                }
            }
          k_--;
          cout << "Sent Update Message as a Reliable Multicast to everybody except to this router \n";
          Sent_Mesg_Counters.Updates += k_;
          Increment_Sent_Updates(k_);
          if(0)cout << "Sent_Mesg_Counters.Updates = " << Sent_Mesg_Counters.Updates << std::endl;          
        }      
      sendUpd=0;

    }



  
  // Updates, Queries, and Replies use reliable delivery, so need to send an ACK
  Neighbor*  nb = GetNeighbor(ip, if_);

  if( !avoidAcking && (e->opcode == 1 || e->opcode == 3  || e->opcode == 4) ) 
    {
      // a Hello packet with a nonzero ACK field is treated as an ACK packet
      //EIGRPHeader* e1 = new EIGRPHeader();
      EIGRPHeader* e1 = new EIGRPHeader(MsgHello,2,(unsigned int)MsgHello,0x0000EFCF,0x00000000,0,1,1000);
      
#ifdef HAVE_QT
      ColoredPacket* p1 = new ColoredPacket(Qt::yellow);
      cout << "Sending a colored Ack\n";
#else
      Packet* p1 = new Packet();
      cout << "Sending a regular Ack\n";
#endif

     
      e1->msgSeq = e->msgSeq;
      e1->mTxt = EMsgAck;
      e1->mSrc = nb->myIP;
      e1->mDst = ip;
      p1->PushPDU(e1);
      // Send the ack
      IPV4ReqInfo r(ip, IPADDR_NONE, 1, 88); // Info for l3
      IPV4::Instance()->DataRequest(myNode, p1, &r);
    }
	    

  
  if(e->mTxt != EMsgAck) 
    {
      // for the message counters
      switch(e->opcode)
	{
	case 1:
	  Recd_Mesg_Counters.Updates +=1;
	  Increment_Recd_Updates();
	  //  cout << "Recd_Mesg_Counters.Updates = " << Recd_Mesg_Counters.Updates << std::endl;
	  break;
	  
	case 2:
	  break;
	  
	case 3:
	  Recd_Mesg_Counters.Queries +=1;
	  Increment_Recd_Queries();
	  //  cout << "Recd_Mesg_Counters.Queries = " << Recd_Mesg_Counters.Queries << std::endl;
	  break;
	  
	case 4:
	  //  cout << "Recd_Mesg_Counters.Replies = " << Recd_Mesg_Counters.Replies << std::endl;
	  Recd_Mesg_Counters.Replies +=1;
	  Increment_Recd_Replies();
	  break;
	  
	case 5:
	  Recd_Mesg_Counters.Hellos +=1;
	  Increment_Recd_Hellos();
#ifdef  SHOWHELLOPACKETS
	  //      cout << "Received a Hello Message from " << IPAddr::ToDotted(ip) << std::endl;
	  //      cout << "Recd_Mesg_Counters.Hellos = " << Recd_Mesg_Counters.Hellos << std::endl;
#endif
	  break;
	  
	default:
	  break;
	}
    }
  

  delete p; // Delete the packet

}





// MODIFIED*
// this is called when all "Reply" messages have been received (all queries have been answered)
// this is called either by activetimeout() or recv()-REPLY functions

void EIGRP::processReplies(int current_state,IPAddr_t dst, EIGRPHeader *reply_header, int *sendRepl ,
			   IPAddr_t *sendReplyTo, EIGRPHeader *update_header, int *sendUpd)
{
  DEBUG0((cout << "Entering processReplies()" << endl));
  
  cout << "All 'Reply' messages have been received (all queries have been answered)\n";

  EIGRPInputEvent_t INPUT_EVENT = RECEIVE_LAST_REPLY;
  EIGRP_FC_Event_t FC_EVENT;
  
  displayreplybuffer();
  if(0)cout << "\ndst=" << IPAddr::ToDotted(dst) << std::endl;

  IPAddr_t best_destination;
  IPAddr_t best_heardfrom=Infinity;
  Metric_t best_finalmetric=Infinity, best_rd=Infinity;
  int sentNewReplies=0;


  if(0)cout << "Entries pertaining to dest=" << IPAddr::ToDotted(dst) << " only\n";
  for (ReplyBufferEntryList_::iterator trav = ReplyBuffer.begin(); trav != ReplyBuffer.end(); ++trav)
    {
      if(trav->destination==dst)
        {
          if(0)cout << IPAddr::ToDotted(trav->destination) << "\t\t" << trav->rd << "\t\t" << trav->finalmetric << "\t\t" 
		    << IPAddr::ToDotted( trav->heardfrom) << std::endl;
          if(trav->finalmetric <= best_finalmetric) best_finalmetric=trav->finalmetric;
        }
    }

  for (ReplyBufferEntryList_::iterator trav = ReplyBuffer.begin(); trav != ReplyBuffer.end(); ++trav)
    {
      if( (trav->destination==dst) && (trav->finalmetric == best_finalmetric))
        {
          if (trav->rd <= best_rd)  best_rd=trav->rd;
        }
    }

  for (ReplyBufferEntryList_::iterator trav = ReplyBuffer.begin(); trav != ReplyBuffer.end(); ++trav)
    {
      if( (trav->destination==dst) && (trav->finalmetric == best_finalmetric) && (trav->rd==best_rd) )
        {
          if( trav->heardfrom <= best_heardfrom )
            {
              best_rd = trav->rd;
              best_finalmetric = trav->finalmetric;
              best_destination = trav->destination;
              best_heardfrom = trav->heardfrom;
            }
        }
      
    }

  if(0)cout << "Best Reply\n";
  if(0)cout << "dest=" << IPAddr::ToDotted( best_destination) << "\t\tRD=" <<  best_rd << "\t\tfinalmetric=" << best_finalmetric << "\t\theardfrom="
	    << IPAddr::ToDotted(best_heardfrom) << std::endl;
  if(0)cout << "Current state of the destination: s=" << current_state << std::endl;

  int pass_fail=0; // initialize to fail

  struct feasibles *new_succ=NULL;
  new_succ= (struct feasibles *)malloc(sizeof(*new_succ));


  // if in ACTIVE1(s=2) or ACTIVE3(s=4) state, set FD=Infinity. This insures the FC check will be satisfied.
  // if in ACTIVE0(s=1) or ACTIVE2(s=3) state, dont change the FD.


  for (topologyEntryList_::iterator traverse = TopologyTable.begin(); traverse != TopologyTable.end(); ++traverse)
    {
      if(traverse->destin==dst)
        {

        mustBecomePassive:
          // sanity check
          if((int)traverse->state!=current_state) { cout << "Error\n"; exit(1); }
          if(current_state==0) { cout << "Impossible! Cant be in Passive state.\n"; exit(1); }

          if ((current_state == 2) ||(current_state==4))
            {
              // reset the FD to Infinity so that the FC is satisfied.
              cout << "set FD to Infinity\n";
              traverse->FD=Infinity;
          
#if defined(SNC)
              traverse->min_metric_till_now = Infinity;
          
#elif defined(DIC)
              traverse->min_metric_till_now = Infinity;
          
#elif defined(CSC)
              traverse->current_successor->min_RD = Infinity;
#endif
            }

          else if ((current_state == 1) ||(current_state==3))
            {
              // do nothing to FD
            }


          // now check to see if the FC is satisfied.
          INPUT_EVENT = RECEIVE_LAST_REPLY;
          pass_fail=FC_check( dst, best_rd, best_finalmetric );

          if(pass_fail==1)
            {
              // FC check passed
              FC_EVENT=FC_SATISFIED;
              if(0)cout << "Best Reply::dest=" << IPAddr::ToDotted(best_destination) << "\t\tRD=" <<  best_rd << "\t\tfinalmetric=" <<  best_finalmetric
			<< "\t\theardfrom=" << IPAddr::ToDotted( best_heardfrom) << std::endl;
	   
              if(0)cout << "set FD to best value\n";
          
#if defined(SNC)
              traverse->min_metric_till_now = best_finalmetric;
              traverse->FD = traverse->min_metric_till_now;
#elif defined(DIC)
              traverse->min_metric_till_now = best_finalmetric;
              traverse->FD = traverse->min_metric_till_now;
#elif defined(CSC)
              traverse->current_successor->min_RD = best_rd;
              traverse->FD = traverse->current_successor->min_RD;
#endif

              // find a new successor to the destination
              new_succ->feas_succ=best_heardfrom;
              new_succ->RD=best_rd;
              new_succ->min_RD=best_rd;
              new_succ->metric=best_finalmetric;
              new_succ->fee=NULL;


              // all replies have been received.
              // now check to see if I need to Reply to anybody.
              // if yes, Send replies to those neighbors, reset the NTR flag in the QORS table.


              struct neighbors_ *itr_;
              for (QORS_EntryList_::iterator t = QORSTable.begin(); t != QORSTable.end(); ++t)
                {
                  if(t->dest ==dst)
                    {
                      itr_=t->ptr_;
                      while(itr_!=NULL)
                        {
                          if(itr_->needToReply==1)
                            {
                              // pointer variables get changed...will be reused by function that called ProcessReplies()
                              // send a Reply to this Neighbor.
			      ReplyHeader* reply = new ReplyHeader();
			      reply->setReplyFields(0x0104,best_heardfrom,best_finalmetric,dst);
			      reply_header->replyHeader.push_back(reply);

                              *sendReplyTo=itr_->neigh;
                              *sendRepl=1;
                              
                              sentNewReplies=1;
                              // reset the NTR flag
                              itr_->needToReply=0;
                            }

                          itr_=itr_->nb;
                        }
                    }
                } //for()
 
              determineState((int)INPUT_EVENT,(int)FC_EVENT, (IPAddr_t)dst);

            }
          // pass_fail==1

          else if(pass_fail==0)
            {
              // FC check failed
              cout << "FC check failed....must try again !\n";
              FC_EVENT=FC_NOT_SATISFIED;
              if(current_state==2 || current_state==4 ) { cout << "FC could not have failed in s=2 or s=4\n"; exit(1); }
              determineState((int)INPUT_EVENT,(int)FC_EVENT, (IPAddr_t)dst);
              if(((int)traverse->state!=2) && ((int)traverse->state!=4 )) { cout << "Has to be in s=2 or s=4\n"; exit(1); }

              current_state=(int)traverse->state;
              goto mustBecomePassive;

            }


          if(traverse->state!=0) { cout << "Still in ACTIVE state. Should be in PASSIVE state\n"; exit(1); }


          if(sentNewReplies==0)
            {
              // didnt send any new Replies.
              // send Updates if the distance changed.

	   
              // IT DOESNT MATTER WHETHER THE SUCCESSOR CHANGED OR NOT.
              // UPDATES ARE ONLY SENT IF THE METRIC VIA THAT SUCCESSOR CHANGES (WRT THE OLD METRIC)
              // THIS IS BECAUSE THE ROUTERS RECEIVING THE UPDATES WILL USE ME AS NEXT HOP IN EITHER CASE.
              
              if(new_succ->metric != traverse->current_successor->metric)
                {
                  // The metric changed via the successor
                  // i.e. new distance is different from previous distance
                  // send out an update

                  // pointer variables get modified...will be reused by function that called ProcessReplies()
		  UpdateHeader* update = new UpdateHeader();
		  update->setUpdateFields(0x0102, new_succ->feas_succ,0,0,1500, new_succ->metric,0,0,0, traverse->significant, dst);
		  update_header->updateHeader.push_back(update);
		  *sendUpd=1;

                } 
              // if()
              else 
                {
                  cout << "Didnt Send.\n";
                }

            }
          // if(sentNewReplies==0)

          traverse->current_successor=new_succ;

        }
      // if(traverse->destin==dest)
    }
  //for() 


  // update TT with this best reply.
  // be careful not to mess up the state. We've reached the correct state already (PASSIVE). So, DONT change state.
  UpdateTopologyTable( (IPAddr_t) dst, 32 ,best_rd/*hopcount*/, best_finalmetric/*metric*/,(IPAddr_t) best_heardfrom /*FS*/, 0 /*appendupdates*/, 0 /*appendqueries*/,0 /*changestate*/, (int)INPUT_EVENT);


  // clear the ReplyBuffer of those entries concerning that particular destination.
  // and update the TT with all replies (including the best reply)...if they satisfy the FC that is.
  ReplyBufferEntryList_::iterator entry_to_be_deleted;
  for (ReplyBufferEntryList_::iterator trav = ReplyBuffer.begin(); trav != ReplyBuffer.end(); ++trav)
    {
      if(trav->destination==dst)
        {
          // update the topology table...with ALL replies (or sometimes updates).
          // make sure you dont send out queries or updates.
          UpdateTopologyTable( (IPAddr_t) trav->destination, 32 /*prefix*/, trav->rd /*hopcount*/, trav->finalmetric/*metric*/,(IPAddr_t) trav->heardfrom /*FS*/, 0 /*appendupdates*/, 0 /*appendqueries*/,0 /*changestate*/, (int)INPUT_EVENT);
          
          // delete the entry from the ReplyBuffer.
          entry_to_be_deleted=trav;
          --trav;
          ReplyBuffer.erase(entry_to_be_deleted);
        }
    }
  
  displayreplybuffer();


  flush_TT();
  TT_to_RT();
  
}
// processReplies()






#ifdef CODE_THIS_LATER
void EIGRP::retransmit(ReTxEvent* e)
{ 
  DEBUG0((cout << "Got retx timeout time " << Simulator::instance->Now()
          << " for seq " << e->m_seq
          << " to " << (string)IPAddr(e->m_ip) << endl));
  
  Neighbor& n = getNeighbor(e->m_ip);
  if (e->m_retx == RETX_LIMIT)
    { // Too many retries, declare this neighbor down and exit
      n.Down();
      cout << "Too Many Retries, declaring neighbor " << e->m_np.first <<"/" << e->m_np.second << "down\n";
      return;
    }
  MsgEventMap_t::iterator i = n.m_sent.find(e->m_seq);
  if (i == n.m_sent.end())
    { // ?
      cout << "HuH? EIGRP::retransmit can't find msg " << e->m_seq << std::endl;
      exit(1);
    }
  if(0)cout << "ReTxTimeout, copying msg " << Now() << std::endl;
  RouteMsg msg = i->second.first;
  if(0)cout << "ReTxTimeout erasing " << Now() << std::endl;
  n.m_sent.erase(i);
  uCast(msg, EMsgReliable, e->m_ip, n, e); // Resend
  if(0)cout << "Exiting ReTxTimeout " << Now() << std::endl;
}
#endif

#ifdef DONT_NEED
void EIGRP::Timeout(TimerEvent*)
{
  DEBUG0((cout << "EIGRP timeout" << endl));
}
#endif

#ifdef CODE_THIS_LATER
int EIGRP::command(int argc, const char*const* argv)
{
  if (argc == 2)
    {
      if (strcmp(argv[1], "alive") == 0)
	{ // Set status to alive
	  Alive();
	  return TCL_OK;
	}
      if (strcmp(argv[1], "dead") == 0)
	{ // Set status to dead
	  Dead();
	  return TCL_OK;
	}
    }
  return Agent::command(argc, argv);
}
#endif



void EIGRP::McastUnreliable(EIGRPHeader* p)
{
  DEBUG0((cout << "Entering McastUnreliable(EIGRPHeader)" << endl));
  mCast(p, EMsgUnreliable);
}

void EIGRP::McastReliable(EIGRPHeader* p, IPAddr_t ip)
{
  DEBUG0((cout << "Entering McastReliable(EIGRPHeader,IPAddr_t) and header of type " << p->opcode << endl));
  mCast(p, EMsgReliable, ip);
}

void EIGRP::UcastUnreliable(EIGRPHeader* p, IPAddr_t ip)
{
  DEBUG0((cout << "Entering UcastUnreliable(EIGRPHeader,IPAddr_t)" << endl));
  uCast(p, EMsgUnreliable, ip);
}

void EIGRP::UcastReliable(EIGRPHeader* p, IPAddr_t ip)
{
  DEBUG0((cout << "Entering UcastReliable(EIGRPHeader)" << endl));
  uCast(p, EMsgReliable, ip);
}

void EIGRP::StartAt(Time_t t)
{
  if (!myNode)
    {
      cout << "Attempt to start EIGRP with no local node" << endl;
      return;
    }
  
  if (!myNode->IsReal()) return; // No action for ghost nodes
  // Schedule the start event
  EIGRPEvent* e = new EIGRPEvent(EIGRPEvent::START);
  if(0)cout << "Starting eigrp " << myNode->Id() << " at time " << t << endl;
  Scheduler::Schedule(e, t - Simulator::Now(), this);
}

void EIGRP::StopAt(Time_t t)
{
  if (!myNode)
    {
      cout << "Attempt to stop EIGRP with no local node" << endl;
      return;
    }
  
  if (!myNode->IsReal()) return; // No action for ghost nodes
  // Schedule the stop event
  EIGRPEvent* e = new EIGRPEvent(EIGRPEvent::STOP);
  DEBUG0((cout << "Stopping eigrp " << myNode->Id()
	  << " at time " << t << endl));
  Scheduler::Schedule(e, t - Simulator::Now(), this);
}

void EIGRP::SetLossProbability(Random_t lp)
{
  if (lossRNG) delete lossRNG;
  lossProb = lp;
  lossRNG = new Uniform(); // [0..1)
}


#ifdef CODE_THIS_LATER
void  EIGRP::recvMessage(RouteMsg& m, IPAddr_t ip, 
			 NPPair_t f, IF_t i)  // msg recv
{ // Should be overridden in subclass
  cout << "EIGRP " <<  name() <<" got msg on if " << i << " from " << f.first << "/" << f.second << std::endl;
}
#endif

Neighbor* EIGRP::GetNeighbor(IPAddr_t ip, Interface* i)
{
  //DEBUG0((cout << "Entering GetNeighbor(IPAddr_t,Interface)" << endl));
  NeighborMap_t::iterator n = neighbors.find(ip);
  if (n != neighbors.end())
    { // Found it.  Make it UP (in case it's presently down)
      //n->second->Up();
      return n->second; // Return it
    }
  
  // Does not exist, create one
  Neighbor* nb = new Neighbor();
  nb->myIP = i->GetIPAddr();   // IPAddress from received interface
  nb->nbIP = ip;               // Neighbor's IP
  nb->nbMask = i->GetIPMask(); // Neighbor's IP Mask
  nb->iface = i;               // Interface to use to talk to this neighbor  

  nb->myEIGRP = this;          // Owning EIGRP
  neighbors.insert(NeighborMap_t::value_type(ip, nb));
  DEBUG0((cout << "EIGRP " << myNode->Id()
	  << " added nb " << (string)IPAddr(ip) << endl));
  return nb;
}



void EIGRP::mCast(EIGRPHeader* e, EMCast_t t, IPAddr_t dontsend)
{ // Common for rel/unreliable
  DEBUG0((cout << "Entering mCast(EIGRPHeader,EMCast_t,IPAddr_t)" << endl));
  
  e->mTxt = t; // Set message transmission type in EIGRP Header
  e->msgSeq = nextSeq++; // Set sequence number
  // Need to save the packet for re-tx if reliable desired
  if (t == EMsgReliable)
    { // Save a copy on every neighbor
      for (NeighborMap_t::iterator i = neighbors.begin();
	   i != neighbors.end(); ++i)
	{ // Save this info for later retransmission
	  Neighbor* n = i->second;

	  if ( n->IsDown() || (n->nbIP == dontsend) ) continue; // Not if neighbor is down or if neighbor sent the query
	  n->AddReTx(e);
	  if ( n->IsDown() || (n->nbIP == dontsend) ) cout << "Huh! shouldn't print?? \n";

	  //send it to all other neighbors
	  if(0)cout << "calling uCast(e,t, n->nbIP, n) from mCast(e,t,dontsend) \n";
	  EIGRPHeader* eCopy = (EIGRPHeader*)e->Copy();
	  uCast(eCopy, t, n->nbIP, n);
	}
    }
  delete e;
}



void EIGRP::mCast(EIGRPHeader* e, EMCast_t t)
{ // Common for rel/unreliable
  DEBUG0((cout << "Entering mCast(EIGRPHeader,EMCast_t)" << endl));
  
  e->mTxt = t; // Set message transmission type in EIGRP Header
  e->msgSeq = nextSeq++; // Set sequence number
  // Need to save the packet for re-tx if reliable desired
  if (t == EMsgReliable)
    { // Save a copy on every neighbor
      for (NeighborMap_t::iterator i = neighbors.begin();
	   i != neighbors.end(); ++i)
	{ // Save this info for later retransmission
	  Neighbor* n = i->second;
	  if (n->IsDown()) continue; // Not if neighbor is down
	  n->AddReTx(e);
	 
	  EIGRPHeader* eCopy = (EIGRPHeader*)e->Copy();
	  uCast(eCopy, t, n->nbIP, n);
	}
    }
  else // EMsgUnreliable
    {
      // Get the interfaces for this node
      IFVec_t IfVec = myNode->Interfaces();
      for (IFVec_t::iterator j = IfVec.begin(); j != IfVec.end(); ++j)
	{
	  IPAddrVec_t IpAddrVec;
	  NodeWeightVec_t NodeWeightVec;
	  myNode->NeighborsByIf(*j, IpAddrVec);
	  
	  Link* myLink = (*j)->GetLink();
	  myLink->AllNeighbors(NodeWeightVec); 
	  
	  for(NodeWeightVec_t::iterator k = NodeWeightVec.begin(); k != NodeWeightVec.end(); ++k)
	    {
	      L4Demux* l4demux = (L4Demux*)(*k).node->LookupProto(4, EIGRP_PROTO);
	      if (l4demux) // this node runs EIGRP
		{
		  IPAddr_t ip = (*j)->NodePeerIP((*k).node);
		  if((string)IPAddr(ip) == "0.0.0.0")
		    continue;
		  
		  // Sanity Check (make sure that the neighbor node is not this node)
		  if( (*k).node != myNode)
		    {
		      Neighbor* n = GetNeighbor(ip, *j);
		      EIGRPHeader* eCopy = (EIGRPHeader*)e->Copy();
		      uCast(eCopy, t, n->nbIP, n);
		    }
		}
	    }
	}
    }

}




   
void EIGRP::uCast(EIGRPHeader* e, EMCast_t t, IPAddr_t i, Neighbor* n)
{
  DEBUG0((cout << "Entering uCast(EIGRPHeader,EMCast_t,Neighbor)" << endl));

  if (Status() == ALIVE)
    { // Send only if alive
      
      if (t == EMsgReliable)
	{ // Save a copy for possible re-tx
	  if (!n->IsDown()) // Not if neighbor is down
	    n->AddReTx(e);
	}
      
#ifdef HAVE_QT
      ColoredPacket* p = new ColoredPacket();
          
      if(e->opcode == 1) {
	if(0)cout << "Update packet\n";
	p->SetColor(Qt::red);
      }
      else if(e->opcode == 3) {
	if(0)cout << "Query packet\n";
	p->SetColor(Qt::green);
      }
      if(e->opcode == 4) {
	if(0)cout << "Reply packet\n";
	p->SetColor(Qt::black);
      }
      else if(e->opcode == 5) {
	if(0)cout << "Hello packet\n";
	p->SetColor(Qt::blue);
      }

      if(e->mTxt == "EMsgAck") {
	if(0)cout << "Ack packet \n";
	p->SetColor(Qt::yellow);
      }
#else
      if(0)cout << "Unknown packet\n";
      Packet* p = new Packet();
#endif
      
      if(0)cout << "ucast():: e->opcode is: " << e->opcode << std::endl;
      p->PushPDU(e);
      IPV4ReqInfo r(i, IPADDR_NONE, 64, 88); // Info for l3
      IPV4::Instance()->DataRequest(myNode, p, &r);
    }
}

void EIGRP::uCast(EIGRPHeader* e, EMCast_t t, IPAddr_t to)
{ // Called when neighbor not known
  DEBUG0((cout << "Entering uCast(EIGRPHeader,EMCast_t,IPAddr_t)" << endl));
  Neighbor* n = GetNeighbor(to, nil);

  uCast(e, t, to, n);
}

void EIGRP::SendHello()
{ // Send the hello messages using unreliable multicast
   DEBUG0((cout << "Entering SendHello()" << endl));

  IPAddr_t myIP = 0;
  // do not set any flag, OpCode=5 for HELLO
  EIGRPHeader* e = new EIGRPHeader(MsgHello,2,(unsigned int)MsgHello,0x0000EFCF,0x00000000,0,0,1000);
  //e->helloHeader = new HelloHeader();
  //e->helloHeader->setHelloFields(1, myIP,EIGRP_DEFAULT_HOLD_TIME);
  HelloHeader* hello = new HelloHeader();
  hello->setHelloFields(1, myIP,EIGRP_DEFAULT_HOLD_TIME);
  e->helloHeader.push_back(hello);
  McastUnreliable(e);
  //McastReliable(h); // For testing, use reliable hellos
  
  
  // increment sent message counter for Hellos
  for (NeighborMap_t::iterator i_ = neighbors.begin();  i_ != neighbors.end();  ++i_)
    {
      if (i_->second->status == Neighbor::UP)  
        {
          Sent_Mesg_Counters.Hellos += 1;
          Increment_Sent_Hellos();
        }
    }
  
#ifdef SHOWHELLOPACKETS
  cout << "Sent_Mesg_Counters.Hellos = " << Sent_Mesg_Counters.Hellos << std::endl;
#endif

  if (pHelloEvent)
    { // Cancel any pending
      Scheduler::Cancel(pHelloEvent);
      delete pHelloEvent;
      pHelloEvent = nil;
    }
  // Schedule another hello at 'timeout' seconds in future
  pHelloEvent = new EIGRPEvent(EIGRPEvent::HELLO);
  Scheduler::Schedule(pHelloEvent, helloTimeout, this);
}


#ifndef WIN32
IPAddr_t EIGRP::GetIPAddress(Interface* interface)
#else
IPAddr_t EIGRP::GetIPAddress(Interface* interface_)
#endif
{ 
#ifdef DO_WE_NEED_THIS
  for (NeighborMap_t::iterator i = m_neighbors.begin();
       i != m_neighbors.end();  ++i)
    {
      if(i->second.m_if==interface) 
	return(i->second.m_myip);
    }
#endif
  return 0;
}



void EIGRP::DisplayNeighborTable()
{
  for (NeighborMap_t::iterator i = neighbors.begin(); i != neighbors.end();  ++i)
    {
      cout << "Neighbors, if " << i->second->iface
	   << " IP " << (string)IPAddr(i->second->myIP) << endl;
    }
}



// MODIFIED*
// check to see if an adjacency exists in the Adjacency table
AdjacencyEntry* EIGRP::FindAdj(IPAddr_t ip)
{
  //DEBUG0((cout << "Entering FindAdj()" << endl));

  int found_=0;  // not found yet

#ifdef SHOWADJACENCYSEARCH
  cout << "\nlooking for " << IPAddr::ToDotted(ip) << std::endl;
  cout << "Adjacency Table:\n";
  cout << "H_\tadjacency_\tinterface_\tholdtime_\tuptime_\t\tsrtt_\tRTO_\tQ_\tseq_no_\tlinkcost_\n";
#endif

  adjacencyEntryList_::iterator it;
  AdjacencyEntry *location, *j;

  double currentTime=  Simulator::instance->Now();

  for (it= AdjacencyTable.begin(); it != AdjacencyTable.end(); ++it) 
    {
      j = *it;

      j->uptime_= currentTime - j->upTimeStart_;

#ifdef SHOWADJACENCYSEARCH
      cout << j->H_ << "\t" << IPAddr::ToDotted(j->adjacency_) << "\t    " << j->interface_ << "\t\t    " << j->holdtime_ 
	   << "\t\t" << j->uptime_ << "\t" << j->srtt_ << "\t" << j->RTO_ << "\t" << j->Q_ << "\t" << j->seq_no_ 
	   << "\t" << j->linkcost_ << std::endl;
#endif      
      if(ip==j->adjacency_) 
	{ 
	  found_=1;
	  location=j;
	}
    }

  if (found_==0)
    {
#ifdef SHOWADJACENCYSEARCH      
      cout << "not found\n\n";
#endif
      return(NULL);
    }

#ifdef SHOWADJACENCYSEARCH
  cout << "FOUND " << IPAddr::ToDotted(ip) << "\n\n";
#endif
  
  return location;    
}
// FindAdj()



// MODIFIED*
// Called whenever a new adjacency comes up.
// add a new adjacency to the Adjacency table, schedule a new hold event for that new adjacency
// called in unison with addtoQORSTable()
AdjacencyEntry* EIGRP::AddAdjacency_ScheduleHold(IPAddr_t ip, Interface* if_, int holdTime, double now, int srtt)
{
  //DEBUG0((cout << "Entering AddAdjacency_ScheduleHold()" << endl));

  AdjacencyEntry *trav,*adj;
  Ulong_t  max;

  double currentTime= Simulator::instance->Now();

  // determine in what order this adjacency was discovered
  adjacencyEntryList_::iterator traverse = AdjacencyTable.begin();
  if(traverse==AdjacencyTable.end()) max=0;
  else max = (*traverse)->H_;

  for (adjacencyEntryList_::iterator it = AdjacencyTable.begin();
       it != AdjacencyTable.end();  ++it)
    {
      trav = *it;
      trav->uptime_= currentTime - trav->upTimeStart_;
      if(max < trav->H_) max=trav->H_;
    }
  max++;

  // add the new adjacency to the AdjacencyTable
  EIGRPEvent* evnt_=new EIGRPEvent(EIGRPEvent::HOLD_EVENT);
  adj = new AdjacencyEntry( max,ip, if_ ,holdTime, now ,srtt, RTO , 0 , 0 , 0, currentTime,evnt_);
  evnt_->adj = adj;
  AdjacencyTable.push_back(adj);
  //AdjacencyTable.push_back( AdjacencyEntry( max,ip, if_ ,holdTime, (unsigned long) now ,srtt, RTO , 0 , 0 , 0, currentTime,evnt_));
  cout << "\nADDED AN ENTRY TO ADJACENCY TABLE,";

  cout << "scheduling a new hold event at " << currentTime << ". holdTime=" << holdTime << std::endl;

  // use holdTime, not EIGRP_DEFAULT_HOLD_TIME
  holdTimer.Schedule( holdTime, (EIGRPEvent*) evnt_);
  return(adj);
}
// AddAdjacency_ScheduleHold()




// MODIFIED*
// find the best_metric (minimum distance) for all destinations
// depending on the FC used, determine the FD for specified(if dest!=0) / all destinations
// returns the new best_metric for the specified destination, else Infinity
// dont update the FD if upateFD=0

Metric_t EIGRP::evaluate_best_metric_and_FD (int updateFD, IPAddr_t dest)
{
  //DEBUG0((cout << "Entering evaluate_best_metric_and_FD()" << endl));

  Metric_t new_best_metric=Infinity, best_metric_dest=Infinity;
  struct feasibles *itr;

  // //displaytopologytable();

  for (topologyEntryList_::iterator traverse = TopologyTable.begin(); traverse != TopologyTable.end(); ++traverse)
    {
      if(!dest || (dest&&traverse->destin==dest) )
	{
	  itr= traverse->pointer;
	  if(itr==NULL) { cout << "\nDAMMIT 2\n\n"; exit(1);}

	  // initialize to the metric contained in the last entry
	  while(itr->fee!=NULL) itr=itr->fee; 
	  new_best_metric=itr->metric;

	  itr= traverse->pointer;
	  while(itr->fee!=NULL)
	    {
	      itr=itr->fee;

	      if(itr->metric < new_best_metric)  new_best_metric=itr->metric;
	      if(itr->RD < itr->min_RD)  itr->min_RD=itr->RD;
	    }

	  traverse->best_metric= new_best_metric;

	  if(dest && traverse->destin==dest) best_metric_dest=new_best_metric;


	  // update FD only if in PASSIVE state
	  if(updateFD==1 && (int)traverse->state==0)
	    {
	      if(new_best_metric < traverse->min_metric_till_now)  traverse->min_metric_till_now = new_best_metric;

#if defined(DIC)
	      traverse->FD= traverse->min_metric_till_now;
#elif defined(CSC)
	      //todo....test this
	      // you have to update the successor first
	      traverse->FD= traverse->current_successor->min_RD;
#elif defined(SNC)
	      traverse->FD= traverse->min_metric_till_now;
#else 
	      cout << "Bogus\n";          
#endif
	    }

	  new_best_metric=Infinity;

	} //if()
    } // for()


  // //displaytopologytable();

  return(best_metric_dest);

} 
// evaluate_best_metric_and_FD()




// MODIFIED*
// if modifyFD==1 the FD gets modified.
int EIGRP::FC_check(IPAddr_t dest, Metric_t rep_dist,Metric_t metr, int modifyFD)
{
  DEBUG0((cout << "Entering FC_check()" << endl));

  int pass_fail=0; /* initialize to fail */
 
  if(modifyFD==1) evaluate_best_metric_and_FD(1);

  for (topologyEntryList_::iterator traverse = TopologyTable.begin(); traverse != TopologyTable.end(); ++traverse)
    {
      if(dest==traverse->destin)
	{

#if defined(SNC)
	  if(rep_dist < traverse->FD)   pass_fail=1;
	  // extreme cases
	  if(traverse->FD==0 && rep_dist==0) pass_fail=1;
	  if(traverse->FD==Infinity && rep_dist==Infinity) pass_fail=1;

#elif defined(DIC)
	  if(metr <= traverse->FD)      pass_fail=1;

#elif defined(CSC)      
	  if(rep_dist <= traverse->FD)  pass_fail=1;
#endif

	} //if()

    } //for()


  return (pass_fail);
}
// FC_check()




// MODIFIED*
// checks if all RS flags=0 for a destination.
// even if one RS=1, then destination is ACTIVE and is expecting more replies.
int EIGRP::checkRSflags(IPAddr_t dest)
{
  //DEBUG0((cout << "Entering checkRSflags()" << endl));

  int r=0;
  struct neighbors_ *itr;

  // displayQORStable();

  for (QORS_EntryList_::iterator traverse = QORSTable.begin(); traverse != QORSTable.end(); ++traverse)
    {
      itr=traverse->ptr_;      
      if( traverse->dest==dest )
	{
	  while(itr!=NULL)
	    {
	      if(itr->RSflag==1) r=1;
	      itr=itr->nb;
	    }

	  if(r==1) 
	    {
	      // r=1 means ACTIVE
	      cout << "Dest " << IPAddr::ToDotted(dest) << " is still ACTIVE and expecting replies.\n";
	    }
	}

    } // for()

  return r;
}
// checkRSflags()




// MODIFIED*
// flush the TT of entries for which FC check isnt satisfied anymore.
void EIGRP::flush_TT()
{
  //DEBUG0((cout << "Entering flush_TT()" << endl));
  
  struct feasibles *prev,*curr,*dummy;

  // //displaytopologytable();

  for (topologyEntryList_::iterator traverse = TopologyTable.begin(); traverse != TopologyTable.end(); ++traverse)
    {
      dummy=traverse->pointer;
      if(dummy==NULL) { cout << "\nDAMMIT 4\n\n"; exit(1);}

      prev=dummy;
      curr=prev->fee;

      while(curr!=NULL)
	{
	  // cout << "flush entry:: dest=" << IPAddr::ToDotted(traverse->destin) << ", RD=" << curr->RD << ", metric=" << curr->metric;
	  if (FC_check(traverse->destin, curr->RD, curr->metric)==0) 
	    {
	      if ( !( (curr->RD==curr->metric) && (curr->RD==traverse->FD) && (traverse->FD==0 || traverse->FD==Infinity)) )
		{
		  // cout << "WILL flush entry:: dest=" << IPAddr::ToDotted(traverse->destin) << ", RD=" << curr->RD << ", metric=" << curr->metric;
		  // first FS (for that destination) is the adjacency to be deleted        

		  prev->fee=curr->fee;
		  free(curr);
		  curr=prev->fee;
		}
	      else 
		{
		  prev=curr;
		  curr=curr->fee;
		}              
	    }

	  else 
	    {
	      prev=curr;
	      curr=curr->fee;
	    }
	}
      // while

    } // for()
  // end::flush the TT of entries for which ReportedDistance >= FD

  TT_to_RT();
  // //displaytopologytable();

} 
// flush_TT()



// MODIFIED*
// uses ModifyRoutingTable()
// Completely wipe out the routing table, create RT afresh from the TT
// sample the TT, take successors and place into RT
void EIGRP::TT_to_RT()
{
  //DEBUG0((cout << "Entering TT_to_RT()" << endl));

  //displaytopologytable();
  // delete all entries from the routing table
  RoutingTable.clear();
  
  //delete all entries from FIB
  RoutingEIGRP* routingObject = (RoutingEIGRP*)myNode->GetRouting();  // Returns the routing object of the node
  if(routingObject)
    routingObject->Clear();

  NodeWeightVec_t NodeWeightVec;

  // take successors from the TT, put into the RT
  for (topologyEntryList_::iterator traverse = TopologyTable.begin(); traverse != TopologyTable.end(); ++traverse)
    {

      ModifyRoutingTable(traverse->destin,traverse->significant,traverse->current_successor->metric,traverse->current_successor->feas_succ);
    
      if(routingObject)
	{
	  // get all neighbors for this eigrp router
	  myNode->Neighbors(NodeWeightVec);
	  for(NodeWeightVec_t::iterator k = NodeWeightVec.begin(); k != NodeWeightVec.end(); ++k)
	    {
	      // get the p2p link for the specified successor (nexthop)
	      Link* PeerLink = myNode->GetLink((*k).node);
	      IPAddr_t PeerIP = PeerLink->NodePeerIP((*k).node);
	     
	      if(PeerIP == traverse->current_successor->feas_succ)
		{
		  /* fib->Add(targetip,mask,inteface,nexthop) */
		  routingObject->Add(traverse->destin,traverse->significant,
                                     (Interface*)((*k).iface),
                                     traverse->current_successor->feas_succ);
		}
		
	    }
	}
    } 
  // for()

  //displayroutingtable();
  //displayfibtable();
}
//TT_to_RT()




// MODIFIED*
// add a new adjacency to the QORS table, for each and every destination in QORS table
// create a new active timer for each neighbor
// called in unison with AddAdjacency_ScheduleHold()
void EIGRP::addtoQORSTable (IPAddr_t ip)
{
  DEBUG0((cout << "Entering addtoQORSTable()" << endl));
    
  QORS_EntryList_::iterator i_=QORSTable.begin();
  struct neighbors_ *n1_,*nn_;

  while(i_ !=QORSTable.end())
    {
      n1_=i_->ptr_;
      if(n1_->RSflag==INVALID_)
	{
	  n1_->neigh=ip;
	  n1_->RSflag=0;
	  n1_->needToReply=0;

	  if(0)cout << "create a new active timer for dest=" << IPAddr::ToDotted(i_->dest) << ", neigh=" << IPAddr::ToDotted(ip) << std::endl;
	  n1_->activeEvent=new EIGRPEvent(EIGRPEvent::ACTIVE_EVENT);
	  n1_->activeEvent->n_ = n1_;
	  n1_->activeEvent->dest = i_->dest;
	  n1_->nb=NULL;
	}
      else
	{
	  // traverse to the last adjacency
	  while(n1_->nb!=NULL)
	    {
	      n1_=n1_->nb;
	    }
	  nn_= (struct neighbors_ *)malloc(sizeof(*nn_));
	  nn_->neigh=ip;
	  nn_->RSflag=0;
	  nn_->needToReply=0;

	  if(0)cout << "create a new active timer for dest=" << IPAddr::ToDotted(i_->dest) << ", neigh=" << IPAddr::ToDotted(ip) << std::endl;
	  nn_->activeEvent=new EIGRPEvent(EIGRPEvent::ACTIVE_EVENT);
	  nn_->activeEvent->n_ = nn_;
	  nn_->activeEvent->dest = i_->dest;
	  nn_->nb=NULL;                          
	  n1_->nb=nn_;
	}

      i_++;                      

    } // while

  //displayQORStable();
}
//addtoQORSTable()



//MODIFIED*
// if dest doesnt exist, adds new dest and feas_succ, sends out updates
// if dest exists, but feas_succ doesnt, adds feas_succ (if FC satisfied). If metric via successor changes, sends out updates
// if dest exists, and feas_succ exists, update feas_succ. If metric via successor changes, sends out updates
// if dest exists, and after updating the entry to TT, cant find a successor, then send out Queries.
// flush TT to remove entries that no longer satisfy the FC


void EIGRP::UpdateTopologyTable( IPAddr_t dest, int prefix , Metric_t reportedDistance, Metric_t metric, IPAddr_t feas,
				 int appendupdates, int appendqueries, int changestate, int inputevent,
				 EIGRPHeader *update_mg, int *sendUpd, EIGRPHeader *query_mg, int *sendQuer )
{

  if(0)cout << "\nUpdateTopologyTable(dest= " << IPAddr::ToDotted(dest) << ",prefix= " << prefix << ", RD=" << reportedDistance
	    << ", metric= " << metric << ", feas= " << IPAddr::ToDotted(feas) << std::endl;
  if(dest)
    {
      cout << "dest=" << IPAddr::ToDotted(dest) << "  prefix=" << prefix << std::endl;
    }
  else 
    {
      cout << "All destinations\n";
    }

  EIGRPInputEvent_t INPUT_EVENT = (EIGRPInputEvent_t) inputevent;
  EIGRP_FC_Event_t FC_EVENT;

  int MustSendUpdates=0, MustSendQueries=0;
  unsigned long entryprefix,pflength;
  int flag_dest_exists=0;
  unsigned long mask,shift,lmp;

  IPAddr_t myIP,yourIP;
  int flag_fs_found=0;

  topologyEntryList_::iterator traversal,ahead;
  unsigned long xxx1 = 0xFFFFFFFF;
  mask=0;
  shift=32-prefix;
  mask=xxx1<<shift;
  lmp=dest&(int)mask;


  // sanity check #1
  // check if the nexthop is myself !! That would be a route Loop , and you just dont add that entry !!
  for (NeighborMap_t::iterator i = neighbors.begin();  i != neighbors.end();  ++i)
    {
      myIP=i->second->myIP;
      if(0)cout << "myIP: " << IPAddr::ToDotted(myIP);
      yourIP=i->first;
      if(0)cout << "yourIP: " << IPAddr::ToDotted(yourIP) << std::endl;
      if(myIP==feas)
	{
	  cout << "BIG FAILURE\n Will create a route loop\n";
	  exit(1);
	}
    }


  // flag tells you if destination already exists in the Topology Table
  flag_dest_exists=0;

  // first check to see if the destination "dest" already exists in the topology table 
  traversal=TopologyTable.begin();    // initialization of traversal
  ahead=traversal;

  while (traversal!=TopologyTable.end())
    { 
      ahead=traversal;
      pflength=ahead->significant;
      shift=32-pflength;
      mask=xxx1<<shift;
      entryprefix=(ahead->destin)&(int)mask;
      //  cout << "\npf " << entryprefix << IPAddr::ToDotted(entryprefix) << std::endl;

      if((entryprefix==lmp)) //&& (ahead->significant==prefix))
	{
	  cout << "\n yes ! found the dest. in TT.\n";
	  flag_dest_exists=1;
	  break;
	}
      ++traversal;
    }


  if(flag_dest_exists)
    {
      // destination exists in Topology table. ahead points to the dest.
      cout << "Destination found in topology table\n";


      // search for exact record (feas_succ) in the TT
      // if record found, simply update it with latest values.

      struct feasibles *itr;
      flag_fs_found=0;

      itr=ahead->pointer;
      while (itr->fee!=NULL)
	{
	  itr=itr->fee;
	  if(itr->feas_succ==feas)
	    {
	      cout << "found the SAME feas_succ. Just replace with new values, since latest !!\n";

	      flag_fs_found=1;

	      itr->feas_succ=feas;
	      itr->RD=reportedDistance;
	      if(reportedDistance < itr->min_RD) itr->min_RD=reportedDistance;
	      itr->metric=metric;

	      // updating the best metric, taking into consideration this new entry
	      evaluate_best_metric_and_FD(1, ahead->destin);
	    }
	}


      if(flag_fs_found==0)
	{
	  // feas_succ not found
	  // FC_check
	  int check = FC_check(lmp,reportedDistance,metric);
	  if( check==1 ) 
	    { 
	      cout << "passed\n";
	    }
	  else
	    {
	      // FC Check Failed
	      cout << "failed. Wont add this entry.\n";
	      return;
	    }

	  // FC check passed or bypassed. Entry has to be added to the destination.

	  // destination exists
	  // feas_succ not found. adding a new record...
	  struct feasibles *z2;
	  z2= (d1*)malloc(sizeof(*z2));
	  z2->feas_succ=feas;
	  z2->RD=reportedDistance;
	  z2->min_RD=reportedDistance;
	  z2->metric=metric;

	  itr=ahead->pointer;
	  while(itr->fee!=NULL) itr=itr->fee;
	  itr->fee=z2;           // (since we know itr->fee=NULL)
	  z2->fee=NULL;          // place z2 last

	  // updating the new FD, taking into consideration this new entry
	  evaluate_best_metric_and_FD(1,lmp);
	  flush_TT();
	}
      // if(flag_fs_found==0)


      struct feasibles *new_succ= findSuccessor(lmp,1);

      if((int)ahead->state ==0)
	{
	  // PASSIVE state

	  if(new_succ!= NULL)
	    {
	      // FC was satisfied
	      FC_EVENT = FC_SATISFIED;

	      // The metric changed via the successor
	      // i.e. updated distance is different from previous distance

	      if(new_succ->metric != ahead->current_successor->metric)
		{
		  if(appendupdates==1)
		    {
		      // send out an update
		      UpdateHeader* update = new UpdateHeader();
		      update->setUpdateFields(0x0102, new_succ->feas_succ,0,0,1500, new_succ->metric,0,0,0, ahead->significant, lmp);
		      update_mg->updateHeader.push_back(update);
		      MustSendUpdates=1;
		      *sendUpd=1;
		    }
		}

	      ahead->current_successor = new_succ;
	    }
	  // if(new_succ!=NULL)

	  else
	    {
	      // new_succ==NULL

	      if(appendqueries==1)
		{
		  // FC wasnt satisfied.
		  FC_EVENT = FC_NOT_SATISFIED;

		  // new_succ==NULL.......findSuccessor failed.
		  // cannot find a feasible successor. Keep the current successor.
		  // Send out Queries
		  // Update the FD....depending on the FC mode used.

		  // sanity check.
		  if( feas != ahead->current_successor->feas_succ) { cout << "Something's Wrong.\n"; exit(1); }

		  cout << "Couldnt find a feasible successor..Now updating the FD and RD.\n";

		  // The RD for that destination is the metric via the current successor.

		  ahead->current_successor->RD =  reportedDistance;
		  if(ahead->current_successor->RD < ahead->current_successor->min_RD)  ahead->current_successor->min_RD=ahead->current_successor->RD;
		  ahead->current_successor->metric =  metric;


		  // update the FD for the destination
#if defined(DIC)
		  ahead->min_metric_till_now = ahead->current_successor->metric;
		  ahead->FD = ahead->current_successor->metric;

#elif defined(CSC)
		  // todo...test this
		  ahead->FD=ahead->current_successor->min_RD;

#elif defined(SNC)
		  ahead->min_metric_till_now = ahead->current_successor->metric;
		  ahead->FD = ahead->current_successor->metric;
#else           
		  cout << "Bogus\n";
#endif

		  cout << "Now see the changes to the TT\n";
		  //displaytopologytable();

		  cout << "Commencing a DIFFUSING COMPUTATION. Sending Queries and going active\n";
		  // Become active and send Queries, set QORS flags
		  QueryHeader* query = new QueryHeader();
		  query->setQueryFields(0x0103, ahead->current_successor->metric, ahead->destin);
		  query_mg->queryHeader.push_back(query);
		  MustSendQueries=1;
		  *sendQuer=1;

		  // set RS flags of all adjacencies of the destination
		  setRSflags(1,(IPAddr_t)ahead->destin);

		  schedule_active_timer(ahead->destin);

		  FC_EVENT = FC_NOT_SATISFIED;
		  if(changestate==1) determineState((int)INPUT_EVENT,(int)FC_EVENT, (IPAddr_t) ahead->destin);
		}
	    }

	}
      // if (ahead->state==0)


      else if (ahead->state!=0)
	{
	  // ACTIVE state
	  // Caution: Cannot modify the router's FD,RD or Successor if dest is active.
	  cout << "Destination is in ACTIVE state\n";

	  // FC_EVENT is irrelevant in this case (refer to state diagram)
	  FC_EVENT = FC_SATISFIED;

	  // if in ACTIVE1 or ACTIVE3  and its a   LINK_DISTANCE INCREASE   or   RECEIVE_UPDATE_DISTANCE_INCREASE   event, must transition state
	  if(changestate==1) determineState((int)INPUT_EVENT,(int)FC_EVENT, (IPAddr_t) ahead->destin);

	}


    } // if flag_dest_exists()



  //  destination not there in the topology table.
  //  add a new destination (and record) to Topology Table
  //  Also add the new destination to the QORSTable
  else
    {
      cout << "adding a new destination to the TT\n";
      struct feasibles *z2, *z3, *curr_succ;
      Metric_t FD_;

#ifdef REJECTUPDATESWITHINFINITEMETRIC
      if (reportedDistance >= Infinity) 
	{
	  cout << "Ignoring the Update..Reported Distance = Infinity !\n"; 
	  goto out;
	}
#endif

      z2= (struct feasibles *)malloc(sizeof(*z2));
      curr_succ= (struct feasibles *)malloc(sizeof(*curr_succ));

      TopologyEntry z;

#if defined(DIC)
      FD_= metric;           // min metric till now
#elif defined(CSC)
      FD_=reportedDistance;  // min RD till now
#elif defined(SNC)
      FD_= metric;           // min metric till now
#else   
      cout << "Bogus\n";          
#endif

      //  z.setTopologyEntry( traverse->destin, traverse->significant, traverse->state, traverse->FD, traverse->best_metric, traverse->min_metric_till_now, curr_succ, fs);
      z.setTopologyEntry( lmp, prefix, 0 /*passive state*/, FD_, metric/*best_metric*/, metric /*min_metric_till_now*/, curr_succ, z2);

      curr_succ->feas_succ=feas;
      curr_succ->RD=reportedDistance;
      curr_succ->min_RD=reportedDistance;
      curr_succ->metric=metric;
      curr_succ->fee=NULL;

      TopologyTable.push_back(z);


      // following entry doesnt show up in the topology table
      z2->feas_succ=Infinity;
      z2->RD=Infinity;
      z2->min_RD=Infinity;
      z2->metric=Infinity;

      //  add a new record
      z3= (struct feasibles*)malloc(sizeof(*z3));
      z2->fee=z3;

      z3->feas_succ=feas;
      z3->RD=reportedDistance;
      z3->min_RD=reportedDistance;
      z3->metric=metric;
      z3->fee=NULL;


      if(appendupdates==1)
	{
	  UpdateHeader* update = new UpdateHeader();
	  update->setUpdateFields(0x0102, curr_succ->feas_succ,0,0,1500, metric,0,0,0, ahead->significant, lmp);
	  update_mg->updateHeader.push_back(update);
	  MustSendUpdates=1;
	  *sendUpd=1;
	}

      cout << "adding a new destination to the QORS Table\n";
      QueryOrigin_ReplyStatus_Entry qors;
      struct neighbors_ *n2_,*nn_;
      n2_= (struct neighbors_ *)malloc(sizeof(*n2_));

      qors.SetQORSEntry(lmp,1,0,n2_);
      QORSTable.push_back(qors);

      if(AdjacencyTable.size()==0)
	{
	  n2_->neigh=0;
	  n2_->RSflag=INVALID_;
	  n2_->needToReply=0;
	  n2_->activeEvent=NULL;
	  n2_->nb=NULL;
	}

      // add all adjacencies to the QORS destination entry
      adjacencyEntryList_::iterator it = AdjacencyTable.begin();
      while(it != AdjacencyTable.end())
	{
	  AdjacencyEntry* traverse = *it;
	  n2_->neigh= traverse->adjacency_;
	  n2_->RSflag =0;
	  n2_->needToReply=0;

	  if(0)cout << "create a new active timer for dest=" << IPAddr::ToDotted(lmp) << " neigh=" << IPAddr::ToDotted(n2_->neigh) << std::endl;
	  n2_->activeEvent=new EIGRPEvent(EIGRPEvent::ACTIVE_EVENT);
	  n2_->activeEvent->n_ = n2_;
	  //          n2_->activeEvent->n_->neigh = n2_->neigh;
	  //          n2_->activeEvent->n_->RSflag = n2_->RSflag;

	  n2_->activeEvent->dest = lmp;
	  n2_->nb=NULL;

	  ++it;
	  if(it!=AdjacencyTable.end())
	    {
	      nn_= (struct neighbors_ *)malloc(sizeof(*nn_));
	      n2_->nb=nn_;
	      n2_=nn_;
	    }
	}

    }
  // else flag_dest_exists()


 out:

  // FD gets updated only if the destination is PASSIVE
  evaluate_best_metric_and_FD(1);
  flush_TT();
  TT_to_RT();

  //displaytopologytable();
  displayQORStable();
  // getchar();

}
//UpdateTopologyTable()




// look for the (single) destination specified
// if dest found and "newmetric" is different from the FD stored in the RT, delete all successors, 
// add new record (successor) for the dest, update its FD
// else if dest found and "newmetric" is the same as FD, add a new successor (only if it doesn't already exist)
// then check for route aggregation::NOT WORKING?
// ELSE if dest not found, add new dest to the Route Table

void EIGRP::ModifyRoutingTable(IPAddr_t dest,unsigned long prefix,Metric_t newmetric,IPAddr_t fromwhom)
{ 
  //DEBUG0((cout << "Entering ModifyRoutingTable()" << endl));
  // cout << "\nMODIFYROUTINGTABLE:: " << IPAddr::ToDotted(dest) << "," << prefix << "," << newmetric << "," << IPAddr::ToDotted(fromwhom) << std::endl;	

  int shift,entryprefix,pflength,lmp,flag=0,succ_found=0;
  unsigned long xxx1 = 0xFFFFFFFF,mask;
  struct successors *prev_,*next_,*prev2_;

  if(prefix>32) prefix=32;
  if(prefix<0) prefix=0;

  // find the best match for the destination
  mask=0;
  shift=32-prefix;
  mask=xxx1<<shift;
  lmp=dest&(int)mask;

  routeEntryList_::iterator traverse,forward,i_;
  traverse=RoutingTable.begin();    // initialization of traversal
  forward=traverse;

  while (traverse !=RoutingTable.end())
    {
      forward=traverse;
      pflength=forward->prefixlength;
      shift=32-pflength;
      mask=xxx1<<shift;
      entryprefix=(forward->destIPAddress)&(int)mask;

      // found the destination
      if((entryprefix==lmp) && (forward->prefixlength==prefix))
	{ 
	  flag=1;

	  if(forward->metric!=newmetric) 
	    {
	      // then delete all successors
	      prev_=forward->pointer_;

	      //try,modified
	      if(prev_!=NULL) 
		{
		  next_=prev_->su;
		  free(prev_);
		  forward->pointer_=NULL;
		  while(next_ !=NULL)
		    {
		      prev_=next_;
		      next_=next_->su;
		      free(prev_);
		    }
		}

	      forward->metric=newmetric;  // update FD
	      // add new record
	      struct successors *z;
	      z= (struct successors*)malloc(sizeof(*z));
	      forward->pointer_=z;          
	      z->nexthopIPAddress=fromwhom;
	      z->su=NULL;
	    }

	  else // (forward->metric==newmetric)
	    {
	      prev_=forward->pointer_;
	      prev2_=prev_;
	      succ_found=0;

	      while(prev_ !=NULL)
		{
		  if(prev_->nexthopIPAddress==fromwhom) succ_found=1;
		  prev2_=prev_;
		  prev_=prev_->su;
		}              

	      if(!succ_found)
		{
		  //  add new record
		  struct successors *z1;
		  z1= (struct successors*)malloc(sizeof(*z1));
		  prev2_->su=z1;
		  z1->nexthopIPAddress=fromwhom;
		  z1->su=NULL;
		}
	    } 
	  break;
	}


      ++traverse;

    }   // while


  if(!flag)  // destination wasnt found, add a new destination
    {
      // cout << "\nentry not found. adding new entry.\n";	
      for ( i_ = RoutingTable.begin(); i_ != RoutingTable.end(); ++i_) if(i_->prefixlength <=prefix) break;

      RouteEntry q;
      // or use -> notation 
      q.destIPAddress=dest;       // originally lmp later changed to dest
      q.prefixlength=prefix;
      q.metric=newmetric;

      struct successors *z2;
      z2= (struct successors*)malloc(sizeof(*z2));
      q.pointer_=z2;
      z2->nexthopIPAddress=fromwhom;
      z2->su=NULL;
      RoutingTable.insert(i_,1,q);     // i_->next=q;
    }

} 
// ModifyRoutingTable()




// MODIFIED*  (new version)
// does NOT modify the FD
// what about two identical successors? should I reply with both, or find a scheme to reply back with just one ?
// GOOD SCHEME: report just one: the guy with the least RD
// If they have the same RD's also (in addition to same metrics), then report the guy with the min IP address.
// This makes it deterministic.
// if modifyFD is set to 0 , FD wont be changed.

struct feasibles* EIGRP::findSuccessor(IPAddr_t dest, int modifyFD, int exclude, IPAddr_t excludeFromComputationofSuccessor)
{
  //DEBUG0((cout << "Entering findSuccessor" << endl));

  int found=0; /* initialize to 'not found'*/
  int someonePassed=0; /*initialize to no one passed the FC as yet */
  successor_list_t SuccList;
  SuccList.clear();


  Metric_t best_metric_among_FS=Infinity;

  struct feasibles *itr, *new_succ=NULL;
  new_succ= (struct feasibles *)malloc(sizeof(*new_succ));

  // must keep the best_metric up to date.
  if(modifyFD==1)  evaluate_best_metric_and_FD(1);
  else evaluate_best_metric_and_FD(0);


  for (topologyEntryList_::iterator traverse = TopologyTable.begin(); traverse != TopologyTable.end(); ++traverse)
    {
      if(dest==traverse->destin)
	{
	  itr= traverse->pointer;          
	  while(itr->fee!=NULL)
	    {
	      itr=itr->fee;
	      if( (exclude==0) || ( (exclude==1) && (itr->feas_succ!=excludeFromComputationofSuccessor) ) )
		{
		  if(FC_check(dest, itr->RD, itr->metric)==1)
		    {
		      // passed
		      someonePassed=1;
		      if(itr->metric <= best_metric_among_FS)  best_metric_among_FS=itr->metric;
		    }
		}

	    }
	  // while

	  // sanity check
	  if(someonePassed==1)
	    {
	      // todo...this is correct
	      /*
		if(best_metric_among_FS != traverse->best_metric) 
		{ cout << "Is this possible ? best metric=" << best_metric_among_FS << ", traverse->bestmetric=" << traverse->best_metric << std::endl; exit(1); }
	      */

	      itr= traverse->pointer;
	      while(itr->fee!=NULL)
		{
		  itr=itr->fee;
		  if( (exclude==0) || ( (exclude==1) && (itr->feas_succ!=excludeFromComputationofSuccessor) ) )
		    {
		      if(itr->metric==best_metric_among_FS)
			{
			  // found one (or more) successors.
			  found=1;
			  SuccList.push_back(*itr);
			}
		    }
		}
	      // while
	    }


	  if(found)
	    {
	      // there's at least one successor in SuccList
	      successor_list_t::iterator trav = SuccList.begin();
	      new_succ->feas_succ=trav->feas_succ;
	      new_succ->RD=trav->RD;
	      new_succ->min_RD=trav->min_RD;
	      new_succ->metric=trav->metric;
	      new_succ->fee=NULL;

	      for (trav = SuccList.begin(); trav != SuccList.end(); ++trav)
		{
		  if( (trav->RD < new_succ->RD) || ((trav->RD == new_succ->RD)&&((int)trav->feas_succ  < (int)new_succ->feas_succ)) )
		    {
		      new_succ->feas_succ=trav->feas_succ;
		      new_succ->RD=trav->RD;
		      new_succ->min_RD=trav->min_RD;
		      new_succ->metric=trav->metric;
		      new_succ->fee=NULL;
		    }

		}// for()

	    } //if()

	  else 
	    {
	      // not found
	      new_succ=NULL;
	    }



	} //if()

    } //for()

  return (new_succ);
}
// findSuccessor()





// MODIFIED*
void EIGRP::InitialExchange (IPAddr_t to, int init)
{
  DEBUG0((cout << "InitialExchange()" << endl));
  unsigned long destinations=0;
  unsigned long flag_;

  if(init==0) flag_=0x00000000;
  else if(init==1) flag_=0x00000001;
  else { cout << "error\n"; exit(1); }


  // Opcode=1 for "Update" , set the INIT flag, it indicates "Initial Topology Table Exchange"
  // INIT flag must only be set in the first Update packet
  EIGRPHeader* e = new EIGRPHeader(MsgUpdate,2,(unsigned int)MsgUpdate,0x0000EFCF,flag_,0,0,1000);

  if(0)cout << "****************************************************************************************************************************\n";
  if(0)cout << "Sending Topology Table Update\n...";

  for (topologyEntryList_::iterator traverse = TopologyTable.begin(); traverse != TopologyTable.end(); ++traverse)
    {
      destinations++;
      if(0)cout << "DEST=" << IPAddr::ToDotted(traverse->destin) << "\t\t" << std::endl;
      if(0)cout << "\t\t\t\t\t\t\tm=" <<  traverse->current_successor->RD << " \t\t " << traverse->current_successor->metric
		<< " \t\t " << IPAddr::ToDotted(traverse->current_successor->feas_succ);

      UpdateHeader* update = new UpdateHeader();
      update->setUpdateFields(0x0102, traverse->current_successor->feas_succ,0,0,1500, traverse->current_successor->metric,0,0,0,
			      traverse->significant, traverse->destin);
      e->updateHeader.push_back(update);
    }

  if(destinations)
    {
      cout << "Sent Update Message as a Reliable Unicast to " << IPAddr::ToDotted(to) << std::endl;
      UcastReliable(e,to);
      Sent_Mesg_Counters.Updates += 1;
      Increment_Sent_Updates(1);
      if(0)cout << "Sent_Mesg_Counters.Updates = " << Sent_Mesg_Counters.Updates << std::endl;
    }
  else 
    {
      cout << "TT empty. Didnt send TT though INIT flag was set" << std::endl;
    }
  if(0)cout << "Clearing the packet." << std::endl;
  if(0)cout << "****************************************************************************************************************************" << std::endl;
}
// InitialExchange()



// MODIFIED*
void EIGRP::displayroutingtable()
{ 
#ifndef SHOWROUTINGTABLE
  return;
#endif

  int destinations=0,entries=0;
  unsigned long tempo,tempo2;
  struct successors *itr;

  cout << "\n\tD E S T I N A T I O N\t\tPREFIX\t\tMETRIC\t\t\tNEXTHOP/SUCCESSOR(S)\n";
  cout << "-------------------------------------------------------------------------------------------------------------------------------\n";
  for (routeEntryList_::iterator traverse = RoutingTable.begin(); traverse != RoutingTable.end(); ++traverse)
    {
      destinations++;
      tempo=htonl(traverse->destIPAddress);
      cout << "\nd= " << traverse->destIPAddress << "    " << IPAddr::ToDotted(traverse->destIPAddress) << "\t " << traverse->prefixlength 
	   << " \t\tm=" << traverse->metric << std::endl;

      itr=traverse->pointer_;
      while(itr!=NULL)  // has some problem        
	{       
	  entries++;
	  tempo2=htonl(itr->nexthopIPAddress);
	  cout << "\t\t\t\t\t\t\t\t\t" << itr->nexthopIPAddress << "    " << IPAddr::ToDotted(itr->nexthopIPAddress) << std::endl;
	  itr=itr->su;            
	}
      cout << "-------------------------------------------------------------------------------------------------------------------------------" << std::endl;
    }


  // will need this 
  //cout << "\n\n" << entries << " Entries.\n[" << destinations << "*" << sizeof(t1) <<"]+[" << entries << "*" 
  //     << sizeof(d1) << "] = " << (entries*sizeof(d1))+(destinations*sizeof(t1)) << " Bytes total\n\n\n\n"; 
}
// displayroutingtable()




void EIGRP::displayfibtable()
{
  RoutingEIGRP* routingObject = (RoutingEIGRP*)myNode->GetRouting();
  if(routingObject)
    routingObject->displayFIB();
}
// displayfibtable()




// MODIFIED*
void EIGRP::displaytopologytable()
{   

#ifndef SHOWTOPOLOGYTABLE
  return;
#endif

  unsigned long tempo,tempo2;
  int destinations=0,entries=0;
  struct feasibles *itr;


  cout << "\n\n\t\t\t\t\tTOPOLOGY TABLE:\n\n\n";

  cout << "\nDESTINATION\tPREFIX\t mmtn\tFD\t\tmetric\t\tSTATE\t\tRep.Dist(RD)\tmin_RD\t\tMETRIC\t\tFEAS_SUCC.\n\n";
  cout << "---------------------------------------------------------------------------------------------------------------------------------------\n";


  for (topologyEntryList_::iterator traverse = TopologyTable.begin(); traverse != TopologyTable.end(); ++traverse)
    {
      destinations++;
      tempo=htonl(traverse->destin);

      char * st_;
      switch(traverse->state)
	{
	case 0:
	  st_="passive";
	  break; 
	case 1:
	  st_="ACTIVE0";
	  break;
	case 2:
	  st_="ACTIVE1";
	  break;
	case 3:
	  st_="ACTIVE2";
	  break;
	case 4:
	  st_="ACTIVE3";
	  break;
	default: st_="passive";
	}
      // cout << "\tFD= " << traverse->significant << "\t" << traverse->FD << "\t" << traverse->min_metric_till_now << "(" << st_,traverse->state << ")\n";

      if(traverse->significant !=Infinity)
	cout << traverse->significant;
      else cout << "INF.";

      if(traverse->min_metric_till_now != Infinity)
	cout << "\t" << traverse->min_metric_till_now;
      else cout << "\tINF.";

      if(traverse->FD != Infinity)
	cout << "\tFD= " << traverse->FD;
      else cout << "\tFD=INF.";

      if(traverse->best_metric != Infinity)
	cout << "\t\t" << traverse->best_metric;
      else cout << "\t\tINF.";

      cout << "\t\t" << st_ << "(" << traverse->state << ")\n";

      itr=traverse->pointer;
      if (!itr) break;
      while(itr->fee!=NULL)
	{              
	  itr=itr->fee;     
	  entries++;
	  tempo2=htonl(itr->feas_succ);
	  // if(traverse->FD==itr->metric) successor_flag=1; else successor_flag=0;
	  cout << "\t\t\t\t\t\t\t\t\tm=" << itr->RD << "\t" << itr->metric << "\t" << itr->feas_succ;
	  
	  if(itr->RD != Infinity)
	    cout << "\t\t\t\t\t\t\t\t\t" << itr->RD;
	  else cout << "\t\t\t\t\t\t\t\t\t    INF.";

	  if(itr->min_RD != Infinity)
	    cout << "\t" << itr->min_RD;
	  else cout << "\t    INF.";

	  if(itr->metric != Infinity)
	    cout << "\t" << itr->metric;
	  else cout << "\t    INF.";

	  if(0)cout << "\t" << itr->feas_succ;

	  //  if(successor_flag) cout << "(S)\n"; else cout << "\n";
	  cout << "\n";
	}


      if(traverse->current_successor ==NULL)  
	  cout << "Couldnt find a successor: FC not satisfied.";
      else
	  cout << "Successor for dest  " << traverse->destin << " = " << traverse->current_successor->feas_succ << ", RD=" << traverse->current_successor->RD
	       << ", min_RD=" << traverse->current_successor->min_RD << ", metric=" << traverse->current_successor->metric << std::endl;


      cout << "---------------------------------------------------------------------------------------------------------------------------------------\n";

    }

  // will need this
   //cout << itr->next << "\n\n" << entries << " Entries.\n[" << destinations << "*" << sizeof(t1) <<"]+[" << entries << "*" 
  //     << sizeof(d1) << "] = " << (entries*sizeof(d1))+(destinations*sizeof(t1)) << " Bytes total\n\n\n\n"; 
}
// displaytopologytable()




// MODIFIED*
void EIGRP::displayQORStable()
{ 

#ifndef SHOWQORSTABLE
  return;
#endif

  unsigned long tempo,tempo2;
  struct neighbors_ *itr;

  cout << "\n\tDEST.\t\tQueryOriginFlag(Oij)\t\tState\t\t\tAdjacency\t\tNeedToReply?\t\tReplyStatusFlag(r)\n";
  cout << "-------------------------------------------------------------------------------------------------------------------------------------\n";

  for (QORS_EntryList_::iterator traverse = QORSTable.begin(); traverse != QORSTable.end(); ++traverse)
    {
      tempo=htonl(traverse->dest);
      if(0)cout << "\nd= " << traverse->dest << "    " << IPAddr::ToDotted(traverse->dest) << "\t0ij=" << traverse->Oij << " \t\ts=" << traverse->state << std::endl;

      itr=traverse->ptr_;
      while(itr!=NULL)
	{       
	  tempo2=htonl(itr->neigh);
	  if(itr->RSflag!=INVALID_)
	    cout << "\t\t\t\t\t\t\t\t\t" << itr->neigh << "   " << IPAddr::ToDotted(itr->neigh) << "\t\tn=" << itr->needToReply << "\t\tRS=" << itr->RSflag
		 << std::endl;
	  else cout << "\t\t\t\t\t\t\t\t\t\tNO ADJACENCIES\n";
	  itr=itr->nb;
	}
      cout << "-------------------------------------------------------------------------------------------------------------------------------------\n"; 
    }

}
// displayQORSTable()





// MODIFIED*
void EIGRP::displayadjacencytable()
{

#ifndef SHOWADJACENCYTABLE
  return;
#endif

  adjacencyEntryList_::iterator it;
  cout << "Adjacency Table:\n";
  cout << "H_\tadjacency_\tinterface_\tholdtime_\tuptime_\t\tsrtt_\tRTO_\tQ_\tseq_no_\tlinkcost_\n";
  for (it= AdjacencyTable.begin(); it != AdjacencyTable.end(); ++it) 
    {   
      AdjacencyEntry* j = *it;
      double currentTime= Simulator::Now();
      j->uptime_= currentTime - j->upTimeStart_;
      if(0)cout << j->H_ << "\t" << j->adjacency_ << "\t    " << j->interface_ << "\t\t    " << j->holdtime_ << "\t\t" << j->uptime_
		<< "\t" << j->srtt_ << "\t" << j->RTO_ << "\t" << j->Q_ << "\t" << j->seq_no_ << "\t" << j->linkcost_ << std::endl;
    }
}
// displayadjacencytable()





//MODIFIED*
void EIGRP::displayreplybuffer()
{

#ifndef SHOWREPLYBUFFER
  return;
#endif

  cout << "\nDestination\t\tRD\t\tMetric\t\tNeighborHeardFrom\n";
  cout << "-------------------------------------------------------------------------------------------------------------------------------------\n";

  for (ReplyBufferEntryList_::iterator trav = ReplyBuffer.begin(); trav != ReplyBuffer.end(); ++trav)
    {
      cout << trav->destination << "\t\t" << trav->rd << "\t\t" << trav->finalmetric << "\t\t" << trav->heardfrom << std::endl;
      cout << "-------------------------------------------------------------------------------------------------------------------------------------\n"; 
    }

}
// displayreplybuffer()




// MODIFIED*
// remove multiple entries from QORSTable(same neighbor from all destinations)
// cancel last scheduled Active timers for all removed entries
// remove entries from the routing table and topology table
// send queries to all your neighbors, for reaching those destinations that are now unreachable
void EIGRP::remove_adjacency_from_RT_TT_QORS(IPAddr_t adjac,EIGRP_Timeout_t timeout_type)
{
  //DEBUG0((cout << "Entering remove_adjacency_from_RT_TT_QORS()" << endl));

  // erase the Routing Table. Use TT_to_RT() later
  RoutingTable.clear();


  // remove multiple entries from QORSTable (same neighbor from all destinations)
  // CAUTION: dont remove the destination, or there will be an inconsistency with the TT
  displayQORStable();
  QORS_EntryList_::iterator i_=QORSTable.begin();
  struct neighbors_ *nprev_,*ncurr_,*nnext_;
  
  while(i_ !=QORSTable.end())
    {
      nprev_=i_->ptr_;
      ncurr_=nprev_;
      nnext_=ncurr_->nb;
      if(ncurr_->neigh==adjac)
	{
	  // cancel any scheduled active timer
	  if(timeout_type==HOLDTIMEOUT)  activeTimer.Cancel(ncurr_->activeEvent);

	  i_->ptr_=nnext_;
	  free(nprev_);
	}
      else
	{
	  while(nnext_!=NULL)
	    {
	      if(nnext_->neigh==adjac)
		{
		  // cancel any scheduled active timer
		  if(timeout_type==HOLDTIMEOUT)  activeTimer.Cancel(nnext_->activeEvent);
          
		  ncurr_->nb=nnext_->nb;
		  free(nnext_);
		  break;
		}
	      nprev_=ncurr_;
	      ncurr_=nnext_;
	      nnext_=nnext_->nb;
	    }
	}
      // CAUTION: DO NOT remove a destination from the QORS table
      // just because a hold timer expired doesnt mean the router went dead 
      // the link could have gone down also !

      i_++;
      
    } // while

  displayQORStable();

  // delete all entries from the topology Table table that use the dead adjacency as a next hop.
  // any destinations in the topology table that use this entry, must be removed.
  // wrong. you must do these ALSO: find new successors !
  //  MakeTopologyEntryUnreachable((IPAddr_t) adjac);


  if(0)cout << "\n\nTOPOLOGY TABLE AGAIN\n";
  //displaytopologytable();

  TT_to_RT();
  cout << "Exiting  remove_adjacency_from_RT_TT_QORS()\n";
}
//remove_adjacency_from_RT_TT_QORS()





// MODIFIED*
// sets RS flags in the QORS table
// 3 choices:
// set RS flags of all adjacencies of all destinations
// set RS flags of all adjacencies of a particular destination
// set RS flag for a particular adjacency of a particular destination
void EIGRP::setRSflags(int rs_flag,IPAddr_t dest, IPAddr_t adjacency)
{
  //DEBUG0((cout << "Entering setRSflags()" << endl));

  struct neighbors_ *itr;

  if(0)cout << "\nSetRSflags()\n";
  if(!dest && !adjacency)
    {
      if(0)cout << "set RS flags of all adjacencies of all destinations\n";
      for (QORS_EntryList_::iterator traverse = QORSTable.begin(); traverse != QORSTable.end(); ++traverse)
	{
	  itr=traverse->ptr_;
	  while(itr!=NULL)
	    {       
	      itr->RSflag=rs_flag;
	      itr=itr->nb;
	    }
	} //for()
    } //if()

  else if(dest && !adjacency)
    {
      if(0)cout << "set RS flags of all adjacencies of a particular destination= " << dest << std::endl;
      for (QORS_EntryList_::iterator traverse = QORSTable.begin(); traverse != QORSTable.end(); ++traverse)
	{
	  if(traverse->dest ==dest)
	    {
	      itr=traverse->ptr_;
	      while(itr!=NULL)
		{       
		  itr->RSflag=rs_flag;
		  itr=itr->nb;
		}
	    }
	} //for()
    } // if()

  else if(dest && adjacency)
    { 
      if(0)cout << "set RS flag for a particular adjacency=" << adjacency << " of a particular destination=" << dest << std::endl;
      for (QORS_EntryList_::iterator traverse = QORSTable.begin(); traverse != QORSTable.end(); ++traverse)
	{
	  if(traverse->dest ==dest)
	    {
	      itr=traverse->ptr_;
	      while(itr!=NULL)
		{       
		  if(itr->neigh==adjacency) itr->RSflag=rs_flag;
		  itr=itr->nb;
		}
	    }
	} //for()
    } // if()
  else 
    {
      cout << "Major Error\n";
    }

  //  displayQORStable();
  cout << "end::setRSflags()\n";
}
//setRSflags()




// MODIFIED*
// sets NTR(need to reply) flag in the QORS table for a particular adjacency of a particular destination
void EIGRP::setNTRflag(int ntr_flag,IPAddr_t dest, IPAddr_t adjacency)
{
  //DEBUG0((cout << "Entering setNTRflag()" << endl));

  struct neighbors_ *itr;

  if(0)cout << "end::setNTRflag() for dest=" << dest << ",  adjacency=" << adjacency << std::endl;

  if(0)cout << "set NTR flag for a particular adjacency=" << adjacency << " of a particular destination=" << dest << std::endl;
  for (QORS_EntryList_::iterator traverse = QORSTable.begin(); traverse != QORSTable.end(); ++traverse)
    {
      if(traverse->dest ==dest)
	{
	  itr=traverse->ptr_;
	  while(itr!=NULL)
	    {       
	      if(itr->neigh==adjacency) itr->needToReply=ntr_flag;
	      itr=itr->nb;
	    }
	}
    } //for()

  //  displayQORStable();
  //if(0) cout << "end::setNTRflag() for dest=" << IPAddr::ToDotted(dest) << ", adjacency=" << IPAddr::ToDotted(adjacency) << std::endl;
}
//setNTRflag()





// MODIFIED*
// this is the DUAL FINITE STATE MACHINE IMPLEMENTATION
// Determine the state(s) and QueryOrigin Flag Oij for each destination in the QORS table
// The state is passive iff ALL adjacencies for a particular destination have their RS flags=0
// else, IF EVEN ONE of the adjacencies has RS flag=1, that destination is in ACTIVE state (determined by input argument s)
// if dest is specified, set/determine state ONLY FOR THAT destination
void EIGRP::determineState(int input_event, int fc_event, IPAddr_t dest)
{
  //DEBUG0((cout << "Entering determineState()" << endl));

  //  typedef enum { LINK_DISTANCE_INCREASE , LINK_DISTANCE_DECREASE, LINK_STATE_CHANGE, RECEIVE_UPDATE, RECEIVE_UPDATE_DISTANCE_INCREASE, RECEIVE_QUERY, QUERY_FROM_SUCCESSOR, RECEIVE_LAST_REPLY } EIGRPInputEvent_t;
  //  typedef enum { FC_NOT_SATISFIED, FC_SATISFIED } EIGRP_FC_Event_t;
  // typedef enum { PASSIVE,ACTIVE0,ACTIVE1,ACTIVE2,ACTIVE3 } eigrp_state_t;
  // s=0:PASSIVE    s=4:ACTIVE3

  int s=0,curr_state=0;

  if(0)cout << "\ndetermineState() input_event=" << input_event << ".   fc_event=" << fc_event << std::endl;


  // displayQORStable();

  int r=0;
  struct neighbors_ *itr;
  EIGRPInputEvent_t INPUT_EVENT=(EIGRPInputEvent_t) input_event;
  EIGRP_FC_Event_t FC_EVENT=(EIGRP_FC_Event_t) fc_event;


  for (QORS_EntryList_::iterator traverse = QORSTable.begin(); traverse != QORSTable.end(); ++traverse)
    {
      r=0;
      itr=traverse->ptr_;

      if(!dest || ((dest) && (traverse->dest==dest)) )
	{
	  curr_state=(int)traverse->state;
	  while(itr!=NULL)
	    {
	      if(itr->RSflag) r=1;
	      itr=itr->nb;
	    }


	  if((r==1)&&(curr_state==0)) 
	    {
	      // r=1 means ACTIVE  curr_state=0 means PASSIVE
	      cout << "Invalid QORS state\n";
	      // exit(1);
	    }


	  if(curr_state==0)
	    {
	      // in PASSIVE state.
	      switch(FC_EVENT)
		{
		case FC_NOT_SATISFIED:
		  {
		    switch(INPUT_EVENT)
		      {
		      case QUERY_FROM_SUCCESSOR:
			// go to ACTIVE3  
			s=4;
			break;

		      case RECEIVE_QUERY:
		      case RECEIVE_UPDATE_DISTANCE_INCREASE:
		      case LINK_DISTANCE_INCREASE:
		      case LINK_DISTANCE_DECREASE:
		      case LINK_STATE_CHANGE:
		      case RECEIVE_UPDATE:
		      case RECEIVE_LAST_REPLY:
			// go to ACTIVE1
			s=2;
			break;

		      default:
			cout << "\ndon't change state.\n";
			//exit(1);
		      }
		    break;
		  }

		case FC_SATISFIED:
		  {
		    switch(INPUT_EVENT)
		      {
			// do nothing, stay in passive state
		      case RECEIVE_QUERY:
		      case RECEIVE_UPDATE_DISTANCE_INCREASE:
		      case QUERY_FROM_SUCCESSOR:
		      case LINK_DISTANCE_INCREASE:
		      case LINK_DISTANCE_DECREASE:
		      case LINK_STATE_CHANGE:
		      case RECEIVE_UPDATE:
		      case RECEIVE_LAST_REPLY:
			s=0;
			break;

		      default:
			cout << "\ndon't change state.\n";
			//exit(1);
		      }
		    break;
		  }
		default:
		  cout << "\ndon't change state.\n";
		  //exit(1);                  
		}

	    } //if(curr_state==0)



	  else if(curr_state==1)
	    {
	      // in ACTIVE0 state.
	      switch(FC_EVENT)
		{
		case FC_NOT_SATISFIED:
		  {
		    switch(INPUT_EVENT)
		      {
		      case QUERY_FROM_SUCCESSOR:
			// go to ACTIVE2  
			s=3;
			break;

		      case RECEIVE_LAST_REPLY:
			// go to ACTIVE1
			s=2;
			break;

		      case RECEIVE_QUERY:
		      case RECEIVE_UPDATE_DISTANCE_INCREASE:
		      case LINK_DISTANCE_INCREASE:
		      case LINK_DISTANCE_DECREASE:
		      case LINK_STATE_CHANGE:
		      case RECEIVE_UPDATE:
			// stay in ACTIVE0
			s=1;
			break;

		      default:
			cout << "\ndon't change state.\n";
			//exit(1);
		      }
		    break;
		  }                  

		case FC_SATISFIED:
		  {
		    switch(INPUT_EVENT)
		      {
		      case QUERY_FROM_SUCCESSOR:
			// go to ACTIVE2
			s=3;
			break;

		      case RECEIVE_LAST_REPLY:
			// go to PASSIVE
			s=0;
			break;

		      case RECEIVE_QUERY:
		      case RECEIVE_UPDATE_DISTANCE_INCREASE:
		      case LINK_DISTANCE_INCREASE:
		      case LINK_DISTANCE_DECREASE:
		      case LINK_STATE_CHANGE:
		      case RECEIVE_UPDATE:
			// stay in ACTIVE0
			s=1;
			break;

		      default:
			cout << "\ndon't change state.\n";
			//exit(1);
		      }
		    break;
		  }
		default:
		  cout << "\ndon't change state.\n";
		  //exit(1);                  
		}

	    } //if(curr_state==1)



	  else if(curr_state==2)
	    {
	      // in ACTIVE1 state.
	      switch(FC_EVENT)
		{
		case FC_NOT_SATISFIED:
		  {
		    switch(INPUT_EVENT)
		      {
		      case QUERY_FROM_SUCCESSOR:
			// go to ACTIVE2  
			s=3;
			break;

		      case RECEIVE_LAST_REPLY:
			// go to PASSIVE
			// set FD=Infinity
			s=0;
			break;

		      case LINK_DISTANCE_INCREASE:
		      case RECEIVE_UPDATE_DISTANCE_INCREASE:
			//go to ACTIVE0
			s=1;
			break;

		      case RECEIVE_QUERY:
		      case LINK_DISTANCE_DECREASE:
		      case LINK_STATE_CHANGE:
		      case RECEIVE_UPDATE:
			// stay in ACTIVE1
			s=2;
			break;

		      default:
			cout << "\ndon't change state.\n";
			//exit(1);
		      }
		    break;
		  }                  

		case FC_SATISFIED:
		  {
		    switch(INPUT_EVENT)
		      {
		      case QUERY_FROM_SUCCESSOR:
			// go to ACTIVE2
			s=3;
			break;

		      case RECEIVE_LAST_REPLY:
			// go to PASSIVE
			// set FD=Infinity
			s=0;
			break;

		      case LINK_DISTANCE_INCREASE:
		      case RECEIVE_UPDATE_DISTANCE_INCREASE:
			//go to ACTIVE0
			s=1;
			break;

		      case RECEIVE_QUERY:
		      case LINK_DISTANCE_DECREASE:
		      case LINK_STATE_CHANGE:
		      case RECEIVE_UPDATE:
			// stay in ACTIVE1
			s=2;
			break;

		      default:
			cout << "\ndon't change state.\n";
			//exit(1);
		      }
		    break;
		  }
		default:
		  cout << "\ndon't change state.\n";
		  //exit(1);                  
		}

	    } //if(curr_state==2)


	  else if(curr_state==3)
	    {
	      // in ACTIVE2 state.
	      switch(FC_EVENT)
		{
		case FC_NOT_SATISFIED:
		  {
		    switch(INPUT_EVENT)
		      {
		      case RECEIVE_LAST_REPLY:
			// go to ACTIVE3
			s=4;
			break;

		      case RECEIVE_QUERY:
		      case RECEIVE_UPDATE_DISTANCE_INCREASE:
		      case QUERY_FROM_SUCCESSOR:                        
		      case LINK_DISTANCE_INCREASE:
		      case LINK_DISTANCE_DECREASE:
		      case LINK_STATE_CHANGE:
		      case RECEIVE_UPDATE:
			// stay in ACTIVE2
			s=3;
			break;

		      default:
			cout << "\ndon't change state.\n";
			//exit(1);
		      }
		    break;
		  }                  

		case FC_SATISFIED:
		  {
		    switch(INPUT_EVENT)
		      {

		      case RECEIVE_LAST_REPLY:
			// go to PASSIVE
			s=0;
			break;

		      case RECEIVE_QUERY:
		      case RECEIVE_UPDATE_DISTANCE_INCREASE:
		      case QUERY_FROM_SUCCESSOR:                        
		      case LINK_DISTANCE_INCREASE:
		      case LINK_DISTANCE_DECREASE:
		      case LINK_STATE_CHANGE:
		      case RECEIVE_UPDATE:
			// stay in ACTIVE2
			s=3;
			break;

		      default:
			cout << "\ndon't change state.\n";
			//exit(1);
		      }
		    break;
		  }
		default:
		  cout << "\ndon't change state.\n";
		  //exit(1);                  
		}

	    } //if(curr_state==3)



	  else if(curr_state==4)
	    {
	      // in ACTIVE3 state.
	      switch(FC_EVENT)
		{
		case FC_NOT_SATISFIED:
		  {
		    switch(INPUT_EVENT)
		      {
		      case RECEIVE_LAST_REPLY:
			// go to PASSIVE
			// set FD=Infinity
			s=0;
			break;

		      case LINK_DISTANCE_INCREASE:
		      case RECEIVE_UPDATE_DISTANCE_INCREASE:
			// go to ACTIVE2
			s=3;
			break;

		      case RECEIVE_QUERY:
		      case QUERY_FROM_SUCCESSOR:                        
		      case LINK_DISTANCE_DECREASE:
		      case LINK_STATE_CHANGE:
		      case RECEIVE_UPDATE:
			// stay in ACTIVE3
			s=4;
			break;

		      default:
			cout << "\ndon't change state.\n";
			//exit(1);
		      }
		    break;
		  }                  

		case FC_SATISFIED:
		  {
		    switch(INPUT_EVENT)
		      {
		      case RECEIVE_LAST_REPLY:
			// go to PASSIVE
			// set FD=Infinity
			s=0;
			break;

		      case LINK_DISTANCE_INCREASE:
		      case RECEIVE_UPDATE_DISTANCE_INCREASE:
			// go to ACTIVE2
			s=3;
			break;

		      case RECEIVE_QUERY:
		      case QUERY_FROM_SUCCESSOR:
		      case LINK_DISTANCE_DECREASE:
		      case LINK_STATE_CHANGE:
		      case RECEIVE_UPDATE:
			// stay in ACTIVE3
			s=4;
			break;

		      default:
			cout << "\ndon't change state.\n";
			//exit(1);
		      }
		    break;
		  }
		default:
		  cout << "\ndon't change state.\n";
		  //exit(1);                  
		}

	    } //if(curr_state==4)

	  else 
	    {
	      cout << "\nInvalid state\n";
	    }
	  cout << "s=" << s << std::endl;
	  traverse->state=(QueryOrigin_ReplyStatus_Entry::eigrp_state_t)s;
	  if(s==0) traverse->Oij=1;
	  else traverse->Oij=(s-1);

	} //if()

    } //for()


  // maintain consistency of state in the TT and QORS table
  maintainConsistentState();

  //displayQORStable();
  //displaytopologytable();
  cout << "end::determineState()\n\n";
}
// determineState()




//MODIFIED*
// maintain consistency of state in the TT and QORS table
void EIGRP::maintainConsistentState()
{
  DEBUG0((cout << "maintain consistency of state with the TT" << endl));

  int found=0,new_state=0;
  for (topologyEntryList_::iterator traverse = TopologyTable.begin(); traverse != TopologyTable.end(); ++traverse)
    {
      new_state=0;
      found=0;

      // first find the same destination in the QORS table
      for (QORS_EntryList_::iterator trav = QORSTable.begin(); trav != QORSTable.end(); ++trav)
	{
	  if(trav->dest==traverse->destin)
	    {
	      new_state=(int)trav->state;
	      found=1;
	    }
	}

      if(found) traverse->state=(TopologyEntry::eigrp_state_t)new_state;
    }

}
//maintainConsistentState()




// MODIFIED*
// find the destination in the QORS table, schedule active timers for each adjacency
void EIGRP::schedule_active_timer(IPAddr_t dest)
{
  return;
  if(0)cout << "In function SAT.....dest=" << dest << std::endl;
  displayQORStable();

  // find the entry in the QORS table, schedule active timer
  struct neighbors_* n1_;

  for (QORS_EntryList_::iterator q_ = QORSTable.begin(); q_ != QORSTable.end(); ++q_)
    {
      if(q_->dest== dest)
	{
	  n1_=q_->ptr_;

	  while(n1_!=NULL)
	    {
	      cout << "scheduling a new Active timer event at " << Simulator::instance->Now() << ". ActiveTime=" << EIGRP_DEFAULT_ACTIVE_TIME << std::endl;
	      if(0)cout << "dest=" << q_->dest << ", neigh=" << n1_->neigh << std::endl;
	      activeTimer.Schedule( EIGRP_DEFAULT_ACTIVE_TIME, (EIGRPEvent*) n1_->activeEvent);
	      n1_=n1_->nb;

	    } // while()

	} // if()
    } //for()

  cout << "End of function SAT\n";

}
// schedule_active_timer



//void EIGRP::SetMetric(Time_t time, char* ip, double BW, double delay, double reliability, double load)
void EIGRP::SetMetric(Time_t time, IPAddr_t ip, double BW, double delay, double reliability, double load)
{
  EIGRPEvent* ev = new EIGRPEvent(EIGRPEvent::METRIC_CHANGE);
  ev->ip = ip;
  ev->bw = BW;
  ev->delay = delay;
  ev->reliability = reliability;
  ev->load = load;
  Scheduler::Schedule(ev, time, this);
}


void EIGRP::setMetric(IPAddr_t IPstr, double BW, double delay, double reliability, double load)
{
  //DEBUG0((cout << "Entering setMetric()" << endl));

  unsigned long ip;
  IPAddr_t myIP;
  NeighborMap_t::iterator i,nb_;
  IPAddr_t your_ip_=0;
  int n_b;
#ifndef WIN32
  inet_pton(AF_INET,IPAddr::ToDotted(IPstr),&ip);
#else
  ip = inet_addr(IPAddr::ToDotted(IPstr));
#endif
  ip=htonl(ip);
  if(0)cout << "setMetric:: ip " << ip << std::endl;

  if (!neighborsKnown)
    { 
      buildNeighborList();
    }

  for (i= neighbors.begin();  i != neighbors.end();  ++i)
    { 
      myIP=i->second->myIP;
      if(myIP==ip) 
	{ 
	  nb_=i;
	  your_ip_=i->first;
	}
    }        
  if(0)cout << "setMetric:: your_ip_ = " << your_ip_ << std::endl;


  if(nb_->second != NULL)
    {
      // old metric to this neighbor
      n_b=(int)nb_->second->metric;

      if(delay == -1)    // metric is given as a number -->   eigrp.setMetric("10.10.10.1", 5)
	{
	  nb_->second->metric = BW;
	  if(0)cout << "metric is::: " << nb_->second->metric << "\n\n";
	}
      else if(reliability == -1)  // metric will be calculated using "bandwidth" and "delay"
	{
	  // bandwidth is in Kbits/sec , delay is in microseconds
	  // note that IGRP uses 0xFFFFFF (6 F's)=167 sec to indicate an unreachable route
	  // multiply by 256 for EIGRP composite metric

	  nb_->second->metric = ( (10000000/BW) + delay )*256 ;
	  if(0)cout << "Composite metric (using BW and Delay) is::: " << nb_->second->metric << "\n\n";
	}
      else if(load != -1)
	{
	  // Bandwidth in Kbits/sec , delay is in microseconds
	  // note that IGRP uses 0xFFFFFF (6 F's)=167 sec to indicate an unreachable route

	  // Reliability (measured dynamically) is expressed as 255=100%reliable , 1=MIN reliability
	  // Reliability is expressed as a fraction of 255

	  // LOAD is also expressed as a fraction of 255:  1=MIN load, 255=100% loaded
	  // multiply by 256 for EIGRP composite metric
	  // reliability and load, when included must be calculated based on Weighted average, to prevent destabilization of n/w
	  // average them over 5 min time constant, updated every 5 seconds.

	  int k1=1; int k3=1;
	  int k2=0; int k4=0; int k5=0;             // all 3 must be zeros
	  double Reliability =  reliability/ 255;
	  double LOAD = load / 255;
	  double BWmin = 10000000 / BW;
	  double k5term= k5==0 ?  1 : (k5/(Reliability+k4)) ;
	  nb_->second->metric = (    (  (k1*BWmin) + ((k2*BWmin)/(256-LOAD)) + k3*(delay)  ) * k5term  )*256 ;
	  if(0)cout << "Composite metric (using BW, Delay, Reliability, Load) is::: " << nb_->second->metric << "\n\n";
	}

      if(n_b == (int)nb_->second->metric)
	cout << "No effective change in metric. Return.\n\n";
	
      EIGRPInputEvent_t INPUT_EVENT;

      //  old < new   means metric increase
      if(n_b < (int)nb_->second->metric)  INPUT_EVENT=LINK_DISTANCE_INCREASE;

      //  old > new   means metric decrease
      if(n_b > (int)nb_->second->metric)  INPUT_EVENT=LINK_DISTANCE_DECREASE;

      
      if(0)cout << "setMetric: your_ip_ is " << your_ip_ << " and nb_->second->metric is " << nb_->second->metric << std::endl;
      modify_TT_link_metrics(your_ip_,(int) nb_->second->metric, INPUT_EVENT);
      // //displaytopologytable();

    }
  cout << "Exiting setMetric()\n";

}




// MODIFIED*
// called by set-metric
// also called indirectly by increase-metric,link-down,link-up
void EIGRP::modify_TT_link_metrics(IPAddr_t your_ip_, int metric_to_this_neighbor, EIGRPInputEvent_t INPUT_EVENT )
{
  cout << "Entering modify_TT_link_metrics" << endl; 

  cout << "input event: " << INPUT_EVENT << std::endl;
  EIGRP_FC_Event_t  FC_EVENT;

  IPAddr_t dest;
  Metric_t old_best_metric;
  Metric_t _new_metr_;
  struct feasibles *itr;

  // prepare for sending out query messages for those destinations whose FD increases
  int sendquery=0;
  // INIT flag not set, EIGRP Version=2
  EIGRPHeader* queryMesg = new EIGRPHeader(MsgQuery,2,(unsigned int)MsgQuery,0x0000EFCF,0x00000000,0,0,1000);


  // prepare for sending out Update messages for those destinations for which we get new Successors.
  int sendupdate=0;

  // INIT flag not set, EIGRP Version=2
  EIGRPHeader* updateMesg = new EIGRPHeader(MsgUpdate,2,(unsigned int)MsgUpdate,0x0000EFCF,0x00000000,0,0,1000);


  // flag that indicates whether your_ip is a feasible successor at all (may or may not be the successor) !
  int used_as_feas_succ=0;  // not a feasible successor

  // maintain consistency of state in the TT and QORS table
  maintainConsistentState();
  //displaytopologytable();

  // modify link metrics in the TT
  for (topologyEntryList_::iterator traverse = TopologyTable.begin(); traverse != TopologyTable.end(); ++traverse)
    {
      dest=traverse->destin;
      old_best_metric=traverse->best_metric;
      used_as_feas_succ=0;


      // modify the TT with new metric to the neighbor....modify all Destinations which use the neighbor as a feasible successor.
      itr= traverse->pointer;
      while(itr->fee!=NULL)
	{
	  itr=itr->fee;
	  cout << "dest is: " << (string)IPAddr(dest) << " and fee is: " <<  (string)IPAddr(itr->feas_succ) << std::endl;

	  if(itr->feas_succ == your_ip_)
	  //if( (itr->feas_succ == IPAddr("0.0.0.0")) && (dest == your_ip_) )
	    {
	      if(1)cout << "Dest uses this feas_succ=" << (string)IPAddr(your_ip_) << std::endl;
	      if(1)cout << "Current state of the destination (" << (string)IPAddr(dest) << ") is s=" << traverse->state << std::endl;

	      // CAN modify the link metrics, even if the destination is active
	      cout << "Modifying the link metrics.....but NOT the RD,FD,successor (yet)\n";
	      
	      // todo test this if() section
	      if(traverse->destin == your_ip_)
		if( itr->RD!=0) { cout << "need to test this\n"; exit(1); }
	      
	      _new_metr_=itr->RD+metric_to_this_neighbor;
	      itr->metric= _new_metr_;

	      used_as_feas_succ=1;  // is a feasible successor (may or may not be the successor)      
	    }

	  if(itr->RD>=Infinity) itr->RD=Infinity;
	  if(itr->metric>=Infinity) itr->metric=Infinity;

	} //while()


      if(used_as_feas_succ==1)
	{
	  // Note:just cannot modify the router's successor, FD,RD(distance it reports) if dest is active.
	  if(1)cout << "dest=" << (string)IPAddr(traverse->destin) << "   old_best_metric=" << old_best_metric << std::endl;

	  // insuring the destination isnt active
	  if((int)traverse->state==0)
	    {
	      cout << "\nSINGLE COMPUTATION, PASSIVE DEST.\n";
	      cout << "try to find a (new) feasible successor, that satisfies the FC (based on old FD)\n";

	      struct feasibles *new_succ;

	      if(INPUT_EVENT==LINK_DISTANCE_INCREASE)
		{
		  // dont modify the FD
		  new_succ= findSuccessor(traverse->destin,0 /*modifyFD*/, 1 /*exclude*/, your_ip_/*excludeFromComputationofSuccessor*/);
		}
	      else if(INPUT_EVENT==LINK_DISTANCE_DECREASE)
		{
		  // dont modify the FD
		  new_succ= findSuccessor(traverse->destin,0 /*modifyFD*/);
		}


	      // after the best_metric is updated
	      displaytopologytable();

	      if(new_succ!= NULL)
		{
		  // found a successor.
		  // DUAL now behaves like DBF, i.e. does LOCAL COMPUTATION to update distance(already done above) and successor

		  // IMPORTANT NOTE:
		  // IT DOESNT MATTER WHETHER THE SUCCESSOR CHANGED OR NOT.
		  // UPDATES ARE ONLY SENT IF THE METRIC VIA THAT SUCCESSOR CHANGES (WRT THE OLD METRIC)
		  // THIS IS BECAUSE THE ROUTERS RECEIVING THE UPDATES WILL USE ME AS NEXT HOP IN EITHER CASE.

		  FC_EVENT=FC_SATISFIED;

		  if(new_succ->metric != traverse->current_successor->metric)
		    {
		      // The metric changed via the successor
		      // i.e. new distance is different from previous distance

		
		      // send out an update
		      cout << "found a feasible successor" << std::endl;
		      sendupdate=1;
		      UpdateHeader* update = new UpdateHeader();
		      update->setUpdateFields(0x0102, new_succ->feas_succ,0,0,1500, new_succ->metric,0,0,0,
					      traverse->significant, traverse->destin);
		      updateMesg->updateHeader.push_back(update);
		    }
		  traverse->current_successor = new_succ;

		  evaluate_best_metric_and_FD( 1, traverse->destin);

		} 
	      //if(new_succ!=NULL)

	      else
		{
		  // new_succ==NULL.......findSuccessor failed.
		  // cannot find a feasible successor. Keep the current successor.
		  // Send out Queries
		  // Update the FD....depending on the FC mode used.

		  // sanity check.
		  if(your_ip_ != traverse->current_successor->feas_succ) { cout << "Something's Wrong.\n"; exit(1); }

		  cout << "Couldnt find a feasible successor..Now updating the FD and RD.\n";

		  // The RD for that destination is the metric via the current successor.
		  traverse->current_successor->metric = traverse->current_successor->RD + metric_to_this_neighbor;
		  if(traverse->current_successor->metric >Infinity) traverse->current_successor->metric=Infinity;
		  if(traverse->current_successor->RD >Infinity) traverse->current_successor->RD=Infinity;

		  traverse->best_metric = traverse->current_successor->metric;

		  // update the FD for the destination
#if defined(DIC)
		  traverse->min_metric_till_now = traverse->current_successor->metric;
		  traverse->FD = traverse->current_successor->metric;

#elif defined(CSC)
		  // todo...test this
		  traverse->current_successor->min_RD = traverse->current_successor->RD;
		  traverse->FD=traverse->current_successor->min_RD;

#elif defined(SNC)
		  traverse->min_metric_till_now = traverse->current_successor->metric;
		  traverse->FD = traverse->current_successor->metric;
#else           
		  cout << "Bogus\n";
#endif

		  cout << "Now see the changes to the TT\n";
		  displaytopologytable();

		  cout << "Commencing a DIFFUSING COMPUTATION. Sending Queries and going active\n";
		  // Become active and send Queries, set QORS flags

		  // query contains the NEW distance via the current successor
		  QueryHeader* query = new QueryHeader();
		  query->setQueryFields(0x0103, traverse->current_successor->metric, traverse->destin);
		  queryMesg->queryHeader.push_back(query);
		  // set RS flags of all adjacencies of the destination
		  setRSflags(1,(IPAddr_t)traverse->destin);

		  schedule_active_timer(traverse->destin);
		  sendquery=1;

		  FC_EVENT = FC_NOT_SATISFIED;
		  determineState((int)INPUT_EVENT,(int)FC_EVENT, (IPAddr_t) traverse->destin);

		}
	      // else()

	    } 
	  // if(traverse->state==0) , i.e destination is passive


	  else
	    {
	      cout << "Destination is in ACTIVE state\n";
	      // Caution: Cannot modify the router's FD,RD or Successor if dest is active.

	      // FC_EVENT is irrelevant in this case (refer to state diagram)
	      FC_EVENT = FC_SATISFIED;

	      // if in ACTIVE1 or ACTIVE3  and its a LINK_DISTANCE INCREASE event, must transition state
	      determineState((int)INPUT_EVENT,(int)FC_EVENT, (IPAddr_t) traverse->destin);
	    }

	}
      // if(used_as_feas_succ==1)

      else
	{
	  // (used_as_feas_succ==0)
	  // The neighbor(whose link metric got modified) is not used as a feasible successor for this destination. 
	  // No need to do anything.
	}

    }
  // for()



  if(sendquery)
    {
      // send mcast to everyone, including the guy whose cost increased.
      cout << "calling McastReliable(queryMesg)from modify_TT_link_metrics\n";
      McastReliable(queryMesg);

      int k_=0;
      // increment sent message counter for Queries
      for (NeighborMap_t::iterator i_ = neighbors.begin();  i_ != neighbors.end();  ++i_)
	{
	  if (i_->second->status == Neighbor::UP)  
	    { 
	      k_++;
	    }
	}

      Sent_Mesg_Counters.Queries += k_;
      Increment_Sent_Queries(k_);
      if(0)cout << "Sent_Mesg_Counters.Queries = " << Sent_Mesg_Counters.Queries << std::endl;
      if(0)cout << "Recd_Mesg_Counters.Queries = " << Recd_Mesg_Counters.Queries << std::endl;

      if(0)cout << "Going Active, starting diffusing computation\n" << std::endl;
      if(0)cout << "Sent Queries as Reliable Multicast to everybody\n" << std::endl;
    }
  
  else if(sendupdate) 
    {
      // send mcast to everyone except the adjacency that just went dead.
      // strictly speaking, mcastReliable(updateMesg) will also work since the eigrp router status has been changed to 'down'
      cout << "calling McastReliable(updateMesg) from modify_TT_link_metrics\n";
      McastReliable(updateMesg);
      
      int k_=0;
      // increment sent message counter for Updates
      for (NeighborMap_t::iterator i_ = neighbors.begin();  i_ != neighbors.end();  ++i_)
	{
	  if (i_->second->status == Neighbor::UP) 
	    { 
	      k_++;
	    }
	}
      Sent_Mesg_Counters.Updates += k_;
      Increment_Sent_Updates(k_);
      
      if(0)cout << "Sent_Mesg_Counters.Updates = " << Sent_Mesg_Counters.Updates << std::endl;
    }

  if(sendquery==0) {
    sendquery=0;
    if(0)cout << "Clearing the packet.\n";
    if(0)cout << "*************************************\n";
  }

  if(sendupdate==0) {
    sendupdate=0;
    if(0)cout << "Clearing the packet.\n";
    if(0)cout << "*************************************\n";
  }

  // find those entries that dont satisfy the FC anymore. Flush them out !
  flush_TT();
  TT_to_RT();

  displayQORStable();
  //displaytopologytable();

  cout << "Exiting modify_TT_link metrics\n";
}
//modify_TT_link_metrics()



void EIGRP::Down()
{
  cout << "eigrp " << myNode->Id() << " down at " << Simulator::Now() << std::endl;

  // cancel the last scheduled hello        
  eigrphelloTimer.Cancel((EIGRPEvent*) this->helloEvent);

  // cancel ALL last scheduled holds
  for (adjacencyEntryList_::iterator it = AdjacencyTable.begin(); it != AdjacencyTable.end(); ++it)
    {
      AdjacencyEntry* traverse = *it;
      double currentTime= Simulator::Now();
      traverse->uptime_= currentTime - traverse->upTimeStart_;
      holdTimer.Cancel(traverse->holdEvent);
    }

  // cancel all last scheduled Active timers
  struct neighbors_ *itr;
  for (QORS_EntryList_::iterator traverse = QORSTable.begin(); traverse != QORSTable.end(); ++traverse)
    {
      itr=traverse->ptr_;
      while(itr!=NULL)
	{
	  // cancel any scheduled active timer
	  activeTimer.Cancel(itr->activeEvent);
	  itr=itr->nb;
	}
    } // for()

  Dead();
  // if (Status() == ALIVE) cout << "Alive !!\n"; else cout << "DEAD\n";
}




void EIGRP::Reset()
{
  cout << std::endl;
  cout << "EIGRP " << myNode->Id() << " Reset at " << Simulator::Now() << std::endl;
  
  NeighborMap_t::iterator i;
  IPAddr_t myIP,yourIP;

  // Insure I'm alive
  Alive();
  // if (Status() == ALIVE) cout << "Alive !!\n";
  
  //#ifdef FORGET
  TopologyTable.clear();
  RoutingTable.clear();
  QORSTable.clear();
  AdjacencyTable.clear();
  ReplyBuffer.clear();
  //#endif

  if (!neighborsKnown)
    { 
      // Need to find neighbor map
      cout << "++++++++++++++++++++++++++building neighbor map \n";
      buildNeighborList();
    }


  for (i = neighbors.begin();  i != neighbors.end();  ++i)
    { 
      myIP=i->second->myIP;
      yourIP=i->first;
      if(0)cout << "myIP: " << myIP << std::endl;
      if(0)cout << "yourIP: " << yourIP << std::endl;

      // in place of addUpdateTopologyEntry
      UpdateTopologyTable ( myIP, 32, 0, 0, 0, 0/*appendupdates*/, 0/*appendqueries*/);

      //todo...maybe the RD should be initialized to 0, not INFINITY
      UpdateTopologyTable ( yourIP, 32, 0 /*RD*/, INITIAL_FINITE_DISTANCE_TO_NEIGHBORS, yourIP, 0/*appendupdates*/, 0/*appendqueries*/);
    }


  //displayroutingtable();
  //displaytopologytable();
   
  // create a new hello timer
  this->helloEvent=new EIGRPEvent(EIGRPEvent::HELLO_EVENT);
   
  //  Send 'HELLO' messages after restart
  SendHello();
   
}
//eigrp-reset



void EIGRP::SetLinkDown(Time_t time, IPAddr_t ip)
{
  EIGRPEvent* ev = new EIGRPEvent(EIGRPEvent::LINK_DOWN);
  ev->ip = ip;
  Scheduler::Schedule(ev, time, this);
}

void EIGRP::linkDown(IPAddr_t ip)
{
  // accepts myIP as argument
  // make neighbor (in neighbor map) go down.
  // wait for hold timer to expire.

  IPAddr_t myIP,yourIP;

  cout << std::endl;
  cout << "EIGRP " << myNode->Id() << " link-down at " << Simulator::Now() << std::endl;          
  
  // make neighbor in the neighbor table go "down"
  for (NeighborMap_t::iterator i = neighbors.begin();  i != neighbors.end();  ++i)
    {
      myIP=i->second->myIP;
      yourIP=i->first;
      if(0)cout << "yourIP: " << IPAddr::ToDotted(yourIP) << std::endl;
      if(myIP== ip)
	{
	  if(0)cout << "Making yourIP(neighbor): " << IPAddr::ToDotted(yourIP) << " go down\n";
	  i->second->Down();
	  if(0)cout << "Status " << i->second->status << std::endl;
	  
	  // Talal: Take the link down here
 	  Interface* InterfaceToNeighbor = myNode->GetIfByIP(myIP);
	  Simulator::instance->InterfaceDownAt(InterfaceToNeighbor, Constant(Simulator::Now()));
	 
	  break;
	}
    }
  
  if(0)cout << "Link went down, i.e metric to the adjacency " << IPAddr::ToDotted(yourIP) << " is INFINITY.\n";
  
  adjacencyEntryList_::iterator it;
  for (it= AdjacencyTable.begin(); it != AdjacencyTable.end(); ++it)
    {
      AdjacencyEntry* j = *it;
      if(j->adjacency_ == yourIP)
	{
	  //cancel the last scheduled hold
	  holdTimer.Cancel(j->holdEvent);
	  HoldTimeout(0, j);
	  break;
	}
    }
  
  setMetric(ip, Infinity);

  cout << "EIGRP " << myNode->Id() << "exiting linkDown()" << std::endl;
}
//link-down






// MODIFIED*
// An EIGRP Hold Timer expires for a neighbor EIGRP Router 15 seconds (default) after it doesnt hear a hello.
// remove the neighbor EIGRP Router that went dead from adjacency table

// remove entries from the routing table and topology table
// remove multiple entries from QORSTable(same neighbor from all destinations)
// send queries to all your neighbors, for reaching those destinations that are now unreachable

void EIGRP::HoldTimeout(int holdTimeout, AdjacencyEntry *e)
{
  DEBUG0((cout << "Entering HOldTimeout()" << endl));

  EIGRP_Timeout_t timeout_ty=HOLDTIMEOUT;
  IPAddr_t yourIP;
  

  // make neighbor in the neighbor table go "down"
  for (NeighborMap_t::iterator i = neighbors.begin();  i != neighbors.end();  ++i)
    {       
      yourIP=i->first;
      if(yourIP==e->adjacency_) 
	{
	  if(0)cout << "Making yourIP(neighbor): " << IPAddr::ToDotted(yourIP) << " go down\n";
	  i->second->Down();
	  if(0)cout << "Status " << i->second->status << std::endl;
	}      
    }


  bool ADJACENCY_JUST_REMOVED=false;

  // remove the adjacency from the adjacency table
  // print out the adjacency table , before removing the entry
  displayadjacencytable();
  
  adjacencyEntryList_::iterator it;
  for (it= AdjacencyTable.begin(); it != AdjacencyTable.end(); ++it) 
    { 
      AdjacencyEntry* j = *it;
      if(j->adjacency_ == e->adjacency_) 
	{ 
	  if(0)cout << "\nRemoving entry " << j->adjacency_ << "\n\n";
	  ADJACENCY_JUST_REMOVED=true;
	  AdjacencyTable.erase(it);
	  break;
	}
    }

  // print out the adjacency table , AFTER removing the entry
  cout << "Adjacency Table:  (after removing the entry)  \n";
  displayadjacencytable();

  // wrong
  // when you remove entry from adjacency table, update H_ of all to reflect the new order



  if(ADJACENCY_JUST_REMOVED==true) 
    {
      // Remove QORS entries, entries from the Routing and topology Tables      
      remove_adjacency_from_RT_TT_QORS(e->adjacency_,timeout_ty);
    }

  
}
// holdtimeout()






// Convergence Model
Network_State_t CheckConvergence()
{
#ifdef DISPLAY_CONVERGENCE_MODELS
  cout << "CheckConvergence....";
#endif

  if( (Global_Sent_Counter.Updates==Global_Recd_Counter.Updates) && (Global_Sent_Counter.Queries==Global_Recd_Counter.Queries)
      && (Global_Sent_Counter.Replies==Global_Recd_Counter.Replies) && (Global_Recd_Counter.Queries==Global_Recd_Counter.Replies) )
    {
      //displaytopologytable();
      cout << "\n\n\n\n";
      
      double convergence_time;
      end_convergence = Simulator::Now();
      NetworkMessages = EQUAL;
#ifdef DISPLAY_CONVERGENCE_MODELS
      cout << "NetworkMessages = EQUAL  at time " << end_convergence << std::endl;
#endif

      if(start_convergence >= start_convergence_measurement)
	{
	  // factor into convergence measurement ONLY IF the topology change started on or after the convergence window opens.


	  if(start_convergence < end_convergence_measurement)
	    {
	      // factor into convergence measurement EVEN IF the topology change started before the convergence window closes.
              

	      if( (end_convergence > start_convergence))
		{
		  convergence_time= end_convergence - start_convergence;
		  average_convergence_time= ((average_convergence_time*(number_of_topology_changes-1))+ convergence_time) / (number_of_topology_changes);
                  
#ifdef DISPLAY_CONVERGENCE_MODELS
		  cout << "start_convergence=" << start_convergence << "  end_convergence=" << end_convergence << std::endl;
		  cout << "Convergence Time=" << convergence_time << std::endl;
		  cout << "Average Convergence Time " << average_convergence_time << std::endl;
		  cout << "Number of Topology changes " <<  number_of_topology_changes << std::endl;
#endif
		}
	    }

	  else 
	    {
	      number_of_topology_changes--;
#ifdef DISPLAY_CONVERGENCE_MODELS
	      cout << "Convergence measurement ended at " << end_convergence_measurement << "....Computation started at " << start_convergence << ". Sorry !!\n";
#endif
	    }

	}
      else 
	{
	  number_of_topology_changes--;
#ifdef DISPLAY_CONVERGENCE_MODELS
	  cout << "Convergence measurement starts at " << start_convergence_measurement << "....Current Simulation time is " <<  end_convergence
	       << ". Please be Patient !!\n";
#endif
	}

      return (Network_State_t) 1;
    }
  else 
    {
#ifdef DISPLAY_CONVERGENCE_MODELS
      cout << "NetworkMessages = NOT_EQUAL\n";
#endif
      NetworkMessages = NOT_EQUAL;
      return (Network_State_t) 0;
    }

}
// CheckConvergence()

	 
