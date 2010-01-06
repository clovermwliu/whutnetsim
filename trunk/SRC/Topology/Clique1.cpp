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

#include "Clique1.h"

#include <iostream>

#include "G_debug.h"
#include "node.h"
#include "linkp2p.h"
#include "mask.h"

using namespace std;

// Constructors

CClique::CClique(Count_t n,
				 IPAddr_t i,
				 const Linkp2p& _link, 
				 SystemId_t _id)
:CPlatTopoBase(n,i,_link,_id)
{
}
bool CClique::GenerateTopo()
{
	//nodeCount = n;
	first = Node::nextId;

	// Create the nodes and links
	for (Count_t k = 0; k < nodeCount; ++k)
	{
		Node* n1 = new Node(sid);
		//nodes.push_back(n1);
		n1->SetIPAddr(ip++);
		for (Count_t j = 0; j < k; ++j)
		{
			Node* n2 = GetNode(j);
			n1->AddDuplexLink(n2, link);
		}
	}
	last = Node::nextId;
	return true;
}
// Access functions
//Node* CClique::GetNode(Count_t c)
//{ // Get specified  node
//	if (c >= nodeCount) return nil;
//	return Node::GetNode(first + c);
//}

Linkp2p* CClique::GetLink(Count_t c1, Count_t c2)
{ // Get link between specified nodes
	Node* n1 = GetNode(c1);
	if (!n1) return nil;
	Node* n2 = GetNode(c2);
	if (!n2) return nil;
	return (Linkp2p*)n1->GetLink(n2);
}

Queue*   CClique::GetQueue(Count_t c1, Count_t c2)
{ // Get queue between specified nodes
	Node* n1 = GetNode(c1);
	if (!n1) return nil;
	Node* n2 = GetNode(c2);
	if (!n2) return nil;
	return n1->GetQueue(n2);
}

void CClique::SetLocationViaBoundBox(const Location& ll, const Location& ur, BoxType type)
{
	// Make copies of the passed in locations, as we may have 
	// to change these.
	Location lll(ll);
	Location lur(ur);

	Meters_t xDist = lur.X() - lll.X();
	Meters_t yDist = lur.Y() - lll.Y();
	Meters_t arcDist = xDist / 2;

	// Notify simulator of bounds, for max x/y computations
	Simulator::instance->NewLocation(lll.X(), lll.Y());
	Simulator::instance->NewLocation(lur.X(), lur.Y());

	Location centerLoc(lll.X() + xDist / 2, lll.Y() + yDist / 2);

	// Now position the leaf nodes
	Angle_t theta = 0;
	Angle_t adder = (2 * M_PI) / NodeCount();
	for (Count_t i = 0; i < NodeCount(); ++i)
	{
		Location nodeLoc(centerLoc.X() + cos(theta) * arcDist,
			centerLoc.Y() + sin(theta) * arcDist);
		GetNode(i)->SetLocation(nodeLoc);
		theta += adder;
	}
}

