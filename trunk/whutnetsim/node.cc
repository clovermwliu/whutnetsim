
#include "node.h"
#include "node-impl.h"
#include "node-real.h"



NodeId_t  Node::nextId = 0;  // Next id to assign
NodeVec_t Node::nodes;       // Vector of all known nodes
Count_t   Node::defaultPixelSize = 10;
Node::Shape_t Node::defaultShape = Node::SQUARE;
double    Node::defaultMaxSpeed = 20;  // Default max speed of a node


Node::Node(SystemId_t sysId):id(nextId++),
proxyIP(IPADDR_NONE)/*, proxyMask(0)*/
{
	//if (sysId != DistributedSimulator::systemId)
	//{ // Mapped to a different logical process.  Use a ghost.
	//	pImpl = new NodeGhost(this);
	//}
	//else
	//{ // Real node
		pImpl = new NodeReal(this);
	/*}*/
	nodes.push_back(this);                  // Add to vector of known nodes
}
void Node::SetLocation(const Location& l)
{
	pImpl->SetLocation(l);
}

Node::~Node()
{
	delete pImpl;                           // Delete the corresponding impl
}
void Node::Color(const QColor& c)
{
	pImpl->Color(c);
}
Location Node::UpdateLocation()
{
	return pImpl->UpdateLocation();
}
#ifdef HAVE_QT

void Node::DefaultPixelSize(Count_t dps)
{
	defaultPixelSize = dps;
}

Count_t Node::DefaultPixelSize()
{
	return defaultPixelSize;
}

void Node::DefaultShape(Shape_t ds)
{
	defaultShape = ds;
}

Node::Shape_t Node::DefaultShape()
{
	return defaultShape;
}

#endif
QCanvasItem* Node::Display(const QPoint& qp, QTWindow* qc)
{
	return pImpl->Display(qp, qc);
}

