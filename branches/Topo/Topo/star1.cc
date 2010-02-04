// GENERAL PUBLIC LICENSE AGREEMENT
// 
// PLEASE READ THIS DOCUMENT CAREFULLY BEFORE UTILIZING THE PROGRAM
// 
// BY UTILIZING THIS PROGRAM, YOU AGREE TO BECOME BOUND BY THE TERMS OF
// THIS LICENSE.  IF YOU DO NOT AGREE TO THE TERMS OF THIS LICENSE, DO
// NOT USE THIS PROGRAM OR ANY PORTION THEREOF IN ANY FORM OR MANNER.
// 
// This Program is licensed, not sold to you by GEORGIA TECH RESEARCH
// CORPORATION ("GTRC"), owner of all code and accompanying documentation
// (hereinafter "Program"), for use only under the terms of this License,
// and GTRC reserves any rights not expressly granted to you.
// 
// 1.  This License allows you to:
// 
// (a) make copies and distribute copies of the Program's source code
// provide that any such copy clearly displays any and all appropriate
// copyright notices and disclaimer of warranty as set forth in Article 5
// and 6 of this License.  All notices that refer to this License and to
// the absence of any warranty must be kept intact at all times.  A copy
// of this License must accompany any and all copies of the Program
// distributed to third parties.
// 
// A fee may be charged to cover the cost associated with the physical
// act of transferring a copy to a third party.  At no time shall the
// program be sold for commercial gain either alone or incorporated with
// other program(s) without entering into a separate agreement with GTRC.
//  
// 
// (b) modify the original copy or copies of the Program or any portion
// thereof ("Modification(s)").  Modifications may be copied and
// distributed under the terms and conditions as set forth above,
// provided the following conditions are met:
// 
//     i) any and all modified files must be affixed with prominent
// notices that you have changed the files and the date that the changes
// occurred.
// 		
//     ii) any work that you distribute, publish, or make available, that
// in whole or in part contains portions of the Program or derivative
// work thereof, must be licensed at no charge to all third parties under
// the terms of this License.
// 
//    iii) if the modified program normally reads commands interactively
// when run, you must cause it, when started running for such interactive
// use in the most ordinary way, to display and/or print an announcement
// with all appropriate copyright notices and disclaimer of warranty as
// set forth in Article 5 and 6 of this License to be clearly displayed.
// In addition, you must provide reasonable access to this License to the
// user.
// 
// Any portion of a Modification that can be reasonably considered
// independent of the Program and separate work in and of itself is not
// subject to the terms and conditions set forth in this License as long
// as it is not distributed with the Program or any portion thereof.
// 
// 
// 2. This License further allows you to copy and distribute the Program
//    or a work based on it, as set forth in Article 1 Section b in
//    object code or executable form under the terms of Article 1 above
//    provided that you also either:
// 
//    i) accompany it with complete corresponding machine-readable source
// code, which must be distributed under the terms of Article 1, on a
// medium customarily used for software interchange; or,
// 
//   ii) accompany it with a written offer, valid for no less than three
// (3) years from the time of distribution, to give any third party, for
// no consideration greater than the cost of physical transfer, a
// complete machine-readable copy of the corresponding source code, to be
// distributed under the terms of Article 1 on a medium customarily used
// for software interchange; or,
// 
// 
// 3.  Export Law Assurance.
// 
// You agree that the Software will not be shipped, transferred or
// exported, directly into any country prohibited by the United States
// Export Administration Act and the regulations thereunder nor will be
// used for any purpose prohibited by the Act.
//  
// 4.  Termination.
// 
// If at anytime you are unable to comply with any portion of this
// License you must immediately cease use of the Program and all
// distribution activities involving the Program or any portion thereof.
// 
// 
// 5.  Disclaimer of Warranties and Limitation on Liability.
// 
// YOU ACCEPT THE PROGRAM ON AN "AS IS" BASIS.  GTRC MAKES NO WARRANTY
// THAT ALL ERRORS CAN BE OR HAVE BEEN ELIMINATED FROM PROGRAM.  GTRC
// SHALL NOT BE RESPONSIBLE FOR LOSSES OF ANY KIND RESULTING FROM THE USE
// OF PROGRAM AND ITS ACCOMPANYING DOCUMENT(S), AND CAN IN NO WAY PROVIDE
// COMPENSATION FOR ANY LOSSES SUSTAINED, INCLUDING BUT NOT LIMITED TO
// ANY OBLIGATION, LIABILITY, RIGHT, CLAIM OR REMEDY FOR TORT, OR FOR ANY
// ACTUAL OR ALLEGED INFRINGEMENT OF PATENTS, COPYRIGHTS, TRADE SECRETS,
// OR SIMILAR RIGHTS OF THIRD PARTIES, NOR ANY BUSINESS EXPENSE, MACHINE
// DOWNTIME OR DAMAGES CAUSED TO YOU BY ANY DEFICIENCY, DEFECT OR ERROR
// IN PROGRAM OR MALFUNCTION THEREOF, NOR ANY INCIDENTAL OR CONSEQUENTIAL
// DAMAGES, HOWEVER CAUSED.  GTRC DISCLAIMS ALL WARRANTIES, BOTH EXPRESS
// AND IMPLIED RESPECTING THE USE AND OPERATION OF PROGRAM AND ITS
// ACCOMPANYING DOCUMENTATION, INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR PARTICULAR PURPOSE AND ANY IMPLIED
// WARRANTY ARISING FROM COURSE OF PERFORMANCE, COURSE OF DEALING OR
// USAGE OF TRADE.  GTRC MAKES NO WARRANTY THAT PROGRAM IS ADEQUATELY OR
// COMPLETELY DESCRIBED IN, OR BEHAVES IN ACCORDANCE WITH ANY
// ACCOMPANYING DOCUMENTATION.  THE USER OF PROGRAM IS EXPECTED TO MAKE
// THE FINAL EVALUATION OF PROGRAM'S USEFULNESS IN USER'S OWN
// ENVIRONMENT.
// 
// GTRC represents that, to the best of its knowledge, the software
// furnished hereunder does not infringe any copyright or patent.
// 
// GTRC shall have no obligation for support or maintenance of Program.
// 
// 6.  Copyright Notice.
// 
// THE SOFTWARE AND ACCOMPANYING DOCUMENTATION ARE COPYRIGHTED WITH ALL
// RIGHTS RESERVED BY GTRC.  UNDER UNITED STATES COPYRIGHT LAWS, THE
// SOFTWARE AND ITS ACCOMPANYING DOCUMENTATION MAY NOT BE COPIED EXCEPT
// AS GRANTED HEREIN.
// 
// You acknowledge that GTRC is the sole owner of Program, including all
// copyrights subsisting therein.  Any and all copies or partial copies
// of Program made by you shall bear the copyright notice set forth below
// and affixed to the original version or such other notice as GTRC shall
// designate.  Such notice shall also be affixed to all improvements or
// enhancements of Program made by you or portions thereof in such a
// manner and location as to give reasonable notice of GTRC's copyright
// as set forth in Article 1.
// 
// Said copyright notice shall read as follows:
// 
// Copyright 2004
// Dr. George F. Riley
// Georgia Tech Research Corporation
// Atlanta, Georgia 30332-0415
// All Rights Reserved
//
// $Id: star.cc,v 1.3 2006/01/20 17:37:47 riley Exp $



// Georgia Tech Network Simulator - Star1 topology
// George F. Riley.  Georgia Tech, Fall 2002

// Define a star topology using point-to-point links

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
  if (c > leafCount) return nil;
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
  for (Count_t i = 1; i <=LeafCount(); ++i)
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
  first = Node::nextId;
  if (!hub)
    { // No hub specified, need to allocate
      hub = new Node(id);
	  hub->SetIPAddr(ip++);
      DEBUG0((cout << "ST HubId " << hub->Id() << endl));
    }
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
	  ln->SetIPAddr(ip++);

      /*DEBUG0((cout << "ST NodeId " << ln->Id() << endl));
      if (ip != IPADDR_NONE)
        {
          
			ln->AddDuplexLink(hub, link, ip, Mask(31), ip + adder, Mask(31));
          DEBUG0((cout << "Star1 leaf " << i << " node " << ln->Id()
                  << " ip " << (string)IPAddr(ln->GetIPAddr()) << endl));
        }
      else
        {*/
          ln->AddDuplexLink(hub, link);
        //}
      // Add a default route
      ln->DefaultRoute(hub);
      //if (ip != IPADDR_NONE) ip += 2; // Advance to next set of ip's
    }
  last = Node::nextId;
}

