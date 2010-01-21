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




// Georgia Tech Network Simulator - Static Routing class
// George F. Riley.  Georgia Tech, Spring 2002

// Statically (one-time) computed and specified routes

//2010.01.04 modified by 张笑盈

#include "routing-static.h"

// Uncomment below to enable debug level 0
// #define DEBUG_MASK 0x01
//#define DEBUG_MASK 0x4
//#define DEBUG_MASK 0x7


void RoutingStatic::Default(RoutingEntry r)
/*
描述：设置默认路由（默认下一跳信息）
*/
{
  defaultRoute = r; // Set the default route
#ifndef WIN32
  DEBUG(1,(cout << "RoutingStatic::Default if " << r.interface
          << " ip " << (string)IPAddr(r.nexthop) << endl));
#else
  DEBUG(1,(cout << "RoutingStatic::Default if " << r.interface_
          << " ip " << (string)IPAddr(r.nexthop) << endl));

#endif
}

RoutingEntry RoutingStatic::GetDefault()
/*
描述：返回默认路由
*/
{
  return defaultRoute;
}

void RoutingStatic::Add( IPAddr_t a, Mask_t m, Interface* i, IPAddr_t ip)
/*
描述：将新的路由信息加入fib中
参数：[IN]a:目标IP地址
      [IN]m:目标IP的掩码
      [IN]i:到下一跳的Interface指针
      [IN]ip:下一跳IP
*/
{
  Count_t mBits = Mask(m).NBits();
  DEBUG(2,(cout << "StaticRouting::Add, target ip " << (string)IPAddr(a)
           << " next hop ip " << (string)IPAddr(ip) 
           << " maskBits " << mBits
           << endl));
  DEBUG(2,(cout << "Befor insert, size of " << mBits 
           << " fib is " << fib[mBits].size() << endl));
  fib[mBits].insert(FIB_t::value_type(a & (Mask_t)Mask(m),
                                  RoutingEntry(i, ip))); // Insert in FIB
  DEBUG(2,(cout << "After insert, size of " << mBits 
           << " fib is " << fib[mBits].size() << endl));
}

RoutingEntry RoutingStatic::Lookup(Node* r, IPAddr_t t)
/*
描述：查找目标IP在fib中对应的RoutingEntry
参数：[IN]n:结点指针，未使用
      [IN]t：待查找的目标IP
备注：将目标IP与各掩码相与得主机号，再在对应的map中查找相应的主机号对应的RoutingEntry
*/
{ // Lookup routing entry for specified target address
  for (FIBMap_t::reverse_iterator ri = fib.rbegin(); ri != fib.rend(); ++ri)
    {
      DEBUG0((cout << "RoutingStatic::Lookup, checking mask lth " 
              << ri->first << endl));
      FIB_t::iterator i = ri->second.find(t & Mask(ri->first));
      if (i != ri->second.end()) return i->second; // Return the routing entry
    }
  return defaultRoute; //如果找不到，则返回默认路由// Return the default route
}

RoutingEntry RoutingStatic::LookupFromPDU(PDU*)
/*
描述:在PDU中查找下一跳路由，未实现
*/
{// Find from PDU (NixVector)..not used for static
  return RoutingEntry();
}

Routing* RoutingStatic::Clone()
/*
描述：返回一个新的RoutingStatic对象
*/
{
  return new RoutingStatic();
}

Routing::RType_t RoutingStatic::Type()
/*
描述：返回当前路由类型，即STATIC
*/
{
  return STATIC;
}

void    RoutingStatic::InitializeRoutes(Node* r)
/*
描述：初始化，生成结点r到其它结点的路由信息并插入到fib中
备注：如果所有边的权值是默认的，则利用BFS方法计算路由信息，否则使用Dijkstra方法，但Dijkstra方法未实现。
      如果该结点只有一个邻居结点，则不需要计算
*/
{ // Initialization, Computing routing table for this node
  // For static routing, use BFS if all link weights default,
  // otherwise use Dijkstra (note...Dijkstra not implemented yet)
  // But first, find out if single neighbor (leaf) node, in which
  // case, no routing computations are necessary
	Count_t nc = r->NeighborCount();//邻居数目
  if (nc <= 1)
    {
      DEBUG0((cout << "Node " << r->Id()
              << " has only 1 neighbor, skipping routes" << endl));
      return;
    }
  DEBUG0((cout << "Node " << r->Id()
          << " has " << nc << " neighbors, calculating routes" << endl));
  NodeIfVec_t nextHop;//从源结点达到各结点时，下一跳路由结点     // Next hop vector
  NodeVec_t   parent; //从源结点到达各结点时，目标结点的前一跳结点      // Parent vector
  const NodeVec_t&  nodes = Node::GetNodes();
  IPAddrVec_t       aliases;
  BFS(nodes, r, nextHop, parent, IPADDR_NONE, aliases);//bfs方法
  // For debug, print the nh vector
  DEBUG0((cout << "Node " << r->Id() << " printing nh table" << endl));
  DEBUG0({for (NodeVec_t::size_type i = 0; i < nextHop.size(); ++i)
    {
      cout << "  NextHop for dest " << i << " is ";
      if (nextHop[i].node)
        {
          cout << nextHop[i].node->Id() << endl;
        }
      else
        {
          cout << "nil pointer" << endl;
        }
    }});
  // First count the most common nexthop, and use that for default
  vector<Count_t> c(nodes.size(), 0);
  Count_t maxCount = 0;      // Maximum value found
  NodeIf  maxNodeIf(nil,nil);
 
  for (NodeVec_t::size_type i = 0; i < nextHop.size(); ++i)//bfs方法返回的nextHop信息
    {
      if (nextHop[i].node)
        { // Next hop exists
          if (++c[nextHop[i].node->Id()] > maxCount)
            { // Found new maximum
              maxCount  = c[nextHop[i].node->Id()];
              maxNodeIf = nextHop[i];
            }
        }
    }
  DEBUG0((cout << "Node " << r->Id() << " max count is " << maxCount
          << " to node " << maxNodeIf.node->Id() << endl));
  if (maxNodeIf.iface)
    { // At least one found
      Default(RoutingEntry(maxNodeIf.iface,
                           maxNodeIf.iface->NodePeerIP(maxNodeIf.node)));
    }
  // Now create routing table entries
  for (NodeVec_t::size_type i = 0; i < nextHop.size(); ++i)
    {
      Node*      target = nodes[i];//各结点
      Node*      node = nextHop[i].node;//从源结点达到各结点时，下一跳路由结点
      Interface* iface = nextHop[i].iface;//从源结点达到各结点时，下一跳Interface
      if (node) 
        { // Node exists
          if (node  != maxNodeIf.node ||
              iface != maxNodeIf.iface)//当前路由不是最好的路由
            { // Non-default found, add the routes
              DEBUG(2,(cout << "Node " << r->Id()
                       << " Non-default route for target " << i << endl));
              // Get a list of all IP addresses for the target node
              IPMaskVec_t ips;
              target->IPAddrs(ips);
              for (IPMaskVec_t::size_type j = 0; j < ips.size(); ++j)
                {
                  DEBUG(2,(cout << "  IPAddr is " << (string)IPAddr(ips[j].ip)
                           << " mask " << Hex8(ips[j].mask) 
                           << " iface " << iface << endl));
                  // Add to routing table
                  Add(ips[j].ip, ips[j].mask, iface, iface->NodePeerIP(node));
                }
            }
        }
    }  
}

void    RoutingStatic::ReInitializeRoutes(Node* r, bool  u)
/*
描述：当拓扑改变时，或者更改路由时，重新初始化结点r在fib中的路由信息
参数：[IN]n：待初始化的结点指针
      [IN]u：未使用
*/
{ // Respond to topology changes
  // Naieve approach, just clear the fibs and re-calculate
  for (FIBMap_t::iterator i = fib.begin(); i != fib.end(); ++i)
    {
      i->second.clear();
    }
  InitializeRoutes(r);
}
  
bool    RoutingStatic::NeedInit()
/*
描述：是否需要初始化路由数据结构，无具体判断，直接返回true
*/
{ // Initialization needed for static
  return true;
}

bool    RoutingStatic::NeedReInit()
/*
描述：是否需要重新初始化路由数据结构，无具体判断，直接返回true
*/
{ // Initialization needed for static
  return true;
}

Size_t  RoutingStatic::Size() const
/*
描述：返回fib占用的字节数
*/
{ // For statistics, return the size (bytes) of the FIB
  Size_t s = 0;
  DEBUG0((cout << "Hello from RoutingStatic::Size() fibsize " << fib.size() 
          << endl));
  for (FIBMap_t::const_iterator i = fib.begin(); i != fib.end(); ++i)
    {
      s += i->second.size() * (sizeof(IPAddr_t) + sizeof(RoutingEntry));
      DEBUG0((cout << "Working on mask length " << i->first
              << " fibsize " << i->second.size()
              << " totsize " << s << endl));
    }
  return s;
}

