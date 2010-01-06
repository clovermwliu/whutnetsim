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

//更改人：李玉
//更改时间：2010-1-4

#include <math.h>

#include "dumbbell1.h"
#include "node.h"
#include "linkp2p.h"
#include "mask.h"
#include "star1.h"
using namespace std;

// Constructors

//Dumbbell1::Dumbbell1(Count_t l, Count_t r, Mult_t m, SystemId_t id)
//{
//  ConstructorHelper(l, r, m, IPADDR_NONE, IPADDR_NONE, Linkp2p::Default(), id);
//}
//
//Dumbbell1::Dumbbell1(Count_t l, Count_t r, Mult_t m,
//                   const Linkp2p& link, SystemId_t id)
//{
//  ConstructorHelper(l, r, m, IPADDR_NONE, IPADDR_NONE, link, id);
//}
//
//Dumbbell1::Dumbbell1(Count_t l, Count_t r, Mult_t m,
//                   IPAddr_t lip, IPAddr_t rip, SystemId_t id)
//{
//  ConstructorHelper(l, r, m, lip, rip, Linkp2p::Default(), id);
//}
//
//Dumbbell1::Dumbbell1(Count_t l, Count_t r, Mult_t m,
//                   IPAddr_t lip, IPAddr_t rip,
//                   const Linkp2p& link, SystemId_t id)
//{
//  ConstructorHelper(l, r, m, lip, rip, link, id);
//}
Dumbbell1::Dumbbell1(Count_t l, Count_t r,Mult_t m,
					 IPAddr_t lip, IPAddr_t rip,
					 const Linkp2p& link, SystemId_t id)
:CHiberTopoBase(lip,link,id),leftCount(l),rightCount(r),bound(m),leftip(lip),rightip(rip)
{
}
bool Dumbbell1::GenerateTopo()
{
	left=new Star1(leftCount,link,leftip);                  
	left->GenerateTopo();
	
	right=new Star1(rightCount,link,rightip);                  
	right->GenerateTopo();

	Node* ln=left->GetHub();
	Node* rn=right->GetHub();
	Linkp2p routerLink(link);
	routerLink.Bandwidth(link.Bandwidth() * bound);
	AddDuplexLink(ln, rn, routerLink);
	return true;
}
void Dumbbell1::SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
									   const Location& BoundBoxRightUpper,
							           BoxType  type)
{
	Meters_t  xlow=BoundBoxLeftDown.X();
	Meters_t  xhigh=BoundBoxRightUpper.X();
	Meters_t  dist=xhigh-xlow;
	left->StarSetLocationViaBoundBox(BoundBoxLeftDown, Location(xlow+2*dist/3,BoundBoxRightUpper.Y()),M_PI/2,M_PI);

	right->StarSetLocationViaBoundBox(Location(xlow+dist/3,0),BoundBoxRightUpper,-M_PI/2,M_PI);
}
//void Dumbbell1::ConstructorHelper(Count_t left, Count_t right,
//                                 Mult_t m, 
//                                 IPAddr_t leftIP, IPAddr_t rightIP,
//                                 const Linkp2p& link, SystemId_t id)
//{
//  leftCount = left;
//  rightCount = right;
//  first = Node::nextId;
//  // First left and right routers, then left side, then right side
//  Node* lr = new Node(id);
//  Node* rr = new Node(id);
//  DEBUG0((cout << "DB LR id " << lr->Id() << endl));
//  DEBUG0((cout << "DB RR id " << rr->Id() << endl));
//
//  // Connect the routers, with reduced bandwidth
//  DEBUG0((cout << "Constructing Dumbbell1 bottleneck, link bw "
//          << link.Bandwidth() << endl));
//  Linkp2p routerLink(link);
//  routerLink.Bandwidth(link.Bandwidth() * m);
//  DEBUG0((cout << "Router Link bw " << routerLink.Bandwidth() << endl));
//  lr->AddDuplexLink(rr, routerLink);
//  
//  // Set default routes
//  lr->DefaultRoute(rr);
//  rr->DefaultRoute(lr);
//  
//  for (Count_t i = 0; i < left; ++i)
//    {
//      Node* ln = new Node(id);
//      DEBUG0((cout << "DB LN id " << ln->Id() << endl));
//      IPAddr_t ip = IPADDR_NONE;
//      if  (leftIP != IPADDR_NONE)
//        { // Assign specified ip addresses
//          ip = leftIP;
//        }
//      ln->AddDuplexLink(lr, link, ip);
//      // Add a default route
//      ln->DefaultRoute(lr);
//      if (leftIP != IPADDR_NONE) leftIP ++; // Advance to next ip
//    }
//  for (Count_t i = 0; i < right; ++i)
//    {
//      Node* rn = new Node(id);
//      DEBUG0((cout << "DB RN id " << rn->Id() << endl));
//      IPAddr_t ip = IPADDR_NONE;
//      if  (rightIP != IPADDR_NONE)
//        { // Assign specified ip addresses
//          ip = rightIP;
//        }
//      rn->AddDuplexLink(rr, link, ip);
//      // Add a default route
//      rn->DefaultRoute(rr);
//      if (rightIP != IPADDR_NONE) rightIP ++; // Advance to next ip
//    }
//  last = Node::nextId;
//}

