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

#include "Ring1.h"

#include "G_debug.h"
#include "node.h"
#include "linkp2p.h"
#include "mask.h"
#include <math.h>
#include <stdio.h>


using namespace std;

// Constructors

CRing::CRing(Count_t count,
			 IPAddr_t i,
			 const Linkp2p& link,
			 SystemId_t id)
:CPlatTopoBase(count,i,link,id)
{
}

bool CRing::GenerateTopo()
{
	ConstructorHelper(link, ip);
	return true;
}

// Private methods
void CRing::ConstructorHelper( const Linkp2p& link,
							  IPAddr_t leafIP)
{
	first = Node::nextId;
	Node *n = new Node ();
	//nodes.push_back(n);
	Node *firstnode = n;
	//nodeCount = count;

	IPAddr_t  nextIP = leafIP;

	for (Count_t l = 1; l <= nodeCount; ++l)
	{ // Create each subsequent level

		Node *newnode;

		if (l!=nodeCount)
		{
			newnode = new Node();
			//nodes.push_back(newnode);
		}
		else
			newnode = firstnode;

		if (nextIP == IPADDR_NONE)
		{ // No IP specified
			n->AddDuplexLink(newnode, link);
		}
		else
		{
			n->AddDuplexLink(newnode, link,
				nextIP++, Mask(32), 
				IPADDR_NONE, Mask(32));
		}
		n = newnode;
	}
	last = Node::nextId;
}
void CRing::SetLocationViaBoundBox(const Location& ll, const Location& ur, BoxType  type)
{
	Meters_t xRadius = fabs(ur.X() - ll.X())/2;
	Meters_t yRadius = fabs(ur.Y() - ll.Y())/2;

	Meters_t xCenter = (ur.X() + ll.X())/2;
	Meters_t yCenter = (ur.Y() + ll.Y())/2;

	double angle = (2 * M_PI)/ nodeCount;

	NodeId_t thisNode = first;
	const NodeVec_t& nodes = Node::GetNodes();

	DEBUG0((cout<<"Putting Ring nodes in place"<<endl));

	// Assign locations for each level
	for (Count_t l = 0; l < nodeCount; ++l)
	{
		Meters_t yLoc = yCenter + yRadius * sin(angle * l);
		Meters_t xLoc = xCenter + xRadius * cos(angle * l);

		nodes[thisNode++]->SetLocation(xLoc,yLoc);

		DEBUG0((cout<<"Putting node "<<l<<" in ("<<xLoc<<","<<yLoc<<")"<<endl));
	}
}



