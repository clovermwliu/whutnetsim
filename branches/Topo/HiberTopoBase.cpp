#include "HiberTopoBase.h"
#include "duplexlink.h "
CHiberTopoBase::CHiberTopoBase(IPAddr_t i,
							 const Linkp2p& link1 ,
							 SystemId_t id1)
:ip(i),link(link1),sid(id1)
{

}
CHiberTopoBase::~CHiberTopoBase(void)
{
}

Interface* CHiberTopoBase::AddDuplexLink(Node* ln,Node* rn, const Linkp2p& l)
{
	DuplexLink dl(ln, l, rn, l);
	return dl.localif;
}
