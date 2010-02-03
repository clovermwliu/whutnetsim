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
#include "G_debug.h"
#include "linkp2p.h"
#include "simulator.h"
#include "macaddr.h"
#include "qtwindow.h"
#include "eventcq.h"

#ifdef HAVE_QT
#include "GUI_Defs.h"
#endif

using namespace std;

Linkp2p* Linkp2p::defaultp2p; //默认p2p链路指针
Count_t  Linkp2p::count = 0;

Linkp2p::Linkp2p()
    : LinkReal(), pPeer(nil), linkAnimation(nil), usedAnim(0), used(false)
//创建一条链路
{
  count++;
}

Linkp2p::Linkp2p(Rate_t b, Time_t d)
    : LinkReal(b, d), pPeer(nil), linkAnimation(nil), usedAnim(0), used(false)
//创建一条链路，设定其带宽和延时
{ 
  count++;
}

Linkp2p::Linkp2p(const Linkp2p& c) : LinkReal(c)
//根据指定链路对象创建一条链路
{
  pPeer = nil;  // Don't copy the peer or local pointers pointer
  linkAnimation = nil;
  usedAnim = 0;
  count++;
}

Linkp2p::~Linkp2p()
{
  count--;
#ifdef HAVE_QT
	//if (used)
	//delete linkAnimation;
#endif
}

Link* Linkp2p::Copy() const
//复制该链路并返回
{
  return new Linkp2p(*this);
}

bool Linkp2p::Transmit(Packet* p, Interface* i, Node* n)
/*
描述：通过本链路传输一个数据包
参数：p——所要传输的数据包
	 i——传输数据包的本地接口指针
	 n——传输数据包的本地结点
返回值：true表示传输成功，false表示数据包丢失
*/
{
  DEBUG0((cout << "Linkp2p::Transmit to peer " << GetPeer(p) << endl));
  return TransmitHelper(p, i, n, (Interface*)GetPeer(p));
}

bool Linkp2p::Transmit(Packet* p, Interface* i, Node* n, Rate_t)
/*
描述：通过本链路传输一个数据包
参数：p——所要传输的数据包
	 i——传输数据包的本地接口指针
	 n——传输数据包时的带宽
返回值：true表示传输成功，false表示数据包丢失
*/
{ 
  return Transmit(p, i, n);
}

MACAddr Linkp2p::IPToMac(IPAddr_t)
//根据结点IP地址获取其MAC地址并返回
{ 
  if (pPeer) return ((Interface*)pPeer)->GetMACAddr();
  return MACAddr::NONE; // Else none
}

Count_t Linkp2p::NeighborCount(Node* n)
{ // Get number of routing neighbors (always 1 for p2p links)
  return 1;
}

void Linkp2p::Neighbors(NodeWeightVec_t& nwv, Interface* i, bool)
/*
描述：向包含所有路由邻接结点的数组中根据本地接口添加与之对应的邻接结点
参数一：[OUT]，返回的包含所有路由邻接结点的数组
参数二：与邻接结点交互的本地接口
参数三：true代表包含所有的结点
*/
{ 
  nwv.push_back(NodeIfWeight(pPeer->GetNode(), i, Weight()));
}

void Linkp2p::AllNeighbors(NodeWeightVec_t& nwv)
//获取该链路的所有邻接结点
{ 
  nwv.push_back(NodeIfWeight(pPeer->GetNode(), pPeer, Weight()));
}

Count_t  Linkp2p::PeerCount() 
//返回本链路连接的结点数（real中未实现，ghost中返回1）
{
  return 1; 
}

IPAddr_t Linkp2p::PeerIP(Count_t npeer)
//根据结点index返回其IP地址，若不存在返回空
{
  if (npeer > 0)
    return IPADDR_NONE;    
  return ((Interface*)pPeer)->GetIPAddr();
}

IPAddr_t Linkp2p::NodePeerIP(Node* n)  
//返回与本链路相连的指定结点的ip地址
{ // Find the peer's ip address for peer n on this link
  // For point to point, there is only one peer, so it must match
  Node* peerNode = pPeer->GetNode();
  if (n != peerNode)
    return IPADDR_NONE;      // Oops, not correct peer
  // Return one and only peer's ip address  
  return ((Interface*)pPeer)->GetIPAddr();
}

bool     Linkp2p::NodeIsPeer(Node* n)
//判断指定结点是不是与本链路相连的一个结点，如果是，返回true
{ 
  Node* peerNode = pPeer->GetNode();
  return (peerNode == n);
}

Count_t  Linkp2p::NodePeerIndex(Node* n)
//根据指定结点，返回其index
{
  return 0; 
}

IPAddr_t Linkp2p::DefaultPeer(Interface*)
//返回与本链路指定接口相连的结点的ip地址
{ 
  return PeerIP(0); // For p2p links, default is same as only
}

Linkp2p& Linkp2p::Default()
//返回默认的p2p链路指针
{ 
  if (!defaultp2p) defaultp2p = new Linkp2p(); // Allocate it
  return *defaultp2p;                           // Return it
}

Linkp2p& Linkp2p::Default(const Linkp2p& l)    
//设置新的默认的p2p链路指针
{
  if (defaultp2p) delete defaultp2p;           // Delete if already exists
  defaultp2p = (Linkp2p*)l.Copy();             // Set new one
  return *defaultp2p;
}

bool Linkp2p::ScheduleFailure(Time_t t)
//设置在特定仿真时间t本链路失效，若设置成功返回true
{
  LinkEvent* evFail = new LinkEvent (LinkEvent::LINK_FAIL, nil);
  Scheduler::Schedule(evFail, t, pPeer);
  return true;
}


bool Linkp2p::ScheduleRestore(Time_t t)
//设置在特定仿真时间t本链路重新激活，若设置成功返回true
{
  LinkEvent* evRestore = new LinkEvent (LinkEvent::LINK_RESTORE, nil);
  Scheduler::Schedule(evRestore, t, pPeer);
  return true;
}


void Linkp2p::AnimateLink()
//在动画显示时画出该链路
{
#ifdef HAVE_QT
  InterfaceBasic* pLocal = GetLocalIf();
  if (!pPeer || !pLocal) return; // Can't animate without interfaces
  QTWindow* qtw = QTWindow::qtWin;
  if (!qtw) return;
  Node* s = pLocal->GetNode();   // Local node
  Node* d = pPeer->GetNode();    // Remote node
  MyPoint fp = qtw->NodeLocation(s);
  MyPoint tp = qtw->NodeLocation(d);
  if (!linkAnimation)
    {
      // Create a new canvas item
      linkAnimation = new MyCanvasLine(qtw->Canvas());
	  used = true;
    }
  
  linkAnimation->show();
  linkAnimation->setPoints(fp.x(), fp.y(), tp.x(), tp.y());
  // If interface is down in either direction color it red
  if (pLocal->IsDown() || pPeer->IsDown())
    {
      linkAnimation->setPen(QPen(Qt::red));
    }
#endif
}

void Linkp2p::ReAnimateLink()
{
}

void Linkp2p::AnimatePackets()
//画出在该链路上传输的数据包
{
#ifdef HAVE_QT
  InterfaceBasic* pLocal = GetLocalIf();
  if (!pPeer || !pLocal) return; // Can't animate without interfaces
  QTWindow* qtw = QTWindow::qtWin;
  if (!qtw) return;
  // First hide any existing packets
  for (PacketVec_t::size_type i = 0; i < usedAnim; ++i)
    {
      animatedPackets[i]->hide();
    }
  usedAnim = 0; // Reset to none used
  // The destination interface has a list of pending packets
  EventCQ* pending = pPeer->GetEventCQ();
  if (!pending) return; // No event cq yet, so no packets
  while(qtw->PlaybackMode() && !pending->empty())
    { // remove any rx events n the past
      EventPair* ep = pending->front();
      if (ep->time > Simulator::Now()) break; // Received in future, keep
      pending->pop_front();       // Remove this one
      LinkEvent* evRx = (LinkEvent*)ep->event;
      DEBUG0((cout << "QTW removing evRx time " << ep->time
              << " now " << Simulator::Now() << endl));
      delete evRx->p;
      delete evRx;
    }
  DEBUG0((cout << "AMP, Time " << Simulator::Now()
          << " local " << pLocal << " peer " << pPeer
          << " pending.size() " << pending->size() << endl));
  
  if (pending->empty()) return; // No packets

  for (Count_t j = 0; j < pending->size();  ++j)
    {
      EventPair* ev = (*pending)[j];
      LinkEvent* le = (LinkEvent*)(ev->event);
      // Ignore first bit events for satellite interfaces
      if (le->event != LinkEvent::PACKET_RX) continue;
      Packet*    p = le->p;
      MyColor     c(Qt::blue);
      // See if the packet has a color assigned
      if (p->IsColored())
        {
          c = MyColor(p->R(), p->G(), p->B());
        }
      else
        { // Display BER pkts in red if not other color specified
          if (le->hasBitError) c = Qt::red;
        }
          
      // The timestamp in the event is last bit received.
      // We also need first bit time.
      Count_t    pSize = p->Size()*8; // Size in bits
      pSize -= pSize/10; // Shorten a bit to show breaks between packets
      Time_t     lastBitTime  = ev->time;
      Time_t     firstBitTime = ev->time - (double)(pSize)/Bandwidth();
      // Check if Pkt sent in future (playback). ignore
      if ((firstBitTime - Delay()) > Simulator::Now())
        {
          DEBUG0((cout << "Ignoring pkt sent in future" << endl));
          break;
        }

      // Check if last bit received in the past (playback), ignore
      if (lastBitTime < Simulator::Now())
        {
          DEBUG0((cout << "Ignoring pkt received in past" << endl));
          break;
        }

      if (firstBitTime < Simulator::Now()) firstBitTime = Simulator::Now();
      Time_t fbFuture = firstBitTime - Simulator::Now();
      Time_t lbFuture = lastBitTime - Simulator::Now();
      if (lbFuture > Delay()) lbFuture = Delay();
      // Compute fraction of the link delay for first and last bit times.
      // This is the "distance" along the link line for the display
      Mult_t dfb = 1.0 - fbFuture/Delay();
      Mult_t dlb = 1.0 - lbFuture/Delay();
      DEBUG0((cout << "Displaying pkt, dfb " << dfb 
              << " dlb " << dlb << endl));
      Node* s = pLocal->GetNode(); // Source node
      Node* d = pPeer->GetNode();  // Destination node
      DisplayOnePacket(s, d, dfb, dlb, c); // Display the packet
    }
#endif
}

void Linkp2p::DisplayOnePacket(Node* s, 
							   Node* d,
                               Mult_t firstBit, 
							   Mult_t lastBit,
                               const MyColor& c)
/*
描述：显示单个的数据包动画
参数：s——发送数据包的结点
	 d——接收数据包的结点
	 firstBit——第一bit位坐标转化系数
	 lastBit——最后bit位坐标转化系数
	 c——数据包显示时的颜色
*/
{
#ifdef HAVE_QT
  QTWindow* qtw = QTWindow::qtWin;
  MyPoint sp = qtw->NodeLocation(s); // Get location of source
  MyPoint dp = qtw->NodeLocation(d); // Get location of destination
  // Compute length of line connecting src/dst
  double dx = dp.x() - sp.x();
  double dy = dp.y() - sp.y();
  DEBUG(2,(cout << " sp.x " << sp.x() << " sp.y " << sp.y()
           << " dp.x " << dp.x() << " dp.y " << dp.y()
           << " dx " << dx << " dy " << dy << endl));
  double dist = sqrt(dx*dx + dy*dy);
  DEBUG(2,(cout << "Distance node " << s->Id() << " to " << d->Id() 
           << " is " << dist << endl));
  // Compute the angle of a line from src to dst
  double theta = atan2(dy, dx);
  DEBUG(2,(cout << "Angle is " << 360.0 * theta / ( 2.0 * M_PI)
           << " degrees" << endl));
  bool verbose = (theta == 0.0);
  verbose = false;
  
  double sinTheta = sin(theta);
  double cosTheta = cos(theta);
  double pxfb = sp.x() + dist * firstBit * cosTheta;
  double pyfb = sp.y() + dist * firstBit * sinTheta;
  double pxlb = sp.x() + dist * lastBit  * cosTheta;
  double pylb = sp.y() + dist * lastBit  * sinTheta;
  // Use MyCanvasPolygon for packets
  MyPointArray qpa(5); // Four Points for rectangle, plus closing point
  // Compute points 2 pixels left and right of specified
  // Also translate the packet so we are 4 pixels "to the right"
  // of the link line, allowing bi-directional packets to not
  // interfere with each other.
  double sinThetaM2 = sin(theta + M_PI_2);
  double cosThetaM2 = cos(theta + M_PI_2);
  double width = 2.0;

  double pxlfb = pxfb - width * sinTheta + 4 * cosThetaM2;
  double pylfb = pyfb + width * cosTheta + 4 * sinThetaM2;
  double pxrfb = pxfb + width * sinTheta + 4 * cosThetaM2;
  double pyrfb = pyfb - width * cosTheta + 4 * sinThetaM2;
  qpa.setPoint(0, (int)pxlfb, (int)pylfb);
  qpa.setPoint(1, (int)pxrfb, (int)pyrfb);
  double pxllb = pxlb - width * sinTheta + 4 * cosThetaM2;
  double pyllb = pylb + width * cosTheta + 4 * sinThetaM2;
  double pxrlb = pxlb + width * sinTheta + 4 * cosThetaM2;
  double pyrlb = pylb - width * cosTheta + 4 * sinThetaM2;
  qpa.setPoint(2, (int)pxrlb, (int)pyrlb);
  qpa.setPoint(3, (int)pxllb, (int)pyllb);
  qpa.setPoint(4, (int)pxlfb, (int)pylfb); // Close the polygon

  // Get a canvas polygon to use
  if (animatedPackets.size() == usedAnim)
    {
      animatedPackets.push_back(new MyCanvasPolygon(qtw->Canvas()));
    }
  MyCanvasPolygon* cp = animatedPackets[usedAnim];
  cp->setPen(QPen(Qt::black));
  cp->setBrush(c);
  cp->show();
  cp->setPoints(qpa);
  usedAnim++;
#endif
}

