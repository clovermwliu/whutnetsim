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
/* 2010.1.4  ����Ƽ                                                     */
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
//����һ��Ĭ�ϵ���·
{
  ConstructorHelper(Link::DefaultRate(), Link::DefaultDelay());
}

LinkReal::LinkReal(Rate_t b, Time_t d)
    : Link(b, d), notify(nil), linkMonitor(nil)
//����һ����·��ָ���������ʱ
{
  ConstructorHelper( b, d);
}

LinkReal::LinkReal(const LinkReal& c)
    : Link(c), notify(c.notify), linkMonitor(nil)
//����ָ����·���󣬴���һ������·
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
//��������
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
������ͨ��һ����·�¼��������ݰ�
������evRx�������ڵ���·�¼�ָ��
	  i�����������ݰ��Ľӿ�ָ��
	  rxTime�����������ݰ���ʱ��
����ֵ��true��ʾ����ɹ���false��ʾ���ݰ���ʧ
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
�������������ݰ������Ƕ����ӳ�
������p������Ҫ��������ݰ�
	 extraDelay�����ӳ�
����ֵ��true��ʾ����ɹ���false��ʾ���ݰ���ʧ
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
������������·�ϵ��¼��б�
������e������������¼�
	   ������ǰ����ʱ��
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
�������߲�Э����������·����ʱ����֪ͨ�����������б������һ��handler
������n�������ӵ�֪ͨ��handler����
	 v������ָ�룬���ݸ�֪ͨ����
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
//����jitter����
{ 
  if (jitter) delete jitter;
  jitter = j.Copy();
}

Time_t LinkReal::NextFreeTime()
//������·��һ�ο��е�ʱ��
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
���������ݰ�������ɣ�������ز���
������b�������������������С
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
// ����֪ͨ�б��еĵ�һ��֪ͨ
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
�������ܣ����췢�ͺͽ������ݰ��¼�
������p���������͵����ݰ�
	  self_if����Link���󶨵�interface
	  n����Link���󶨵�node
	  i����Link����˵����󶨵�interface
	  bCast�����Ƿ���й㲥����
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

	busy = true; // ��Link��Ϊæµ
	Time_t txTime = ((Rate_t)p->Size() * 8) / Bandwidth(); // �����ݰ���С�ʹ���������ݰ�����ʱ��
	// ���켰�������ݰ������¼�
	LinkEvent* evTx = new LinkEvent(LinkEvent::PACKET_TX);//�������ݰ������¼�
	evTx->numberBytes = p->Size();
	Scheduler::Schedule(evTx, txTime, this);//���¼�����ȫ�ֵ��¼������У��¼���handlerΪLinkReal����
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

	Time_t rxTime = txTime + Delay() + jitter->Value();//�������ݰ�����ʱ��
	//Delay()Ϊ��·�Ĵ�����ʱ���ڶ���Linkʱ���ã�jitterΪ����Ŷ�ʱ��
	//Scheduler::Schedule(evRx, rxTime, i);
	// New way below...more efficient than old way
	// Pass this directly to the interface for inclusion in fifo event queue
	// This works since p2p and ethernet links are FIFO.  Packets are received
	// at the far end of the link in the order sent.

	Time_t* ptime = new Time_t(nextFreeTime);
	//Interface* self_if = n->GetIfByLink(this); // INEFFICIENT!
	AddNotify(self_if, ptime); // GFR Comment out later

	if (!bCast)
	{ //���㲥��ֻ���͵����ڵ�
		//������������ݰ������¼�
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
		LinkEvent* evRx = new LinkEvent(LinkEvent::PACKET_RX, p);//�������ݰ������¼�
		evRx->hasBitError = lost; // ֪ͨ������λ����
		//self_if->RegisterRxEvent(evRx);
		Transmit(evRx, i, rxTime); // ���ú����������ݰ����¼���handlerΪ����ڵ��interface
	}
	else
	{ // �����ݰ����͸���ǰLink�������ھ�
		NodeWeightVec_t nwv;
		AllNeighbors(nwv);//ȡ��Link�������ھ�
		//Node* thisNode = i->GetNode(); // For ignoring self
		for (NodeWeightVec_t::size_type j = 0; j < nwv.size(); ++j)
		{
			Node* node = nwv[j].node;
			Interface* iface = (Interface*)nwv[j].iface;//ȡ���ھӵ�interface
			if (!iface)
			{
				cout << "HuH?  Link broadcast has nil interface" << endl;
				continue;
			}
			if (node != n || RxBroadcast())
			{ // �����յ����Լ������Ĺ㲥���ݰ������򲻽���
				LinkEvent* evRx = new LinkEvent(LinkEvent::PACKET_RX, p->Copy());
				evRx->hasBitError = lost; // ֪ͨ������λ����
				Transmit(evRx, iface, rxTime);// ���ú����������ݰ����¼���handlerΪ�ھӽڵ��interface
			}
		}
		delete p; // ����ֻ������������ݰ���һ�����ݣ�������Ҫɾ���˶���
	}
	Stats::pktsTransmitted++;
	return true;
} 


LinkMonitor* LinkReal::AddLinkMonitor(const LinkMonitor& lm)
//Ϊ����·���һ��LinkMonitor����
{ 
  if (linkMonitor) delete linkMonitor;
  linkMonitor = lm.Copy();
  return linkMonitor;
}

void LinkReal::ConstructorHelper(Rate_t r, Time_t d)
//��·��Ա��ʼ��
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
//�������ݰ����շ�����
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

