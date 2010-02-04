
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
/*
描述：Dumbbell1网络拓扑的生成         
参数：[IN] l     ：
      [IN] r     ：
	  [IN] m     ：
	  [IN] lip   ： 左边的拓扑节点的基IP
	  [IN]  rip  ： 右边的拓扑节点的基IP
	  [IN] link  ：
	  [IN]  id   ：分布式系统标识符
      [IN]   i   ：
      [IN] link  ：拓扑的节点间的连接方式                                                 
返回：无                                                                                       
备注：
*/
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

