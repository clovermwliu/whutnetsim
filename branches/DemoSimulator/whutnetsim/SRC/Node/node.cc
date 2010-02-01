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
#include <math.h>

//#define DEBUG_MASK 0x02
#include "G_debug.h"
#include "node.h"
#include "node-impl.h"
#include "node-real.h"
//#include "node-ghost.h"
#include "protograph.h"
#include "portdemux.h"
#include "ipaddr.h"
#include "duplexlink.h"
#include "application.h"
#include "mobility.h"
#include "routing-nixvector.h"
#include "interface-real.h"  //v1.01
//#include "interface-wireless.h"
//#include "distributed-simulator.h" v1.01

using namespace std;

NodeId_t  Node::nextId = 0;  //要分配的下一个结点Id
NodeVec_t Node::nodes;       //Vector容器，包括了所有的结点
#ifdef HAVE_QT //v1.01
Count_t   Node::defaultPixelSize = 10;  //运行中，结点的像素大小
Node::Shape_t Node::defaultShape = Node::SQUARE;  //所有结点的默认形状:方形
#endif
double    Node::defaultMaxSpeed = 20;  //结点默认的最大传输率：20

bool operator==(const Location& l, const Location& r)
/*
描述：位置比较运算符"=="，判断Location l与r在xyz坐标系中是否相等，若相等返回true，否则返回false
*/
{
  return (l.X() == r.X() && l.Y() == r.Y() && l.Z() == r.Z());
}

Node::Node(SystemId_t sysId) : id(nextId++),
  proxyIP(IPADDR_NONE), proxyMask(0)
/*
描述：创建一个模拟拓扑结点。可选参数只对分布式模拟器有用，并指定维护该结点状
态的仿真器。若结点指定id与指定系统id相匹配，那么会创建一个Real结点。如果不是，
则创建一个Ghost结点             
参数：SystemId_t sysId —— 分布式系统标识符                                                      
返回值：空                                                                                       
备注：存储所有结点的Vector被更新，加入了新建的结点。                                              
*/
{
  ////if (sysId != DistributedSimulator::systemId)
  ////  { //映射到不同的逻辑过程中，若不是分发模拟器中的Id，则创建的是ghost结点
  ////    pImpl = new NodeGhost(this);
  ////  }
  ////else
  ////  { //否则，是Real结点
      pImpl = new NodeReal(this);
    ////}
  nodes.push_back(this);   //将该结点添加到已知结点所在的Vector中
}

Node::Node(NodeImpl* ni, SystemId_t sysId)
    : id(nextId++), proxyIP(IPADDR_NONE), proxyMask(MASK_NONE)
/*
描述：创建一个指定的模拟拓扑结点。可选参数只对分布式模拟器有用，并指定维护该结点状态的模拟器。若结点指定id与
      构造器上指定系统id相匹配，那么会创建一个Real结点。如果不是，则创建一个Ghost结点。             
参数：ni——指定的结点对象
      SystemId_t sysId —— 分布式系统标识符                                                     
返回值：空                                                                                      
备注：存储所有结点的Vector被更新，加入了新建的结点。                                              
*/
{
  ////if (sysId != DistributedSimulator::systemId)
  ////  { 
  ////    pImpl = new NodeGhost(this);
  ////    delete ni; 
  ////  }
  ////else
  ////  { 
      pImpl = ni;
	////}
  nodes.push_back(this);  
}

Node::~Node()
//删除该结点
{
  nextId = 0;
  
  delete pImpl;                    
}

IPAddr_t  Node::GetIPAddr()
//返回该结点分配的第一个IP地址，因为一个结点可以与多个接口绑定，因此可能存在多个IP地址
{ 
  return pImpl->GetIPAddr(); 
}

void  Node::SetIPAddr(IPAddr_t i)
//设置该结点的IP地址为输入参数i
{ 
  pImpl->SetIPAddr(i);
}

void Node::IPAddrs(IPMaskVec_t& IPMaskVect)
/*
描述：为该结点创建一列IP地址和掩码
参数：[OUT]IPMaskVect带出插入了新的IP地址和掩码后的IP地址列表
备注：一个结点可有多个接口，每个接口都有一对相关联的IP地址和掩码。
*/
{ 
  pImpl->IPAddrs(IPMaskVect);
}

bool Node::LocalIP( IPAddr_t ip)
//判断ip是否为本结点IP地址，是则返回true
{ 
  return pImpl->LocalIP(ip);
}

bool Node::IPKnown()
//判断是否分配了一个非默认的IP地址，是则返回true
{ 
  return pImpl->IPKnown();
}

bool Node::IsReal()
//判断是否是一个Real类型的结点，是则返回true
{
  return pImpl->IsReal();
}

void Node::IsSwitchNode(bool b)
//是否将该结点置为交换机，是则返回true
{ 
  return pImpl->IsSwitchNode(b);
}

bool Node::IsSwitchNode()
//判断该结点是否是一个交换机
{
  return pImpl->IsSwitchNode();
}

void Node::PacketRX(Packet* p, Interface* iface)
//若交换机结点收到一个数据包，则该函数被执行
{ 
  return pImpl->PacketRX(p, iface);
}

Interface* Node::AddInterface(Interface* pif)
/*
描述：添加一个已经设置好的接口pif
返回值:加入本地结点以后的接口的指针
*/
{
  return pImpl->AddInterface(pif);
}

Interface* Node::AddInterface(const L2Proto& l2, bool bootstrap)
/*
描述：添加新的空接口，该接口分配第二层网络协议，没有IP地址和掩码
参数：l2——为新增接口分配的第二层网络协议对象，默认为802.3
      bootstrap——与MAC地址相关的布尔变量，已经设为false
返回值:加入本地结点以后的接口的指针
*/
{
  return pImpl->AddInterface(l2, bootstrap);
}

Interface* Node::AddInterface(const L2Proto& l2,
                              IPAddr_t ip, 
							  Mask_t mask,
			                  MACAddr mac, 
							  bool bootstrap)
/*
描述：为该结点新增指定IP地址、掩码的接口，MAC地址默认，并分配第二层网络协议
参数：l2——为新增接口分配的第二层网络协议对象，默认为802.3
      ip——为新增接口分配的IP地址
	  mask——为新增接口分配的掩码
      mac——为新增接口分配的MAC地址，该地址是默认的
      bootstrap——与MAC地址相关的布尔变量，已经设为false
返回值：加入本地结点以后的接口的指针
*/
{
  return pImpl->AddInterface(l2, ip, mask, mac, bootstrap);
}

Interface* Node::AddInterface(const L2Proto& l2,
							  const Interface& iface,
                              IPAddr_t ip, 
							  Mask_t mask, 
							  MACAddr mac,
			                  bool bootstrap)
/*
描述：为本地结点新增接口，IP地址、掩码、接口类型指定，MAC地址默认，并分配第二层网络协议
参数：l2——为新增接口分配的第二层网络协议对象，默认为802.3
      iface——新增接口指定的接口类型
      ip——为新增接口分配的IP地址
	  mask——为新增接口分配的掩码
	  mac——为新增接口分配的MAC地址，该地址是默认的
	  bootstrap——与MAC地址相关的布尔变量，已经设为false
*/
{
  return pImpl->AddInterface(l2, iface, ip, mask, mac, bootstrap);
}

Count_t Node::InterfaceCount()
//获取与该结点绑定的接口的数目，返回值是接口数目
{
  return pImpl->InterfaceCount();
}

const IFVec_t& Node::Interfaces()
/*
描述：获取与本地结点绑定的所有接口的信息。
返回值：Vector类型指针，其中包含了与该结点绑定的所有接口的指针
*/
{ 
  return pImpl->Interfaces();
}

Interface* Node::GetIfByLink(Link* l)
/*
描述：根据边查找对应接口
参数：l——与本地结点所连的某条边
返回值：本地结点与l对应的接口的指针，若没有找到，返回空
*/
{
  return pImpl->GetIfByLink(l);
}

Interface* Node::GetIfByNode(Node* n)
/*
描述：根据点查找对应接口
参数：n——与本地结点连通的远程结点
返回值：本地结点与n对应的接口的指针，若没有找到，返回空
*/
{ 
  return pImpl->GetIfByNode(n);
}

Interface* Node::GetIfByIP(IPAddr_t ip)
/*
描述：根据本地接口IP地址查找对应接口
参数：n——与本地结点连通的远程结点
返回值：本地结点与n对应的接口的指针，若没有找到，返回空
*/
{
  return pImpl->GetIfByIP(ip);
}

void Node::Broadcast(Packet* pack, Proto_t proto)
/*
描述：通过该结点的所有接口广播数据包。
参数：pack-需要广播的数据包的指针
      proto-广播数据包包头添加的协议类型
备注：该数据包的包头添加第三层协议。每个接口复制一份独立的数据包，呼叫结点对数据包仍然有效，当不需要数据包时再删除。
*/
{ 
  return pImpl->Broadcast(pack, proto);
}

// Links.
// All of the following are just syntactic shortcuts to the DuplexLink object.
// Each creates a local DuplexLink object, which is destroyed on exit.
Interface* Node::AddDuplexLink(Node* remotenode)
 /*
 描述：连接远程结点，本地结点新增一条双向链路和一个接口，双向链路类型为默认。
 参数：remotenode——远程结点指针
 返回值：新增双向链路所对应的本地接口指针
 */
{
  return pImpl->AddDuplexLink(remotenode);
}

Interface* Node::AddDuplexLink(Node* remotenode, const Linkp2p& l)
/*
描述：连接远程结点，本地结点新增一条双向链路和一个接口，双向链路类型指定
参数：remotenode——远程结点指针
      l——新增双向链路的类型
返回值：新增双向链路所对应的本地接口指针
*/
{ 
  return pImpl->AddDuplexLink(remotenode, l);
}

Interface* Node::AddDuplexLink(Interface* li, Interface* ri)
/*
描述：新建一个连接两个指定接口的双向链路，双向链路类型默认
参数：li——本地结点接口指针
      ri——远程结点接口指针
返回值：本地新增接口指针
*/
{ 
  return pImpl->AddDuplexLink(li, ri);
}

Interface* Node::AddDuplexLink(Interface* li, 
							   Interface* ri,
                               const Linkp2p& l)
/*
描述：新建一个连接两个指定接口的双向链路，双向链路类型默认
参数：li——本地结点接口指针
      ri——远程结点接口指针
      l——新增双向链路类型
返回值：本地新增接口指针
*/
{
  return pImpl->AddDuplexLink(li, ri, l);
}

// Interface* Node::AddDuplexLink(Interface* li, Interface* ri,
//                                const Linkp2p& l, Node* rn)
// {
//   return pImpl->AddDuplexLink(li, ri, l, rn);
// }

Interface* Node::AddDuplexLink(Node* remotenode,
                               IPAddr_t li,
                               Mask_t lm,
                               IPAddr_t ri, 
							   Mask_t rm)
/*
描述：连接远程结点，本地结点新增一条双向链路和一个接口，双向链路类型为默认。为本地与远程结点新增的接口分配IP地址和掩码
参数：remotenode——远程结点指针
      li——为本地结点新增接口分配的IP地址
	  lm——为本地结点新增接口分配的掩码
	  ri——为远程结点新增接口分配的IP地址
	  rm——为远程结点新增接口分配的掩码
返回值：本地新增接口指针
*/
{
  return pImpl->AddDuplexLink(remotenode, li, lm, ri, rm);
}

Interface* Node::AddDuplexLink(Node* remotenode, 
							   const Linkp2p& l,
                               IPAddr_t li, 
							   Mask_t lm,
                               IPAddr_t ri, 
							   Mask_t rm)
/*
描述：连接远程结点，本地结点新增一条双向链路和一个接口，双向链路类型为默认。为本地与远程结点新增的接口分配IP地址和掩码
参数：remotenode——远程结点指针
	  l——新增双向链路类型
	  li——为本地结点新增接口分配的IP地址
	  lm——为本地结点新增接口分配的掩码
	  ri——为远程结点新增接口分配的IP地址
	  rm——为远程结点新增接口分配的掩码
返回值：本地新增接口指针
*/
{
  return pImpl->AddDuplexLink(remotenode, l, li, lm, ri, rm);
}

Link* Node::GetLink(Node* remotenode)
/*
描述:返回与指定结点联通的双向链路
参数：n——远程结点
返回值：所查找的双向链路存在，则返回它的指针，否则返回空
*/
{
  return pImpl->GetLink(remotenode);
}
//v1.01
//Interface* Node::AddRemoteLink(IPAddr_t ip, Mask_t mask)
///*
//描述：仅在分布式模拟器中使用，定义一条连通本地结点与其它结点的双向链路，在另一个模拟过程中使用。双向链路的带宽和延迟默认
//参数：ip——为本地结点新增接口分配的IP地址
//      mask——为本地结点新增接口分配的掩码
//返回值：本地新增接口指针
//*/
//{
//  return pImpl->AddRemoteLink(ip, mask);
//}
//
//Interface* Node::AddRemoteLink(IPAddr_t ip, Mask_t mask, Rate_t rate, Time_t delay)
///*
//描述：仅在分布式模拟器中使用，定义一条连通本地结点与其它结点的双向链路，在另一个模拟过程中使用。双向链路的带宽和延迟指定
//参数：ip——为本地结点新增接口分配的IP地址
//      mask——为本地结点新增接口分配的掩码
//	  rate——新增双向链路的带宽
//	  delay——新增双向链路的传播延迟
//返回值：本地新增接口指针
//*/
//{
//  return pImpl->AddRemoteLink(ip, mask, rate, delay);
//}

Interface* Node::AddSimplexLink(Node* remotenode)
/*
描述：连接远程结点，本地结点新增一条单向链路和一个接口，链路类型为默认。
参数：remotenode——远程结点指针
返回值：新增链路所对应的本地接口指针
*/
{
  return pImpl->AddSimplexLink(remotenode);
}

Interface* Node::AddSimplexLink(Node* remotenode, const Linkp2p& l)
/*
描述：连接远程结点，本地结点新增一条单向链路和一个接口，链路类型指定
参数：remotenode——远程结点指针
	  l——新增链路的类型
返回值：新增链路所对应的本地接口指针
*/
{
  return pImpl->AddSimplexLink(remotenode, l);
}

Interface* Node::AddSimplexLink(Node* remotenode, 
								const Linkp2p& l, 
								IPAddr_t ip,
								Mask_t mask)
/*
描述：连接远程结点，本地结点新增一条单向链路和一个接口，链路类型指定，本地接口IP地址和掩码指定。
参数：remotenode——远程结点指针
      l——新增链路的类型
	  ip——新增链路对应本地接口的ip地址
	  mask——新增链路对应本地接口的掩码
返回值：新增链路所对应的本地接口指针
*/
{
  return pImpl->AddSimplexLink(remotenode, l, ip, mask);
}

Queue* Node::GetQueue()
/*
描述：获取本地结点的队列，仅对单接口的结点有效。
返回值:返回本地单接口结点的队列；若结点没有接口，或者有多个接口，则返回空
*/
{ 
  return pImpl->GetQueue();
}

Queue* Node::GetQueue(Node* targetnode)
/*
描述：获取本地结点通往指定目标结点的队列
返回值:若本地结点与目标结点连通，该队列存在，则返回它的指针，否则返回空
*/
{ 
  return pImpl->GetQueue(targetnode);
}


Application* Node::AddApplication(const Application& app)
/*
描述：给本地结点添加指定应用
参数：app——应用对象，用于确定新增应用的类型
返回值：返回该新增应用对象指针
*/
{
  return pImpl->AddApplication(app);
}

ApplicationVec_t* Node::GetApplication()
/*
描述：查询本节点应用
*/
{
	return pImpl->GetApplicationVec(); 
}
////
////
////void Node::UseWormContainment(bool b) 
////{
////  return pImpl->UseWormContainment(b);
////}
////
////
////bool Node::UseWormContainment()
////{ 
////  return pImpl->UseWormContainment();
////}
////
////void Node::SetWormContainment(WormContainment* cont)
////{
////  return pImpl->SetWormContainment(cont);
////}
////
////WormContainment* Node::GetWormContainment()
////{
////  return pImpl->GetWormContainment();
////}
////
////bool Node::FirstBitRx()
////{
////  return pImpl->FirstBitRx();
////}
////
////void Node::FirstBitRx(bool is)
////{
////  pImpl->FirstBitRx(is);
////}
////
////void Node::UseARP(bool b)
////{
////  return pImpl->UseARP(b);
////}

void Node::Neighbors(NodeWeightVec_t& neighbornodes, bool noLeaf)
/*
描述：获取所有的邻近结点，具体方法是通过询问本地结点的每一个接口
参数：[OUT]neighbornodes——包含了本地结点所有的临近结点、与之相关的接口和链路的权重
      noLeaf——是否在查找过程中忽略非路由结点，默认为false即不忽略
*/
{ 
  pImpl->Neighbors(neighbornodes, noLeaf);
}

void Node::AddNeighbor(Node*, Weight_t)
{//非ghost型结点不会保存邻接结点表

}

Count_t Node::NeighborCount()
/*
描述：获取路由表中邻近结点的数目，通过将每个接口的邻接结点相加：
返回值：邻接结点的个数
*/
{ 
  return pImpl->NeighborCount();
}


void Node::NeighborsByIf(Interface* i, IPAddrVec_t& v)
/*
描述：获取与本地结点指定接口相连的邻接结点IP地址
参数：i——本地结点指定接口
      [OUT] v——所获取的一组邻接结点IP地址
*/
{
  return pImpl->NeighborsByIf(i, v);
}

void Node::DefaultRoute( RoutingEntry r)
// 指定发送数据包的默认路由项
{
	pImpl->DefaultRoute(r);
}

void Node::DefaultRoute( Node* n)
//指定发送数据包的默认下一跳结点
{ 
	pImpl->DefaultRoute(n);
}

void Node::AddRoute( IPAddr_t ip, Count_t mask, Interface* i, IPAddr_t ip1)
/*
描述：增加一个路由项，目的IP及其掩码、本地接口、下一跳IP地址确定
参数：ip——该路由项的目的IP
	mask——该路由项目的IP相应的掩码
	i——本地结点到目的地下一跳使用到的本地接口
	ip1——下一条结点的IP地址
*/
{ 
	pImpl->AddRoute(ip, mask, i, ip1);
}

RoutingEntry Node::LookupRoute(IPAddr_t ip)
//根据目的IP地址查找本地路由项，若存在则返回
{ 
	return pImpl->LookupRoute(ip);
}

RoutingEntry Node::LookupRouteNix(Count_t nix)
//根据邻接结点索引查找本地路由项，若存在则返回
{
	return pImpl->LookupRouteNix(nix);
}
////
////AODVRoutingEntry *
////Node::LookupRouteAODV(IPAddr_t ip) {
////	return pImpl->LookupRouteAODV(ip);
////}
////
////void
////Node::SetRoutingAODV(void *pRouting){
////	return pImpl->SetRoutingAODV(pRouting);
////}
////
////RoutingAODV *
////Node::GetRoutingAODV(){
////	return pImpl->GetRoutingAODV();
////}

Routing::RType_t Node::RoutingType()
//获取本地结点当前所使用的路由类型
{
	return pImpl->RoutingType();
}

Interface* Node::LocalRoute(IPAddr_t ip)
//判断某IP地址是否是直接与本地结点某接口相连，若是则返回该接口
{ 
	return pImpl->LocalRoute(ip);
}

void       Node::InitializeRoutes()
//路由初始化，为路由协议做准备
{ 
	pImpl->InitializeRoutes();
}

void       Node::ReInitializeRoutes(bool  status)
/*
描述：若拓扑发生变化，需要重新初始化路由
参数：status——结点或接口是否正常
*/
{
	pImpl->ReInitializeRoutes(status);
}

Count_t    Node::RoutingFibSize() const
//统计本地结点路由的大小
{ 
	return pImpl->RoutingFibSize();
}

Count_t    Node::GetNix(Node* n) const
//根据指定结点查找邻接结点的索引
{ 
	return pImpl->GetNix(n);
}

Routing* Node::GetRouting()
//返回本地结点所使用的路由对象的指针
{
	return pImpl->GetRouting();
}

RoutingNixVector* Node::GetNixRouting()
//若本地结点存在NixVector类型路由，则返回该路由对象的指针，否则返回空
{ 
  return pImpl->GetNixRouting();
}

void Node::DumpNV()
//调试使用
{
  RoutingNixVector* r = pImpl->GetNixRouting();
  if (!r) return;
  r->DBDump(this);
}

// Protocol Graph Interface
#ifdef COMPACT
// No private protocol graphs in compact mode
Protocol* Node::LookupProto(Layer_t l, Proto_t p) // Lookup protocol by layer
{
  return pImpl->LookupProto(l, p);
}

void      Node::InsertProto(Layer_t l, Proto_t p, Protocol* pp) // Insert proto
{
  pImpl->InsertProto(l, p, pp);
}
#else
Protocol* Node::LookupProto(Layer_t l, Proto_t proto)
//在本地结点，根据协议层和编号查找对应的协议对象
{
  return pImpl->LookupProto(l, proto);
}

void Node::InsertProto(Layer_t l, Proto_t p, Protocol* pp)
//在本地结点插入一个协议对象，.协议层和编号指定
{ 
  pImpl->InsertProto(l, p, pp);
}
#endif

// Port Demux Interface
bool Node::Bind(Proto_t proto, PortId_t port , Protocol* p)
/*
描述：为指定端口绑定指定协议
参数：proto——协议号
	  port——指定的端口
	  p——待绑定的协议
返回：是否绑定成功
*/
{
  return pImpl->Bind(proto, port, p);
}

bool Node::Bind(Proto_t proto,
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
  return pImpl->Bind(proto, lport, laddr, rport, raddr, p);
}

PortId_t Node::Bind(Proto_t proto, Protocol* p)
/*
描述：绑定指定协议，对应本地端口随机
参数：proto——协议号
      p——待绑定协议
返回值：所绑定的本地结点的端口
*/
{ 
  return pImpl->Bind(proto, p);
}

bool  Node::Unbind(Proto_t proto, PortId_t port, Protocol* p)
/*
描述：删除本地结点指定接口绑定的指定协议
参数：proto——待删除协议的协议号
      port——当前绑定的本地端口号
      p——待删除的协议
*/
{
  return pImpl->Unbind(proto, port, p);
}

bool  Node::Unbind(Proto_t proto,
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
  return pImpl->Unbind(proto, lport, laddr, rport, raddr, p);
}

Protocol* Node::LookupByPort(Proto_t proto, PortId_t port)
/*
描述：根据本地端口和协议号查找所绑定的协议
参数：proto——协议号
	  port——本地端口号
返回值：若找到相应的协议，将其返回；否则返回空
*/
{
  return pImpl->LookupByPort(proto, port);
}

Protocol* Node::LookupByPort(Proto_t proto,
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
  return pImpl->LookupByPort(proto, sport, sip, dport, dip);
}

bool Node::TracePDU(Protocol* proto, PDU* h, Packet* p, char* s)
/*
描述：将指定PDU内容从指定协议记录到跟踪文件
参数：proto——请求路径的指定协议指针
	  h——PDU单元
	  p——包含此PDU单元的数据包
	  s——跟踪文件额外文本信息
返回值：若该跟踪文件存在，返回true；否则返回false
*/
{
  return pImpl->TracePDU(proto, h, p, s);
}

void Node::SetTrace(Trace::TraceStatus t)
/*
描述：设置本地结点的跟踪状态。跟踪状态有三种：
	1.ENABLED——该结点所有请求跟踪的数据包都将会被跟踪；
	2.DISABLED——该结点所有数据包都不会被跟踪
	3.DEFAULT——该状态取决于本地结点绑定的协议族
参数:t——跟踪状态
*/
{
  pImpl->SetTrace(t);
}

void Node::SetLocation(Meters_t xl, Meters_t yl, Meters_t zl)
/*
描述：直接在xyz坐标系中设置本地结点的坐标
参数：xl——x轴坐标
      yl——y轴坐标
	  zl——z轴坐标
*/
{
  pImpl->SetLocation(xl, yl, zl);
}

void Node::SetLocation(const Location& l)
//根据location对象设置本地结点在xyz坐标系中的坐标
{
  pImpl->SetLocation(l);
}

void Node::SetLocationLongLat(const Location& l)
{
  pImpl->SetLocationLongLat(l);
}

bool Node::HasLocation()
//检查本地结点是否已分配地址，若已分配，返回真，否则返回假
{
  return pImpl->HasLocation();
}

Meters_t Node::LocationX()
//返回本地结点的x坐标
{
  return pImpl->LocationX();
}

Meters_t Node::LocationY()
//返回本地结点的y坐标
{
  return pImpl->LocationY();
}

Meters_t Node::LocationZ()
//返回本地结点的z坐标
{
  return pImpl->LocationZ();
}

Location Node::GetLocation()
//返回本地结点当前位置
{
  return pImpl->GetLocation();
}

Location Node::UpdateLocation()
/*
描述：更新本地结点的位置。活动结点的位置不是持续跟新的，只有该函数被调用时才进行更新
返回值：更新后的本地结点的位置
*/
{
  return pImpl->UpdateLocation();
}

Mobility* Node::AddMobility(const Mobility& m)
//为本地结点增添一个mobility模型，返回值为新增的mobility对象指针
{
  return pImpl->AddMobility(m);
}

Mobility* Node::GetMobility()
//获取本地结点当前的mobility模型
{
  return pImpl->GetMobility();
}

bool Node::IsMoving()
//判断本地结点的当前状态是否是正在运行
{
  return pImpl->IsMoving();
}

bool Node::IsMobile()
//判断本地结点是否含有mobility模型
{
  return pImpl->IsMobile();
}
#ifdef HAVE_QT //v1.01
void Node::Show(bool s)
//根据参数确定是否要显示窗口
{
  pImpl->Show(s);
}

bool Node::Show()
//返回显示是否要被激活
{
  return pImpl->Show();
}

MyCanvasItem* Node::Display(QTWindow* qc)
//确定点显示的形状、大小等，显示位置为点当前的位置
{
  return pImpl->Display(qc);
}

MyCanvasItem* Node::Display(const MyPoint& qp, QTWindow* qc)
//确定点显示的形状、大小等，显示位置参数中提供的位置
{
  return pImpl->Display(qp, qc);
}

void Node::PixelSize(Count_t nps) 
//设置点在显示时的大小
{
  pImpl->PixelSize(nps);
}

Count_t Node::PixelSizeX()
//返回点在显示时的宽度
{
  return pImpl->PixelSizeX();
}

Count_t Node::PixelSizeY()
//返回点在显示时的高度
{
  return pImpl->PixelSizeY();
}

void Node::Shape(Shape_t ns) 
//设置点在显示时的形状
{
  pImpl->Shape(ns);
}

Node::Shape_t Node::Shape()
//返回点在显示时的形状
{
  return pImpl->Shape();
}

CustomShape_t Node::CustomShape()
//返回一般图形的回调函数
{
  return pImpl->CustomShape();
}

void Node::CustomShape(CustomShape_t cs)
//设置一般图形的回调函数
{
  pImpl->CustomShape(cs);
}

bool Node::CustomShapeFile(const char* fn)
//载入一个包含一般图形的文件，若成功返回true，否则返回false
{
  return pImpl->CustomShapeFile(fn);
}

bool Node::CustomShapeImage(const Image& im)
//载入一个一般图形，若成功返回true，否则返回false
{
  return pImpl->CustomShapeImage(im);
}

void Node::Color(const MyColor& c)
//指定本地结点的颜色
{
  pImpl->Color(c);
}

bool Node::HasColor()
//检查本地结点是否已经分配颜色
{
  return pImpl->HasColor();
}

MyColor& Node::Color()
//返回本地结点的颜色
{
  return pImpl->Color();
}

NodeAnimation* Node::GetNodeAnimation() const
//返回指向所有结点结点动作信息块的指针
{
  return pImpl->GetNodeAnimation();
}
#endif

////bool Node::ICMPEnabled() const
//////检查本地结点是否支持ICMP
////{
////  return pImpl->ICMPEnabled();
////}
////
////void Node::DisableICMP()
//////设定本地结点不支持ICMP
////{
////  pImpl->DisableICMP();
////}



void Node::Down()
//设定该结点无效，无法产生或转发数据包
{
  pImpl->Down();
}

void Node::Up()
//设定该结点已经恢复，重新有效
{
  pImpl->Up();
}

bool Node::IsDown()
//查看结点是否有效，若无效则返回true，若正常则返回false
{
  return pImpl->IsDown();
}

void Node::AddCallback(Layer_t l, 
					   Proto_t p,
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
  pImpl->AddCallback(l, p, t, i, f);
}

void Node::AddCallbackHead(Layer_t l, 
						   Proto_t p,
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
  pImpl->AddCallbackHead(l, p, t, i, f);
}

void Node::DeleteCallback(Layer_t l, 
						  Proto_t p, 
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
  pImpl->DeleteCallback(l, p, t, i);
}

bool Node::CallCallbacks(Layer_t l, 
						 Proto_t p,
                         PacketCallbacks::Type_t t, 
						 Packet* pkt,
                         Interface* i)
/*
描述：调用本地结点的回调函数
参数：l——调用的回调的协议层。值为0则代表回调对所有的协议层均有效
	  p——回调的协议号。值为0则代表回调对指定协议层的所有协议均有效
	  t——回调的类型。RX代表数据包接收的回调；TX代表数据包传输的回调；
	  pkt——待处理的数据包
	  i——接口指针。第二协议层能额外限制指定唯一一个接口
*/
{
  return pImpl->CallCallbacks(l, p, t, pkt, i);
}

#ifdef HAVE_QT

void Node::UserInformation(void* p)
/*
描述：存储用户仿真过程中任何有用信息
参数：p——本地结点指向用户任意信息的指针
*/
{
  pImpl->UserInformation(p);
}

void* Node::UserInformation()
//返回之前指定的用户信息指针，若没有返回空
{
  return pImpl->UserInformation();
}


////bool Node::WirelessTx()
////{
////  return pImpl->WirelessTx();
////}
////
////bool Node::WirelessRx()
////{
////  return pImpl->WirelessRx();
////}
////
////bool Node::WirelessCx()
////{
////  return pImpl->WirelessCx();
////}
////
////bool Node::WirelessRxMe()
////{
////  return pImpl->WirelessRxMe();
////}
////
////bool Node::WirelessRxZz()
////{
////  return pImpl->WirelessRxZz();
////}

#endif

Meters_t Node::Distance(Node* n)
/*
描述：返回本地结点到指定对等结点的距离
参数：n——对等结点
返回值：两点之间的距离
*/
{
  return pImpl->Distance(n);
}

////void Node::BuildRadioInterfaceList(WirelessLink* l)
////{
////  pImpl->BuildRadioInterfaceList(l);
////}
////
////const RadioVec_t& Node::GetRadioInterfaceList()
////{
////  return pImpl->GetRadioInterfaceList();
////}
////
////void Node::SetRadioRange(Meters_t range)
////{
////  pImpl->SetRadioRange(range);
////}
////
////Meters_t Node::GetRadioRange()
////{
////  return pImpl->GetRadioRange();
////}

Joules_t Node::getBattery()
{
  return pImpl->getBattery();
}

void Node::setBattery(Joules_t joules)
{
  return pImpl->setBattery(joules);
}

void Node::setComputePower(double pow)
{
  return pImpl->setComputePower(pow);
}

double Node::getComputePower(void)
{
  return pImpl->getComputePower();
}

// Static Methods

Node* Node::GetNode(NodeId_t i)
{
  if (i >= nodes.size()) return nil; // Out of range
  return nodes[i];
}

void Node::Clear()
{
  nodes.clear();
}

#ifdef HAVE_QT

void Node::DefaultPixelSize(Count_t dps)
{
  defaultPixelSize = dps;
}

Count_t Node::DefaultPixelSize()
{
  return defaultPixelSize;
}

void Node::DefaultShape(Shape_t ds)
{
  defaultShape = ds;
}

Node::Shape_t Node::DefaultShape()
{
  return defaultShape;
}

#endif

void Node::DefaultMaxSpeed(double speed)
{
  defaultMaxSpeed = speed;
}

double Node::DefaultMaxSpeed()
{
  return defaultMaxSpeed;
}


// The following is for Proxy Routing alogorithms

void Node::SetProxyRoutingConfig(IPAddr_t ip, Mask mask)
{
  proxyIP = ip;
  proxyMask = mask;
}

bool Node::CanProxyRouteIP(IPAddr_t ip)
{
  return (proxyIP & proxyMask) == (ip & proxyMask);
}

IPAddr_t Node::GetProxyIP()
{
  return proxyIP;
}

Mask Node::GetProxyMask()
{
  return proxyMask;
}

int Node::GetLongestPrefixLength(IPAddr_t ip)
{
  int left = 0, right = proxyMask.NBits(), m;
  Mask_t mask1, mask2;

  if (!HasProxyRoutingConfig()) return 0;

  // I have used a binary search method that works in O(logN) time (max(N)=32)

  do {
    m=(left+right)/2;
    if (m==32) break;

    mask1 = 0xFFFFFFFF << (32-m);
    if ((proxyIP & mask1) != (ip & mask1))
      right = m-1;
    else {
      mask2 = 0xFFFFFFFF << (32- (m+1));
      if ((proxyIP & mask2) == (ip & mask2))
        left = m+1;
      else
        break; // we have found it
    }
  } while (left<=right);
  return m;
}

//void Node::setRouteTable(RouteTable* rtbl)
//{
//  return pImpl->setRouteTable(rtbl);
//}
//
//RouteTable* Node::getRouteTable(void)
//{
//  return pImpl->getRouteTable();
//}
