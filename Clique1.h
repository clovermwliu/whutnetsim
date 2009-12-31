#pragma once

#ifndef __clique_h__
#define __clique_h__

#include <math.h>

#include "common-defs.h"
#include "linkp2p.h"
#include "PlatTopoBase.h"

class Node;
class Queue;
class Location;

//Doc:ClassXRef
class CClique : public CPlatTopoBase
{
public:
	CClique(Count_t,
			IPAddr_t i= IPADDR_NONE,
			const Linkp2p& = Linkp2p::Default(),
			SystemId_t = 0);

	bool GenerateTopo();
	void SetLocationViaBoundBox(const Location&, const Location&, BoxType type = DEFAULT);
	
	
	Linkp2p* GetLink(Count_t, Count_t); // Get link between specified nodes

	Queue* GetQueue(Count_t, Count_t);  // Get queue between specified nodes
};

#endif
