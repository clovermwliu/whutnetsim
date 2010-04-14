//Copyright (c) 2010, Information Security Institute of Wuhan Universtiy(ISIWhu)
//All rights reserved.
//
//PLEASE READ THIS DOCUMENT CAREFULLY BEFORE UTILIZING THE PROGRAM
//BY UTILIZING THIS PROGRAM, YOU AGREE TO BECOME BOUND BY THE TERMS OF
//THIS LICENSE.  IF YOU DO NOT AGREE TO THE TERMS OF THIS LICENSE, DO
//NOT USE THIS PROGRAM OR ANY PORTION THEREOF IN ANY FORM OR MANNER.
//
//This License allows you to:
//1. Make copies and distribute copies of the Program's source code provide that any such copy 
//   clearly displays any and all appropriate copyright notices and disclaimer of warranty as set 
//   forth in this License.
//2. Modify the original copy or copies of the Program or any portion thereof ("Modification(s)"). 
//   Modifications may be copied and distributed under the terms and conditions as set forth above. 
//   Any and all modified files must be affixed with prominent notices that you have changed the 
//   files and the date that the changes occurred.

//Termination:
//   If at anytime you are unable to comply with any portion of this License you must immediately 
//   cease use of the Program and all distribution activities involving the Program or any portion 
//   thereof.

//Statement:
//   In this program, part of the code is from the GTNetS project, The Georgia Tech Network 
//   Simulator (GTNetS) is a full-featured network simulation environment that allows researchers in 
//   computer networks to study the behavior of moderate to large scale networks, under a variety of 
//   conditions. Our work have great advance due to this project, Thanks to Dr. George F. Riley from 
//   Georgia Tech Research Corporation. Anyone who wants to study the GTNetS can come to its homepage:
//   http://www.ece.gatech.edu/research/labs/MANIACS/GTNetS/
//


//File Information:
//
//
//File Name:
//File Purpose:
//Original Author:
//Author Organization:
//Construct Data:
//Modify Author:
//Author Organization:
//Modify Data:




// Implementation of Breadth First Search algorithm
// George Riley, Georgia Tech, Winter 2000

// Converted to work with GTNetS, Summer 2002

//2010.01.04 modified by ��Цӯ

//#define DEBUG_MASK 0x08

#include "bfs.h" 
//int bfs_calls=0;
//int bfs_nodessearched=0;

//#define PROXY_PRUNE

#ifndef TEST_BFS

//double shortestpath(int src, void *p);//��Դ���·������src���������������·�����ȣ�����ֵ���������ֵ

#define  MAXDIST 1000000
NodeId_t BFSForDefaultRoute(map<NodeId_t, int>& N,       // List of nodes in the graph
							NodeId_t rootID,    // Root node for route computations
							NodeId_t targetID)// List of IP's with same path
/*
��������������㹹�ɵ������У�����Դ���root��idΪtargetID��Ŀ��������·��������root��targetID�����·������һ�����id 
����ֵ��-1 rootID��ΪtargetID
        -2 rootID��targetID��������N��
		-3 N�е�ĳ����ĳһ�ھӽ����ȫ��nodes���Ҳ�����һ�㲻��
		-4 ��rootID�޷�����targetID����N�н�㲻��ͨ		
*/
{	
	if (rootID == targetID)
	{
		return -1;
	}
	//���rootID��targetID�Ƿ������N��
	if (!N.count(rootID) || !(N.count(targetID)))
	{
		return -2;
	}
	//���·����ʼֵΪMAXDIST
	map<NodeId_t, NodeId_t> nextHop;//��Դ���ﵽ�����ʱ����һ��·�ɽ��
	map<NodeId_t, int>::iterator N_it = N.begin();//id dist
	while(N_it != N.end())
	{
		nextHop.insert(make_pair(N_it->first, -4));//nextHop��ʼֵ
		N_it->second = MAXDIST;
		++N_it;
	}
	N.find(rootID)->second = 0;
	//�ֲ�����
	double weight;
	double tmp_weight;
	double default_weight = 1;//ȨֵĬ�϶�Ϊ1
	//NodeId_t neighbor_id;
	NodeId_t sou_id = rootID;
	multimap<double, int> active;//Ȩֵ��Դ�ڵ�id,�����·��ʱ��
	multimap<double, int>::iterator active_it;		
	active.insert(make_pair(0, sou_id));

	while(!active.empty())
	{
		active_it = active.begin();
		sou_id = active_it->second;//Դ���
		weight = active_it->first;//Ȩֵ
		active_it = active.erase(active_it);
		//��sou_id�ĸ��ھӽ��
		NodeWeightVec_t nwv;
		Node * u = Node::GetNode(sou_id);
		if (!u)//sou_id������
		{
			return -3;
		}
		u->Neighbors(nwv, true); // Get neighbors, ignoring leaf nodes
		for (NodeWeightVec_t::size_type i = 0; i < nwv.size(); ++i)//ѭ��ȡ���u���ھӽ��
		{
			NodeIfWeight& nw = nwv[i];
			Node*         neighbor = nw.node;//�ھ�
			NodeId_t      neighbor_id = neighbor->Id();
			if (neighbor_id == sou_id) continue;              // Insure not self adjacancy
			//����ھӽ�㲻��N��
			if (!N.count(neighbor_id) ) continue;
			tmp_weight = weight + default_weight; //Ȩֵ
			double tmp = N.find(neighbor_id)->second;
			if (tmp_weight < tmp)
			{
				N.find(neighbor_id)->second = tmp_weight;
				active.insert(make_pair(tmp_weight, neighbor_id));
				if (sou_id == rootID)
				{
					nextHop.find(neighbor_id)->second = neighbor_id;
				}
				else
				{
					nextHop.find(neighbor_id)->second = nextHop.find(sou_id)->second;
				}
			}
		}
	}
	return nextHop.find(targetID)->second;	
}

NodeId_t BFS(
			 const NodeVec_t& N,
			 Node*            root,
			 NodeIfVec_t&     NextHop,
			 NodeVec_t&       Parent,
			 //IPAddr_t         targetIP,
			 IPAddr_t        targetIP,
			 IPAddrVec_t&     aliases)
/*
���������ÿ�����ȱ����㷨����Ӹ���㣨Դ��㣩���������������·��
������[IN]     N:  ���н��ָ�뼯��
	  [IN]     root:  Դ���ָ��
	  [IN][OUT]NextHop: ��Դ���ﵽ�����ʱ����һ��·�ɽ��
	  [IN][OUT]Parent: ��Դ��㵽������ʱ��Ŀ�����ǰһ�����
	  [IN]     targetIP: Ŀ��IP��ַ
	  [IN][OUT]aliases: List of IP's with same path,һ����ã�������Ϊ��
����ֵ��Ŀ��IP��ַ��Ӧ�Ľ��id�����Ŀ��IP�����ڣ���һ�㷵��Դ���id
*/
{  
	// Compute shortest path to all nodes from root via Breadth First Search
	BitMap B(N.size());           //����ʱ�ж�һ����Ƿ��Ѿ��������
	NodeDeque_t Q;                //��������
	DistVec_t   D(N.size(), INF); //Դ��㵽���������ľ��룬��СΪ����������ʼ��Ϊ�����
	NodeId_t    r = NODE_NONE;    // ����ֵ������Ŀ��IP��Ӧ�ĵĽ��id�����Ŀ��IP�����ڣ��򷵻�Դ���id

	bool        pruned;           // Boolean that is used for proxy pruning
	IPAddr_t    rootIP;

#ifdef DEBUG_QSIZE
	Count_t maxQSize = 0; // debug
#endif

	//bfs_calls++;//һ�������У�bfs�������ô�����δʹ��

	NextHop.clear();
	Parent.clear();
	//NextHop��Parent�Ĵ�СΪ������
	NextHop.reserve(N.size());
	NextHop.insert(NextHop.begin(), N.size(), NodeIf(nil,nil));
	Parent.reserve(N.size());
	Parent.insert(Parent.begin(), N.size(), nil);

	// Set up information for root node for proxy routing
	int longestPrefix, uPrefix;

	//�ж�root��proxyIP & root��proxyMask �� targetip & root��proxyMask�Ƿ����
	//root��proxyMaskΪ0ʱ����ȣ�����
	//rootΪ·�ɽ�㣬������·�ɵ�targetʱ�����
	if (root->CanProxyRouteIP(targetIP)) 
	{
		longestPrefix = root->GetLongestPrefixLength(targetIP);//
		r = root->Id();
	}
	else
		longestPrefix = 0;

	// Start the algorithm
	B.Set(root->Id()); //��ʾ��������ý��
	DEBUG(1,(B.DBPrint()));
	Q.push_back(NodeIf(root,nil)); // And put the root in Q
	D[root->Id()] = 0;//����㵽�Լ��ľ���Ϊ0
	rootIP = root->GetIPAddr();
	if (rootIP==IPADDR_NONE && root->HasProxyRoutingConfig())
		rootIP = root->GetProxyIP();

	DEBUG0((cout << "Starting BFS Loop" << endl));
	while(Q.size() != 0)//����ѭ������
	{
		NodeIf& nodeIf = Q.front();
		Node* u = nodeIf.node;//��ǰ������
		DEBUG0((cout << "Working on node " << u->Id() << endl));

		NodeWeightVec_t nwv;//��ǰ��������ھӽ�㼰Ȩֵ
		DEBUG0((cout << "Getting neighbors for node " << u->Id() << endl));

		uPrefix = 0; // ��ʼΪû�з���·�ɵ�����
		//bfs_nodessearched++;//�Ѿ�������Ľ�������������δʹ��

		if (targetIP != IPADDR_NONE)
		{ // See if this node is the target
			DEBUG0((cout << "Checking local ip" << endl));
			bool localIP = u->LocalIP(targetIP);//�ж��Ƿ�Ϊu��㱾��ip
			DEBUG(1,(cout << "NodeId " << u->Id()
				<< " targetIP " << (string)IPAddr(targetIP)
				<< " localIP " << localIP << endl));
			Interface* localIf = nil;
			DEBUG0((cout << "Checking local route" << endl));
			if (!localIP) 
				localIf = u->LocalRoute(targetIP);//�ж�Ŀ��ip�Ƿ�Ϊ���u����Interface�������ı���ip
			DEBUG(1,(cout << "NodeId " << u->Id()
				<< " targetIP " << (string)IPAddr(targetIP)
				<< " localIf " << localIf << endl));

			// added for proxy routing
			DEBUG0((cout << "Checking proxy routing ability" << endl));

			if (!localIf) 
			{
				if (u->HasProxyRoutingConfig()) //u����Ƿ�Ϊ·�ɽڵ�
				{
					DEBUG(1, (cout<< "NodeId " << u->Id() <<" targetIP "
						<< (string)IPAddr(targetIP)
						<< " Proxy Route Config=> IP:"
						<< (string)IPAddr(u->GetProxyIP())
						<< "NetMask:" <<(string)IPAddr(u->GetProxyMask())
						<<endl));
					if (u->CanProxyRouteIP(targetIP)) //�Ƿ�Ϊu·�ɽ���Ͻ���
					{
						uPrefix = u->GetLongestPrefixLength(targetIP);
						// if the longest prefix match is not better than where
						// we started from then the node should not participate
						// in proxy routing
						DEBUG(1, (cout<<"Prefix len="<<uPrefix
							<<" longest prefix len="<<longestPrefix<<endl));
						if (uPrefix>longestPrefix) 
						{
							longestPrefix = uPrefix;
							r = u->Id();//���õ�·��
							//targetIP = Node::GetNode(r)->GetIPAddr();
							DEBUG(1, (cout<<"Found a better one  Routable!"<<endl));
						}
					} else
						DEBUG(1, (cout<<"Cannot proxy route this IP"<<endl));
				}
			}

			if (localIP || localIf)
			{ // Found the target
				r = u->Id();
				//targetIP = Node::GetNode(r)->GetIPAddr();
#undef USE_ALIASES
#ifdef USE_ALIASES
				if (localIf)
				{ // Found last hop with local route, get aliases
					DEBUG(1,(cout << "Found NV, getting neighbors" << endl));
					const IFVec_t ifaces = u->Interfaces();
					DEBUG(1,(cout << "Found it...ifsize " << ifaces.size()
						<< " calculating aliases" << endl));
					for (IFVec_t::size_type k = 0; k < ifaces.size(); ++k)
					{
						IPAddrVec_t maybeAliases;
						u->NeighborsByIf(ifaces[k], maybeAliases);
						DEBUG(1,(cout << "Iteration " << k
							<< " maybeAliaas size " << maybeAliases.size()
							<< endl));
						// See if local route to each potential alias
						for (IPAddrVec_t::size_type j = 0;
							j < maybeAliases.size(); ++j)
						{
							if (u->LocalRoute(maybeAliases[j]))
							{ // insure not parent in the tree
								Node* myParent = Parent[u->Id()];
								if (!myParent->LocalIP(maybeAliases[j]))
								{
									aliases.push_back(maybeAliases[j]);
								}
							}
						}
					}
					DEBUG(1,(cout << "Found NV to " <<(string)IPAddr(targetIP)
						<< " last hop " << (string)IPAddr(u->GetIPAddr())
						<< " lhid " << u->Id()
						<< " with " << aliases.size() << " aliases"
						<< endl));
					DEBUG(1,(cout << "Done with alias calculation" << endl));
#ifdef  VERBOSE_DEBUG1
					for (IPAddrVec_t::size_type k = 0; k < aliases.size();++k)
					{
						cout << (string)IPAddr(aliases[k]) << endl;
					}
#endif
				}
#endif
				if (localIf)
				{ // ok, go ahead and calculate all the way to end target
					DEBUG(4,(cout << "localif nc " << localIf->NeighborCount(u) <<
						endl));
					if (localIf->NeighborCount(u) == 1)//·���ھӽ��
					{ // Only one neighbor, find it
						Link* l = localIf->GetLink();
						if (l)
						{
							DEBUG(3,(cout << "Getting peer" << endl));
							InterfaceBasic* pPeer = l->GetPeer(Count_t(0));
							if (pPeer && pPeer != localIf)
							{
								DEBUG(3,(cout <<  "Getting node" << endl));
								Node* n1 = pPeer->GetNode();//localif���ھӽ��
								if (n1)
								{
									DEBUG(3,(cout << "returning based on localif"
										<< endl));
									Parent[n1->Id()] = u; // Set parent
									if (n1->Id() == u->Id())
									{
										cout << "HuH?  BFS Self Parent"<<endl;
										exit(1);
									}
									return n1->Id();
								}
							}
						}
					}
				}
				//targetIP = Node::GetNode(r)->GetIPAddr();
				return r;
			}
		}

		pruned = false;//pruned�����ж��Ƿ���Ҫ�������ҵ�ǰ����·���ھӽ��

#ifdef PROXY_PRUNE

		DEBUG0((cout<<"Proxy routing Config:"<<u->HasProxyRoutingConfig()<<endl));
		if (u->HasProxyRoutingConfig()) 
		{
			DEBUG0((cout<<"RootIP:"<<IPAddr::ToDotted(rootIP)
				<<" TargetIP:"<<IPAddr::ToDotted(rootIP)<<endl));
			DEBUG0((cout<<"Can do root IP:"<<u->CanProxyRouteIP(rootIP)
				<<" targetIP:"<<u->CanProxyRouteIP(targetIP)<<endl));

			if (u->HasProxyRoutingConfig() && !u->CanProxyRouteIP(rootIP)
				&& !u->CanProxyRouteIP(targetIP)) //���uΪ·�ɽڵ� ���� ͨ�����u����·�ɵ�Դ����Ŀ����
			{
					DEBUG0((cout<<"Pruned..."<<endl));
					pruned = true;
			}
		}

#endif

		if (!pruned) //
		{
			u->Neighbors(nwv, true); // Get neighbors, ignoring leaf nodes
			DEBUG0((cout << "Number adj " << nwv.size() << endl));
			DEBUG0((cout << "Neighbor count is " << nwv.size() << endl));

			for (NodeWeightVec_t::size_type i = 0; i < nwv.size(); ++i)//ѭ��ȡ���u���ھӽ��
			{
				NodeIfWeight& nw = nwv[i];
				Node*         n = nw.node;
				Interface*    iface = (Interface*)nw.iface;
				NodeId_t      id = n->Id();
				DEBUG0((cout << "Node " << u->Id() << " checking iface "
					<< iface << " down " << iface->IsDown() << endl));
				if (iface->IsDown()) continue;
				if (n == u) continue;              // Insure not self adjacancy

				DEBUG0((cout << " Working on adj " << id << endl));

				// before enqueuing a new node we check for proxy routing
				// we mainly want to prune subtrees that will not have the target
				// node at anycost

				// now n is the node that we want to add.

				bool discarded=false;


				if (!discarded && B.Get(id) == 0)//���ھӽ�㼴idδ������
				{ // White
					Q.push_back(NodeIf(n, iface)); // �������Q
#ifdef DEBUG_QSIZE
					if (Q.size() > maxQSize)
					{
						maxQSize = Q.size();
						cout << "New Q size " << maxQSize << endl;
					}
#endif
					B.Set(id);                     // Change to grey
					D[id] = D[u->Id()] + 1;        // Set new distance
					Parent[id] = u;                // Set parent
					if (u == root)
					{ // First hop is new node since this is root
						NextHop[id] = NodeIf(n, iface);//ΪԴ���
					}
					else
					{ // First hop is same as this one
						NextHop[id] = NextHop[u->Id()];//�ͽ��u��ͬ
					}
					DEBUG0((cout << "Enqueued " << id << endl));
				}
			}
		}

		Q.pop_front();
	}
	//targetIP = Node::GetNode(r)->GetIPAddr();
	return r;
}
#endif


#ifdef TEST_BFS
RNodeVec_t Nodes;

int main()
{
	// See the sample BFS search in Fig23.3, p471 CLR Algorithms book
	Node N0(0);
	Node N1(1);
	Node N2(2);
	Node N3(3);
	Node N4(4);
	Node N5(5);
	Node N6(6);
	Node N7(7);
	RoutingVec_t NextHop;
	RoutingVec_t Parent;

	N0.AddAdj(1);
	N0.AddAdj(2);

	N1.AddAdj(0);

	N2.AddAdj(0);
	N2.AddAdj(3);

	N3.AddAdj(2);
	N3.AddAdj(4);
	N3.AddAdj(5);

	N4.AddAdj(3);
	N4.AddAdj(5);
	N4.AddAdj(6);

	N5.AddAdj(4);
	N5.AddAdj(7); 

	N6.AddAdj(4);
	N6.AddAdj(7);

	N7.AddAdj(5);
	N7.AddAdj(6);

	Nodes.push_back(&N0);
	Nodes.push_back(&N1);
	Nodes.push_back(&N2);
	Nodes.push_back(&N3);
	Nodes.push_back(&N4);
	Nodes.push_back(&N5);
	Nodes.push_back(&N6);
	Nodes.push_back(&N7);

	for (nodeid_t i = 0; i < Nodes.size(); i++)
	{ // Get shortest path for each root node
		printf("\nFrom root %ld\n", i);
		BFS(Nodes, i, NextHop, Parent);
		PrintParents(Parent);
		for (unsigned int k = 0; k < Nodes.size(); k++)
			printf("Next hop for node %d is %ld\n", k, NextHop[k]);
		printf("Printing paths\n");
		for (nodeid_t j = 0; j < Nodes.size(); j++)
		{
			PrintRoute(i, j, Parent);
		}
	}
	return(0);
}
#endif
