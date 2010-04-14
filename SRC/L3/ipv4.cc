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


//File Information: IPV4协议类
//File Name: ipv4.cc
//File Purpose: ipv4网络层仿真
//Original Author:  George F. Riley.
//Author Organization:  Georgia Tech
//Construct Data:  Winter 2003
//Modify Author:  PengZhen
//Author Organization: WHU
//Modify Data: JAN, 2010




// Uncomment below to enable debug level 0
//#define DEBUG_MASK 0x01
// Uncomment below to enable debug level 1
//#define DEBUG_MASK 0x02
// Uncomment below to enable debug level 2
//#define DEBUG_MASK 0x04
//#define DEBUG_MASK 0x07
//#define DEBUG_MASK 0x08
//#define DEBUG_MASK 0x20
//#define DEBUG_MASK 0x80
#include <iostream>

//v1.01
#include "G_debug.h"
#include "ipv4.h"
#include "protograph.h"    //构造函数中会用到
//#include "l2proto802.3.h"  //编译通过
//#include "l4demux.h"    //编译通过
//#include "routing.h"  //编译通过
#include "routing-nixvector.h"  //DataRequest等会用到
//#include "routing-dsr.h" //DataIndication会用到
//#include "routing-nvr.h" //DataIndication会用到
//#include "node.h"  //编译通过
#include "droppdu.h" //DataRequest会用到
//#include "G_globalstats.h"  //编译通过
#include "hex.h"  //IPV4Header构造函数中会用到
#include "tcp.h"   //IPV4Header构造函数中会用到
#include "udp.h"   //IPV4Header构造函数中会用到
#include "icmp.h"  //DataIndication函数中会用到
//#include "routing-aodv.h"   //DataRequest函数中会用到  无线网络的路由策略
//#include "simulator.h"  //编译通过 DEBUG中会用到，因此编译没有影响
#include "interface.h"   //数据类型会用到
//#include "interface-wireless.h"   //DataRequest会用到 无线网络的路由策略
//#include "packet-callbacks.h"   //编译通过


using namespace std;

//定义指向唯一的ipv4实例的指针
IPV4*   IPV4::instance = nil; 
//ipv4从backplane导入的指针
IPV4Header* IPV4::bpHeader = nil; // IPV4 header imported from backplane

IPV4Header::IPV4Header() :
  version(4),
  headerLength(0),
  serviceType(0),
  totalLength(0),
  identification(0),
  flags(0),
  fragmentOffset(0),
  ttl(IPV4::DefaultTTL),
  protocol(0),
  headerChecksum(0),
  src(), dst()
  /*默认构造函数*/
{
}

IPV4Header::IPV4Header(const IPV4Header& r) :
  version(r.version),
  headerLength(r.headerLength),
  serviceType(r.serviceType),
  totalLength(r.totalLength),
  identification(r.identification),
  flags(r.flags),
  fragmentOffset(r.fragmentOffset),
  ttl(r.ttl),
  protocol(r.protocol),
  headerChecksum(r.headerChecksum),
  src(r.src), dst(r.dst)
  /*通过已有协议报头构建的构造函数*/
{
  // Copy the options
  for (PDUVec_t::size_type i = 0; i < r.options.size(); ++i)
    {
      options.push_back(r.options[i]->Copy());
    }
}


IPV4Header::IPV4Header(char* b, Size_t& sz, Packet* p)
/*从串行化Buffer中构建的构造函数*/
{
  DEBUG(3, (cout << "Constructing ipv4, initial size " << sz << endl));
  Size_t s = 0;
  b = Serializable::GetSize(b, sz, s);
  DEBUG(3, (cout << "l3pdu size is " << s << " (" << Hex8(s) << ") " << endl));
  s -= sizeof(Size_t);
  sz -= s;
  b = Construct(b, s);
  sz += s; // Add back in residual (subtraced below)
  DEBUG(3,(cout << "Constructing ipv4, final size " << sz << endl));
  p->PushPDUBottom(this);  // Add to packet
  while (s)
    { // Size remaining, must be options
      // Need to peek ahead inthe buffer to find option number
      DEBUG(3,(cout << "IPv4 excess size is " << s << endl));
      char*  sb  = b;
      Size_t thisSize  = 0;
      Size_t ssz = sz;
      b = Serializable::GetSize(b, sz, thisSize);
      DEBUG(3,(cout << "IPv4 nv size is " << thisSize << endl));
      Byte_t optionNumber = 0;
      b = ConstructFromBuffer(b, sz, optionNumber);
      // Put everything back and construct the appropriate options
      b  = sb;
      sz = ssz;
      switch (optionNumber) {
        // Presently only option implemented is NixVector routing
        case 24 :
          //new NixVectorOption(b, sz, p); // Not an option
          //b += thisSize;
          break;
        }
      s -= thisSize;
    }
  switch (protocol) { // Constuct appropriate l4pdu
    case 6 :  // TCP
      DEBUG(3,(cout << "Constructing TCP header" << endl));
      new TCPHeader(b, sz, p); // Create and push TCP header
      break;
    case 17 : // UDP
      new UDPHeader(b, sz, p); // Create and push UDP header
      DEBUG(3,(cout << "Constructing TCP header" << endl));
      break;
    }
}

IPV4Header::~IPV4Header()
/*析构函数*/
{
  // Delete the options
  for (PDUVec_t::size_type i = 0; i < options.size(); ++i)
    {
      delete options[i];
    }
}

Size_t IPV4Header::Size() const
/*查询ipv4协议报头长度*/
{
  Count_t sz = 20;
  for (PDUVec_t::size_type i = 0; i < options.size(); ++i)
    {
      sz += options[i]->Size();
    }
  return sz;
}

Priority_t IPV4Header::Priority()
/*查询ipv4服务类型（优先级）*/
{
  return (Priority_t)serviceType;
}

void IPV4Header::Trace(Tfstream& tos, Bitmap_t b, Packet* p, const char* s)
/*trace记录ipv4协议报头相关内容*/
{
  DEBUG0((cout << "IPV4 tracing src " << (string)src));
  DEBUG0((cout << " dst "             << (string)dst << endl));
  tos << " ";
  if (s)
    tos << s;
  tos << "L3";
  if (Detail(IPV4::VERSION, b))         tos << "-" << version;
  if (Detail(IPV4::HEADERLENGTH, b))    tos << " " << headerLength;
  if (Detail(IPV4::SERVICETYPE, b))     tos << " " << serviceType;
  if (Detail(IPV4::TOTALLENGTH, b))     tos << " " << totalLength;
  if (Detail(IPV4::IDENTIFICATION, b))  tos << " " << identification;
  if (Detail(IPV4::FLAGS, b))           tos << " " << flags;
  if (Detail(IPV4::FRAGMENTOFFSET, b))  tos << " " << fragmentOffset;
  if (Detail(IPV4::TTL, b))             tos << " " << ttl;
  if (Detail(IPV4::PROTOCOL, b))        tos << " " << protocol;
  if (Detail(IPV4::HEADERCHECKSUM, b))  tos << " " << headerChecksum;
  if (Detail(IPV4::SRC, b))             tos << " " << src;
  if (Detail(IPV4::DST, b))             tos << " " << dst;
  if (Detail(IPV4::UID, b))
    { // Get uid from packet
      if (p)
        tos << " " << p->uid;
      else
        tos << " " << 0;
    }

  // Need to trace options (code later)
}

Size_t IPV4Header::SSize()
/*查询ipv4串行化所需大小*/
{ 
  Size_t r =
    sizeof(version) +
    sizeof(headerLength) +
    sizeof(serviceType) +
    sizeof(totalLength) +
    sizeof(identification) +
    sizeof(flags) +
    sizeof(fragmentOffset) +
    sizeof(ttl) +
    sizeof(protocol) +
    sizeof(headerChecksum) +
    sizeof(src) +
    sizeof(dst);
  DEBUG0((cout << "IPV4Hdr, basic size " << r << endl));
  for (PDUVec_t::size_type i = 0; i < options.size(); ++i)
    { // add the size of the options
      DEBUG0((cout << "ipv4 ssize, opt size " << options[i]->SSize() << endl));
      r += options[i]->SSize() + sizeof(Size_t);
    }
  return r;
}

char*  IPV4Header::Serialize(char* b, Size_t& sz)
/*将ipv4报头串行化到buffer*/
{
  IPAddr_t srcip = src;
  IPAddr_t dstip = dst;
  b = SerializeToBuffer(b, sz, version);
  b = SerializeToBuffer(b, sz, headerLength);
  b = SerializeToBuffer(b, sz, serviceType);
  b = SerializeToBuffer(b, sz, totalLength);
  b = SerializeToBuffer(b, sz, identification);
  b = SerializeToBuffer(b, sz, flags);
  b = SerializeToBuffer(b, sz, fragmentOffset);
  b = SerializeToBuffer(b, sz, ttl);
  b = SerializeToBuffer(b, sz, protocol);
  b = SerializeToBuffer(b, sz, headerChecksum);
  b = SerializeToBuffer(b, sz, srcip);
  b = SerializeToBuffer(b, sz, dstip);
  // Now serialize the options
  for (PDUVec_t::size_type i = 0; i < options.size(); ++i)
    {
      Size_t s = options[i]->SSize() + sizeof(Size_t);
      b = Serializable::PutSize(b, sz, s);
      b = options[i]->Serialize(b, sz);
    }
  return b;
}

char*  IPV4Header::Construct(char* b, Size_t& sz)
/*从串行化buffer中重构ipv4协议报头*/
{
  IPAddr_t srcip;
  IPAddr_t dstip;
  b = ConstructFromBuffer(b, sz, version);
  b = ConstructFromBuffer(b, sz, headerLength);
  b = ConstructFromBuffer(b, sz, serviceType);
  b = ConstructFromBuffer(b, sz, totalLength);
  b = ConstructFromBuffer(b, sz, identification);
  b = ConstructFromBuffer(b, sz, flags);
  b = ConstructFromBuffer(b, sz, fragmentOffset);
  b = ConstructFromBuffer(b, sz, ttl);
  b = ConstructFromBuffer(b, sz, protocol);
  b = ConstructFromBuffer(b, sz, headerChecksum);
  b = ConstructFromBuffer(b, sz, srcip);
  b = ConstructFromBuffer(b, sz, dstip);
  src = IPAddr(srcip);
  dst = IPAddr(dstip);
  // Need to construct options...code later
  return b;
}



IPV4::IPV4()
/*默认构造函数*/
{
	//
  if (instance == nil)
    { // First instance
      instance = this;
      ProtocolGraph::CreateCommon();
      ProtocolGraph::common->Insert(3, Proto(), this); // Layer 3, proto 0x800
      // Set default tracing options
      DetailOff(HEADERLENGTH);
      DetailOff(SERVICETYPE);
      DetailOff(TOTALLENGTH);
      DetailOff(IDENTIFICATION);
      DetailOff(FLAGS);
      DetailOff(FRAGMENTOFFSET);
      DetailOff(HEADERCHECKSUM);
      DetailOff(OPTIONS);
      route_locally = false;
    }
}

void IPV4::DataRequest(Node* n, Packet* p, void* v)
/*网络层协议行为并加上ipv4报头（由传输层调用）*/
{
  //查询数据包是否存在
  if (!n->CallCallbacks(Layer(), Proto(), PacketCallbacks::TX, p)) return;

  //检查数据包是否是广播包
  IPV4ReqInfo* ipInfo = (IPV4ReqInfo*)v;
  if ((IPAddr_t)ipInfo->dst == IPAddrBroadcast)
    { // Special case for broadcasts
      Broadcast(n, p, ipInfo);
      return;
    }

  DEBUG0((cout << "Data Request Size " << p->Size()
		  << " time " << Simulator::Now() << endl));
 
  //是否是本地路由
  if (route_locally)
  {   
	  //是否是发给自身节点
      if(n->LocalIP((IPAddr_t)ipInfo->dst))
      { 
	      cout << "Routing locally" << endl;
          IPV4Header* iphdr = new IPV4Header();
          iphdr->ttl = ipInfo->ttl;
          iphdr->protocol = ipInfo->l4proto;
          iphdr->src = ipInfo->src;
          if ((IPAddr_t)iphdr->src == IPADDR_NONE)
          { //使用节点默认的ip地址
             iphdr->src = n->GetIPAddr();
          }
          iphdr->dst = ipInfo->dst;               //得到目标节点IP
          iphdr->serviceType = ipInfo->tos;
          p->PushPDU(iphdr);                      //添加ipv4报头到数据包
          iphdr->totalLength = p->Size();         //修改报头大小
          if (Trace::Enabled()) n->TracePDU(this, iphdr, p, "-");//trace记录相关信息
          
	      //将数据包向上交付
	      DataIndication(n->GetIfByIP((IPAddr_t)ipInfo->dst), p);
          
	      return;
	 }
  }


/*是否使用新的路由策略*/
#ifdef USE_NEW_ROUTING
 
	RoutingEntry routingEntry = n->GetRouting()->Lookup(NULL,ipInfo->dst);	

	bool local = false;
	if ( n->LocalIP(routingEntry.nexthop) && routingEntry.interface)
	{
		local = true;
	}	

  if (local || n->LocalIP(ipInfo->dst) || (IPAddr_t)ipInfo->dst == IPAddrBroadcast )
  {
      // Destined for this node, get the layer 4 port demux
      //p->SkipPDU(); // Remove the header from the pdu list
      L4Demux* l4demux = (L4Demux*)n->LookupProto(4, ipInfo->l4proto);
      if (!l4demux)
      {
          cout << "HuH?  IPV4 indication with no l4demux object "
               << " proto number " << ipInfo->l4proto
                   << endl;
          delete p;
          return;
      }
      l4demux->DataIndication(n, p, ipInfo->src, *this, n->Interfaces()[0]);
          return;
  }
	else 
  if (!routingEntry.interface )
  {

      //no routing entry ; drop packet
      if (Trace::Enabled()) {
          DropPDU d("L3-SNR", p);
          n->TracePDU(this, &d);
      }
      delete p;
      Stats::pktsDropped++;
      return;
  }

  //pick up the first ip on the list and send it to that ;
  // need to handle load balancing later
  
  Interface *iface = routingEntry.interface;

  //create new ip hdr and send
  IPV4Header* iphdr = new IPV4Header();
  iphdr->ttl = ipInfo->ttl;
  iphdr->protocol = ipInfo->l4proto;
  if (ipInfo->src)
  { // Spoofed source requested
      iphdr->src = ipInfo->src;
  }
  else
  {
      iphdr->src = iface->GetIPAddr(); // Source address
  }
  if ((IPAddr_t)iphdr->src == IPADDR_NONE)
  { // Use default address from node
      iphdr->src = n->GetIPAddr();
  }
  iphdr->dst = ipInfo->dst;               // Destination address
  iphdr->serviceType = ipInfo->tos;
  //iphdr->uid = p->uid;                    // Set unique id from packet
  p->PushPDU(iphdr);                      // Add header to packet
  iphdr->totalLength = p->Size();         // Size including header
  if (Trace::Enabled()) n->TracePDU(this, iphdr, p, "-");// Add to trace file
  
  iface->Send(p, routingEntry.nexthop, 0x0800);
  
  return;
 
#else
      RoutingEntry re;

//v1.01
  // See if wireless device with one interface only
  //if (n->InterfaceCount() == 1)
  //  {
  //    Interface* iface = n->Interfaces()[0];
//      if (iface->IsWireless())
//        { // Wireless device, only one interface
//          InterfaceWireless* ifw = (InterfaceWireless*)iface;
//          if (ifw->GetOpMode() == InterfaceWireless::BSS)
//            {
//#ifndef WIN32
//              re.interface = iface;
//#else
//			re.interface_ = iface;
//#endif
//              re.nexthop = IPADDR_NONE;
//            }
//          else if (ifw->GetOpMode() == InterfaceWireless::ONEHOP)
//            { // No routing, just send with appropriate destination
//#ifndef WIN32
//              re.interface = iface;
//#else
//			  re.interface_ = iface;
//#endif
//
//              re.nexthop = ipInfo->dst;
//            }
//        }
    //}
  
              
#ifdef DONT_DO_THIS          
  // See if one neighbor only, and use it if so
  if (n->NeighborCount() == 1)
    {
      const IFVec_t& ifs = n->Interfaces();
      re.interface = ifs[0];
      re.nexthop = IPADDR_NONE;
    }
#endif

  NixVectorOption* nv = nil;
#ifndef WIN32
  if (!re.interface)
#else
  if (!re.interface_)
#endif
    {
      // Test for NixVector routing, and use if so
      RoutingNixVector* pnv = n->GetNixRouting();
      if (pnv )
        { // Can't use simple single-neighbor route,
          // create a nixvector to the destination and use it
          nv = pnv->GetNixVector(n, ipInfo->dst);
          if (nv)
            { // Found or created a valid NixVector
              DEBUG(5,(cout << "DR - New or Cached NV on node "
                       << (string)IPAddr(n->GetIPAddr())
                       << " dst " << (string)IPAddr(ipInfo->dst)
                       << endl));
              DEBUG(5,(nv->Dump()));
              NixBits_t nix = nv->Extract(nv->BitCount(n->NeighborCount()));
              re = n->LookupRouteNix(nix);

 #ifndef WIN32
  if (!re.interface)
#else
  if (!re.interface_)
#endif
                { // Could not route with this nv, get rid of it.
                  // This is a bug..need to fix later
                  delete nv;
                  nv = nil;
                }
            }
        }
    }

#ifndef WIN32
  if (!re.interface)
#else
  if (!re.interface_)
#endif
   {
      //switch (Routing::Default()->Type())
      //  {
      //    case Routing::DSR:
      //    case Routing::NVR:
      //      n->GetRouting()->DataRequest(n, p, v);
      //      return;
      //    default:
      //      re = n->LookupRoute(ipInfo->dst);
      //      break;
      //  }
	  re = n->LookupRoute(ipInfo->dst);
   }

  //Now try to look up in aodv routing entry if aodv routing
  //exists. if no aodv routing, drop packet.
  //
//v1.01
//#ifndef WIN32
//  if (!re.interface)
//#else
//  if (!re.interface_)
//#endif
//  { //try AODV routing.
//	  RoutingAODV *pRoutingAODV = n->GetRoutingAODV();
//	  if(pRoutingAODV) {
//		  //do AODV routing lookup
//		  AODVRoutingEntry *pRt = n->LookupRouteAODV(ipInfo->dst);
//		  if(pRt && (pRt->Flags==RTF_UP)) {
//			  cout <<"arrive at aodv routing search, Amasing!\n"<<endl;
//			  cout <<"Dst Address is "<<(string)IPAddr(ipInfo->dst) <<endl;
//			  cout <<"pRt->DstIPAddr = " <<(string)IPAddr(pRt->DstIPAddr) <<endl;
//			  cout <<"pRt->Address = " << (string)IPAddr(pRt->Address) <<endl;
//			  //assert(pRt->pInterface!=nil); //test purpose
//#ifndef WIN32
//			  re.interface = pRt->pInterface; //to reuse the following header process
//#else
//			  re.interface_ = pRt->pInterface; //to reuse the following header process
//#endif
//			  re.nexthop = pRt->Address; //this is the next hop!
//		  }
//		  else {//if not found, queue the packet, and do RouteResolve
//			  // Prepare the IP header
//			  IPV4Header* iphdr = new IPV4Header();
//			  iphdr->ttl = ipInfo->ttl;
//			  iphdr->protocol = ipInfo->l4proto;
//			  iphdr->src = n->GetIPAddr();	//Fill this first, modify it when rt found
//			  iphdr->dst = ipInfo->dst;               // Destination address
//			  //iphdr->uid = p->uid;                    // Set unique id from packet
//			  p->PushPDU(iphdr);                      // Add header to packet
//			  iphdr->totalLength = p->Size();         // Size including header
//			  // In this callback function, do queue and do RouteResolve.
//			  pRoutingAODV->RouteResolve(p, (void *)pRoutingAODV->who);
//			  DEBUG0((cout <<"Queue PACKET in AODV and do RouteResolve to Addr"
//				  <<(string)IPAddr(ipInfo->dst) << endl));
//			  return ;
//		  }
//	  }
//	  else { //no aodv routing, still don't know how to route
//		  if (Trace::Enabled()) {
//			  DropPDU d("L3-SNR", p);
//			  n->TracePDU(this, &d);
//		  }
//		  delete p;
//		  Stats::pktsDropped++;
//		  return;
//	  }
//  }//endof trying AODV routing


  // Create and add the IPV4 header
  //构造新的网络层协议报头
  IPV4Header* iphdr = new IPV4Header();
  iphdr->ttl = ipInfo->ttl;

  iphdr->protocol = ipInfo->l4proto;
  if (ipInfo->src)
  { // Spoofed source requested
      iphdr->src = ipInfo->src;
  }
  else
  {

#ifndef WIN32
      iphdr->src = re.interface->GetIPAddr(); // Source address
#else
	  iphdr->src = re.interface_->GetIPAddr(); // Source address
#endif
  }
  if ((IPAddr_t)iphdr->src == IPADDR_NONE)
  { 
	 // 使用节点默认的IP地址
     iphdr->src = n->GetIPAddr();
  }
  iphdr->dst = ipInfo->dst;               // 获取目标IP地址
  iphdr->serviceType = ipInfo->tos;
  //iphdr->uid = p->uid;                  // 用数据包的唯一ID来设置ID值
  if (nv) p->nixVec = nv;                 // Set the nixvector in packet
  p->PushPDU(iphdr);                      // 添加网络层协议报头到数据包
  iphdr->totalLength = p->Size();         // 获取协议报头长度
  if (Trace::Enabled()) n->TracePDU(this, iphdr, p, "-");// trace记录相关信息

  //使用下一跳地址作为目标IP，调用接口的发送函数
#ifndef WIN32
  re.interface->Send(p, re.nexthop, 0x0800);
#else
  re.interface_->Send(p, re.nexthop, 0x0800);
#endif
#endif

}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void IPV4::DataIndication(Interface* i,Packet* p)
/*网络层协议相关内容，解析协议报头，向传输层交付（由链路层调用）*/
{

  //通过接口得到节点指针
  Node* n = i->GetNode();
  //查看数据包是否已经被删除
  if (!n->CallCallbacks(Layer(), Proto(), PacketCallbacks::RX, p, i)) return;
  
  //获得ipv4协议报头指针
  IPV4Header* iphdr = (IPV4Header*)p->PeekPDU();
  //trace协议报头相关信息
  if (Trace::Enabled()) n->TracePDU(this, iphdr, p, "+");
  //修改生存期
  iphdr->ttl--;

  // Process IP options
  if (!iphdr->options.empty())
    {
      for (PDUVec_t::size_type j = 0; j < iphdr->options.size(); ++j)
        {
          IPV4Options* opt = (IPV4Options*)iphdr->options[j];
          //switch (opt->Option())  //v1.01
          //  {
          //  case DSROptionsHeader::Number:
          //  case NVROption::Number:
          //    if (n->GetRouting()->DataIndication(i, p)) return;
          //    break;
          //  default:
          //    cout << "Unknown IP option number " << opt->Option() << endl;
          //    break;
          //  }
        }
    }

/* Direct population of FIB. Used by BGP. This will override Nix and
   might lead to no successful connections at all if adequate care is not taken
   at application level. Use with caution.
*/

#ifdef USE_NEW_ROUTING

	RoutingEntry routingEntry = n->GetRouting()->Lookup(NULL,iphdr->dst);

  bool local = false;
	if ( n->LocalIP(routingEntry.nexthop)&& routingEntry.interface)
	{
		local = true;
	}

  if (local || n->LocalIP(iphdr->dst) || (IPAddr_t)iphdr->dst == IPAddrBroadcast )
  {
      // Destined for this node, get the layer 4 port demux
      p->SkipPDU(); // Remove the header from the pdu list
      L4Demux* l4demux = (L4Demux*)n->LookupProto(4, iphdr->protocol);
      if (!l4demux)
      {
          cout << "HuH?  IPV4 indication with no l4demux object "
               << " proto number " << iphdr->protocol
                   << endl;
          delete p;
          return;
      }
      l4demux->DataIndication(n, p, iphdr->src, *this, i);
          return;
  }
  
  // Needs forwarding
  if (!iphdr->ttl)
  { // TTL expired, drop and log
      if (Trace::Enabled())
      {
          DropPDU d("L3-TTL", p);
          n->TracePDU(this, &d);
      }
      DEBUG(0,(cout << "TTL Expired" << endl));
      // Notify ICMP to (possibly) send a time exceeded message
      ICMP::TimeExceeded(n, p, iphdr, ICMP::TTL_EXCEEDED);
      // Count stats, delete packet, and return
      Stats::pktsDropped++;
      delete p;
      return;
  }

	else if (!routingEntry.interface)
  {
       // can't route, drop the packet,log it.
      // Simply host unreachable for now, needs more checking later
      // Notify ICMP to (possibly) send a time exceeded message
      TCPHeader* pdu = (TCPHeader*)p->PeekPDU(1); // Get the TCP pdu header (peek)
      
      ICMP::DestinationUnreachable(n , p, iphdr, pdu,
                                   ICMP::HOST_UNREACHABLE);
      
      //  delete packet, and return
      if (Trace::Enabled()) {
          DropPDU d("L3-NR", p);
          n->TracePDU(this, &d);
      }
      DEBUG(0,(cout << "No routing interface" << endl));
      Stats::pktsDropped++;
      delete p;
      return;
  }

	Interface *iface = routingEntry.interface;
	
  //pick up the first ip on the list and send it to that ;
  // need to handle load balancing later

	//////////////////////////////////////////////////////////////////////////  
  if (iface == i ||n->GetDomainRoute())
  { // Routing loop (output on same interface as input, log err, drop, return
      if (Trace::Enabled())
      {
          DropPDU d("L3-RL", p);
          n->TracePDU(this, &d);
      }
      DEBUG(0,(cout << "Routing loop" << endl));
      Stats::pktsDropped++;
      delete p;
      return;
  }
  
  //create new ip hdr and send
  IPV4Header* newiphdr = new IPV4Header();
  newiphdr->ttl = iphdr->ttl;
  newiphdr->protocol = iphdr->protocol;
  if (iphdr->src)
  { // Spoofed source requested
      newiphdr->src = iphdr->src;
  }
  else
  {
      newiphdr->src = iface->GetIPAddr(); // Source address
  }
  if ((IPAddr_t)iphdr->src == IPADDR_NONE)
  { // Use default address from node
      newiphdr->src = n->GetIPAddr();
  }
  newiphdr->dst = iphdr->dst;               // Destination address
  newiphdr->serviceType = iphdr->serviceType;
  //iphdr->uid = p->uid;                    // Set unique id from packet


  p->SkipPDU(); //skip the current header
  p->PushPDU(newiphdr);                      // Add new header to packet
  newiphdr->totalLength = p->Size();         // Size including header
  if (Trace::Enabled()) n->TracePDU(this, newiphdr, p, "-");// Add to trace file
  
  iface->Send(p, routingEntry.nexthop,Proto());

return;
#endif


  // For efficiency, first look for non-empty NixVector.  If exists
  // we can skip a lot of the slower lookups below
  bool useNv = false;
  NixVectorOption* nv = nil;
  RoutingNixVector* pnv = n->GetNixRouting();
  if (p->nixVec)
    {
      useNv = p->nixVec->Remaining();
    }

  RoutingEntry re;
  if (useNv)
    {
      nv = p->nixVec;
      NixBits_t nix = nv->Extract(nv->BitCount(n->NeighborCount()));
      DEBUG(5,(cout << "Ind w/ nix at node " << (string)IPAddr(n->GetIPAddr())
               << " nodeid " << n->Id()
               << " is " << nix  << endl));
      DEBUG(5,(cout << "After extract at node "
                   << (string)IPAddr(n->GetIPAddr())
                   << " dst " << (string)IPAddr(iphdr->dst)
                   << " nodeid " << n->Id()
                   << " nv size " << nv->size
                   << " nv used " << nv->used
                   << " nv remaining " << nv->Remaining()
                   << endl));
      DEBUG(5,(nv->Dump()));
      re = n->LookupRouteNix(nix);
    }
#ifndef WIN32
  if (!re.interface)
#else
if (!re.interface_)
#endif
    { // For efficiency, see if we have a cached nv, if so use it
      DEBUG(5,(cout << "IP-DI, no If, checking cached nv" << endl));
      if (pnv)
        { // Look for cached nv
          DEBUG(7,(cout << "Looking or cahced nv" << endl));
          nv = pnv->GetCachedNixVector(iphdr->dst);
          if (nv)
            { // Good, found it.
              DEBUG(5,(cout << "Found cached nv" << endl));
              p->nixVec = nv;
              NixBits_t nix = nv->Extract(nv->BitCount(n->NeighborCount()));
              DEBUG(5,(cout << "Using cached nix at node " << (string)IPAddr(n->GetIPAddr())
                       << " nodeid " << n->Id()
                       << " is " << nix  << endl));
              re = n->LookupRouteNix(nix);
              DEBUG(5,(cout << "After extract at node "
                   << (string)IPAddr(n->GetIPAddr())
                   << " dst " << (string)IPAddr(iphdr->dst)
                   << " nodeid " << n->Id()
                   << " nv size " << nv->size
                   << " nv used " << nv->used
                   << " nv remaining " << nv->Remaining()
                   << endl));
              DEBUG(5,(nv->Dump()));
#ifndef WIN32
              DEBUG(5,(cout << "Used SS cached nv, iface " << re.interface << endl));
#else
			  DEBUG(5,(cout << "Used SS cached nv, iface " << re.interface_ << endl));
#endif
            }
        }
    }
#ifndef WIN32
if (!re.interface)
#else
if (!re.interface_)
#endif
{
    DEBUG(5,(cout << "IP-DI, no If, checking aodv" << endl));
      //if it is for AODV protocol, it MUST continue to pass up for
      //further processing.
      //
      //peek the next protcol data unit
      bool IsAODV = false;
      if(iphdr->protocol==17)
        {//protocol 17 is UDP
          UDPHeader *pUDPHdr = (UDPHeader *)p->PeekPDU(1);
          if(pUDPHdr->destPort == 564)//UDP port 564 is for AODV
            IsAODV = true;
        }
      DEBUG(5,(cout << "Node " << n->Id() << " checking localip, dst "
               << (string)IPAddr(iphdr->dst) << " isLocal "
               << n->LocalIP(iphdr->dst) << endl));
      // Find out if the packet is addressed to this node or needs forwarding
      if (n->LocalIP(iphdr->dst) || (IPAddr_t)iphdr->dst == IPAddrBroadcast
					  || IsAODV )
        {
          // Destined for this node, get the layer 4 port demux
          p->SkipPDU(); // Remove the header from the pdu list
          L4Demux* l4demux = (L4Demux*)n->LookupProto(4, iphdr->protocol);
          if (!l4demux)
            {
              cout << "HuH?  IPV4 indication with no l4demux object "
                   << " proto number " << iphdr->protocol
                   << endl;
              delete p;
              return;
            }
          l4demux->DataIndication(n, p, iphdr->src, *this, i);
          return;
        }
}
#ifndef WIN32
  if (!re.interface)
#else
  if (!re.interface_)
#endif
    { // No nix vector, try route lookup
      DEBUG(5,(cout << "IP-DI, no If, Trying Route lookup" << endl));
      re = n->LookupRoute(iphdr->dst);
    }
#ifndef WIN32
  if (!re.interface)
#else
if (!re.interface_)
#endif
    { // Still no routing info, test for NixVector routing in use
      DEBUG(5,(cout << "IP-DI, no If, checking nv routing" << endl));
      nv = nil;
      if (pnv)
        { // Create a nix vector
          DEBUG(5,(cout << "IP-DI, no If, getting nixvec" << endl));
          nv = pnv->GetNixVector(n, iphdr->dst);
        }
      DEBUG(7,(cout << "IP::DataInd, pnv " << pnv
               << " nv " << nv << endl));
      if (nv)
        { // Created, use it
   	  DEBUG(5,(cout << "New or Cached NV1 at node "
                   << (string)IPAddr(n->GetIPAddr())
                   << " dst " << (string)IPAddr(iphdr->dst)
                   << " nodeid " << n->Id()
                   << " nv size " << nv->size
                   << " nv used " << nv->used
                   << " nv remaining " << nv->Remaining()
                   << endl));
	  DEBUG(5,(nv->Dump()));
          p->nixVec = nv;
          NixBits_t nix = nv->Extract(nv->BitCount(n->NeighborCount()));
   	  DEBUG(5,(cout << "After Extract, new or Cached NV1 at node "
                   << (string)IPAddr(n->GetIPAddr())
                   << " dst " << (string)IPAddr(iphdr->dst)
                   << " nodeid " << n->Id()
                   << " nv size " << nv->size
                   << " nv used " << nv->used
                   << " nv remaining " << nv->Remaining()
                   << endl));
          re = n->LookupRouteNix(nix);
#ifndef WIN32
          DEBUG(5,(cout << "IP::Dataind, nix " << nix
                   << " re.iface " << re.interface << endl));
#else
		  DEBUG(5,(cout << "IP::Dataind, nix " << nix
			       << " re.iface " << re.interface_ << endl));
#endif
        }
    }
#ifndef WIN32
  if (!re.interface)
#else
  if (!re.interface_)
#endif
    { // Still can't route, use default
      Routing* pRouting = n->GetRouting();
      if (pRouting) re = pRouting->GetDefault();
    }

  // Needs forwarding
  if (!iphdr->ttl)
    { // TTL expired, drop and log
      if (Trace::Enabled())
        {
          DropPDU d("L3-TTL", p);
          n->TracePDU(this, &d);
        }
      DEBUG(0,(cout << "TTL Expired" << endl));
      // Notify ICMP to (possibly) send a time exceeded message
	  //v1.01
      //ICMP::TimeExceeded(n, p, iphdr, ICMP::TTL_EXCEEDED);
      // Count stats, delete packet, and return
      Stats::pktsDropped++;
      //delete p;
      return;
    }
  //Now try to look up in aodv routing entry if aodv routing exists.
  //Special NOTE: for wireless, the incoming interface can
  //              be the outgoing interface.!
//v1.01
//#ifndef WIN32
//  if (!re.interface)
//#else
//  if (!re.interface_)
//#endif
//
//  { //try AODV routing entry lookup
//	  RoutingAODV *pRoutingAODV = n->GetRoutingAODV();
//	  if(pRoutingAODV) {
//		  //do AODV routing lookup
//		  AODVRoutingEntry *pRt = n->LookupRouteAODV(iphdr->dst);
//		  if(pRt&&(pRt->Flags==RTF_UP)){
//			  pRt->pInterface->Send(p, pRt->Address, 0x0800);
//			  return;
//		  }
//		  else if(iphdr->dst.ip == IPAddrBroadcast) {
//			  //Not sure if this kind of process is correct or not.
//			  //
//			  IPV4ReqInfo *ipInfo = new IPV4ReqInfo(iphdr->dst,iphdr->src,
//				  iphdr->ttl, iphdr->protocol);
//			  Broadcast(n ,p, ipInfo); //using private method if broadcast.
//			  delete ipInfo;
//			  return;
//		  }
//		  else {//if not found,drop the packet and log the event.
//			  DEBUG0((cout <<"NO ROUTE, DROP PACKET to Addr"
//				  <<(string)IPAddr(iphdr->dst) << endl));
//			  if (Trace::Enabled()) {
//				  DropPDU d("L3-NR", p);
//				  n->TracePDU(this, &d);
//			  }
//			  DEBUG(0,(cout << "No routing interface" << endl));
//			  Stats::pktsDropped++;
//			  delete p;
//			  return ;
//		  }
//	  }
//	  else { //still can't route, drop the packet,log it.
//		  // Simply host unreachable for now, needs more checking later
//		  // Notify ICMP to (possibly) send a time exceeded message
//		  TCPHeader* pdu = (TCPHeader*)p->PeekPDU(1); // Get the TCP pdu header (peek)
//
//		  ICMP::DestinationUnreachable(n , p, iphdr, pdu,
//			  ICMP::HOST_UNREACHABLE);
//
//		  //  delete packet, and return
//		  if (Trace::Enabled()) {
//			  DropPDU d("L3-NR", p);
//			  n->TracePDU(this, &d);
//		  }
//		  DEBUG(0,(cout << "No routing interface" << endl));
//		  Stats::pktsDropped++;
//		  delete p;
//		  return;
//	  }
//  }//end of trying aodv routing.
//



#ifndef WIN32
  if (re.interface == i)
#else
  if (re.interface_ == i ||n->GetDomainRoute())
#endif
    { // Routing loop (output on same interface as input, log err, drop, return
      if (Trace::Enabled())
        {
          DropPDU d("L3-RL", p);
          n->TracePDU(this, &d);
        }
      DEBUG(0,(cout << "Routing loop" << endl));
      Stats::pktsDropped++;
      delete p;
      return;
    }
  // Forward to next hop
  if (Trace::Enabled()) n->TracePDU(this, iphdr, p, "-"); // Add to trace file
#ifndef WIN32
  re.interface->Send(p, re.nexthop, Proto());
#else
  re.interface_->Send(p, re.nexthop, Proto());
#endif

#ifdef OLD_SUPERCEEDED
  // Need better..not all l3 interfaces are 802.3
  DEBUG0((cout << "IPV4 Forwarding pkt to if " << re.interface << endl));
  L2Info802_3 l2Info(re.interface->GetMACAddr(),         // Src MAC Addr
                     re.interface->IPToMac(re.nexthop),  // Dst MAC Addr
                     Proto());                           // Protocol
  L2Proto* l2proto = re.interface->GetL2Proto(); // Get l2 protocol obj
  l2proto->DataRequest(re.interface, p, &l2Info); // Pass down to layer2
#endif
}

Interface* IPV4::PeekInterface(Node* n, void* v)
{ // The value passed to ipv4 is an ip address pointer
  IPAddr_t* ip = (IPAddr_t*)v;

  // First see if we only have one interface and one neighbor
  if (n->InterfaceCount() == 1)
    { // See if only one peer on this interface
      const IFVec_t& interfaces = n->Interfaces();
      Interface* i = interfaces[0];
      if (i->NeighborCount(n) == 1)
        { // Ok, just use the single peer
          return i;
        }
    }

  // Find the interface used for this route and pass back to caller
  RoutingEntry re = n->LookupRoute(*ip);
#ifndef WIN32
  if (!re.interface)
#else
if (!re.interface_)
#endif
    { // No known route, test for NixVector routing.
      RoutingNixVector* pnv = n->GetNixRouting();
      if (pnv)
        { // Create a nixvector to the destination and use it
          NixVectorOption* nv = pnv->GetNixVector(n, *ip);
          if (nv)
            { // Found or created a valid NixVector
              NixBits_t nix = nv->Extract(nv->BitCount(n->NeighborCount()));
              re = n->LookupRouteNix(nix);
              delete nv; // Don't need this nixvector at the moment
            }
        }
    }
#ifndef WIN32
  return re.interface;
#else
  return re.interface_;
#endif
}

// Private Methods
void IPV4::Broadcast(Node* n, Packet* p, IPV4ReqInfo* r)
{
  // First Common processing (before looping on all interfaces)
  // For broadcasts, the "src" field of IPV4ReqInfo is used to determine
  // which interfaces the packet is broadcast to.  If it is IPADDR_NONE
  // or IPAddrBroadcast, then it is sent on all interfaces.  If not,
  // it is sent only on the interface with an IP address matching
  // the specified src address.
  IPV4Header* iphdr = new IPV4Header();
  iphdr->ttl = r->ttl;
  iphdr->protocol = r->l4proto;
  iphdr->dst = r->dst;                    // Destination address
  iphdr->serviceType = r->tos;
  //iphdr->uid = p->uid;                    // Set unique id from packet
  p->PushPDU(iphdr);                      // Add header to packet
  iphdr->totalLength = p->Size();         // Size including header

  DEBUG0((cout << "IPV4 Broadcast, src " << (string)IPAddr(r->src) << endl));
  // Now forward the packet to all interfaces
  const IFVec_t ifVec = n->Interfaces();  // Vector of all interface this node
  for (IFVec_t::size_type i = 0; i < ifVec.size(); ++i)
    {
      Interface* iface = ifVec[i];
      if (iface->GetLink() == nil) continue; // Wrong side of simplex link
      bool sendIt = false;
      if ((IPAddr_t)(r->src) == IPADDR_NONE ||
          (IPAddr_t)(r->src) == IPAddrBroadcast)
        {
          sendIt = true; // Send on all interfaces
          iphdr->src = iface->GetIPAddr(); // Source address
          if ((IPAddr_t)iphdr->src == IPADDR_NONE)
            { // Use default address from node
              iphdr->src = n->GetIPAddr();
            }
        }
      else
        {
          iphdr->src = r->src;
          sendIt = (r->src == iface->GetIPAddr()); // Send only on spec'd iface
        }
      if (!sendIt) continue; // Don't send on this interface
      // Add to trace file
      if (Trace::Enabled()) n->TracePDU(this, iphdr, p, "-");
      Packet* p1 = p->Copy();
      iface->Send(p1, 0x0800);
    }
  delete p; // Since we replicated the packet on each iface, done with this one
}

// Static methods
IPV4* IPV4::Instance()
{
	//保证只有一个ipv4的对象
  if (!instance) instance = new IPV4();
  return instance;
}

// Backplane related methods
class BPIPV4
{
public:
  typedef enum {
      VERSION,
      HLEN,
      TOS,
      TLEN,
      ID,
      TTL,
      PROTO,
      CKSUM,
      SADDR,
      DADDR,
      OPTIONSSIZE,
      OPTIONSDATA
  } IPV4Items_t;
};
#ifndef WIN32
void IPV4::RegisterBackplane(DSHandle_t DST)
{
PRHandle_t prh;
ITHandle_t ith;

  prh = RegisterProtocol("IP", PROTO_REQUIRED, IPV4ExportQuery, nil);
  ith = RegisterItem(prh, "Header Length", ITEM_OPTIONAL, 8,
                     IPV4ExportCallback, (void*)BPIPV4::HLEN,
                     IPV4ImportCallback, (void*)BPIPV4::HLEN,
                     IPV4DefaultCallback, (void*)BPIPV4::HLEN);
  ith = RegisterItem(prh, "Type of Service", ITEM_OPTIONAL, 8,
                     IPV4ExportCallback, (void*)BPIPV4::TOS,
                     IPV4ImportCallback, (void*)BPIPV4::TOS,
                     IPV4DefaultCallback, (void*)BPIPV4::TOS);
  ith = RegisterItem(prh, "Total Length", ITEM_REQUIRED, 32,
                     IPV4ExportCallback, (void*)BPIPV4::TLEN,
                     IPV4ImportCallback, (void*)BPIPV4::TLEN,
                     IPV4DefaultCallback, (void*)BPIPV4::TLEN);
  ith = RegisterItem(prh, "Identification", ITEM_REQUIRED, 16,
                     IPV4ExportCallback, (void*)BPIPV4::ID,
                     IPV4ImportCallback, (void*)BPIPV4::ID,
                     IPV4DefaultCallback, (void*)BPIPV4::ID);
  ith = RegisterItem(prh, "Time to Live", ITEM_OPTIONAL, 8,
                     IPV4ExportCallback, (void*)BPIPV4::TTL,
                     IPV4ImportCallback, (void*)BPIPV4::TTL,
                     IPV4DefaultCallback, (void*)BPIPV4::TTL);
  ith = RegisterItem(prh, "Protocol", ITEM_OPTIONAL, 8,
                     IPV4ExportCallback, (void*)BPIPV4::PROTO,
                     IPV4ImportCallback, (void*)BPIPV4::PROTO,
                     IPV4DefaultCallback, (void*)BPIPV4::PROTO);
  ith = RegisterItem(prh, "Source Address", ITEM_OPTIONAL, 32,
                     IPV4ExportCallback, (void*)BPIPV4::SADDR,
                     IPV4ImportCallback, (void*)BPIPV4::SADDR,
                     IPV4DefaultCallback, (void*)BPIPV4::SADDR);
  ith = RegisterItem(prh, "Destination Address", ITEM_REQUIRED, 32,
                     IPV4ExportCallback, (void*)BPIPV4::DADDR,
                     IPV4ImportCallback, (void*)BPIPV4::DADDR,
                     IPV4DefaultCallback, (void*)BPIPV4::DADDR);
}
#endif // WIN32
int   IPV4::IPV4ExportQuery(char* pSource, int, void*)
{ // Find if an IPV4 header is present in this packet
  Packet*    pPacket = (Packet*)pSource;
  IPV4Header* pHdrIP = nil;
  // Find the IP header
  for (PDUVec_t::size_type i = 0; i < pPacket->top; ++i)
    {
      PDU* pdu = pPacket->PDUs[i];
      if (pdu && (pdu->Layer() == 3) & (pdu->Version() == 4))
        { // Found it
          pHdrIP = (IPV4Header*)pdu;
          break;
        }
    }
  return pHdrIP != nil;
}

int   IPV4::IPV4DefaultCallback(char*, void*)
{
  return 0;
}

int   IPV4::IPV4ExportCallback(char* pSource, char* pRet, int lRet, void* p)
{
Packet*    pPacket = (Packet*)pSource;
BPIPV4::IPV4Items_t  e = (BPIPV4::IPV4Items_t)((int)p);
unsigned long*  pul = (unsigned long*)pRet;
unsigned short* pus = (unsigned short*)pRet;
unsigned char*  puc = (unsigned char*)pRet;

  DEBUG0((cout << "Hello from IPPCallback, item " << e << endl));
  IPV4Header* pHdrIP = nil;
  // Find the IP header
  for (PDUVec_t::size_type i = 0; i < pPacket->top; ++i)
    {
      PDU* pdu = pPacket->PDUs[i];
      if (pdu && (pdu->Layer() == 3) & (pdu->Version() == 4))
        { // Found it
          pHdrIP = (IPV4Header*)pdu;
          break;
        }
    }

  if (!pHdrIP) return 0; // Nope, no IPV4 header here

  switch(e) {
  case BPIPV4::HLEN:
    *puc = pHdrIP->headerLength;
    return sizeof(*puc);

  case BPIPV4::TOS:
    *puc = pHdrIP->serviceType;
    return sizeof(*puc);

  case BPIPV4::TLEN:
    *pus = pHdrIP->totalLength;
    return sizeof(*pus);

  case BPIPV4::ID:
    *pus = pHdrIP->identification;
    return sizeof(*pus);

  case BPIPV4::TTL:
    *puc = pHdrIP->ttl;
    return sizeof(*puc);

  case BPIPV4::PROTO:
    *puc = pHdrIP->protocol;
    return sizeof(*puc);

  case BPIPV4::CKSUM:
    *pus = pHdrIP->headerChecksum;
    return sizeof(*pus);

  case BPIPV4::SADDR:
    *pul = pHdrIP->src;
    return sizeof(*pul);

  case BPIPV4::DADDR:
    *pul = pHdrIP->dst;
    return sizeof(*pul);

  default:
    break;
  }
  return 0;
}

int   IPV4::IPV4ImportCallback(char* pSource, int lSource, char* pDest,void* p)
{
  BPIPV4::IPV4Items_t e = (BPIPV4::IPV4Items_t)((int)p);
  unsigned long*  pul = (unsigned long*)pSource;
  unsigned short* pus = (unsigned short*)pSource;
  unsigned char*  puc = (unsigned char*)pSource;

  if (!bpHeader)
    { // Need a header
      bpHeader = new IPV4Header();
    }

  DEBUG0((cout << "Hello from IPV4ImportCallback, item " << e << endl));
  switch(e) {
  case BPIPV4::HLEN:
    bpHeader->headerLength = *puc;
    break;

  case BPIPV4::TOS:
    bpHeader->serviceType = *puc;
    break;

  case BPIPV4::TLEN:
    bpHeader->totalLength = *pus;
    break;

  case BPIPV4::ID:
    bpHeader->identification = *pus;
    break;

  case BPIPV4::TTL:
    bpHeader->ttl = *puc;
    break;

  case BPIPV4::PROTO:
    bpHeader->protocol = *puc;
    break;

  case BPIPV4::CKSUM:
    bpHeader->headerChecksum = *pus;
    break;

  case BPIPV4::SADDR:
    bpHeader->src = *pul;
    break;

  case BPIPV4::DADDR:
    bpHeader->dst = *pul;
    break;

  default:
    break;
  }
  return 0;
}

