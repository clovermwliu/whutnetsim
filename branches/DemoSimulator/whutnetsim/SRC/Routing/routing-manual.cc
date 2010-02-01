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




// Georgia Tech Network Simulator - Manual Routing class
// George F. Riley.  Georgia Tech, Spring 2002

// Manually computed and specified routes
// Manual routing has no way to delete or aggregate entries

//2010.01.04 modified by 张笑盈

#include "routing-manual.h"
// Uncomment below to enable debug level 0
// #define DEBUG_MASK 0x01


void RoutingManual::Default(RoutingEntry r)
/*
描述：设置默认路由（默认下一跳信息）
*/
{
  defaultRoute = r; // Set the default route
#ifndef WIN32
  DEBUG0((cout << "RoutingManual::Default if " << r.interface
          << " ip " << (string)IPAddr(r.nexthop) << endl));

#else
  DEBUG0((cout << "RoutingManual::Default if " << r.interface_
          << " ip " << (string)IPAddr(r.nexthop) << endl));



#endif
}

RoutingEntry RoutingManual::GetDefault()
/*
描述：返回默认路由
*/
{
  return defaultRoute;
}

void RoutingManual::Add( IPAddr_t a, Mask_t m, Interface* i, IPAddr_t ip)
/*
描述：将新的路由信息加入fib中
参数：[IN]a:目标IP地址
      [IN]m:目标IP的掩码
      [IN]i:到下一跳的Interface指针
      [IN]ip:下一跳IP
*/
{
  fib[m].insert(FIB_t::value_type(a & (Mask_t)Mask(m),
                                  RoutingEntry(i, ip))); // Insert in FIB
}

void RoutingManual::Delete( IPAddr_t a, Mask_t m)
/*
描述：删除目标IP地址a在fib中的信息
参数：[IN]a:待删除信息的IP
      [IN]m:待删除信息的IP的掩码 
*/
{
  fib[m].erase(a & (Mask_t)Mask(m));
}

RoutingEntry RoutingManual::Lookup(Node*, IPAddr_t t)
/*
描述：查找目标IP在fib中对应的RoutingEntry
参数：[IN]n:结点指针，未使用
      [IN]t：待查找的目标IP
备注：将目标IP与各掩码相与得主机号，再在对应的map中查找相应的主机号对应的RoutingEntry
*/
{ // Lookup routing entry for specified target address
  // Lookup in FIB starting with longest mask working backwards
  for (FIBMap_t::reverse_iterator ri = fib.rbegin(); ri != fib.rend(); ++ri)
    {
      DEBUG0((cout << "RoutingManual::Lookup, checking mask lth " 
              << ri->first << endl));
      FIB_t::iterator i = ri->second.find(t & Mask(ri->first));
      if (i != ri->second.end()) return i->second; // Return the routing entry
    }
  return defaultRoute; // Return the default route
}

RoutingEntry RoutingManual::LookupFromPDU(PDU*)
/*
描述:在PDU中查找下一跳路由，未实现
*/
{// Find from PDU (NixVector)..not used for manual
  return RoutingEntry();
}

Routing* RoutingManual::Clone()
/*
描述：返回一个新的RoutingManual对象
*/
{
  return new RoutingManual();
}

Routing::RType_t RoutingManual::Type()
/*
描述：返回当前路由类型，即MANUAL
*/
{
  return MANUAL;
}

Size_t RoutingManual::Size() const
/*
描述：返回fib占用的字节数
*/
{ // For statistics, return the size (bytes) of the FIB
  Size_t s = 0;
  for (FIBMap_t::const_iterator i = fib.begin(); i != fib.end(); ++i)
    {
      s += i->second.size() * (sizeof(IPAddr_t) + sizeof(RoutingEntry));
    }
  return s;
}

