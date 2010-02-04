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
/*
描述：通过绑定位置来给节点设置坐标
参数：[in]ll                 ——左下角的位置
      [in]ur                 ——右上角的位置
      [in]type               ——设置位置的类型
返回值：无
*/
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

