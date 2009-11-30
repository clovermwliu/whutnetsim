/*



* GTNetS provides a portable C++ class library for network simulations

*

* Copyright (C) 2003 George F. Riley

*

* No guarantees or warranties or anything are provided or implied in any way

* whatsoever.  Use this program at your own risk.  Permission to use this

* program for any purpose is given, as long as the copyright is kept intact.

*

* This program is free software; you can redistribute it and/or

* modify it under the terms of the GNU General Public License

* as published by the Free Software Foundation; either version 2

* of the License, or (at your option) any later version.

*

* This program is distributed in the hope that it will be useful,

* but WITHOUT ANY WARRANTY; without even the implied warranty of

* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the

* GNU General Public License for more details.

*

* You should have received a copy of the GNU General Public License

* along with this program; if not, write to the Free Software

* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.



* Permission is hereby granted for any non-commercial use of this code



*/



// Georgia Tech Network Simulator - Random Tree 

// Monirul I Sharif,  Georgia Tech, Fall 2003

// Modified from original tree.cc



// Define the random tree topology using point-to-point links



//#define DEBUG_MASK 1



#include "myrandomtree.h"

#include "node.h"

#include "linkp2p.h"

#include "mask.h"

#include <stdio.h>

#include <math.h>



using namespace std;



bool myTurnOffProxyRouting = false;



// Constructors



myrandomtree::myrandomtree(Count_t l, Count_t mf, Mask m, 

		       Random_t t, const Random &r, const Location &ll, const Location& ur, SystemId_t id)

{

  ConstructorHelper(l, mf, Linkp2p::Default(), 0, 0, Linkp2p::Default(), m, t, r, ll, ur, id);

}



myrandomtree::myrandomtree(Count_t l, Count_t mf, const Linkp2p& link, Mask m, 

		       Random_t t, const Random &r, const Location &ll, const Location& ur, SystemId_t id)

{

  ConstructorHelper(l, mf, link,  0, 0, Linkp2p::Default(),  m, t, r, ll, ur, id);

}



myrandomtree::myrandomtree(Count_t l, Count_t mf, const Linkp2p& link, 

		       Count_t lls, Count_t mfls, const Linkp2p& linkfs, Mask m, 

		       Random_t t, const Random &r, const Location &ll, const Location& ur, SystemId_t id)

{

  ConstructorHelper(l, mf, link,  lls, mfls, linkfs,  m, t, r, ll, ur, id);

}





// Access functions

Node* myrandomtree::GetRoot()

{ // Get the root node

  return GetNode(0,0);

}





Node* myrandomtree::GetNode(Count_t l, Count_t c) // Level, index

{ // Get Specified right side leaf node

  if (l>=nodes.size()) return nil;



  if (c>nodes[l].size()) return nil;



  return nodes[l][c]; // Out of range

}





Node* myrandomtree::GetLeaf(Count_t c)

{ // Get specified leaf

  if (c < leafCount) return GetNode(levels-1,c);

  return nil; // Out of range

}



/*

Linkp2p* myrandomtree::GetChildLink(Count_t l,  // Level

                            Count_t i,  // Index on level

                            Count_t c)  // Child number

{ 

  if (c >= maxfanout) return nil;

  Node* n = GetNode(l, i);

  if (!n) return nil;

  Node* cn = GetNode(l + 1, i * fanout + c);

  if (!cn) return nil;

  return (Linkp2p*)n->GetLink(cn);

}



Linkp2p* myrandomtree::GetParentLink(Count_t l, Count_t i)

{ // Get link to parent from specified node

  Node* n = GetNode(l, i);

  if (!n) return nil;

  Node* pn = GetNode(l - 1, i / fanout);

  if (!pn) return nil;

  return (Linkp2p*)n->GetLink(pn);

}



Queue* myrandomtree::GetChildQueue(Count_t l,  // Level

                           Count_t i,  // Index on level

                           Count_t c)  // Child number

{ 

  if (c >= fanout) return nil;

  Node* n = GetNode(l, i);

  if (!n) return nil;

  Node* cn = GetNode(l + 1, i * fanout + c);

  if (!cn) return nil;

  return n->GetQueue(cn);

}



Queue* myrandomtree::GetParentQueue(Count_t l, Count_t i)

{ // Get link to parent from specified node

  Node* n = GetNode(l, i);

  if (!n) return nil;

  Node* pn = GetNode(l - 1, i / fanout);

  if (!pn) return nil;

  return n->GetQueue(pn);

}

*/





void myrandomtree::BoundingBox(const Location& ll, const Location& rr, double angle)

{

  Meters_t xx,yy,x,y,nx,ny;

  Count_t thisLevelCount = 1;



  angle = M_PI/180.0 * angle;  // convert to radians



  xx = (ll.X()+rr.X())/2;

  yy = rr.Y();



  for (Count_t lev = 0; lev < levels; ++lev) {

    thisLevelCount = nodes[lev].size();

    DEBUG(1,(cout<<"Nodes in this level "<<thisLevelCount<<endl));

    for (Count_t count = 0; count < thisLevelCount; ++count) {

      Node *node = GetNode(lev, count);



      DEBUG(1,(cout<<"In level "<<lev<<" count "<<count<<endl));

      

      x = node->GetLocation().X()-xx;

      y = node->GetLocation().Y()-yy;



      nx = x* cos(angle)-y*sin(angle);

      ny = x* sin(angle)+y*cos(angle);



      nx = nx + xx;

      ny = ny + yy;



      DEBUG0((printf("Location %6.2lf %6.2lf\n", nx,ny)));

      

      node->SetLocation(Location(nx,ny));

      

    }

   

    DEBUG0((cout<<"Going to the next level"<<endl));

  }



}





void myrandomtree::MakeSubtree(Node *parent, Count_t l,  const Linkp2p& link, const Linkp2p& linkfs,

			     IPAddr_t leafIP, Mask mask, const Location& ll, const Location& ur, 

			     const Meters_t ydiff, SystemId_t id)

{

  Count_t  thisLevelCount;

  IPAddr_t nextIP = leafIP;

  bool     lastLevel = (l == levels-1);

  //Count_t  maskincrease;

  Meters_t width = (ur.X()-ll.X()) / maxfanout;

  bool     infs = (l<=levels_fs);



  if (infs) {

    thisLevelCount = maxfanout_fs;

    DEBUG(1,(cout<<"IN First stage"<<endl));

  }  else

    thisLevelCount = maxfanout;



  //maskincrease = (Count_t) (log((double)thisLevelCount+1)/log((double)2));

  //DEBUG(1,(cout<<"Mask increase "<<maskincrease<<endl));



  //step/=thisLevelCount;

  //Mask     newmask(mask.NBits()+maskincrease);



  //DEBUG0((cout<<"ID="<<parent->Id()<<" IP="<<IPAddr::ToDotted(nextIP)<<" Level="

	 // <<l<<" Mask="<<mask.NBits()<<" Step(for children)="<<step<<endl));  



  //  if (!myTurnOffProxyRouting)

  if (l>1)  // set proxy routing config on all parent nodes except the root!    

    parent->SetProxyRoutingConfig(nextIP, mask); // the level of the proxy router is the previous level

   

  Location nll, nur;



  for (Count_t c = 0; c < thisLevelCount; ++c)

    { // Create each child

      if (probRandom->Value()<probGenerateChild) {

	Node* child = new Node(id);

	

	if (nodes.size()<=l) {

	  NodeVec_t nl;

	  nl.push_back(child);

	  nodes.push_back(nl);

	} else {

	  nodes[l].push_back(child);

	}



	nll.X(ll.X()+width*c);

	nll.Y(ll.Y());

	nur.X(ll.X()+width*(c+1));

	nur.Y(ur.Y()-ydiff);

	child->SetLocation( (nur.X()+nll.X())/2, nur.Y());

	

	if (lastLevel)

	  { // Create link with IP Address

	    if (nextIP == IPADDR_NONE)

	      { // No IP specified

		if (infs) 

		  child->AddDuplexLink(parent, linkfs);

		else

		  child->AddDuplexLink(parent, link);

	      }

	    else

	      {

		if (infs)

		  child->AddDuplexLink(parent, linkfs,

				       nextIP, Mask(32), 

				       IPADDR_NONE, Mask(32));

		else

		  child->AddDuplexLink(parent, link,

				       nextIP, Mask(32),

				       IPADDR_NONE, Mask(32));

	      }



	    DEBUG(1,(cout<<"Setting leaf node ID="<<child->Id()<<" with IP="<<IPAddr::ToDotted(nextIP)<<endl));

	  }

	else

	  { // Create anonymous (no IPAddr) link

	    DEBUG(1,(cout<<"Making subtree with IP "<<IPAddr::ToDotted(nextIP)<<endl));

	    if (infs)

	      child->AddDuplexLink(parent, linkfs);

	    else

	      child->AddDuplexLink(parent, link);



	    MakeSubtree(child, l+1, link, linkfs, nextIP, mask, nll, nur, ydiff, id); 

	  }

	// Add a default route from child to parent

	child->DefaultRoute(parent);

      }

      //nextIP+=step;
	  //****///nextIP = GetIPRandomly();
    }      

}





void myrandomtree::ConstructorHelper(Count_t l, Count_t mf, const Linkp2p& link, 

				   Count_t lfs, Count_t mffs, const Linkp2p& linkfs,

				   Mask m, Random_t t, const Random &r, 

				   const Location& ll, const Location& ur, SystemId_t id)

{

  Node *root = new Node(id);



  levels = l;

  maxfanout = mf;



  levels_fs = lfs;

  maxfanout_fs = mffs;



  probGenerateChild = t;

  probRandom = r.Copy();



  DEBUG(1, (cout<<"CREATING myrandomtree: levels="<<levels<<", fanout="<<maxfanout<<endl));



  //Count_t  initstep;                        // defining step

  //initstep = (Count_t) pow((double)mf, (double)l-1-lfs)*

    //(Count_t)pow((double)mffs,(double)lfs) ;

 // DEBUG0((cout<<"Initial step "<<initstep<<endl));



  NodeVec_t nl;



  nl.push_back(root);

  nodes.push_back(nl);



  Meters_t ydiff;

  if (levels<=1) 

    ydiff = 0;

  else

    ydiff = (ur.Y()-ll.Y()) / (levels);

  

  Location nll(ll.X(),ll.Y()+ydiff/2), nur(ur.X(),ur.Y()-ydiff/2);



  root->SetLocation((nll.X()+nur.X())/2, nur.Y());



  //Count_t  initsubmasklen = ((Count_t)(log((double)maxfanout+1)/log((double)2)))*(levels-1-levels_fs);

  //Count_t  initsubmasklenfs = ((Count_t)(log((double)maxfanout_fs+1)/log((double)2)))*(levels_fs); 

  //for first stage

  

  //DEBUG0((cout << "Initial submask length ="<<initsubmasklen<<" for first stage ="<<initsubmasklenfs<<endl));

  IPAddr_t leafIP;
  //*****//leafIP = GetIPRandomly();

  MakeSubtree(root, 1, //level

	      link, linkfs, leafIP, 

	      m, //mask

	      nll,nur, ydiff, id);



  if (nodes.size()==levels)

    leafCount = nodes[levels-1].size();

  else

    leafCount = 0;

  

  for (unsigned int i = 0; i<nodes.size(); i++)

    DEBUG0((cout<<"Level "<<i<<" nodes="<<nodes[i].size()<<endl));

}







myrandomtree::myrandomtree(Count_t stages, struct RandomTreeStage stage[], Mask m, Random_t t, SystemId_t id, const Random &r)

{

  this->stages = stages;

  this->stage = new struct RandomTreeStage[stages];



  for (unsigned i = 0; i<stages; i++) 

    memcpy(&this->stage[i],&stage[i], sizeof(struct RandomTreeStage));



  // I only support 2 stages now, not more than that



  // I'll do the rest later, so for now I'll use the previous methods



  if (stages==1) 

    ConstructorHelper(stage[0].levels+1, stage[0].fanout, stage[0].link, 0, 0, Linkp2p::Default(), m, t, r, Location(0, 0), Location(1,1), id);

  else 

    ConstructorHelper(stage[0].levels+stage[1].levels+1, stage[1].fanout, stage[1].link, stage[0].levels+1, 

		      stage[0].fanout, stage[0].link, m, t, r, Location(0,0), Location(1,1), id);



}

