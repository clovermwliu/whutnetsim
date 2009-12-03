#include "node-impl.h"

NodeImpl::NodeImpl(Node* n)
: pNode(n), ipAddr(IPADDR_NONE), neighborCount(0), down(false)
{
}
NodeImpl::~NodeImpl()
{
}