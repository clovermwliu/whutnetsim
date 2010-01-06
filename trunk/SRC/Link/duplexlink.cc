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
#include <iostream>
#include <string>

#include "G_debug.h"
#include "duplexlink.h"
#include "l2proto802.3.h"
#include "node.h"
#include "interface.h"
//v1.01
//#include "link-ghost.h"
//#include "link-rti.h"
#include "ipaddr.h"

using namespace std;

DuplexLink::DuplexLink(Node* ln, Node* rn)
    : local (nil), remote(nil), localif(nil), remoteif(nil)
//根据本地结点和远程结点添加默认的双向链路
{
  AddSimplexLinks(ln, IPADDR_NONE, MASK_ALL, Link::Default(),
                  rn, IPADDR_NONE, MASK_ALL, Link::Default());
}

DuplexLink::DuplexLink(Node* ln, 
					   const Linkp2p& ll,
                       Node* rn, 
					   const Linkp2p& rl)
  : local (nil), remote(nil), localif(nil), remoteif(nil)
//根据本地结点、待添加链路和远程结点、待添加链路添加双向链路
{ 
  AddSimplexLinks(ln, IPADDR_NONE, MASK_ALL, ll,
                  rn, IPADDR_NONE, MASK_ALL, rl);
}

DuplexLink::DuplexLink(Interface* li,
                       Interface* ri)
  : local (nil), remote(nil),
    localif(li), remoteif(ri)
//根据本地结点接口和远程结点接口添加默认的双向链路
{
  AddSimplexLinks(li->GetNode(), IPADDR_NONE, MASK_ALL, Link::Default(),
                  ri->GetNode(), IPADDR_NONE, MASK_ALL, Link::Default());
}

DuplexLink::DuplexLink(Interface* li, 
					   const Linkp2p& ll,
                       Interface* ri, 
					   const Linkp2p& rl)
//根据本地结点接口、待添加链路和远程结点接口、待添加链路添加双向链路
  : local ((Linkp2p*)ll.Copy()),
    remote((Linkp2p*)rl.Copy()),
    localif(li), remoteif(ri)
{ 
  AddSimplexLinks(li->GetNode(), IPADDR_NONE, MASK_ALL, ll,
		  ri->GetNode(), IPADDR_NONE, MASK_ALL, rl);
}

DuplexLink::DuplexLink(Node* ln, 
					   IPAddr_t la, 
					   Mask_t lm,
                       Node* rn, 
					   IPAddr_t ra, 
					   Mask_t rm)
//根据本地结点、IP地址、掩码和远程结点、IP地址、掩码添加双向链路
  : local(nil), remote(nil), localif(nil), remoteif(nil)
{
  AddSimplexLinks(ln, la, lm, Link::Default(),
                  rn, ra, rm, Link::Default());
}

DuplexLink::DuplexLink(Node* ln, IPAddr_t la, Mask_t lm,  const Linkp2p& ll,
                       Node* rn, IPAddr_t ra, Mask_t rm,  const Linkp2p& rl)
  : local(nil), remote(nil), localif(nil), remoteif(nil)
{
  AddSimplexLinks(ln, la, lm, ll, rn, ra, rm, rl);
}

DuplexLink::~DuplexLink()
{ 

}


void DuplexLink::AddSimplexLinks(Node* ln, 
								 IPAddr_t la, 
								 Mask_t lm, 
								 const Link& ll,
								 Node* rn, 
								 IPAddr_t ra, 
								 Mask_t rm, 
								 const Link& rl)
/*
描述：创建两条链路，本地结点、IP地址、掩码、待添加链路以及远程结点、IP地址、掩码、待添加链路已知。
     （新添加的链路和接口分别被赋值给成员local，localif，remote，remoteif）
参数：ln——本地结点 
	  la——本地结点ip地址 
	  lm——本地结点掩码 
	  ll——本地结点待添加的链路
	  rn——远程结点 
	  ra——远程结点ip地址 
	  rm——远程结点掩码 
	  rl——远程结点待添加的链路
*/

{
//检查本地结点是否已经存在ip地址为la的接口，若存在直接使用，否则创建
  if (la != IPADDR_NONE && !localif) localif = ln->GetIfByIP(la);
  if ( !localif)
    {
      DEBUG0((cout << "Node " << ln->Id()
              << " adding new local i/f, ip " << (string)IPAddr(la) << endl));
      //先查看链路是否需要一个特殊类型的接口
      localif = ll.GetNewInterface();
      if (localif)//若需要
        {
          localif->SetIPAddr(la);
        }
      else
        { //不需要则创建一个802.3 类型的p2p接口
          localif = ln->AddInterface(L2Proto802_3(), la, lm);
        }
    }
  if (ra != IPADDR_NONE && !remoteif) remoteif = rn->GetIfByIP(ra);//创建远程结点接口
  if ( !remoteif)
    {
      DEBUG0((cout << "Node " << rn->Id()
              << " adding new remote i/f, ip " << (string)IPAddr(ra) << endl));
      // First ask link if a particular interface type is needed
      remoteif = ll.GetNewInterface();
      if (remoteif)
        {
          remoteif->SetIPAddr(ra);
        }
      else
        { // Link doesn't care, create a 802.3 style p2p interface
          remoteif = rn->AddInterface(L2Proto802_3(), ra, rm);
        }
    }

  bool localReal = ln->IsReal();
  bool remoteReal = rn->IsReal();
  if (localReal && remoteReal)//若都是real类型结点
    { 
      local = ll.Copy();
      remote = rl.Copy();
    }
  //if (!localReal)//本地结点是ghost型，使用ghost链路
  //  { 
  //    local = new LinkGhost();
  //    DEBUG0((cout << "Using Ghost from localNode " << ln->Id()
  //            << " to node " << rn->Id() << endl));
  //  }
  
  //if (!remoteReal)//远程结点是ghost型，使用ghost链路
  //  { 
  //    remote = new LinkGhost();
  //    DEBUG0((cout << "Using Ghost from remoteNode " << rn->Id()
  //            << " to node " << ln->Id() << endl));
  //  }
  
  //if (localReal && !remoteReal)//本地结点为real，远程为ghost，使用RTI类型的链路
  //  { 
  //    Rate_t bw = ll.Bandwidth();
  //    Time_t delay = ll.Delay();
  //    local = new LinkRTI(la, lm, localif, bw, delay, rn->Id() );
  //    DEBUG0((cout << "Using RTI from localNode " << ln->Id()
  //            << " to node " << rn->Id() << endl));
  //  }
  //    
  //if (remoteReal && !localReal)
  //  { //本地结点为ghost，远程为real，使用RTI类型的链路
  //    Rate_t bw = rl.Bandwidth();
  //    Time_t delay = rl.Delay();
  //    remote = new LinkRTI(ra, rm, remoteif, bw, delay, ln->Id());
  //    DEBUG0((cout << "Using RTI from remoteNode " << rn->Id()
  //            << " to node " << ln->Id() << endl));
  //  }
      
  localif->SetLink(local);
  remoteif->SetLink(remote);
  local->SetPeer(remoteif);
  local->SetLocalIf(localif);
  remote->SetPeer(localif);
  remote->SetLocalIf(remoteif);
 
#ifdef USE_NEW_ROUTING
//更新路由
  IPAddr ip(ra);
  string str(IPAddr::ToDotted(ip));
  int pos = str.rfind('.');
  string ipstr = string ( str,0,pos+1);
  char subnet[4];
  bzero(subnet,4);
  sprintf(subnet,"%d",0);
  ipstr += subnet;

	ln->GetRouting()->Add(IPAddr(ipstr.c_str()),24,localif,IPAddr(ipstr.c_str()));

	//NOTE: This is quite a lot of customization for bgp++; When a link is added,
	//we add the router id and the prefix the router is advertising to the 
	//FIB. This might not work for all other purposes.

	ipstr[ipstr.size()-1]= '1';
	ln->GetRouting()->Add(IPAddr(ipstr.c_str()),32,localif,IPAddr(ipstr.c_str()));

  ip = IPAddr(la);
  str = string(IPAddr::ToDotted(ip));
  pos = str.rfind('.');
  ipstr = string ( str,0,pos+1);
  bzero(subnet,4);
  sprintf(subnet,"%d",0);
  ipstr += subnet;

	rn->GetRouting()->Add(IPAddr(ipstr.c_str()),24,remoteif,IPAddr(ipstr.c_str()));

	ipstr[ipstr.size()-1]= '1';
	rn->GetRouting()->Add(IPAddr(ipstr.c_str()),32,remoteif,IPAddr(ipstr.c_str()));
#endif

  DEBUG0((cout << "DuplexLink::AddSimplexLinks, node " << ln->Id()
          << " if " << localif << endl));
  DEBUG0((cout << "DuplexLink::AddSimplexLinks, node " << rn->Id()
          << " if " << remoteif << endl));
}


