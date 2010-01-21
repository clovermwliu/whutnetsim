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


//更改人：李玉
//更改时间：2010-1-4


#include <iostream>

#include "G_debug.h"
#include "star1.h"
#include "node.h"
#include "linkp2p.h"
#include "mask.h"

using namespace std;

// Constructors

Star1::Star1(Count_t l, IPAddr_t i, SystemId_t id)
:CPlatTopoBase(l,i,Linkp2p::Default(),id),n(nil)
{
	//ConstructorHelper(l, nil, i, Linkp2p::Default(), id);
}

Star1::Star1(Count_t l, const Linkp2p& link, IPAddr_t i, SystemId_t id)
:CPlatTopoBase(l,i,link,id),n(nil)
{
  //ConstructorHelper(l, nil, i, link, id);
}

Star1::Star1(Count_t l, Node* _n, IPAddr_t i, SystemId_t id)
:CPlatTopoBase(l,i,Linkp2p::Default(),id),n(_n)
{
  //ConstructorHelper(l, n, i, Linkp2p::Default(), id);
}

Star1::Star1(Count_t l, Node* _n, const Linkp2p& link, IPAddr_t i, SystemId_t id)
:CPlatTopoBase(l,i,link,id),n(_n)
{
//  ConstructorHelper(l, n, i, link, id);
}
 bool Star1::GenerateTopo()
 {
	ConstructorHelper(nodeCount, n, ip, link, sid);
	return true;
 }
// Access functions
Node* Star1::GetHub()
{ // Get left side router
  return hub;
}

Node* Star1::GetLeaf(Count_t c)
{ // Get specified leaf node
  if (c >= leafCount) return nil;
  return Node::GetNode(first + c);
}

Linkp2p* Star1::HubLink(Count_t c)
{ // Get link from hub to specified leaf
  Node* n = GetLeaf(c);
  if (!n) return nil;
  return (Linkp2p*)hub->GetLink(n);
}

Linkp2p* Star1::LeafLink(Count_t c)
{ // Get link from leaf to hub
  Node* n = GetLeaf(c);
  if (!n) return nil;
  return (Linkp2p*)n->GetLink(hub);
}

Queue*   Star1::HubQueue(Count_t c)
{ // Get queue from hub to leaf
  Node* n = GetLeaf(c);
  if (!n) return nil;
  return hub->GetQueue(n);
}

Queue*   Star1::LeafQueue(Count_t c)
{ // Get queue from hub to leaf
  Node* n = GetLeaf(c);
  if (!n) return nil;
  return n->GetQueue(hub);
}

void Star1::StarSetLocationViaBoundBox(const Location& ll, const Location& ur,
									  Angle_t startAngle, Angle_t arcSize)
{
  // Make copies of the passed in locations, as we may have 
  // to change these.
  Location lll(ll);
  Location lur(ur);
  
  Meters_t xDist = lur.X() - lll.X();
  Meters_t yDist = lur.Y() - lll.Y();
  Meters_t arcDist = xDist / 2;
  Node*    hub = GetHub();

  if (hub->HasLocation())
    { // The hub already has a location.  Use this as the bounding box
      // as a relative bb, with the hub at the center.
      Location hl = hub->GetLocation();
      lll = Location(hl.X() - xDist / 2, hl.Y() - yDist / 2);
      lur = Location(hl.X() + xDist / 2, hl.Y() + yDist / 2);
    }
    
  // Notify simulator of bounds, for max x/y computations
  Simulator::instance->NewLocation(lll.X(), lll.Y());
  Simulator::instance->NewLocation(lur.X(), lur.Y());

  Location hubLoc(lll.X() + xDist / 2, lll.Y() + yDist / 2);
  GetHub()->SetLocation(hubLoc);
  // Now position the leaf nodes
  //Angle_t theta = startAngle - arcSize / 2;
  Angle_t theta = startAngle;
  Angle_t adder = arcSize / LeafCount();
  for (Count_t i = 0; i < LeafCount(); ++i)
    {
      Location leafLoc(hubLoc.X() + cos(theta) * arcDist,
                       hubLoc.Y() + sin(theta) * arcDist);
      GetLeaf(i)->SetLocation(leafLoc);
      theta += adder;
    }
}

// Private methods
void Star1::ConstructorHelper(Count_t l, Node* h,
                             IPAddr_t ip,
                             const Linkp2p& link,
                             SystemId_t id)
{
  leafCount = l;
  hub = h;
  int adder = 1;
  
  if (!hub)
    { // No hub specified, need to allocate
      hub = new Node(id);
      DEBUG0((cout << "ST HubId " << hub->Id() << endl));
    }
  first = Node::nextId;
  if (ip != IPADDR_NONE)
    {
      if (ip & 0x1)
        { // Ends in 1, use ip and ip - 1 for endpoints
          adder = -1;
        }
    }
  for (Count_t i = 0; i < leafCount; ++i)
    {
      Node* ln = new Node(id);
      DEBUG0((cout << "ST NodeId " << ln->Id() << endl));
      if (ip != IPADDR_NONE)
        {
          ln->AddDuplexLink(hub, link, ip, Mask(31), ip + adder, Mask(31));
          DEBUG0((cout << "Star1 leaf " << i << " node " << ln->Id()
                  << " ip " << (string)IPAddr(ln->GetIPAddr()) << endl));
        }
      else
        {
          ln->AddDuplexLink(hub, link);
        }
      // Add a default route
      ln->DefaultRoute(hub);
      if (ip != IPADDR_NONE) ip += 2; // Advance to next set of ip's
    }
  last = Node::nextId;
}

