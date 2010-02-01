#pragma once
#ifndef __CRing_h__ 
#define __CRing_h__ 

#include "G_common_defs.h"
#include "node.h"
#include "PlatTopoBase.h"

class Node;
class Linkp2p;
class Queue;

class CRing : public CPlatTopoBase
{
	//Doc:Class Defines a tree topology using point--to--point links.
public:
	// Size, IP, Mask
	//Doc:Method
	CRing(Count_t,
		  IPAddr_t = IPADDR_NONE,
		  const Linkp2p& = Linkp2p::Default(),
		  SystemId_t = 0);
	bool GenerateTopo();
	//
	//Node* GetNode(Count_t);    // Get node at specified level, index
	
	void SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
								const Location& BoundBoxRightUpper,
								BoxType  type = DEFAULT);

public:
	//NodeId_t   first;      // There is a global node list, we don't need one 
	//NodeId_t   last;       // Just log first/last+1 entries
	//Count_t    nodeCount;  // Number of nodes
	IPAddr_t   firstNodeIP;// Starting IP Address for nodes
	NodeId_t   firstNodeId;// Starting NodeId for nodes
private:
	void ConstructorHelper(const Linkp2p&,IPAddr_t);
};
#endif