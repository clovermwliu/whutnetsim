#include "BA.h"
#include <qnamespace.h>
CBA::CBA(Count_t count,                                        //生成的拓扑的节点的个数
		 int onceAddEdge_m,                                    //每次添加的点与多少个点相连
		 IPAddr_t i,                                           //给所有节点赋的基IP
		 SystemId_t id,const Linkp2p& link)                    //节点号,点之间的连接方式
:CPlatTopoBase(count,i,link,id),onceAddEdge(onceAddEdge_m)
{
     allNodeDegrees = 0;
}

bool CBA::GenerateTopo()
{
	for (Count_t i = 0; i < nodeCount; i++)
	{
		if (i <=onceAddEdge)                                   //如果网络中的节点数小于等于需要添加的边的个数,
		{                                                      //则新添加的节点与已有的所有的节点相连
			Node* addNode = new Node(sid);                     //新建一个节点
			degrees.push_back(1); 
			allNodeDegrees++;
			//                              
			//for (Count_t connect=0;connect<i;connect++)
			//{
			//	addNode->AddDuplexLink(Node::nodes[connect]);  //新添加的节点与已有的所有的节点相连
			//	degrees[connect]++; 
			//}
			//allNodeDegrees +=i*2;                               //allNodeDegrees总度数增加了新增边的两倍
		}
		else 
		{
			AddOneNode(i);                                      //如果网络中的节点数大于需要添加的边的个数,调用AddOneNode
			allNodeDegrees +=onceAddEdge+1;                     //allNodeDegrees总度数增加了新增边的两倍,新增了onceAddEdge条边
		}
	}
	return true;
}

bool CBA::AddOneNode(Count_t nodeNum)
{
	Node* addNode = new Node(sid);                              //新建一个节点           

	vector<Count_t> addEdge;                                    //用来记录为新增节点选择的连接节点
	vector<Count_t>::iterator iter;
	for (Count_t i = 0;i < onceAddEdge;)
	{
		int ran =  random(allNodeDegrees);                      //产生一个随机数,最大为度总数-1
		int findthenode = 0;                                    //findthenode用来寻找满足条件的节点
		for (vector<Count_t>::size_type  findnum = 0;
			findnum != degrees.size(); findnum++)
		{
			findthenode += degrees[findnum];                    //从第一个节点开始,对度进行累加,直到大于产生的随机数ran
			if (findthenode >=ran)
			{
				iter = find(addEdge.begin(), addEdge.end(), findnum);//是否已经和这个节点相连
				
				if (iter == addEdge.end())                        //没有和这个节点相连,则
				{
					addEdge.push_back(findnum);                   //在addEdge中记录,表示新增节点已经和这个节点相连
					Node *old=Node::nodes[findnum];
					degrees[findnum]++;
					allNodeDegrees++;
					addNode->AddDuplexLink(old,Linkp2p::Default());
					i++;
					break;
				}
			}
		}	
	}
	degrees.push_back(onceAddEdge);
	return true;
}

void CBA::SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
							     const Location& BoundBoxRightUpper,
							     BoxType  type)
{
	const NodeVec_t& nodes = Node::GetNodes();

	//随机生成位置
	/*for (NodeVec_t::size_type nodeNum=0;nodeNum!=nodes.size();nodeNum++)
	{
		int x =  random(10000);
		int y =  random(10000);
		nodes[nodeNum]->SetLocation(x,y);
	}*/

	//按照度的大小生成
	Meters_t x0 = (BoundBoxRightUpper.X()+BoundBoxLeftDown.X())/2;
	Meters_t y0 = (BoundBoxRightUpper.Y()+BoundBoxLeftDown.Y())/2;

    //Meters_t x0,y0;
	for (NodeVec_t::size_type nodeNum=0;nodeNum!=nodes.size();nodeNum++)
	{
		Node* node=nodes[nodeNum];
		if (node->HasLocation())
		{
			x0 =  random(10000);
			y0 =  random(10000);
		}
		else
		{
			//x0 =  random(10000);
			//y0 =  random(10000);
			node->SetLocation(x0,y0);
			//node->Color(Qt::red);
		
			NodeWeightVec_t nwv;
			node->Neighbors(nwv);
			Count_t  neighNum = nwv.size();
			for (Count_t  numsize = 0;numsize<nwv.size();numsize++)
			{
				NodeIfWeight& nodeNeigh = nwv[numsize];
				Count_t  NodeId=nodeNeigh.node->Id();
				Meters_t   lx = x0 +50*degrees[NodeId]*cos((M_PI/180)*(360*(numsize+1)/neighNum));
				Meters_t   ly = y0 +50*degrees[NodeId]*sin((M_PI/180)*(360*(numsize+1)/neighNum));
				nodeNeigh.node->SetLocation(Location(lx,ly));
			}

		}
	}
}
bool CBA::AddEdges(Count_t nodeNum)
{
	return true;
}
CBA::~CBA(void)
{

}
