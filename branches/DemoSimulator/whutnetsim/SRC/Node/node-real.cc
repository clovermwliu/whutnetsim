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
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef HAVE_QT
#include "GUI_Defs.h"
#include "qtwindow.h"
#endif

#include "G_debug.h"
#include "protograph.h"
#include "portdemux.h"
#include "ipaddr.h"
#include "duplexlink.h"
#include "application.h"
#include "mobility.h"
//#include "link-rti.h" v1.01
#include "mask.h"
#include "interface-real.h"
//#include "interface-wireless.h"  v1.01
#include "node-real.h"
#ifdef HAVE_QT //v1.01
#include "image.h"
#endif
//#include "wlan.h"  v1.01

#define NOT_FOUND -1

using namespace std;

Count_t NodeReal::count = 0;
Count_t NodeReal::count1 = 0;
Count_t NodeReal::count2 = 0;
Count_t NodeReal::count3 = 0;
Count_t NodeReal::count4 = 0;

NodeReal::NodeReal(Node* n)
    : NodeImpl(n),
      //pRoutingAODV(nil),
      x(0.0), y(0.0), hasLocation(false),
#ifndef COMPACT
      graph(0),
#endif
      traceStatus(Trace::DEFAULT), mobility(nil),userInfo(nil),
      demux(0), radioRange(0), battery(INFINITE_VALUE), computePower(0),
      //radioMarginTime(0),
#ifdef HAVE_QT
      animInfo(nil), wirelessColor(Qt::black), 
#endif
        icmpEnabled(true),
      timeout(DEFAULT_SW_TIMEOUT), isswitch(false)
      //wormcontainment(nil)
{
  pRouting = Routing::Default()->Clone(); // Copy of default routing object
  DEBUG0((cout << "Node " << this << " prouting " << pRouting << endl));
  pRouting->node = n;    // yj
  //rtable = new RouteTable();
  count++;
}

NodeReal::~NodeReal()
{
	if (mobility) delete mobility;
	if (pRouting) delete pRouting;
#ifndef COMPACT
	 if (graph) delete graph;
#endif
	 if (demux) delete demux;
	//if(wormcontainment) delete wormcontainment;
	 count--;
	 NodeImpl::~NodeImpl();
}


NodeImpl* NodeReal::Copy() const
{
  return new NodeReal(*this);
}

bool NodeReal::IsReal()
//对Real型结点为真
{
  return true;
}

void NodeReal::Broadcast(Packet* p, Proto_t proto)
 //通过所有接口广播一个数据包
{ //通过所有接口发送数据包
  const IFVec_t ifVec = Interfaces();// 该结点所有接口的数组
  MACAddr macBC;
  macBC.SetBroadcast(); //设置广播的目的地
  for (IFVec_t::size_type i = 0; i < ifVec.size(); ++i)
    {
      Interface* iface = ifVec[i];
      if (iface->GetLink())//确保该接口与外部结点相通
        { 
          iface->Send(p->Copy(), (Word_t)proto);
#ifdef OLD_SUPERCEEDED
          L2Info802_3 l2Info(iface->GetMACAddr(),   // Src MAC Addr
                             macBC,                 // Broadcast
                             proto);                // Protocol
          L2Proto* l2proto = iface->GetL2Proto();   // Get l2 protocol obj
          l2proto->DataRequest(iface, p->Copy(), &l2Info);  // Pass down to l2
#endif
        }
    }
  delete p; //由于在每个接口都复制了一份数据包，因此要删除p
}

Queue* NodeReal::GetQueue()
//获取本地结点的队列，仅对单接口的结点有效，该函数为函数Queue* GetQueue(Node* n)服务
{ 
  Count_t ifCount = InterfaceCount();
  if (!ifCount) return nil;         // No interfaces
  if (ifCount > 1) return nil;      // Multiple interfaces
  return interfaces[0]->GetQueue(); //返回唯一接口的队列
}

Queue* NodeReal::GetQueue(Node* n)
//获取本地结点通往指定远程结点的队列
{ 
  Interface* i = GetIfByNode(n);
  if (!i) return nil;   //本地结点与远程结点n不连通
  return i->GetQueue(); //返回指定的队列
}

Application* NodeReal::AddApplication(const Application& a)
//给本地结点添加指定应用
{
  
  Application* pa = a.Copy(); //复制一份应用a的对象
  pappvec.push_back(pa);
  pa->AttachNode(pNode);//给本地结点添加应用     
  return pa;
}

void NodeReal::DefaultRoute(RoutingEntry r)
//指定发送数据包的默认路由项
{  //首先确定指定结点在本地结点存在
  for (IFVec_t::iterator i = interfaces.begin(); i != interfaces.end(); ++i)
    {
#ifndef WIN32
      if ((*i) == r.interface)
#else
      if ((*i) == r.interface_)
#endif
        { //找到
          pRouting->Default(r);
          return;
        }
    }
#ifndef WIN32
  
  DEBUG0(cout << "Ignoring Default Route for Node " << pNode->Id()
         << " interface " << r.interface
         << " not present" << endl);
#else
  DEBUG0(cout << "Ignoring Default Route for Node " << pNode->Id()
         << " interface " << r.interface_
         << " not present" << endl);
#endif

  pRouting->Default(r);
}

void NodeReal::DefaultRoute(Node* n)
//指定发送数据包的默认下一跳结点
{ 
  RoutingEntry r;
#ifndef WIN32
  r.interface = GetIfByNode(n);
DEBUG0((cout << "NR::DefRoute, if " << r.interface << endl));
  if (!r.interface) return; // Nope, not a routing neighbor
#else
  r.interface_ = GetIfByNode(n);
  DEBUG0((cout << "NR::DefRoute, if " << r.interface_ << endl));
  if (!r.interface_) return; // Nope, not a routing neighbor

#endif
  
  Interface* peerIFace = n->GetIfByNode(pNode);
  if (peerIFace) r.nexthop = peerIFace->GetIPAddr();
  DEBUG0((cout << "NR::DefRoute, nh " << (string)IPAddr(r.nexthop) << endl));
  pRouting->Default(r);
}

void NodeReal::AddRoute(IPAddr_t ip, Count_t mask, Interface* i, IPAddr_t ip1)
//增加一个路由项，目的IP及其掩码、本地接口、下一跳IP地址确定
{ 
  pRouting->Add(ip, mask, i, ip1);
}

RoutingEntry NodeReal::LookupRoute(IPAddr_t ip)
//根据目的IP地址查找本地路由项，若存在则返回
{ 
  RoutingEntry r;
#ifndef WIN32
  r.interface = LocalRoute(ip);
  if (r.interface)
    { // Found it
      r.nexthop =  ip;
      return r;
    }
#else
  r.interface_ = LocalRoute(ip);
  if (r.interface_)
    { // Found it
      r.nexthop =  ip;
      return r;
    }
#endif
  // Not local subnet, find a route
  // First see if we only have one interface and one neighbor
  if (InterfaceCount() == 1)
    { // See if only one peer on this interface
      Interface* i = interfaces[0];
      if (i->NeighborCount(pNode) <= 1)
        { // Ok, just use the single peer
          return RoutingEntry(i, i->DefaultPeer());
        }
    }
  // Multiple choices, lookup in the FIB
  r = pRouting->Lookup(pNode, ip);
  return r;
}

RoutingEntry NodeReal::LookupRouteNix(Count_t nix)
//查找NIX路由中的路由项
{
#ifdef OLD_WAY
  // First see if cached
  RENixMap_t::iterator k = nixCache.find(nix);
  if (k != nixCache.end())
    { // Found it,
      return k->second;
    }
  // Need to calculate (less efficient)
  Count_t cnc = 0;      // Cumulative neighbor count
  // Find which interface has the specified nix
  for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
    {
      Count_t nc = interfaces[i]->NeighborCount();
      Count_t priorCnc = cnc;
      cnc += nc; // Advance cumulative neighbor count
      if (nix < cnc)
        { // Yes, it's on this interface
          RoutingEntry re;
          re.interface = interfaces[i];
          re.nexthop   = interfaces[i]->PeerIP(nix-priorCnc);
          // insert in the cache for later reuse
          nixCache[nix] = re;
          return re;
        }
    }
  return RoutingEntry(); // HuH? Not found
#endif
  // See if the lookup vector exists, create if not
  if (nixLookup.empty())
    { // build it
      for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
        {
          Count_t nc = interfaces[i]->NeighborCount(pNode);
          Interface* iface = interfaces[i];
          for (Count_t n = 0; n < nc; ++n)
            {
              nixLookup.push_back(RoutingEntry(iface, iface->PeerIP(n)));
            }
        }
    }
  if (nix >= nixLookup.size()) return RoutingEntry(); // HuH?
  return nixLookup[nix];
}

////AODVRoutingEntry* NodeReal::LookupRouteAODV(IPAddr_t ip)
////
////{
////  if (pRoutingAODV)
////    return pRoutingAODV->Lookup(pNode, ip);
////  return nil;
////}
////
////void NodeReal::SetRoutingAODV(void* aodv)
////{
////  pRoutingAODV = (RoutingAODV*)aodv;
////}
////
////RoutingAODV* NodeReal::GetRoutingAODV()
////{
////  return pRoutingAODV;
////}

Routing::RType_t NodeReal::RoutingType()
//获取本地结点当前所使用的路由类型
{
  return pRouting->Type();
}

void NodeReal::InitializeRoutes()
//本地路由初始化
{ 
  DEBUG0((cout << "Node " << pNode->Id() << " initializing routes, routing "
          << " prouting " << pRouting << " rtype "
          << pRouting->Type() << endl));
  pRouting->InitializeRoutes(pNode);
}

void NodeReal::ReInitializeRoutes(bool u)
//本地路由更新
{ 
  DEBUG0((cout << "Node " << pNode->Id() << " reinitializing routes, rtype "
          << pRouting->Type() << endl));
  pRouting->ReInitializeRoutes(pNode, u);
}

Count_t NodeReal::RoutingFibSize() const
//统计本地结点路由的大小
{ 
  return pRouting->Size();
}

RoutingNixVector* NodeReal::GetNixRouting()
//若本地结点存在NixVector类型路由，则返回该路由对象的指针，否则返回空
{ 
  if (RoutingType() == Routing::NIXVECTOR)
    {
      return (RoutingNixVector*)pRouting;
    }
  return nil; //若没有，返回空
}

// Protocol Graph Interface
#ifdef COMPACT
// No private protocol graphs in compact mode
Protocol* NodeReal::LookupProto(Layer_t l, Proto_t p)// Lookup protocol by layer
{
  return ProtocolGraph::common->Lookup(l,p);
}

void NodeReal::InsertProto(Layer_t, Proto_t, Protocol*) // Insert a protocol
{
  cout << "Private protocol graph not supported in compact mode" << endl;
  return;
}
#else
Protocol* NodeReal::LookupProto(Layer_t l, Proto_t proto)
//在本地结点，根据协议层和编号查找对应的协议对象
{
  Protocol* p = NULL;
  if (graph)//若存在本地结点协议族
    { 
      p = graph->Lookup(l,proto);//存在于本结点协议族
    }
  if (!p)
    {
      return ProtocolGraph::common->Lookup(l,proto);//是否存在于共享协议族
    }
  return p;
}

void NodeReal::InsertProto(Layer_t l, Proto_t proto, Protocol* p)
//在本地结点插入一个协议对象，.协议层和编号指定
{
  if (!graph) graph = new ProtocolGraph;
  graph->Insert(l, proto, p);
}
#endif

bool NodeReal::Bind(Proto_t proto, PortId_t port , Protocol* p)
/*
描述：为指定端口绑定指定协议
参数：proto——协议号
	  port——指定的端口
	  p——待绑定的协议
返回：是否绑定成功
*/
{
  if (!demux) demux = new PortDemux();
  return demux->Bind(proto, port, p);
}

bool NodeReal::Bind(Proto_t proto,
                    PortId_t lport, 
					IPAddr_t laddr,
                    PortId_t rport, 
					IPAddr_t raddr,
                    Protocol* p)
/*
描述：为本地结点指定端口和远程结点指定端口绑定指定协议
参数：proto——待绑定协议号
	  lport——本地端口号
	  laddr——本地IP地址
	  rport——远程端口号
	  raddr——远程IP地址
	  p——待绑定的协议
返回值：true——绑定成功
	    false——绑定失败
*/
{
  if (!demux) demux = new PortDemux();
  DEBUG0((cout << "Node " << pNode->Id() << " binding proto " << p
          << " to lport " << lport << " laddr " << (string)::IPAddr(laddr)
          << " to rport " << rport << " raddr " << (string)::IPAddr(raddr)
          << endl));
  return demux->Bind(proto, lport, laddr, rport, raddr, p);
}

PortId_t NodeReal::Bind(Proto_t proto, Protocol* p)
/*
描述：绑定指定协议，对应本地端口随机
参数：proto——协议号
	  p——待绑定协议
返回值：所绑定的本地结点的端口
*/
{ // Choose available port and register
  if (!demux) demux = new PortDemux();
  return demux->Bind(proto, p);
}

bool NodeReal::Unbind(Proto_t proto, PortId_t port, Protocol* p)
/*
描述：删除本地结点指定接口绑定的指定协议
参数：proto——待删除协议的协议号
	  port——当前绑定的本地端口号
	  p——待删除的协议
*/
{ 
  if (!demux) demux = new PortDemux();
  return demux->Unbind(proto, port, p);
}

bool NodeReal::Unbind(Proto_t proto,
                      PortId_t lport, 
					  IPAddr_t laddr,
                      PortId_t rport, 
					  IPAddr_t raddr,
                      Protocol* p)
/*
描述：本地结点指定端口和远程结点指定端口绑定指定协议
参数：proto——待绑定协议号
	  lport——本地端口号
	  laddr——本地IP地址
	  rport——远程端口号
	  raddr——远程IP地址
	  p——待绑定的协议
返回值：true——绑定成功
	    false——绑定失败
*/
{ 
  if (!demux) demux = new PortDemux();
  return demux->Unbind(proto, lport, laddr, rport, raddr, p);
}

Protocol* NodeReal::LookupByPort(Proto_t proto, PortId_t port)
/*
描述：根据本地端口和协议号查找所绑定的协议
参数：proto——协议号
	 port——本地端口号
返回值：若找到相应的协议，将其返回；否则返回空
*/
{
  if (!demux) demux = new PortDemux();
  return demux->Lookup(proto, port);
}

Protocol* NodeReal::LookupByPort(Proto_t proto,
                                 PortId_t sport, 
								 IPAddr_t sip,
                                 PortId_t dport, 
								 IPAddr_t dip)
/*
描述：查找协议，根据协议号、本地结点端口、IP地址、远程结点端口、IP地址
参数：proto——待绑定协议号
	  sport——本地端口号
	  sip——本地IP地址
	  dport——远程端口号
	  dip——远程IP地址
返回值：若找到相应的协议，将其返回；否则返回空
*/
{
  if (!demux) demux = new PortDemux();
  return demux->Lookup(proto, sport, sip, dport, dip);
}

bool NodeReal::TracePDU(Protocol* proto, PDU* h, Packet* p, char* s)
/*
描述：将指定PDU内容从指定协议记录到跟踪文件
参数：proto——请求路径的指定协议指针
	  h——PDU单元
	  p——包含此PDU单元的数据包
	  s——跟踪文件额外文本信息
返回值：若该跟踪文件存在，返回true；否则返回false
*/
{
  if (!CheckTrace(proto,h)) return false;//检查是否需要跟踪该对象
  Trace::Instance()->NewNode(pNode);//声明是哪个结点
  Bitmap_t bm;
  if (proto) bm = proto->Details();
  else   bm = ALLBITS;  //若协议为空，跟踪所有
  DEBUG(0,(cout << "NodeReal::TracePDU bm " << bm << endl));
  h->Trace(Trace::Instance()->GetTS(), bm, p, s);  // Trace the pdu
  return true;
}

void NodeReal::SetTrace(Trace::TraceStatus t)
//设置本地结点的跟踪状态
{
  traceStatus = t;
}

void NodeReal::SetLocation(Meters_t xl, Meters_t yl, Meters_t zl)
/*
描述：直接在xyz坐标系中设置本地结点的坐标
参数：xl——x轴坐标
	  yl——y轴坐标
	  zl——z轴坐标
*/
{
  x = xl;
  y = yl;
  z = zl;
  if (Simulator::instance)
    { 
      Simulator::instance->NewLocation(x,y);
    }
  hasLocation = true;
#ifdef HAVE_QT  
  if (animInfo) animInfo->show = true;
#endif
}

void NodeReal::SetLocation(const Location& l)
//根据location对象设置本地结点在xyz坐标系中的坐标
{
  SetLocation(l.X(), l.Y(), l.Z());
}

void NodeReal::SetLocationLongLat(const Location& l)
{ // Adjust the x (longitude) coordinate by cos(lat), using
  // a centerline projection.
  RectRegion& bb = Simulator::boundingBox;
  Meters_t centerLine =
      bb.LowerLeft().X() +
      (bb.UpperRight().X() - bb.LowerLeft().X()) / 2;

  Meters_t x = l.X();
  Meters_t y = l.Y();
  Meters_t delta = x - centerLine;
  delta *= cos(y * DegreesToRadians);
  cout << "Setting adjusted location " << centerLine + delta
       << " " << y << endl;
  SetLocation(centerLine + delta, y, l.Z());
}

bool NodeReal::HasLocation()
//检查本地结点是否已分配地址，若已分配，返回真，否则返回假
{
  return hasLocation;
}

Meters_t NodeReal::LocationX()
//返回本地结点的x坐标
{
  return x;
}

Meters_t NodeReal::LocationY()
//返回本地结点的y坐标
{
  return y;
}

Meters_t NodeReal::LocationZ()
//返回本地结点的z坐标
{
  return z;
}

Location NodeReal::GetLocation()
//返回本地结点当前位置
{
  return Location(x, y, z);
}

Location NodeReal::UpdateLocation()
/*
描述：更新本地结点的位置。活动结点的位置不是持续跟新的，只有该函数被调用时才进行更新
返回值：更新后的本地结点的位置
*/
{
  if (mobility) mobility->UpdateLocation(pNode);
  return GetLocation();
}

Mobility* NodeReal::AddMobility(const Mobility& m)
//为本地结点增添一个mobility模型，返回值为新增的mobility对象指针
{ 
  if (mobility) delete mobility;
  mobility = m.Copy();
  mobility->Initialize();
  Simulator::instance->HasMobility(true); // 记录已经添加
  return mobility;
}

Mobility* NodeReal::GetMobility() const
//获取本地结点当前的mobility模型
{
  return mobility;
}

bool NodeReal::IsMoving()
//判断本地结点的当前状态是否是正在运行
{
  if (!mobility) return false;
  return mobility->IsMoving();
}

bool NodeReal::IsMobile()
//判断本地结点是否含有mobility模型
{
  if (!mobility) return false;
  return true;
}

#ifdef HAVE_QT
MyCanvasItem* NodeReal::Display(QTWindow* qtw)
//确定点显示的形状、大小等，显示位置为点当前的位置
{
  if (!qtw) return nil;
  MyPoint p = qtw->LocationToPixels(GetLocation());
  return Display(p, qtw);

}

MyCanvasItem* NodeReal::Display(const MyPoint& p, QTWindow* qtw)
//确定点显示的形状、大小等，显示位置参数中提供的位置
{
  if (!qtw) return nil; // Not animating
  MyCanvas* canvas = qtw->Canvas();
  CustomShape_t cs;
  if (!animInfo)
    {
      animInfo = new NodeAnimation(); // Allocate the animation info
      animInfo->show = hasLocation;
    }
  if (Shape() != Node::IMAGE && !animInfo->nodePolygon)
    { // No existing polygon, make one
      switch(Shape()) {
        case Node::NONE:
          return nil;
        case Node::CIRCLE:
          animInfo->nodePolygon =
              new MyCanvasEllipse(animInfo->pixelSizeX, animInfo->pixelSizeY,
                                 canvas);
          animInfo->nodePolygon->move(p.x(), p.y());
          break;
        case Node::SQUARE:
          animInfo->nodePolygon = new MyCanvasRectangle(
              p.x() - animInfo->pixelSizeX / 2,
              p.y() - animInfo->pixelSizeY / 2,
              animInfo->pixelSizeX,
              animInfo->pixelSizeY,
              canvas);
           break;
        case Node::HEXAGON:
          break;
        case Node::OCTAGON:
          break;
        case Node::CUSTOM:
          cs = CustomShape();
          if (cs)
            {
              animInfo->nodePolygon = cs(canvas, pNode, p, animInfo->nodePolygon);
            }
          break;
        default:
          break;
      }
      if (!animInfo->nodePolygon) return nil; // Oops!  No known shape
      animInfo->nodePolygon->show();
    }
  if (IsMobile() || animInfo->placeImage)
    { // Need to (possibly) move to new location
      if (Shape() == Node::CUSTOM)
        { // Custom items may redraw depending on motion, we need to call
          cs = CustomShape();
          if (cs)
            {
              animInfo->nodePolygon = cs(canvas,pNode,p, animInfo->nodePolygon);
            }
        }
      else
        {
          MyCanvasItem* pi;
          if (Shape() == Node::IMAGE)
            pi = animInfo->nodeSprite;
          else
            pi = animInfo->nodePolygon;
          pi->move( p.x() - animInfo->pixelSizeX / 2,
                    p.y() - animInfo->pixelSizeY / 2);
          animInfo->placeImage = false;
          if (Shape() == Node::IMAGE)
            { // We want images behind links and packets
              pi->setZ(-1);
            }
          else
            {
              pi->setZ(1.0); // We want nodes "in front of" links
            }
        }
    }
  // Choose color of node object based on wireless conditions
  // or user specified color.
  if (Shape() != Node::CUSTOM && Shape() != Node::IMAGE)
    { // Custom shapes and images set their own color
      MyCanvasPolygonalItem* pi = (MyCanvasPolygonalItem*)animInfo->nodePolygon;
      if (IsDown())
        {
          pi->setBrush(Qt::red);
        }
      else if (HasColor())
        { // Use user specified color if one is specified
          pi->setBrush(Color());
        }
	  //v1.01
      //else if (WirelessCx())
      //  {
      //    pi->setBrush(Qt::red);
      //  }
      //else if (WirelessTx())
      //  {
      //    pi->setBrush(Qt::cyan);
      //  }
      //else if (WirelessRx())
      //  {
      //    pi->setBrush(Qt::green);
      //  }
      //else if (WirelessRxMe())
      //  {
      //    pi->setBrush(Qt::blue);
      //  }
      //else if (WirelessRxZz())
      //  {
      //    pi->setBrush(Qt::yellow);
      //  }
#ifdef DONT_DO_THIS
      else if (mobility && mobility->IsMoving())
        {
          pi->setBrush(Qt::magenta);
        }
#endif
      else
        {
          pi->setBrush(Qt::black);
        }
    }
  return animInfo->nodePolygon;

}

////void NodeReal::WirelessTxColor(const MyColor& c)
////{
////  wirelessColor = c;
////}
////
////const MyColor& NodeReal::WirelessTxColor()
////{
////  return wirelessColor;
////}
////
////bool NodeReal::PushWirelessTx(MyCanvasItem* c)
////{
////  canvasItems.push_back(c);
////  return true;
////}
////
////MyCanvasItem* NodeReal::PopWirelessTx()
////{
////  if (canvasItems.empty()) return nil;
////  MyCanvasItem* c = canvasItems.front();
////  canvasItems.pop_front();
////  return c;
////}

void NodeReal::PixelSize(Count_t nps)  // Set node size
//设置点在显示时的大小
{
  if (!animInfo)
    {
      animInfo = new NodeAnimation(); // Allocate the animation info
      animInfo->show = hasLocation;
    }
  animInfo->pixelSizeX = nps;
  animInfo->pixelSizeY = nps;
}

Count_t NodeReal::PixelSizeX()
//返回点在显示时的宽度
{
  if (!animInfo)
    {
      animInfo = new NodeAnimation(); // Allocate the animation info
      animInfo->show = hasLocation;
    }
  return animInfo->pixelSizeX;
}

Count_t NodeReal::PixelSizeY()
//返回点在显示时的高度
{
  if (!animInfo)
    {
      animInfo = new NodeAnimation(); 
      animInfo->show = hasLocation;
    }
  return animInfo->pixelSizeY;
}

void NodeReal::Shape(Node::Shape_t ns) 
//设置点在显示时的形状
{
  if (!animInfo)
    {
      animInfo = new NodeAnimation(); // Allocate the animation info
      animInfo->show = hasLocation;
    }
  animInfo->shape = ns;
}

Node::Shape_t NodeReal::Shape()
//返回点在显示时的形状
{
  if (!animInfo)
    {
      animInfo = new NodeAnimation(); // Allocate the animation info
      animInfo->show = hasLocation;
    }
  return animInfo->shape;
}

CustomShape_t NodeReal::CustomShape()
//返回一般图形的回调函数
{
  if (!animInfo) return nil;
  return animInfo->csCallback;
}

void NodeReal::CustomShape(CustomShape_t cs)
//设置一般图形的回调函数
{
  if (!animInfo)
    {
      animInfo = new NodeAnimation(); // Allocate the animation info
      animInfo->show = hasLocation;
    }
  animInfo->csCallback = cs;
}

bool NodeReal::CustomShapeFile(const char*  fn)
//载入一个包含一般图形的文件，若成功返回true，否则返回false
{
  QTWindow* qtw = QTWindow::qtWin;
  if (!qtw) return false; // Simulator::StartAnimation must be already called
  ifstream t(fn);
  if (!t)
    {
      cout << "Can't open custom shape file " << fn << endl;
      return false;
    }
  t.close();
  MyPixmap qpm;
  if (!qpm.load(fn))
    {
      cout << "Can't load pixmap " << fn << endl;
      return false;
    }
  return CustomShapeHelper(qpm);
}

bool NodeReal::CustomShapeImage(const Image& im)
//载入一个一般图形，若成功返回true，否则返回false
{
  QTWindow* qtw = QTWindow::qtWin;
  if (!qtw) return false; // Simulator::StartAnimation must be already called
  MyPixmap qpm;
  qpm.loadFromData((const uchar*)im.Data(), im.Size());
  if (!qpm.width() || !qpm.height()) return false; // Failed
  return CustomShapeHelper(qpm);
}

void NodeReal::Color(const MyColor& c)
//指定本地结点的颜色
{
  if (!animInfo)
    {
      animInfo = new NodeAnimation();
      animInfo->show = hasLocation;
    }
  animInfo->color = c;
  animInfo->hasColor = true;
  if (animInfo->nodePolygon)
    { // Already displayed, update the canvas item
      animInfo->nodePolygon->setBrush(c);
    }
}

bool NodeReal::HasColor()
//检查本地结点是否已经分配颜色
{
  if (!animInfo) return false;
  return animInfo->hasColor;
}

MyColor& NodeReal::Color()
//返回本地结点的颜色
{
  if (!animInfo)
    {
      animInfo = new NodeAnimation();
      animInfo->show = hasLocation;
    }
  return animInfo->color;
}

NodeAnimation* NodeReal::GetNodeAnimation() const
//返回指向所有结点结点动作信息块的指针
{
  return animInfo;
}

#endif

// ICMP
////bool NodeReal::ICMPEnabled() const
//////检查本地结点是否支持ICMP
////{
////  return icmpEnabled;
////}
////
////void NodeReal::DisableICMP()
//////设定本地结点不支持ICMP
////{
////  icmpEnabled = false;
////}

#ifdef MOVED_TO_IMPL
//结点失败管理
void NodeReal::Down()
//设定该结点无效，无法产生或转发数据包
{ // 关闭每个接口
  for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
    {
      interfaces[i]->Down();
    }
  down = true;
  Simulator::instance->TopologyChanged();
#ifdef HAVE_QT  
  QTWindow* qtw = QTWindow::qtWin;
  Display(qtw);
#endif
}

void NodeReal::Up()
//设定该结点已经恢复，重新有效
{ // Node has recovered from failure
  // specify each interface is up
  for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
    {
      interfaces[i]->Up();
    }
  down = false;
  Simulator::instance->TopologyChanged();
#ifdef HAVE_QT
  QTWindow* qtw = QTWindow::qtWin;
  Display(qtw);
#endif
}

bool NodeReal::IsDown()
//查看结点是否有效，若无效则返回true，若正常则返回false
{
  return down;
}
#endif

Meters_t NodeReal::Distance(Node* n)
/*
描述：返回本地结点到指定对等结点的距离
参数：n——对等结点
返回值：两点之间的距离
*/
{
  Meters_t xdiff = LocationX() - n->LocationX();
  Meters_t ydiff = LocationY() - n->LocationY();
  Meters_t zdiff = LocationZ() - n->LocationZ();
  return sqrt(xdiff * xdiff + ydiff * ydiff + zdiff * zdiff);
}

//#define RADIO_OLD_WAY
//#define RADIO_ADAPT_WAY

////void NodeReal::BuildRadioInterfaceList(WirelessLink* wLink)
////{
////#ifdef DONT_DUPLICATE_CODE
////  // If no mobility assumed, need to build the list only once.
////  if (!Simulator::instance->HasMobility()) {
////    if (!radioInterfaces.empty()) return;
////    if (IsMobile()) UpdateLocation();
////    const NodeVec_t& nodes = Node::GetNodes();
////    for (NodeVec_t::const_iterator i = nodes.begin(); i != nodes.end(); ++i)
////      {
////        Node* n = *i;
////        if (Id() == n->Id()) continue;    // skip self node
////        if (n->IsMobile()) n->UpdateLocation();
////        Meters_t d = Distance(n);
////        if (d <= GetRadioRange()) {
////          Interface* iface = n->GetIfByNode(pNode);
////          if (iface->IsWireless())
////            {
////              InterfaceWireless* ifW = (InterfaceWireless*)iface;
////              radioInterfaces.push_back(RadioItem(ifW,d));
////            }
////        }
////      }
////    return;
////  }
////#endif
////  if (!Simulator::instance->HasMobility())
////    {  // Nothing to  do if no mobility and already built
////      if (!radioInterfaces.empty()) return;
////      radioMarginTime = 0.0; // list is empty, force one-time rebuile
////    }
////  
////      
////#ifdef RADIO_OLD_WAY
////  radioInterfaces.clear();
////  if (IsMobile()) UpdateLocation();
////  for (NodeVec_t::iterator i = nodes.begin(); i != nodes.end(); ++i)
////    {
////      Node* n = *i;
////      if (id == n->id) continue;    // skip self node
////      if (n->IsMobile()) n->UpdateLocation();
////      Meters_t d = Distance(n);
////      if (d <= GetRadioRange()) {
////        Interface* iface = n->GetIfByNode(this);
////        radioInterfaces.push_back(RadioItem(iface,d));
////      }
////    }
////#elif defined RADIO_ADAPT_WAY
////  Time_t now = Simulator::Now();
////  double radio_range = GetRadioRange();
////  if (IsMobile()) UpdateLocation();
////  if (now >= radioMarginTime) {
////    // need to re-build radio interface list
////    radioInterfaces.clear();
////    radioInf2.clear();
////    double max_dist = 0;
////#ifdef OLD_INEFFICIENT
////    for (NodeVec_t::iterator i = nodes.begin(); i != nodes.end(); ++i)
////      {
////        Node* n = *i;
////        if (id == n->id) continue;    // skip self node
////        if (n->IsMobile()) n->UpdateLocation();
////        Meters_t d = Distance(n);
////        Interface* iface = n->GetIfByNode(this);
////        if (!iface) cout << "HuH? nil iface???\n";
////        if (d <= radio_range) {
////          radioInterfaces.push_back(RadioItem(iface,d));
////          if (d > max_dist) max_dist = d;
////        }
////        else
////          radioInf2.push_back(RadioItem(iface,d));
////      }
////#else // use wlan object instead
////    for (NodeId_t nid = wLink->first; nid < wLink->last; ++nid)
////      { // Check each node in the wlan object
////        if (nid == Id()) continue; // Skip self
////        Node* n = Node::GetNode(nid);
////        if (n->IsMobile()) n->UpdateLocation();
////        Meters_t d = Distance(n); // Distance to this node
////        const IFVec_t ifaces = n->Interfaces();
////        for (IFVec_t::size_type i = 0; i < ifaces.size(); ++i)
////          {
////            Interface* iface = ifaces[i];
////            if (iface->IsWireless())
////              { // Found a wireless
////                if (d <= radio_range)
////                  {
////                    radioInterfaces.push_back(RadioItem(iface,d));
////                    if (d > max_dist) max_dist = d;
////                  }
////                else
////                  {
////                    radioInf2.push_back(RadioItem(iface,d));
////                  }
////              }
////          }
////      }
////#endif
////
////    double mar_dist = radio_range - max_dist;
////    Time_t mar_time = mar_dist / (2*defaultMaxSpeed);
////    double out_bound = radio_range + mar_dist;
////    for (RadioVec_t::iterator i = radioInf2.begin(); i != radioInf2.end(); )
////      {
////        if (i->dist > out_bound)
////          radioInf2.erase(i++);
////        else
////          ++i;
////      }
////    radioMarginTime = now + mar_time;
////  }
////  else {
////    // use only radioInterfaces and radioInf2
////    // need only to update distance
////    RadioVec_t::iterator i;
////    for (i = radioInterfaces.begin(); i != radioInterfaces.end(); )
////      {
////        Node* n = i->inf->GetNode();
////        if (n->IsMobile()) n->UpdateLocation();
////        i->dist = Distance(n);
////        if (i->dist > radio_range) {
////          radioInf2.push_back(*i);
////          radioInterfaces.erase(i++);
////        }
////        else
////          ++i;
////      }
////    for (i = radioInf2.begin(); i != radioInf2.end(); )
////      {
////        Node* n = i->inf->GetNode();
////        if (n->IsMobile()) n->UpdateLocation();
////        i->dist = Distance(n);
////        if (i->dist <= radio_range) {
////          radioInterfaces.push_back(*i);
////          radioInf2.erase(i++);
////        }
////        else
////          ++i;
////      }
////  }
////#else
////  Time_t now = Simulator::Now();
////  double radio_range = GetRadioRange();
////  if (IsMobile()) UpdateLocation();
////  if (now > radioMarginTime)
////    {
////      // need to re-build radio interface list
////      radioInterfaces.clear();
////      radioInf2.clear();
////      Time_t mar_time = 2.0;
////      double out_bound = radio_range + 2*Node::defaultMaxSpeed*mar_time;
////#ifdef OLD_INEFFICIENT
////      const NodeVec_t& nodes = Node::GetNodes();
////      for (NodeVec_t::const_iterator i = nodes.begin(); i != nodes.end(); ++i)
////        {
////          Node* n = *i;
////          if (Id() == n->Id()) continue;    // skip self node
////          if (n->IsMobile()) n->UpdateLocation();
////          Meters_t d = Distance(n);
////          Interface* iface = n->GetIfByNode(pNode);
////          if (!iface) cout << "HuH? nil iface???\n";
////          if (iface->IsWireless())
////            {
////              InterfaceWireless* ifW = (InterfaceWireless*)iface;
////              if (d <= radio_range)
////                radioInterfaces.push_back(RadioItem(ifW,d));
////              else if (d <= out_bound)
////                radioInf2.push_back(RadioItem(ifW,d));
////            }
////
////        }
////#else // Use wlan object
////      IFVec_t& ifaces = wLink->ifaceVec;
////      for (IFVec_t::size_type i = 0; i < ifaces.size(); ++i)
////        { // Check each interface in the wlan object
////          Node* n = ifaces[i]->GetNode();
////          if (n->Id() == Id()) continue; // Skip self
////          if (n->IsMobile()) n->UpdateLocation();
////          Meters_t d = Distance(n); // Distance to this node
////          InterfaceWireless* ifW = (InterfaceWireless*)ifaces[i];
////          if (d <= radio_range)
////            radioInterfaces.push_back(RadioItem(ifW,d));
////          else if (d <= out_bound)
////            radioInf2.push_back(RadioItem(ifW,d));
////        }
////#endif
////      radioMarginTime = now + mar_time;
////    }
////  else
////    {
////      // use only radioInterfaces and radioInf2
////      // need only to update distance
////      RadioVec_t::iterator i;
////      for (i = radioInterfaces.begin(); i != radioInterfaces.end(); )
////        {
////          Node* n = i->inf->GetNode();
////          if (n->IsMobile()) n->UpdateLocation();
////          i->dist = Distance(n);
////          if (i->dist > radio_range) {
////            radioInf2.push_back(*i);
////            radioInterfaces.erase(i++);
////          }
////          else
////            ++i;
////        }
////      for (i = radioInf2.begin(); i != radioInf2.end(); )
////        {
////          Node* n = i->inf->GetNode();
////          if (n->IsMobile()) n->UpdateLocation();
////          i->dist = Distance(n);
////          if (i->dist <= radio_range)
////            {
////              radioInterfaces.push_back(*i);
////              radioInf2.erase(i++);
////            }
////          else
////            ++i;
////        }
////    }
////#endif
////}
////
////const RadioVec_t& NodeReal::GetRadioInterfaceList()
////{
////  return radioInterfaces;
////}
////

void NodeReal::AddCallback(Layer_t l, Proto_t p,
                           PacketCallbacks::Type_t t,
                           Interface* i,
                           PacketCallbacks::Function_t f)
/*
描述：给本结点增加一个回调函数。回调允许检测本结点接收或传输的任意协议层的任意数据包
参数：l——添加回调的协议层。值为0则代表回调对所有的协议层均有效
	  p——回调的协议号。值为0则代表回调对指定协议层的所有协议均有效
	  t——回调的类型。RX代表数据包接收的回调；TX代表数据包传输的回调；
	  i——接口指针。第二协议层能额外限制指定唯一一个接口
	  f——待添加的回调函数，若回调函数返回true则继续传包，否则丢掉数据包。它有五个参数，
		   Layer_t（产生回调函数的协议层），Type_t （回调函数类型），Packet*（待处理的
		   数据包），Node*（处理数据包的结点），Interface*（只针对第二协议层，处理数据
	       包的接口）
*/
{
  callbacks.Add(l, p, t, i, f);
}

void NodeReal::AddCallbackHead(Layer_t l, Proto_t p,
                               PacketCallbacks::Type_t t,
                               Interface* i,
                               PacketCallbacks::Function_t f)
/*
描述：给本地结点田间一个回调函数，该函数回调优先级最高
参数：l——添加回调的协议层。值为0则代表回调对所有的协议层均有效
	  p——回调的协议号。值为0则代表回调对指定协议层的所有协议均有效
	  t——回调的类型。RX代表数据包接收的回调；TX代表数据包传输的回调；
	  i——接口指针。第二协议层能额外限制指定唯一一个接口
	  f——待添加的回调函数，若回调函数返回true则继续传包，否则丢掉数据包。它有五个参数，
	       Layer_t（产生回调函数的协议层），Type_t （回调函数类型），Packet*（待处理的
		   数据包），Node*（处理数据包的结点），Interface*（只针对第二协议层，处理数据
		   包的接口）
*/
{
  callbacks.AddHead(l, p, t, i, f);
}

void NodeReal::DeleteCallback(Layer_t l, Proto_t p, 
                              PacketCallbacks::Type_t t,
                              Interface* i)
/*
描述：从本地结点删除一个回调函数
参数：l——删除回调的协议层。值为0则代表回调对所有的协议层均有效
	  p——回调的协议号。值为0则代表回调对指定协议层的所有协议均有效
	  t——回调的类型。RX代表数据包接收的回调；TX代表数据包传输的回调；
	  i——接口指针。第二协议层能额外限制指定唯一一个接口
*/
{
  callbacks.Delete(l, p, t, i);
}

bool NodeReal::CallCallbacks(Layer_t l, Proto_t p,
                             PacketCallbacks::Type_t t,
                             Packet* pkt, Interface* i)
/*
描述：调用本地结点的回调函数
参数：l——调用的回调的协议层。值为0则代表回调对所有的协议层均有效
	  p——回调的协议号。值为0则代表回调对指定协议层的所有协议均有效
	  t——回调的类型。RX代表数据包接收的回调；TX代表数据包传输的回调；
	  pkt——待处理的数据包
	  i——接口指针。第二协议层能额外限制指定唯一一个接口
*/
{
  return callbacks.Call(l, p, t, pkt, pNode, i);
}


#ifdef HAVE_QT

////bool NodeReal::WirelessTx()
////{
////  for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
////    {
////      if (interfaces[i]->IsWireless())
////        {
////          InterfaceWireless* iw = (InterfaceWireless*)interfaces[i];
////          if (iw->GetLinkState() == InterfaceWireless::TX)
////            {
////              return true;
////            }
////        }
////    }
////  return false;
////}
////
////bool NodeReal::WirelessRx()
////{
////  for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
////    {
////      if (interfaces[i]->IsWireless())
////        {
////          InterfaceWireless* iw = (InterfaceWireless*)interfaces[i];
////          if (iw->GetLinkState() == InterfaceWireless::RX)
////            {
////              return true;
////            }
////        }
////    }
////  return false;
////}
////
////bool NodeReal::WirelessCx()
////{
////  for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
////    {
////      if (interfaces[i]->IsWireless())
////        {
////          InterfaceWireless* iw = (InterfaceWireless*)interfaces[i];
////          if (iw->GetLinkState() == InterfaceWireless::CX)
////            {
////              return true;
////            }
////        }
////    }
////  return false;
////}
////
////bool NodeReal::WirelessRxMe()
////{
////  for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
////    {
////      if (interfaces[i]->IsWireless())
////        {
////          InterfaceWireless* iw = (InterfaceWireless*)interfaces[i];
////          if (iw->GetLinkState() == InterfaceWireless::RX_ME)
////            {
////              return true;
////            }
////        }
////    }
////  return false;
////}
////
////bool NodeReal::WirelessRxZz()
////{
////  for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
////    {
////      if (interfaces[i]->IsWireless())
////        {
////          InterfaceWireless* iw = (InterfaceWireless*)interfaces[i];
////          if (iw->GetLinkState() == InterfaceWireless::RX_ZZ)
////            {
////              return true;
////            }
////        }
////    }
////
////  return false;
////}

void NodeReal::Show(bool s)
//根据参数确定是否要显示窗口
{
  if (!animInfo) animInfo = new NodeAnimation();
  animInfo->show = true;
}

bool NodeReal::Show()
//返回显示是否要被激活
{
  if (!animInfo)
    { 
      return hasLocation;
    }
  return animInfo->show;
}

void NodeReal::UserInformation(void* info)
/*
描述：存储用户仿真过程中任何有用信息
参数：p——本地结点指向用户任意信息的指针
*/
{
  userInfo = info;
}

void* NodeReal::UserInformation()
//返回之前指定的用户信息指针，若没有返回空
{
  return userInfo;
}
#endif

void NodeReal::SetTimeout(Time_t t)
//设置仿真器时间
{
  timeout = t;
}

int NodeReal::FindSRCPort(Interface* ifc)
//在本地接口中查找是否与目标接口相同的项，若存在则返回与之对应的端口号，否则返回空
{
  for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
    {
      if (interfaces[i] == ifc)
        { 
          return i;
        }
    }
  return NOT_FOUND;
}

void NodeReal::PacketRX(Packet* p, Interface* iface)
//从指定接口接收并处理数据包
{
  int port = FindSRCPort(iface);//查找接口对应端口号

  if(port == NOT_FOUND)
    {
      cout << "huh? interface not connected to a port! " << endl;
      return ;
    }
  L2802_3Header* l2pdu = (L2802_3Header*)p->PeekPDU();
  MACPort_t::iterator i = portmap.find(l2pdu->src); //查找有没有端口对应数据包
  if(i != portmap.end())//找到该端口
    {
      (*i).second.time = Simulator::Now() + GetTimeout();//更新时间
    }
  else
    {
      //若没有找到则新增加一个
      portmap.insert(
          MACPort_t::value_type(l2pdu->src, PortPair(port, Simulator::Now() + GetTimeout())));
    }
  //是否有路由项对应目的结点的地址
  i = portmap.find(l2pdu->dst);
  if (i != portmap.end() && (*i).second.time > Simulator::Now())
    {
      if((*i).second.port != (unsigned int)port)
	{
	  DEBUG0((cout << "Forwarding packet to corresponding port " << (*i).second.port << endl ));
	  interfaces[(*i).second.port]->GetL2Proto()->DataRequest(p);
	}
      return ;
    }

  // No entry found for the destination mac or the entry is expired

  DEBUG0((cout << "----------------Forwarding packet to all  ports-------------- "  << endl));

  // forward packet to all ports except the src port
  for( Count_t j = 0; j < interfaces.size(); j++ )
    {
      if (j != (unsigned int)port)
	{
	  DEBUG0((cout << "Forwarding packet to corresponding port " << j << endl));
	  interfaces[j]->GetL2Proto()->DataRequest(p->Copy());
 	}
    }
  if (p) delete p;
  return;
}

void NodeReal::IsSwitchNode(bool b)
//根据参数设置结点是否为交换机
{
  isswitch = b;
}

////void NodeReal::UseARP(bool b)
////{
////  for (IFVec_t::size_type j = 0; j < interfaces.size(); ++j)
////    {
////      Interface* i = interfaces[j];
////      i->UseARP(b);
////    }
////}

////void NodeReal::SetWormContainment(WormContainment* cont)
////{
////  wormcontainment = cont;
////  usewormcontainment = true;
////    
////     
////  // set the worm containment module on all connected interface
////  for (IFVec_t::size_type j = 0; j < interfaces.size(); ++j)
////    {
////      Interface* i = interfaces[j];
////      i->SetWormContainment(cont);
////    }
////}

// Private Methods

bool NodeReal::CheckTrace(Protocol* p, PDU* h) 
//检查跟踪是否能进行 
{
  DEBUG(0,(cout << "Trace::Instance()->IsEnabled "
           << Trace::Instance()->IsEnabled()<<endl));
  if (!Trace::Instance()->IsEnabled()) return false;
  DEBUG(0,(cout << "traceStatus " << traceStatus << endl));
  // First make a decision, then call the callback (if defined)
  Trace::TraceStatus ts = Trace::DEFAULT;
  // Global trace is on, first check node
  if (ts == Trace::DEFAULT) ts = traceStatus;
  // Node trace is default, next check this protocol
  if (!p) ts = Trace::ENABLED;  // If called with no protocol, is a drop
  DEBUG(0,(cout << "GetTrace() " << p->GetTrace() << endl));
  if (ts == Trace::DEFAULT) ts = p->GetTrace();
  // Protocol trace is also default, check global settings
  if (ts == Trace::DEFAULT)
    if (Trace::Instance()->IsOn(p->Layer())) ts = Trace::ENABLED;
  bool tb = (ts == Trace::ENABLED);

  if (Trace::callback)
    { // Allow the callback to override the decision
      tb = Trace::callback(pNode, p, h, tb);
    }

  return tb;  // True or False to trace this pdu
}

#ifdef HAVE_QT
bool NodeReal::CustomShapeHelper(MyPixmap& qpm)
//处理一般图形
{
  QTWindow* qtw = QTWindow::qtWin;
  MyValueList list;
  list.push_back(qpm);
  MyCanvasPixmapArray* qcpa = new MyCanvasPixmapArray(list);
  if (!qcpa->isValid())
    { 
      delete qcpa;
      return false;
    }
  if (!animInfo)
    {
      animInfo = new NodeAnimation();
      animInfo->show = hasLocation;
    }
  if (animInfo->nodePolygon) delete animInfo->nodePolygon;
  animInfo->nodePolygon = nil;
  animInfo->shape = Node::IMAGE;
  animInfo->nodeSprite = new MyCanvasSprite(qcpa, qtw->Canvas());
  animInfo->nodeSprite->show();
  MyCanvasPixmap* qcp = qcpa->image(0);
  animInfo->pixelSizeX = qcp->width();
  animInfo->pixelSizeY = qcp->height();
  DEBUG0((cout << "Image width " << qcp->width()
          << " height " << qcp->height() << endl));
  animInfo->placeImage = true;
  animInfo->nodeSprite->setZ(-1); 
  return true;
}
#endif
