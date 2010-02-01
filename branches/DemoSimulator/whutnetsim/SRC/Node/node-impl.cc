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
//#include <unistd.h>
#endif

#ifdef HAVE_QT
#include "GUI_Defs.h"
#endif

//#define DEBUG_MASK 0x02
#include "G_debug.h"
#include "node-real.h"
#include "protograph.h"
#include "portdemux.h"
#include "ipaddr.h"
#include "duplexlink.h"
#include "application.h"
#include "mobility.h"
//#include "link-rti.h"  v1.01
#include "mask.h"
#include "interface-real.h"
//#include "interface-wireless.h"  v1.01
//#include "interface-ghost.h"
//#include "interface-ethernet.h"

using namespace std;

NodeImpl::NodeImpl(Node* n)
    : pNode(n), ipAddr(IPADDR_NONE), neighborCount(0), down(false)
{
}

NodeImpl::~NodeImpl()
{
	//释放节点的每一个应用，在应用当中释放了第四层协议 2010-1-29

	ApplicationVec_t::iterator iter; 
	for ( iter=pappvec.begin(); iter!=pappvec.end(); ++iter )
	{
		delete *iter;
	}
	pappvec.clear(); //应用数组的清空

	//节点接口的释放 2010-1-29
	IFVec_t::iterator iterinterface;
	for ( iterinterface=interfaces.begin(); iterinterface!=interfaces.end(); iterinterface++ )
	{
		delete *iterinterface;
	}
	int test = interfaces.size();
	interfaces.clear();

}

// IP Address Management
IPAddr_t    NodeImpl::GetIPAddr()
//返回该结点分配的第一个IP地址，因为一个结点可以与多个接口绑定，因此可能存在多个IP地址
{ 
  return ipAddr;  
}

void NodeImpl::SetIPAddr(IPAddr_t i)
//设置该结点的IP地址为输入参数i
{ 
  ipAddr = i;
  if (interfaces.size() == 1)
    { // Only one iface, set the same IPAddr there
      interfaces[0]->SetIPAddr(i);
    }
}

void NodeImpl::IPAddrs(IPMaskVec_t& IPMaskVect)
/*
描述：为该结点创建一列IP地址和掩码
参数：[OUT]IPMaskVect带出插入了新的IP地址和掩码后的IP地址列表
备注：一个结点可有多个接口，每个接口都有一对相关联的IP地址和掩码。
*/
{ 
  bool localFound = false;  //保证结点的IP地址已经包含在内
  for (IFVec_t::size_type j = 0; j < interfaces.size(); ++j)
    {
      Interface* i = interfaces[j];
      if (i->GetIPAddr() != IPADDR_NONE)
        {
          IPMaskVect.push_back(IpMask(i->GetIPAddr(), i->GetIPMask()));
          if ((ipAddr & i->GetIPMask()) == (i->GetIPAddr() & i->GetIPMask()))
            {
              localFound = true;
            }
        }
    }
  if (!localFound && (ipAddr != IPADDR_NONE))
    { // 需要加入结点本身
      IPMaskVect.push_back(IpMask(ipAddr, MASK_ALL));
    }
}

bool NodeImpl::LocalIP( IPAddr_t ip)
/*
描述：判断ip是否为本结点IP地址。
返回值：true —— ip是本地结点IP地址
        false ——ip不是本地结点IP地址
*/
{ 
  if (ipAddr == IPADDR_NONE) return false; // 本地结点没有IP地址
  if (ip == ipAddr) return true; //正好是本地结点IP地址
  NodeReal::count4 += interfaces.size();
  for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
    {
      if ((IPAddr_t)interfaces[i]->GetIPAddr() == ip)
        {
          return true; //在本地结点IP地址中找到
        }
    }
  return false; //没有找到，不是本地结点IP地址
}

bool NodeImpl::IPKnown()
//判断是否分配了一个非默认的IP地址，是则返回true
{ 
  return ipAddr != IPADDR_NONE;
}

Interface* NodeImpl::AddInterface(Interface* pif)
/*
描述：添加一个已经设置好的接口pif
返回值:加入本地结点以后的接口的指针
*/
{ 
  Interface::Add(pif, pif->GetMACAddr());
  pif->SetNode(pNode);
  interfaces.push_back(pif);
  if (!IPKnown() && (pif->GetIPAddr() != IPADDR_NONE))
    { //第一个接口分配默认IP地址
      SetIPAddr(pif->GetIPAddr());
    }
  return pif;
}

  
Interface* NodeImpl::AddInterface(const L2Proto& l2, bool bootstrap)
/*
描述：添加新的空接口，该接口分配第二层网络协议，没有IP地址和掩码
参数：l2——为新增接口分配的第二层网络协议对象
	  bootstrap——与MAC地址相关的布尔变量，已经设为false
返回值:加入本地结点以后的接口的指针
备注：给该新增的接口分配一个第二层网络协议对象，由于这个对象是通过复制产生的，因此可以供多个接口使用
*/
{
  return AddInterface(l2, IPADDR_NONE, MASK_ALL, MACAddr::Allocate(),
		      bootstrap);
}

Interface* NodeImpl::AddInterface(const L2Proto& l2,
                                  IPAddr_t ip, 
								  Mask_t mask,
                                  MACAddr mac, 
								  bool bootstrap)
/*
描述：为该结点新增指定IP地址、掩码的接口，MAC地址默认，并分配第二层网络协议
参数：l2——为新增接口分配的第二层网络协议对象
	  ip——为新增接口分配的IP地址
	  mask——为新增接口分配的掩码
	  mac——为新增接口分配的MAC地址，该地址是默认的
	  bootstrap——与MAC地址相关的布尔变量，已经设为false
返回值：加入本地结点以后的接口的指针
*/
{
  Interface* pif;

  if (IsReal())
    { // Real类型的接口, use either wired or wireless interface v1.01
      //if (l2.IsWireless())
      //  {
      //    DEBUG(2,(cout << "Node " << pNode->Id()
      //              << " adding wireless interface" << endl));
      //    pif = new InterfaceWireless(l2, ip, mask, mac, bootstrap);
      //  }
      //else
        {
          DEBUG(2,(cout << "Node " << pNode->Id()
                   << " adding real interface" << hex << ip << endl));
          pif = new InterfaceReal(l2, ip, mask, mac, bootstrap);
        }
    }
  //else v1.01
  //  { // Use a ghost interface
  //    DEBUG(2,(cout << "Node " << pNode->Id()
  //             << " adding ghost interface" << endl));
  //    pif = new InterfaceGhost(l2, ip, mask, mac, bootstrap);
  //  }

  // If firstbitrx node, note first bit rx interface
  if (FirstBitRx())
    {
      pif->FirstBitRx(true);
    }
  
  
  // Add to list  of interface by Mac addresses
  Interface::Add(pif, mac.macAddr);
  pif->SetNode(pNode);
  interfaces.push_back(pif);
  if (!IPKnown())
    { // Default IP address to first interface
      SetIPAddr(ip);
    }
  return pif;
}

Interface* NodeImpl::AddInterface(const L2Proto& l2, 
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
{  //v1.01
  //Interface* pif;
  //// if the interface parameter is of type interface ethernet
  //if(iface.IsEthernet())
  //   {
  //      DEBUG(2,(cout << "Node " << pNode->Id()
  //                 << " adding Ethernet interface" << hex << ip << endl));
  //     pif = new InterfaceEthernet(l2, ip, mask, mac, bootstrap);
  //  }
  //else
  //  {
      return AddInterface(l2, ip, mask, mac, bootstrap);
  //  }

  //pif->SetNode(pNode);
  //interfaces.push_back(pif);
  //if (!IPKnown())
  //  { // Default IP address to first interface
  //    SetIPAddr(ip);
  //  }
  //return pif;
}

Count_t NodeImpl::InterfaceCount()
//获取与该结点绑定的接口的数目，返回值是接口数目
{
  return interfaces.size();
}

const IFVec_t& NodeImpl::Interfaces()
/*
描述：获取与本地结点绑定的所有接口的信息
返回值：Vector类型指针，其中包含了与该结点绑定的所有接口的指针
*/
{ 
  return interfaces;
}

Interface* NodeImpl::GetIfByLink(Link* l)
/*
描述：根据边查找对应接口
参数：l——与本地结点所连的某条边
返回值：本地结点与l对应的接口的指针。若没有找到，返回空
*/
{
  for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
    {
      if (interfaces[i]->GetLink() == l) return interfaces[i]; //找到相应的接口
    }
  return nil;  // Does not exist
}

////void NodeImpl::UseWormContainment(bool b) 
////{
////  usewormcontainment = b;
////}

Interface* NodeImpl::GetIfByNode(Node* n)
/*
描述：根据点查找对应接口
参数：n——与本地结点连通的远程结点
返回值：本地结点与n对应的接口的指针，若没有找到，返回空
*/
{ 
  for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
    {
      Link* pLink = interfaces[i]->GetLink();
      if (!pLink) continue;
      if (pLink->NodeIsPeer(n)) return interfaces[i];//找到相应的接口
    }
  return nil;
}

Interface* NodeImpl::GetIfByIP(IPAddr_t ip)
/*
描述：根据本地接口IP地址查找对应接口
参数：n——与本地结点连通的远程结点
返回值：本地结点与n对应的接口的指针，若没有找到，返回空
*/
{
  for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
    {
      if ((IPAddr_t)interfaces[i]->GetIPAddr() == ip)
        {
          return interfaces[i]; //找到相应的接口
        }
    }
  return nil;
}


Interface* NodeImpl::AddDuplexLink(Node* remotenode)
/*
描述：连接远程结点，本地结点新增一条双向链路和一个接口，双向链路类型为默认。
参数：remotenode——远程结点指针
返回值：新增双向链路所对应的本地接口指针
*/
{
  DuplexLink dl(pNode, remotenode);
  return dl.localif;
}

Interface* NodeImpl::AddDuplexLink(Node* remotenode, const Linkp2p& l)
/*
描述：连接远程结点，本地结点新增一条双向链路和一个接口，双向链路类型指定
参数：remotenode——远程结点指针
      l——新增双向链路的类型
返回值：新增双向链路所对应的本地接口指针
*/
{ 
  DuplexLink dl(pNode, l, remotenode, l);
  return dl.localif;
}

Interface* NodeImpl::AddDuplexLink(Interface* li, Interface* ri)
/*
描述：新建一个连接两个指定接口的双向链路，双向链路类型默认
参数：li——本地结点接口指针
	  ri——远程结点接口指针
返回值：本地新增接口指针
*/
{ 
  DuplexLink dl(li, ri);
  return dl.localif;
}

Interface* NodeImpl::AddDuplexLink(Interface* li, 
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
  DuplexLink dl(li, l, ri, l);
  return dl.localif;
}

Interface* NodeImpl::AddDuplexLink(Node* remotenode,
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
  DuplexLink dl(pNode, li, lm, remotenode, ri, rm);
  return dl.localif;
}

Interface* NodeImpl::AddDuplexLink(Node* rn, 
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
*/
{
  DuplexLink dl(pNode, li, lm, l, rn, ri, rm, l);
  return dl.localif;
}

Link* NodeImpl::GetLink(Node* remotenode)
/*
描述:返回与指定结点联通的双向链路
参数：n——远程结点
返回值：所查找的双向链路存在，则返回它的指针，否则返回空
*/
{
  Interface* i = GetIfByNode(remotenode); //与远程结点连通的本地结点接口
  if (!i) return nil;            //接口不存在
  return i->GetLink();           //接口存在，返回对应的双向链路指针
}

//v1.01
//Interface* NodeImpl::AddRemoteLink(IPAddr_t ip, Mask_t mask)
///*
//描述：仅在分布式模拟器中使用，定义一条连通本地结点与其它结点的双向链路，在另一个模拟过程中使用。双向链路的带宽和延迟默认
//参数：ip——为本地结点新增接口分配的IP地址
//      mask——为本地结点新增接口分配的掩码
//返回值：本地新增接口指针
//*/
//{
//  Interface* iFace = AddInterface(L2Proto802_3(), ip, mask);
//  iFace->SetLink(new LinkRTI(ip, mask, iFace));
//  return iFace;
//}
//
//Interface* NodeImpl::AddRemoteLink(IPAddr_t ip, Mask_t mask, Rate_t rate, Time_t delay)
///*
//描述：仅在分布式模拟器中使用，定义一条连通本地结点与其它结点的双向链路，在另一个模拟过程中使用。双向链路的带宽和延迟指定
//参数：ip——为本地结点新增接口分配的IP地址
//	  mask——为本地结点新增接口分配的掩码
//	  rate——新增双向链路的带宽
//	  delay——新增双向链路的传播延迟
//返回值：本地新增接口指针
//*/
//{
//  Interface* iFace = AddInterface(L2Proto802_3(), ip, mask);
//  iFace->SetLink(new LinkRTI(ip, mask, iFace, rate, delay, 0));
//  return iFace;
//}


Interface* NodeImpl::AddSimplexLink(Node* remotenode)
/*
描述：连接远程结点，本地结点新增一条单向链路和一个接口，链路类型为默认。
参数：remotenode——远程结点指针
返回值：新增链路所对应的本地接口指针
*/
{
  return AddSimplexLink(remotenode, Linkp2p::Default(), IPADDR_NONE, Mask(32));
}

Interface* NodeImpl::AddSimplexLink(Node* remotenode, const Linkp2p& l)
/*
描述：连接远程结点，本地结点新增一条单向链路和一个接口，链路类型指定
参数：remotenode——远程结点指针
	  l——新增链路的类型
返回值：新增链路所对应的本地接口指针
*/
{
  return AddSimplexLink(remotenode, l, IPADDR_NONE, Mask(32));
}

Interface* NodeImpl::AddSimplexLink(Node* remotenode, 
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
  Interface* localif = AddInterface(L2Proto802_3(), ip, mask);
  Interface* remoteif = remotenode->AddInterface(L2Proto802_3(), IPADDR_NONE, Mask(32));
  Linkp2p* lp = (Linkp2p*)l.Copy();
  localif->SetLink(lp);
  lp->SetPeer(remoteif);
  lp->SetLocalIf(localif);
  return localif;
}

void NodeImpl::Neighbors(NodeWeightVec_t& neighbornodes, bool noLeaf)
/*
描述：获取所有的邻近结点，具体方法是通过询问本地结点的每一个接口
参数：[OUT]neighbornodes——包含了本地结点所有的临近结点、与之相关的接口和链路的权重
      noLeaf——是否在查找过程中忽略非路由结点，默认为false即不忽略
*/
{ 
  NodeReal::count3++;
  for (IFVec_t::size_type i = 0; i != interfaces.size(); ++i)
    {
      Interface* iface = interfaces[i];
  //    if (iface->IsWireless()) continue; // ? Is this right?  v1.01
      if (noLeaf)
        { //检查当前路由中的非路由结点，true则忽略，false则不忽略
          if (iface->NeighborCount(pNode) <= 1)
            { // Only one neighbor, find it
              // ?? Why is this so hard?
              Link* l = iface->GetLink();
              if (l)
                {
                  InterfaceBasic* pPeer = l->GetPeer((Packet*)nil);
                  if (pPeer)
                    {
                      Node* neighbornodes = pPeer->GetNode();
                      if (neighbornodes)
                        {
                          if (neighbornodes->NeighborCount() <= 1)
                            { // 最后查到到一个非路由的结点
                              IPAddr_t ip = neighbornodes->GetIPAddr();
                              // 如果是本地路由，忽略
                              if (iface->IsLocalIP(ip)) continue;
                            }
                        }
                    }
                }
            }
        }
      DEBUG(1,(cout << "Node " << pNode->Id() 
               << " asking if about neighbors " << endl));
      iface->Neighbors(neighbornodes);
    }
}

Count_t NodeImpl::NeighborCount()
/*
描述：获取路由表中邻近结点的数目，通过将每个接口的邻接结点相加
返回值：邻接结点的个数
*/
{ 
  if (neighborCount) return neighborCount;//如果前面计算过，直接返回
   
  Count_t t = 0;
  for (IFVec_t::iterator i = interfaces.begin(); i != interfaces.end(); ++i)
    {
      t += (*i)->NeighborCount(pNode);
    }
  neighborCount = t;
  return t;
}

void NodeImpl::NeighborsByIf(Interface* i, IPAddrVec_t& v)
/*
描述：获取与本地结点指定接口相连的邻接结点IP地址
参数：i——本地结点指定接口
	  [OUT]v——所获取的一组邻接结点IP地址
*/
{
  NodeReal::count2++;
  NodeWeightVec_t nwv;
  //查找通过该接口连接的邻接结点
  i->Neighbors(nwv, true); //包括所有结点，乃至单以太网网关
  for (NodeWeightVec_t::iterator j = nwv.begin(); j != nwv.end(); ++j)
    {
      v.push_back(j->node->GetIPAddr()); //返回一串邻接结点的IP地址
    }
}


Interface* NodeImpl::LocalRoute(IPAddr_t ip)
//判断某IP地址是否是直接与本地结点某接口相连，若是则返回该接口
{ 
	DEBUG0((cout << "NodeImpl::LocalRoute, id " << pNode->Id() << endl));
	NodeReal::count1 += interfaces.size();
	for (IFVec_t::iterator i = interfaces.begin(); i != interfaces.end(); ++i)
	{
		if ((*i)->IsLocalIP(ip)) return *i; // found it
	}
	return nil; // Not local
}

Count_t NodeImpl::GetNix(Node* n) const
//根据指定结点查找邻接结点的索引
{
	Count_t nix = 0;
	// Scan the interfaces looking for specified neighbor
	for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
	{
		if (interfaces[i]->NodeIsPeer(n))
		{ // Found it
			return nix + interfaces[i]->NodePeerIndex(n);
		}
		nix += interfaces[i]->NeighborCount(pNode); // Advance neighbor count
	}
	return MAX_COUNT; // Not found
}

#ifdef HAVE_QT

CustomShape_t NodeImpl::CustomShape()
//返回一般图形的回调函数，如未被重载，可忽略
{
  return nil; 
}

void NodeImpl::CustomShape(CustomShape_t)
//如未被重载，可忽略
{ 
}

bool NodeImpl::CustomShapeFile(const char*)
{ 
  return true;
}

bool NodeImpl::CustomShapeImage(const Image&)
{
  return true;
}

#endif

////bool NodeImpl::ICMPEnabled() const
////// 检查本地结点是否支持ICMP,在node—real中重载
////{
////  return false; 
////}
////
////void NodeImpl::DisableICMP()
//////设定本地结点不支持ICMP，在node—real中重载
////{
////  
////}

// Failure management
void NodeImpl::Down()
//设定该结点无效，无法产生或转发数据包
{ //设定每个接口无效
  for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
    {
      interfaces[i]->Down(true);
    }
  down = true;
  Simulator::instance->TopologyChanged(false);
#ifdef HAVE_QT  
  QTWindow* qtw = QTWindow::qtWin;
  Display(qtw);
#endif
}

void NodeImpl::Up()
//设定该结点已经恢复，重新有效
{ //设定每个接口状态为up
  for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
    {
      interfaces[i]->Up();
    }
  down = false;
  Simulator::instance->TopologyChanged(true);
#ifdef HAVE_QT
  QTWindow* qtw = QTWindow::qtWin;
  Display(qtw);
#endif
}

bool NodeImpl::IsDown()
//查看结点是否有效，若无效则返回true，若正常则返回false
{
  return down;
}



  
