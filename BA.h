#ifndef __BA_H__ 
#define __BA_H__
#pragma once
#include "PlatTopoBase.h"
#include "common-defs.h"
#include "linkp2p.h"

class CBA : public CPlatTopoBase
{
public:
     CBA( Count_t count,  
		  int onceAddEdge_m = 5,
		  IPAddr_t i = IPADDR_NONE,     
		  SystemId_t id = 0,const Linkp2p& link = Linkp2p::Default());
	~CBA(void);
	bool GenerateTopo();

	void SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
								const Location& BoundBoxRightUpper,
								BoxType  type = DEFAULT);
private:
	bool AddOneNode(Count_t nodeNum);
	bool AddEdges(Count_t nodeNum);
private:
	int  onceAddEdge;
	vector<int>  degrees;

	Count_t  allNodeDegrees;
};
#endif
