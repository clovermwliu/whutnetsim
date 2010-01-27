
#include "Simple.h"
CSimple ::CSimple(IPAddr_t i,                                           //给所有节点赋的基IP
				 const Linkp2p& link,
				  SystemId_t id)
:CPlatTopoBase(2,i,link,id)
{
}

bool CSimple::GenerateTopo()
{
	first = Node::nextId;
	Node* left = new Node(); 
	left->SetIPAddr(ip++);

	Node* right = new Node(); 
	right->SetIPAddr(ip++);

	left->AddDuplexLink(right,link);
	return true;
}
void CSimple::SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
									 const Location& BoundBoxRightUpper,
									 BoxType  type)
{
	const NodeVec_t& nodes = Node::GetNodes();

	Meters_t x0 = (BoundBoxRightUpper.X()+BoundBoxLeftDown.X())/2;   //第一个大圆的中心位置
	Meters_t y0 = (BoundBoxRightUpper.Y()+BoundBoxLeftDown.Y())/2;
	nodes[first]->SetLocation(Location(BoundBoxLeftDown.X(),y0));
	nodes[first+1]->SetLocation(Location(BoundBoxRightUpper.X(),y0));
}
CSimple::~CSimple(void)
{
}