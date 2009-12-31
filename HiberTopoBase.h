#ifndef __HIBERTOPOBASE_H__   
#define __HIBERTOPOBASE_H__ 

#pragma once
#include "node.h"
#include "linkp2p.h"
#include "PlatTopoBase.h"
#include "common-defs.h"

class CHiberTopoBase
{
public:
	CHiberTopoBase(IPAddr_t i= IPADDR_NONE,
				  const Linkp2p& = Linkp2p::Default(),
				  SystemId_t = 0);
	typedef enum  {DEFAULT, CIRCLE, GRID, CIRCLES} BoxType;
public:
	~CHiberTopoBase(void);
	 Interface* AddDuplexLink(Node* ln,Node* rn, const Linkp2p& l = Linkp2p::Default());

	 virtual void SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
										const Location& BoundBoxRightUpper,
									    BoxType  type = DEFAULT) {};

	 virtual bool GenerateTopo() = 0;

protected:
	 IPAddr_t ip;
	 const Linkp2p&  link;
	 SystemId_t sid;
};
#endif