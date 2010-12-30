#include "StdAfx.h"
#include "Edge.h"
#include "GlobalVariable.h"

CEdge::~CEdge(void)
{
}
CEdge::CEdge(NodeId_t sid, NodeId_t did ) : _sou_id(sid), _des_id(did), _cost(1),_delay(1)
{ 
	DEBUG3(GlobalVariable::out_new<<"New Edge"<<std::endl);
}
