#pragma once
#ifndef __GNPAIRSTRAFFICEVENT_H__
#define __GNPAIRSTRAFFICEVENT_H__

#include "TrafficEvent.h"

//�����¼��������࣬��ָ���Ľڵ��֮���������
class CGNPairsTrafficEvent : public CTrafficEvent
{
	typedef enum { GNPairsTrafficEvent } GNPairsTrafficEvent_t;
public:
	CGNPairsTrafficEvent(void);
	~CGNPairsTrafficEvent(void);
};
#endif