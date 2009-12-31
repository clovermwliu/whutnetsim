 
#include "WS.h"
#include <math.h>
#include "qtwindow.h"
#include "linkp2p.h"
#include <algorithm>

#define PI 3.1415926
#define s  100 //定义两个点之间的距离

CWS::~CWS(void)
{
}
CWS::CWS(Count_t _nodeCount,
		 Count_t _lineForOneNode,
		 double _p,
		 IPAddr_t i,
		 const Linkp2p& link,
		 SystemId_t id)
:CPlatTopoBase(_nodeCount,i,link,id)
{
	lineForOneNode=_lineForOneNode;
	p=_p;
}

bool CWS::GenerateTopo()
{
	//double r=Computer_r();
	//addNodes(r);
	////qtWin->DisplayTopologyAndReturn();
	//addLines();
	ConstructorHelper(link, ip);
	return true;
}
void CWS::ConstructorHelper( const Linkp2p& link,
							  IPAddr_t leafIP)
{
	AddNodes();
		
	AddLines(link, leafIP);
}
void CWS::SetLocationViaBoundBox(const Location& ll, const Location& ur, BoxType  type)
{
	Meters_t xRadius = fabs(ur.X() - ll.X())/2;
	Meters_t yRadius = fabs(ur.Y() - ll.Y())/2;

	Meters_t xCenter = (ur.X() + ll.X())/2;
	Meters_t yCenter = (ur.Y() + ll.Y())/2;

	double angle = (2 * M_PI)/ nodeCount;

	NodeId_t thisNode = first;
	const NodeVec_t& nodes = Node::GetNodes();

	DEBUG0((cout<<"Putting Ring nodes in place"<<endl));

	// Assign locations for each level
	for (Count_t l = 0; l < nodeCount; ++l)
	{
		Meters_t yLoc = yCenter + yRadius * sin(angle * l);
		Meters_t xLoc = xCenter + xRadius * cos(angle * l);

		nodes[thisNode++]->SetLocation(xLoc,yLoc);

		DEBUG0((cout<<"Putting node "<<l<<" in ("<<xLoc<<","<<yLoc<<")"<<endl));
	}
}
void CWS::AddNodes()
{
	first = Node::nextId;
	Node *n = new Node ();
	//nodes.push_back(n);
	Node *firstnode = n;
	//nodeCount = count;

	for (Count_t l = 1; l <= nodeCount; ++l)
	{ // Create each subsequent level

		Node *newnode;

		if (l!=nodeCount)
		{
			newnode = new Node();
			//nodes.push_back(newnode);
		}
		else
			newnode = firstnode;
		n = newnode;
	}
	last = Node::nextId;
}
void CWS::AddLines(const Linkp2p& link,
				   IPAddr_t leafIP)//需要传递参数,连接的方式
{
	int    remote;
    Node*  node0;
	Node*  node1;
	IPAddr_t  nextIP = leafIP;
	for (int i=0;i<nodeCount;i++)
	{
		node0=Node::nodes[i];
		for (int addline=1;addline<=lineForOneNode/2;addline++)
		{
			remote=(i+addline+nodeCount)%nodeCount;
			
			int cp=random(100);
			//cout<<cp<<endl;
			if (cp>p*100)
			{
				node1=Node::nodes[remote];
			}
			else
			{
				bool flag=true;

				NodeWeightVec_t nwv;
				node0->Neighbors(nwv);

				NodeWeightVec_t::iterator iter;
				int nonum;
				while(flag)
				{
					nonum=random(nodeCount);
					
					for (iter=nwv.begin();iter!=nwv.end();iter++)
					{
						if ((*iter).node->Id()==(Count_t)nonum)
						break;
					}
					if (iter==nwv.end())
					{
						flag=false;
					}
				}
				node1=Node::nodes[nonum];
			}
			if (nextIP == IPADDR_NONE)
			{ // No IP specified
				node0->AddDuplexLink(node1, link);
			}
			else
			{
				node0->AddDuplexLink(node1, link,
					nextIP++, Mask(32), 
					IPADDR_NONE, Mask(32));
			}
		}
	}
}
