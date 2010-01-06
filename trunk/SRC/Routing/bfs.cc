//Copyright (c) 2010, Information Security Institute of Wuhan Universtiy(ISIWhu)
//Project Homepage:http://code.google.com/p/whutnetsim/
//corresponding author's email: guochi@mail.whu.edu.cn


//All rights reserved
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
//Construct Date:
//Modify Author:
//Author Organization:
//Modify Date:



// Implementation of Breadth First Search algorithm
// George Riley, Georgia Tech, Winter 2000

// Converted to work with GTNetS, Summer 2002

#include <iostream>

//#define DEBUG_MASK 0x08
#include "G_debug.h"
#include "bfs.h"        
#include "node.h"
#include "link.h"
#include "bitmap.h"
#include "interface.h"

using namespace std;

int bfs_calls=0;
int bfs_nodessearched=0;

#define PROXY_PRUNE

#ifndef TEST_BFS

NodeId_t BFS(
         const NodeVec_t& N,
         Node*            root,
         NodeIfVec_t&     NextHop,
         NodeVec_t&       Parent,
         IPAddr_t         targetIP,
         IPAddrVec_t&     aliases)
{  // Compute shortest path to all nodes from root via Breadth First Search
 BitMap B(N.size());           // Bitmap for the colors (grey/white) (white = 0)
 NodeDeque_t Q;                // Vector for the Q list
 DistVec_t   D(N.size(), INF); // Distance vector, initialized to infinity
 NodeId_t    r = NODE_NONE;    // Return value

 bool        pruned;           // Boolean that is used for proxy pruning
 IPAddr_t    rootIP;

#ifdef DEBUG_QSIZE
Count_t maxQSize = 0; // debug
#endif

 bfs_calls++;
 
 // Insure vectors passed in are empty 
 NextHop.clear();
 Parent.clear();
 // Fill in all "NONE" in the next hop neighbors and predecessors
 NextHop.reserve(N.size());
 NextHop.insert(NextHop.begin(), N.size(), NodeIf(nil,nil));
 Parent.reserve(N.size());
 Parent.insert(Parent.begin(), N.size(), nil);


 // Set up information for root node for proxy routing
 int longestPrefix, uPrefix;

 if (root->CanProxyRouteIP(targetIP)) {
   longestPrefix = root->GetLongestPrefixLength(targetIP);
   r = root->Id();
 }
 else
   longestPrefix = 0;

 // Start the algorithm
 B.Set(root->Id()); // Color the root grey
 DEBUG(1,(B.DBPrint()));
 Q.push_back(NodeIf(root,nil)); // And put the root in Q
 D[root->Id()] = 0;
 rootIP = root->GetIPAddr();
 if (rootIP==IPADDR_NONE && root->HasProxyRoutingConfig())
   rootIP = root->GetProxyIP();


 DEBUG0((cout << "Starting BFS Loop" << endl));
 while(Q.size() != 0)
   {
     NodeIf& nodeIf = Q.front();
     Node* u = nodeIf.node;
     DEBUG0((cout << "Working on node " << u->Id() << endl));
     // Get the neighbor and link weight vector
     NodeWeightVec_t nwv;
     DEBUG0((cout << "Getting neighbors for node " << u->Id() << endl));

     uPrefix = 0; // by default no prefix match for proxy routing
     bfs_nodessearched++;

     if (targetIP != IPADDR_NONE)
       { // See if this node is the target
	 DEBUG0((cout << "Checking local ip" << endl));
	 bool localIP = u->LocalIP(targetIP);
	 DEBUG(1,(cout << "NodeId " << u->Id()
		  << " targetIP " << (string)IPAddr(targetIP)
		  << " localIP " << localIP << endl));
	 Interface* localIf = nil;
	 DEBUG0((cout << "Checking local route" << endl));
	 if (!localIP) localIf = u->LocalRoute(targetIP);
	 DEBUG(1,(cout << "NodeId " << u->Id()
		  << " targetIP " << (string)IPAddr(targetIP)
		  << " localIf " << localIf << endl));

	 // added for proxy routing
	 DEBUG0((cout << "Checking proxy routing ability" << endl));

	 if (!localIf) {
	   if (u->HasProxyRoutingConfig()) {
	     DEBUG(1, (cout<< "NodeId " << u->Id() <<" targetIP "
		       << (string)IPAddr(targetIP)
		       << " Proxy Route Config=> IP:"
		       << (string)IPAddr(u->GetProxyIP())
		       << "NetMask:" <<(string)IPAddr(u->GetProxyMask())
		       <<endl));
	     if (u->CanProxyRouteIP(targetIP)) {
	       uPrefix = u->GetLongestPrefixLength(targetIP);
	       // if the longest prefix match is not better than where
	       // we started from then the node should not participate
	       // in proxy routing
	       DEBUG(1, (cout<<"Prefix len="<<uPrefix
			 <<" longest prefix len="<<longestPrefix<<endl));
	       if (uPrefix>longestPrefix) {
		 longestPrefix = uPrefix;
		 r = u->Id();
		 DEBUG(1, (cout<<"Found a better one  Routable!"<<endl));
	       }
	     } else
	       DEBUG(1, (cout<<"Cannot proxy route this IP"<<endl));
	   }
	 }
	 
         if (localIP || localIf)
	   { // Found the target
	     r = u->Id();
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
		 if (localIf->NeighborCount(u) == 1)
		   { // Only one neighbor, find it
		     Link* l = localIf->GetLink();
		     if (l)
		       {
			 DEBUG(3,(cout << "Getting peer" << endl));
			 InterfaceBasic* pPeer = l->GetPeer(Count_t(0));
			 if (pPeer && pPeer != localIf)
			   {
			     DEBUG(3,(cout <<  "Getting node" << endl));
			     Node* n1 = pPeer->GetNode();
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

	     return r;
	   }
       }

     pruned = false;

#ifdef PROXY_PRUNE

     DEBUG0((cout<<"Proxy routing Config:"<<u->HasProxyRoutingConfig()<<endl));
     if (u->HasProxyRoutingConfig()) {
       DEBUG0((cout<<"RootIP:"<<IPAddr::ToDotted(rootIP)
	       <<" TargetIP:"<<IPAddr::ToDotted(rootIP)<<endl));
       DEBUG0((cout<<"Can do root IP:"<<u->CanProxyRouteIP(rootIP)
	       <<" targetIP:"<<u->CanProxyRouteIP(targetIP)<<endl));

       if (u->HasProxyRoutingConfig() && !u->CanProxyRouteIP(rootIP)
	   && !u->CanProxyRouteIP(targetIP)) {
	 DEBUG0((cout<<"Pruned..."<<endl));
	 pruned = true;
       }
     }

#endif
     
     if (!pruned) {
	 
       u->Neighbors(nwv, true); // Get neighbors, ignoring leaf nodes
       DEBUG0((cout << "Number adj " << nwv.size() << endl));
       DEBUG0((cout << "Neighbor count is " << nwv.size() << endl));
       
       for (NodeWeightVec_t::size_type i = 0; i < nwv.size(); ++i)
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


	   if (!discarded && B.Get(id) == 0)
	     { // White
	       Q.push_back(NodeIf(n, iface)); // Add to Q set
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
		   NextHop[id] = NodeIf(n, iface);
		 }
	       else
		 { // First hop is same as this one
		   NextHop[id] = NextHop[u->Id()];
		 }
	       DEBUG0((cout << "Enqueued " << id << endl));
	     }
	 }
     }

     Q.pop_front();
   }
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
