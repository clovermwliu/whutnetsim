#pragma once

#include "PlatTopoBase.h"
#include "G_common_defs.h"
#include "linkp2p.h"
#include "node.h"
#include <map>

class CSimple : public CPlatTopoBase
{
public:
	CSimple(IPAddr_t i = IPADDR_NONE,                                           //�����нڵ㸳�Ļ�IP
		    const Linkp2p& link = Linkp2p::Default(),
		    SystemId_t id = 0);
	~CSimple(void);
public:
	bool GenerateTopo();
	void SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
								const Location& BoundBoxRightUpper,
								BoxType  type = DEFAULT);
private:
	
};
