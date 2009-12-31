#include "Ring1.h"

#include "debug.h"
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



