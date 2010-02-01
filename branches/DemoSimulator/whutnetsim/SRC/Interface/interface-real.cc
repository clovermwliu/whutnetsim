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


/************************************************************************/
/* 2010.1.4  关怡萍                                                     */
/***********************************************************************/
#include <iostream>

#include "G_debug.h"
#include "hex.h"
#include "interface-real.h"
#include "link.h"
#include "queue.h"
#include "droppdu.h"
#include "node.h"
#include "trace.h"
#include "G_globalstats.h"
#include "ipv4.h"
#include "llcsnap.h"
#include "ratetimeparse.h"
#include "l3protocol.h"
#include "routing-nixvector.h"

#define DEFAULT_ARP_TIMEOUT 1200  //20分钟

using namespace std;

Count_t InterfaceReal::count = 0;

InterfaceReal::InterfaceReal(const L2Proto& l2, 
							 IPAddr_t i,
                             Mask_t m, 
							 MACAddr mac, 
							 bool bootstrap)  
: Interface(l2, i, m, mac),
  pQueue(Queue::Default().Copy()),
  notifEvent(nil),
  totBytesSent(0), 
  pL2Proto(l2.Copy()), 
  defaultPeer(IPADDR_NONE)
  ////usearp(Interface::defaultUseARP), 
  ////arpmodule(nil),
  ////arptimeout(DEFAULT_ARP_TIMEOUT), 
  ////wormcontainment(nil)
/*
描述：创建一个新的real型接口对象
参数：l2——与接口交互的第二层协议对象
	  i——接口IP地址
	  m——接口掩码
	  mac——接口MAC地址
*/  
{
  pQueue->SetInterface(this); //队列设定与之对应的接口
  pL2Proto->SetInterface(this); //第二层协议设定与之对应的接口
  count++;
  ////if (usearp)
  ////  {
  ////    arpmodule = new ARP(pNode);
  ////    arpmodule->SetARPTimeout(arptimeout);
  ////    arpmodule->SetTrace(Trace::ENABLED);
  ////  }
}

InterfaceReal::~InterfaceReal()
//析构函数
{
//  delete arpmodule;
  delete pQueue;
  delete pL2Proto;
  //调用父类的析构函数，用来释放Link和本地event队列
  Interface::~Interface();
}

////void InterfaceReal::SetWormContainment(WormContainment* cont)
////{
////  usewormcontainment = true;
////  wormcontainment = cont;
////}
////
////void InterfaceReal::UseARP(bool b)
////{
////  usearp = b;
////  if (usearp)
////    {
////      arpmodule = new ARP(pNode);
////      arpmodule->SetARPTimeout(arptimeout);
////      arpmodule->SetTrace(Trace::ENABLED);
////    }
////}

bool InterfaceReal::Send(Packet* p, IPAddr_t i, int type)
/*
描述：通过本接口传输数据包
参数：p——待传输的数据包
	  i——目的地IP地址
	  type——LLC/SNAP类型
返回值：true——发送或已经加入队列
	    false——数据包丢失
*/
{
   //if(usewormcontainment)
   // {
   //   wormcontainment->ProcessOutPacket(p, i, type, this);
   //   return true;
   // }
   return Xmit(p, i, type);
}

bool InterfaceReal::Xmit(Packet* p, IPAddr_t i, int type)
/*
描述：通过本接口传输数据包，检查是否需要使用ARP
参数：p——待传输的数据包
	  i——目的地IP地址
	  type——LLC/SNAP类型
返回值：true——发送或已经加入队列
		false——数据包丢失
*/
{
  
  //if(usearp) //若MAC地址合法，请求ARP
  //  {
  //    arpmodule->ARPRequest(this, p, type);
  //    return true;
  //  }
  MACAddr dst = IPToMac(i);
  return Send(p, dst, type);
}

bool InterfaceReal::Send(Packet* p, const MACAddr& dst, int type)
/*
描述：通过本接口传输数据包，不使用ARP
参数：p——待传输的数据包
	  i——目的地IP地址
	  type——LLC/SNAP类型
返回值：true——发送或已经加入队列
		false——数据包丢失
*/
{
  if (IsDown())//若接口已经关闭，丢弃数据包
    { 
      DropPDU d("L2-ID", p);
      pNode->TracePDU(nil, &d);
      Stats::pktsDropped++; //记录丢弃的数据包
      delete p;
      return false;
    }
      
  LLCSNAPHeader* llc = new LLCSNAPHeader(type);
  p->PushPDU(llc);
  pL2Proto->BuildDataPDU(macAddr, dst, p);
  if (!pL2Proto->Busy() && !pQueue->CheckForcedLoss(false))
    { //检查链路是否是空闲状态，且没有强迫或欺骗损失
      bool spoofLoss = pQueue->CheckSpoofedSource(p);
      if (spoofLoss)
        {
          DEBUG0((cout << "Interface dropping spoofed packet" << endl));
          DropPDU d("L2-FD", p); // Filter drop
          pNode->TracePDU(nil, &d);
          Stats::pktsDropped++; // Count dropped packets
          DEBUG0((cout << "Interface " << this
                  << " dropping pkt due to spoofing detected" << endl));
          if (p->notification)
            { //发送者需要确证，因此我们需要去记录一个，并需要计算队列全部排空所需时间
              Simulator::instance->AddNotify(
                  p->notification, Time("1ms"), nil);
            }
          delete p;     //丢弃数据包
          return false; 
        }

      Stats::pktsEnqued++;
      Stats::pktsDequed++;

      pL2Proto->DataRequest(p);
      return true;
    }
  return EnquePacket(p);
}


bool InterfaceReal::Send(Packet* p, Word_t type)
/*
描述：通过本接口广播数据包
参数：p——待传输数据包
	  type——目的地MAC地址
返回值：true——发送或已经加入队列
	  false——数据包丢失
*/
{
  return Send(p, MACAddr::Broadcast(), type);
}

bool InterfaceReal::EnquePacket(Packet* p)
//将数据包加入队列，若成功返回true，否则返回false
{
  if (!pQueue->Enque(p))
    {
      DropPDU d("L2-QD", p);
      pNode->TracePDU(nil, &d);
      Stats::pktsDropped++; // Count dropped packets
      DEBUG0((cout << "Interface " << this << " dropping pkt" << endl));
      if (p->notification)
        { // Sender has requested notification, so we need to
          // schedule one.
          // hack for now just to 1ms...need to calculate queue drain time
          DEBUG(1,(cout << "Adding notif for " << p->notification << endl));
          Simulator::instance->AddNotify(p->notification, Time("1ms"), nil);
        }
      delete p;     // Delete the packet
      return false; // Dropped
    }
  return true;
}

void InterfaceReal::Handle(Event* e, Time_t t)
{
  DEBUG0((cout << "InterfaceReal::Handle, time " << t << endl));
  LinkEvent* le = (LinkEvent*)e; // Convert to right event type

  Size_t sz = 0;
  switch (le->event) {
    case  LinkEvent::PACKET_RX:
      if (le->fifoEvent)
        { // Should be head of fifo event queue
#undef  EXTRA_CHECKS
#ifdef  EXTRA_CHECKS
          if (evList->empty())
            {
              cout << "HuH?  Interface got fifo event on empty evList" << endl;
              exit(1);
            }
          EventPair* ep = evList->front();
          if (ep->event != le)
            { 
              cout << "HuH?  Interface got fifo event mismatch" << endl;
              cout << "eventcq " << evList << endl;
              cout << "le " << le << " ep.second " << ep->event << endl;
              exit(1);
            }
#endif
          evList->pop_front(); // Remove from fifo event list
          if (!evList->empty())
            { // Need to schedule the next simulator event
              EventPair* ep1 = evList->front();
              Time_t when = ep1->time - Simulator::Now();
              DEBUG0((cout << "Scheduling evlist front " << ep1->event
                      << " ev " << ep1->event->event << endl));
              Scheduler::ScheduleEarly(ep1->event, when, this);
            }
        }
      //if (GetNode()->FirstBitRx())
      //  { 
      //    delete le;
      //    return;
      //  }

      Trace::Instance()->AppendEOL(); // End of log line
      DEBUG0((cout << "Interface " << this 
              << " Handle node " << pNode->Id() << endl));

      if (IsDown())
        { // Link is down, just drop packet and return
          DropPDU d("L2-ID", le->p);
          pNode->TracePDU(nil, &d);
          Stats::pktsDropped++;
          delete le->p;
          le->p = nil;
          delete le;
          return;
        }

      if(pNode->IsSwitchNode())  // if node is switch
        {
          // Pass the packet to the switch
          pNode->PacketRX(le->p, this);
          delete le;
          return;
        }

      PacketRxStart(le);
      sz = le->p->Size();
      if (le->hasBitError)
        { // just drop the packet due to bit error
          DropPDU d("L2-BER", le->p);
          pNode->TracePDU(nil, &d);
          DEBUG(0,(cout << "Packet Dropped due to BER" << endl));
          Stats::pktsDropped++;
          delete le->p;
          delete le;
        }
      else
        {
          //	cout << "port matching = " << useportmatching << endl;
          //if(usewormcontainment)
          //  {
          //    wormcontainment->ProcessInPacket(le->p, this);
          //    delete le;
          //    break;
          //  }
          GetL2Proto()->DataIndication(le->p, le);
        }
    
      PacketRxEnd(le, sz);
      break;
//    case  LinkEvent::PACKET_FIRSTBIT_RX:
//      {
//        Node* n = GetNode();
//        //if (!n->FirstBitRx())
//        //  {
//        //    cout << "HuH? First bit rx on non-fbrx node" << endl;
//        //    break;
//        //  }
//#ifdef OLD_WAY
//        const IFVec_t& iflist = n->Interfaces();
//        // This is a real hack...need to think through a good solution.
//        // For now, find the first interface that is not this one
//        // and forward the packet
//        bool foundIt = false;
//        for (IFVec_t::size_type k = 0; k < iflist.size(); ++k)
//          {
//            if (iflist[k] != this)
//              { // Found it, just send it on.
//                Link* lk = iflist[k]->GetLink();
//                Time_t rxTime = lk->Delay() +
//                    (le->p->Size() * 8) / lk->Bandwidth();
//                // We need to transmit using same rxEvent to use same
//                // packet ber flags
//                Interface* peerIf = lk->GetPeer(le->p);
//                le->event = LinkEvent::PACKET_RX; // Convert to normal rx
//                lk->Transmit(le, peerIf, rxTime);
//                //lk->Transmit(le->p, iflist[k], n);  //Xold way
//                foundIt = true;
//                if (le->p->nixVec)
//                  { // has a nixvector, extract the bits even though not used
//                    le->p->nixVec->Extract(
//                        le->p->nixVec->BitCount(n->NeighborCount()));
//                  }
//                le = nil; // Don't delete
//                break;
//              }
//          }
//        if (!foundIt)
//          {
//            cout << "HuH?  Satellite can't forward packet?" << endl;
//            delete le->p;
//          }
//#endif
//        GetL2Proto()->DataIndication(le->p, le); // Pass fb-rx event to l2proto
//      }
//    break;
    case LinkEvent::NOTIFY :
      DEBUG(0,(cout << "Got notify event" << endl));
      // Inform the link object that  link is free
      CallNotification(true);
      break;
    case LinkEvent::LINK_FAIL:
      DEBUG(0,(cout << "Got link fail event" << endl));
      down=true;
      delete le;
      break;
    case LinkEvent::LINK_RESTORE:
      DEBUG(0,(cout << "Got link restore event" << endl));
      down=false;
      delete le;
      break;
    default:
      DEBUG(0, (cout << "Huh!! unknown link event"<< endl));
    }
}

void InterfaceReal::HandleLLCSNAP(Packet* p, bool fbrx)
//
{
  /* Time for the LLC/SNAP*/
  LLCSNAPHeader* llcsnap = (LLCSNAPHeader*)p->PopPDU(); 
  DEBUG(2,(cout << "LLCSnap ind Layer " << llcsnap->Layer()
           << " version " << llcsnap->Version()
           << endl));
  
  L3Protocol* l3 = (L3Protocol*)pNode->LookupProto(3, llcsnap->snap_ethtype);
  if (l3)
    {
      DEBUG0((cout << "Found l3 " << llcsnap->snap_ethtype
              << " in proto graph" << endl));
      l3->DataIndication(this, p);
    }
  else
    {
      cout << "Oops! Can't find l3 proto " << llcsnap->snap_ethtype
           << " in InterfaceReal::HandleLLCSNAP"
           << " at node " << pNode->Id()
           << endl;
      delete p;
    }
}

void InterfaceReal::Notify(void* v)
//通知队列空间是否充足
{ 
  if (v) delete (Time_t*)v;
  if (IsDown())
    { //若接口关闭，清空队列并丢弃所有的包
      while(true)
        {
          Packet* p = pQueue->Deque();
          if (!p) break; // no more
          DropPDU d("L2-ID", p);
          pNode->TracePDU(nil, &d);
          Stats::pktsDropped++; // Count dropped packets
          delete p;
        }
    }
  if (pL2Proto->Busy()) return;
  Packet* p = pQueue->Deque();
  if (p)
    {
      pL2Proto->DataRequest(p);
    }
  CallNotification(false); //发送队列空间充足的通知
}

Count_t  InterfaceReal::PeerCount() const
//返回与该接口相连的结点个数
{ 
  return pLink->PeerCount();
}

IPAddr InterfaceReal::PeerIP(int npeer) const
//根据结点index查找其IP地址并返回，若不存在则返回空
{ 
  return pLink->PeerIP(npeer);
}

IPAddr InterfaceReal::NodePeerIP(Node* n) const
//根据结点查找它与本接口相连的接口，并将其IP地址返回，若不存在则返回空
{ 
  return pLink->NodePeerIP(n);
}

bool InterfaceReal::NodeIsPeer(Node* n) const
//检查结点是否是与本接口直接相连的结点，若是则返回真
{ 
  if (!pLink) return false;
  return pLink->NodeIsPeer(n);
}

Count_t  InterfaceReal::NodePeerIndex(Node* n) const
//获取结点的index
{ 
  return pLink->NodePeerIndex(n);
}

void  InterfaceReal::SetNode(Node* n)
//设置本接口连接的结点
{
  pNode = n;
  /*if (arpmodule) pNode->InsertProto(3, arpmodule->Proto(), arpmodule);*/
}

#ifdef MOVED_TO_BASIC
Link* InterfaceReal::GetLink() const
{
  return pLink;
}

void InterfaceReal::SetLink(Link* pl)
{
  pLink = pl;
}
#endif

Queue* InterfaceReal::GetQueue() const
//返回与本结点相连的队列对象
{
  return pQueue;
}
  
void InterfaceReal::SetQueue(const Queue& q)
//设置一个新的队列对象
{
  if (pQueue) delete pQueue; 
  pQueue = q.Copy();         
  pQueue->SetInterface(this);
}

bool InterfaceReal::QueueDetailed() const
//若队列已被定义，则返回true，否则返回flase
{
  if (pQueue) return pQueue->Detailed();
  return false;
}

void InterfaceReal::SetL2Proto(const L2Proto& l2)
//设置与本接口相连的第二层协议对象
{
  pL2Proto = l2.Copy();
  pL2Proto->SetInterface(this);
}

void InterfaceReal::AddNotify(NotifyHandler* n,void* v)
//添加一个notify对象，用来通知与本接口相连的链路是空闲的
{ 
  DEBUG(1,(cout << "IFReal " << this << " AddNotify handler " << n << endl));
  notifications.push_back(Notification(n,v));
  if (!notifEvent && !pQueue->Detailed())
    { 
      Time_t qd = pQueue->QueuingDelay(); 
      notifEvent = new LinkEvent(LinkEvent::NOTIFY);
      Scheduler::Schedule(notifEvent, qd, this);
    }
}

void InterfaceReal::CancelNotify(NotifyHandler* n)
//删除一个notify通知
{
  DEBUG(1,(cout << "iface " << this 
           << " Node " << pNode
           << " CancelNotify handler " << n << endl));
  for (NList_t::iterator i = notifications.begin();
       i != notifications.end(); ++i)
    {
      if (i->handler == n)
        { // Found it
          DEBUG(1,(cout << "iface " << this << " found notif " << n << endl));
          i->handler = nil;
          // return; // Keep looking in case more than one
        }
    }
}

MACAddr InterfaceReal::IPToMac(IPAddr_t i) const
//根据远程结点的IP地址，返回其MAC地址 
{ 
  if (pLink) return pLink->IPToMac(i);
  return MACAddr::NONE; 
}

Count_t InterfaceReal::NeighborCount(Node* n) const
//通过查询与本接口相连的链路来获取路由上邻接结点的个数
{
  if(pLink) return pLink->NeighborCount(n);
  return 0;
}

void  InterfaceReal::Neighbors(NodeWeightVec_t& nwv, bool forceAll)

/*
描述：获取一串与本接口直接相连的邻接结点
参数：nwv——存放所求邻接结点的数组对象
      forceAll——表示所有的结点都要考虑（考虑到Ethernet的情况）
*/
{ 
  if (pLink) pLink->Neighbors(nwv, this, forceAll);
}

IPAddr_t InterfaceReal::DefaultPeer()
//返回绑定本接口的默认结点IP地址，若没有通过链路默认结点IP地址
{
  if (defaultPeer != IPADDR_NONE)
    {
      return defaultPeer;
    }
  if (pLink) defaultPeer = pLink->DefaultPeer(this);
  return defaultPeer;
}

bool InterfaceReal::IsLocalIP(IPAddr_t i) const
//判断指定IP是否与本接口在同一子网
{ 
  DEBUG0((cout << "Checking local ip " << Hex8(i)
          << " ip " << Hex8(ip)
          << " mask " << Hex8(mask) << endl));
  if ((ip & mask) == (i & mask)) return true;
  // Check remote IP's (used only for distributed simulations)
  ////for (IPMaskVec_t::size_type j = 0; j < remoteIPList.size(); ++j)
  ////  {
  ////    const IpMaskCount& ipm = remoteIPList[j];
  ////    if ((ipm.ip & ipm.mask) == (i & ipm.mask)) return true;
  ////  }
  return false;
}

Mult_t InterfaceReal::TrafficIntensity() const
//计算本接口的业务量强度(每秒传输数据量/链路带宽)
{ 
  DCount_t maxBytes = Simulator::Now() * pLink->Bandwidth() / 8;
  return totBytesSent / maxBytes;
}

////void InterfaceReal::AddRemoteIP(IPAddr_t i, Mask_t m, Count_t c)
////{
////  remoteIPList.push_back(IpMaskCount(i, m, c));
////}
////
void InterfaceReal::PacketRxStart(LinkEvent*)
{
}

void InterfaceReal::PacketRxEnd(bool, Size_t)
{
}

void InterfaceReal::CallNotification(bool resched)
{
  while(1)
    {
      if (notifications.empty())
        {
          if (notifEvent && resched)
            { // Delete pending event
              delete notifEvent;
              notifEvent = nil;
            }
          return;       // No pending
        }
      Notification n = notifications.front();  // Get next notifier
      if (n.handler)
        { // Handler exists
          notifications.pop_front();           // And remove it
          DEBUG(1,(cout << "IFace " << this
                   << " Node " << pNode
                   << " Notifying " << n.handler << endl));
          n.handler->Notify(n.userData);       // Call the notification handler
          break;
        }
      else
        {
          notifications.pop_front();           // Remove null handler, continue
        }
    }
  // If any notifications remaining, need to reschedule notification timer
  if (resched)
    { // Need to schedule another
      Time_t qd = pQueue->QueuingDelay(); // Time to drain queue
      if (!notifEvent)
        notifEvent = new LinkEvent(LinkEvent::NOTIFY);
      Scheduler::Schedule(notifEvent, qd, this);
    }
}

