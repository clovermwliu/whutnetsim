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

/************************************************************************/
/* 2010.1.4  关怡萍                                                     */
/***********************************************************************/
#include <math.h>
#include <stdlib.h>

//#define DEBUG_MASK 0x10
#include "G_debug.h"
#include "link.h"
#include "simulator.h"
#include "event.h"
#include "ratetimeparse.h"
#include "linkp2p.h"
#include "droptail.h"
#include "hex.h"
#include "linkmonitor.h"

using namespace std;

LinkReal::LinkReal()
    : Link(), notify(nil), linkMonitor(nil)
//创建一条默认的链路
{
  ConstructorHelper(Link::DefaultRate(), Link::DefaultDelay());
}

LinkReal::LinkReal(Rate_t b, Time_t d)
    : Link(b, d), notify(nil), linkMonitor(nil)
//创建一条链路，指定带宽和延时
{
  ConstructorHelper( b, d);
}

LinkReal::LinkReal(const LinkReal& c)
    : Link(c), notify(c.notify), linkMonitor(nil)
//根据指定链路对象，创建一条新链路
{ 
  busy = false;
  jitter = c.jitter->Copy();
  if (c.linkMonitor) linkMonitor = c.linkMonitor->Copy();
  bw = c.bw;
  delay = c.delay;
  weight = c.weight;
  bitErrorRate = c.bitErrorRate;
  bytesSent = 0;
  pktsSent = 0;
  utilizationStart= 0.0;
  debugVerbose = c.debugVerbose;
  DBG((Stats::linksCreated++));
}

LinkReal::~LinkReal()
//析构函数
{
  if (jitter) delete jitter;
  if (linkMonitor) delete linkMonitor;
}

LinkReal& LinkReal::operator=(const LinkReal& rhs)
{
  if (&rhs == this) return *this; // Self-Assignment
  cout << "Hello fro LR assignment" << endl;
  busy = false;
  jitter = rhs.jitter->Copy();
  if (rhs.linkMonitor) linkMonitor = rhs.linkMonitor->Copy();
  bw = rhs.bw;
  delay = rhs.delay;
  weight = rhs.weight;
  bitErrorRate = rhs.bitErrorRate;
  bytesSent = 0;
  pktsSent = 0;
  utilizationStart= 0.0;
  debugVerbose = rhs.debugVerbose;
  DBG((Stats::linksCreated++));
  return *this;
}

bool LinkReal::Transmit(LinkEvent* evRx, Interface* i, Time_t rxTime)
/*
描述：通过一个链路事件传输数据包
参数：evRx——存在的链路事件指针
	  i——接收数据包的接口指针
	  rxTime——接收数据包的时间
返回值：true表示传输成功，false表示数据包丢失
*/  
{
  if (useSeqEvents)
    { // Bypass central event list, using a fifo queue at receiver
      i->ScheduleRx(evRx,rxTime);
    }
  else
    { 
      Scheduler::Schedule(evRx, rxTime, i); 
    }
  return true;
}

bool LinkReal::TransmitWithDelay(Packet* p, Time_t extraDelay)
/*
描述：传输数据包，考虑队列延迟
参数：p——所要传输的数据包
	 extraDelay——延迟
返回值：true表示传输成功，false表示数据包丢失
*/
{
  // Schedule receipt of new packet
  // at time extraDelay + txTime + delay + jitter;
  Time_t txTime = ((Rate_t)p->Size() * 8) / Bandwidth(); // Time to transmit
  Time_t txDelay = Delay();
  Time_t jit = jitter->Value();
  DEBUG0((cout << "Link " << this
          << " transmitting with extraDelay " << extraDelay
          << " txtime " << txTime
          << " delay " << txDelay
          << " bandwidth " << Bandwidth() << endl));
  Time_t rxTime = extraDelay + txTime + txDelay + jit;
  InterfaceBasic* i = GetPeer(p);
  // Create a new packet receipt event
  LinkEvent* evRx = new LinkEvent(LinkEvent::PACKET_RX, p);
  if(bitErrorRate != 0.0)
    { // random loss requested
      Mult_t lossProb = 1.0 - pow(1.0 - bitErrorRate, (double)(p->Size() * 8));
      if (!berRng) berRng = new Uniform();
      Random_t rv = berRng->Value();
      evRx->hasBitError = (rv < lossProb);
    }
  DEBUG0((cout << "Scheduling RX interface " << i
          << " now " << Simulator::Now()
          << " rxTime " << rxTime
          << " time " << Simulator::Now() + rxTime << endl));
  i->ScheduleRx(evRx,rxTime);
  Stats::pktsTransmitted++;
  return true;
}

void LinkReal::Handle(Event* e, Time_t)
/*
描述：处理本链路上的事件列表
参数：e——待处理的事件
	   ——当前仿真时间
*/
{
  LinkEvent* le = (LinkEvent*)e;
  switch(le->event) {
  case LinkEvent::PACKET_TX :
    TransmitComplete(le->numberBytes);
    break;
  default:
    break;
  }
  delete le;
}

void LinkReal::ResetUtilization()
{ 
  bytesSent = 0;
  pktsSent = 0;
  utilizationStart = Simulator::Now();
}

Mult_t LinkReal::Utilization()
{
  Time_t now = Simulator::Now();
  if (now == utilizationStart) return 0.0; // Avoid divide by zero
  return (bytesSent * 8) / ((now - utilizationStart) * bw);
}

void LinkReal::AddNotify(NotifyHandler* n, void* v)
/*
描述：高层协议对象会在链路空闲时请求通知，本函数在列表中添加一个handler
参数：n——增加的通知的handler对象
	 v——空指针，传递给通知方法
*/
{
  if (notifications.empty())
    notifications.push_back(Notification(n,v));
  else {
    NList_t::iterator i;
    for (i = notifications.begin(); i != notifications.end(); ++i)
      if (*((Time_t*)v) < *((Time_t*)i->userData)) break;         
    notifications.insert(i, Notification(n,v));
  }
}

void LinkReal::Jitter(const Random& j)
//设置jitter参数
{ 
  if (jitter) delete jitter;
  jitter = j.Copy();
}

Time_t LinkReal::NextFreeTime()
//返回链路下一次空闲的时刻
{
  Time_t now = Simulator::Now();
  if (now > nextFreeTime)
    {
      nextFreeTime = now;
      return now;
    }
  return nextFreeTime;
}

void   LinkReal::TransmitComplete(Count_t b) 
/*
描述：数据包传输完成，设置相关参数
参数：b——所传输的数据量大小
*/
{
  DEBUG0((cout << "adding " << b << " to bytesSent " << bytesSent << endl));
  bytesSent += b;     // Count total bytes sent
  pktsSent++;         // Count total packets sent
  busy = false;       // No longer busy
  // The SendNotification() below may result in another packet being
  // transmitted, which may overwrite the notification request.  Therefore
  // we need to copy it locally before calling SendNotificatiion()
  NotifyHandler* copyNotify = notify;


  SendNotification(); // Notify the interface(s) link not busy

  if (copyNotify)
    {
      copyNotify->Notify(nil); // Notify sender that tx is complete
    }


  DEBUG0((cout << "LinkReal::TransmitComplete on link " << endl));
  if (debugVerbose) cout << "LinkReal::TXC time " << Simulator::Now() << endl;
}


void LinkReal::SendNotification()
// 处理通知列表中的第一个通知
{ 
  if (notifications.size() == 0) return;   // No pending
  Notification n = notifications.front();  // Get next notifier
  notifications.pop_front();               // And remove it
  n.handler->Notify(n.userData);           // Call the notification handler
}


#ifdef  TEST_BACKPLANE
Packet* TestImportExport(Packet* p);
#endif

bool LinkReal::TransmitHelper(Packet* p, Interface* self_if, Node* n, 
                              Interface* i, bool bCast)
/*
函数功能：构造发送和接受数据包事件
参数：p——待发送的数据包
	  self_if——Link所绑定的interface
	  n——Link所绑定的node
	  i——Link对面端点所绑定的interface
	  bCast——是否进行广播发送
*/
{
	bool lost = false;
	if(bitErrorRate != 0.0)
	{ // random loss requested
		Mult_t lossProb = 1.0 - pow(1.0 - bitErrorRate, (double)(p->Size() * 8));
		if (!berRng) berRng = new Uniform();
		Random_t rv = berRng->Value();
		lost = (rv < lossProb);
		//if (lost) cout << "Setting loss due to BER" << endl;
	}
	// Log with the link monitor if defined
	if (linkMonitor)
	{
		linkMonitor->Transmit(p);
	}


	// Note the object to notify when tx complete
	notify = p->notification;
	p->notification = nil;

	busy = true; // 将Link设为忙碌
	Time_t txTime = ((Rate_t)p->Size() * 8) / Bandwidth(); // 用数据包大小和带宽计算数据包发送时间
	// 构造及设置数据包发送事件
	LinkEvent* evTx = new LinkEvent(LinkEvent::PACKET_TX);//构造数据包发送事件
	evTx->numberBytes = p->Size();
	Scheduler::Schedule(evTx, txTime, this);//将事件放入全局的事件队列中，事件的handler为LinkReal本身
	DEBUG(4,(cout << "Link txTime " << txTime
		<< " bandwidth " << Bandwidth()
		<< " delay " << Delay() 
		<< " pktSize " << p->Size() 
		<< " pktUid " << p->uid << endl));
	if (debugVerbose) cout << "Link scheduling link free at " 
		<< Simulator::Now() << " for future "
		<< txTime << endl;
	if (debugVerbose) cout << "pktSize " << p->Size()
		<< " bw " << Bandwidth()
		<< endl;
	nextFreeTime = Simulator::Now() + txTime; // Note time link will be free

	Time_t rxTime = txTime + Delay() + jitter->Value();//计算数据包接收时间
	//Delay()为链路的传输延时，在定义Link时设置，jitter为随机扰动时间
	//Scheduler::Schedule(evRx, rxTime, i);
	// New way below...more efficient than old way
	// Pass this directly to the interface for inclusion in fifo event queue
	// This works since p2p and ethernet links are FIFO.  Packets are received
	// at the far end of the link in the order sent.

	Time_t* ptime = new Time_t(nextFreeTime);
	//Interface* self_if = n->GetIfByLink(this); // INEFFICIENT!
	AddNotify(self_if, ptime); // GFR Comment out later

	if (!bCast)
	{ //不广播，只发送单个节点
		//构造和设置数据包接收事件
#ifdef  TEST_BACKPLANE
		// For testing the backplane, export and import the message
		Packet* p1 = TestImportExport(p);
		p = p1;
#endif
		// If the peer interface is a fbrx, we need a first bit event
		if (i->FirstBitRx())
		{
			LinkEvent* evRxFirst =
				new LinkEvent(LinkEvent::PACKET_FIRSTBIT_RX, p);
			evRxFirst->hasBitError = lost;
			Scheduler::Schedule(evRxFirst, Delay(), i);
		}
		LinkEvent* evRx = new LinkEvent(LinkEvent::PACKET_RX, p);//构造数据包接收事件
		evRx->hasBitError = lost; // 通知可能有位错误
		//self_if->RegisterRxEvent(evRx);
		Transmit(evRx, i, rxTime); // 调用函数发送数据包，事件的handler为对面节点的interface
	}
	else
	{ // 将数据包发送给当前Link的所有邻居
		NodeWeightVec_t nwv;
		AllNeighbors(nwv);//取得Link的所有邻居
		//Node* thisNode = i->GetNode(); // For ignoring self
		for (NodeWeightVec_t::size_type j = 0; j < nwv.size(); ++j)
		{
			Node* node = nwv[j].node;
			Interface* iface = (Interface*)nwv[j].iface;//取得邻居的interface
			if (!iface)
			{
				cout << "HuH?  Link broadcast has nil interface" << endl;
				continue;
			}
			if (node != n || RxBroadcast())
			{ // 除非收到了自己发来的广播数据包，否则不接受
				LinkEvent* evRx = new LinkEvent(LinkEvent::PACKET_RX, p->Copy());
				evRx->hasBitError = lost; // 通知可能有位错误
				Transmit(evRx, iface, rxTime);// 调用函数发送数据包，事件的handler为邻居节点的interface
			}
		}
		delete p; // 我们只发送了这个数据包的一个备份，所以需要删掉此对象
	}
	Stats::pktsTransmitted++;
	return true;
} 


LinkMonitor* LinkReal::AddLinkMonitor(const LinkMonitor& lm)
//为本链路添加一个LinkMonitor对象
{ 
  if (linkMonitor) delete linkMonitor;
  linkMonitor = lm.Copy();
  return linkMonitor;
}

void LinkReal::ConstructorHelper(Rate_t r, Time_t d)
//链路成员初始化
{
  busy = false;
  if (defaultJitter)
    jitter = defaultJitter->Copy();
  else
    jitter = new Constant(0);
  bw = r;
  delay = d;
  weight = 1.0;
  bitErrorRate = 0.0;
  bytesSent = 0;
  pktsSent = 0;
  utilizationStart= 0.0;
  nextFreeTime = 0.0;
  debugVerbose = false;
  DBG((Stats::linksCreated++));
}

#ifdef  TEST_BACKPLANE
#include "backplane.h"
#include "tcp.h"
#include "ipv4.h"
#include "llcsnap.h"
#include "l2proto802.3.h"

#include "packet.h"
char*  bpBuf = nil;
int    bpBufSize = 0;

Packet* TestImportExport(Packet* p)
//测试数据包的收发功能
{
  DEBUG0((cout << "Hello from TestImportExport" << endl));
  if (!GetDSimHandle())
    { // Not initialized
      DSHandle_t dsimHandle = InitializeDSim(0);
      // Register the TCP and IP data items
      TCP::RegisterBackplane(dsimHandle);
      IPV4::RegisterBackplane(dsimHandle);
      cout << "Done Registering Items " << endl;
    }

  DEBUG0((cout << "Inquiring Message Size " << endl));
  int mSize = InquireMessageSize(); // Number of bytes needed
  DEBUG0((cout << "Message size is " << mSize << endl));
  if (bpBufSize < mSize)
    { // Existing buffer not big enough
      delete bpBuf;
      cout << "Allocating new bpbuf size " << mSize << endl;
      bpBuf = new char[mSize];
      bpBufSize = mSize;
    }
  DEBUG0((cout << "bpBuf is " << Hex8((unsigned long)bpBuf)
          << " size " << bpBufSize << endl));
  int outSize = mSize;
  DEBUG0((cout << "Exporting message" << endl));
#ifdef VERBOSE
  cout << "Exporting message, packet pdu size " << p->PDUs.size()
       << " pkt size " << p->Size()
       << " top " << p->top
       << endl;

  for (PDUVec_t::size_type k = 0; k < p->PDUs.size(); ++k)
    {
      PDU* pdu = p->PDUs[k];
      cout << "PDU " << k << " layer " << pdu->Layer()
           << " size " << pdu->Size() << endl;
    }
#endif
  ExportMessageWrapper((char*)p, nil, 0, bpBuf, &outSize);

  // Now import it back
  int outlth1 = 0; // no baggage
  Packet* p1 = new Packet();
  ImportMessageWrapper(bpBuf, outSize, (char*)p1,  nil, &outlth1);
  // The import creates the TCP and IP headers..now recreate the packet
  TCPHeader* tcp = TCP::bpHeader;
  IPV4Header* ip = IPV4::bpHeader;
  Size_t totalLength = ip->totalLength;
  Size_t dataLength = totalLength - tcp->Size() - ip->Size();
  // Add the data
  if (dataLength) p1->PushPDU(new Data(dataLength)); // Application data
  p1->PushPDU(tcp);                  // TCP Header
  p1->PushPDU(ip);                   // IP Header
  p1->PushPDU(new LLCSNAPHeader());  // LLCSNap header
  // L2 Header (802.3)
  p1->PushPDU(new L2802_3Header(MACAddr(), MACAddr(), p1->Size()));
#ifdef VERBOSE
  cout << "Imported message, packet pdu size " << p1->PDUs.size()
       << " pkt size " << p1->Size()
       << " top " << p1->top
       << endl;
  for (PDUVec_t::size_type k = 0; k < p1->PDUs.size(); ++k)
    {
      PDU* pdu = p1->PDUs[k];
      cout << "PDU " << k << " layer " << pdu->Layer()
           << " size " << pdu->Size() << endl;
    }
#endif
  delete p;
  // We need to nil out the backplane  headers for the next import
  TCP::bpHeader = nil;
  IPV4::bpHeader = nil;
  return p1;
}
#endif

