#pragma once
#include "eventbase.h"

class CEndEvent :
	public CEventBase
{
public:
	typedef enum { ENDEVENT  = 300} EndEvent_t;
	CEndEvent(void);
	~CEndEvent(void);
};
