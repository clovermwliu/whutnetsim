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



// Georgia Tech Network Simulator - Routing base class
// George F. Riley.  Georgia Tech, Spring 2002

// Virtual Base class for all routing interfaces

#include <iostream>

#include "G_common-defs.h"
#include "routing.h"
#include "routing-static.h"
#include "routing-nixvector.h"

using namespace std;

// Default routing object.
Routing* Routing::defaultRouting = nil;


Routing::Routing()
{
}

Routing::~Routing()
{
}

// Provide default empty implementations for  the initializations
// since many of the routing objects do not need this
void Routing::InitializeRoutes(Node*)
{ // Empty
}

void Routing::ReInitializeRoutes(Node*, bool)
{ // Empty
}

bool Routing::NeedInit()
{
  return false;
}

bool Routing::NeedReInit()
{
  return false;
}

// Static methods

Routing* Routing::Default()
{
  if (!defaultRouting) defaultRouting = new RoutingNixVector();
  return defaultRouting;
}

void Routing::SetRouting(Routing* pNew)
{ // Set new routing method.
  delete defaultRouting;          // Delete existing
  defaultRouting = pNew;          // Set new
}

void Routing::TopologyChanged(bool u)
{
  if (!Default()->NeedReInit()) return;
  const NodeVec_t& n = Node::GetNodes(); // Get the global list of nodes
  for (NodeVec_t::size_type i = 0; i < n.size(); ++i)
    { // Call the initializer for each node

      n[i]->ReInitializeRoutes(u);
    }
}

  
/* -------------- Routing Overhaul -1 SDR ---------- */

RouteTable::RouteTable()
{
  
}

RouteTable::~RouteTable()
{
  
}

void RouteTable::AddRoute(rt_key_t* key, rt_value_t* val, bool aggr)
{
  /*  if (!aggr) */
  rt_valuelist_t* listptr = fib[key];
  bool entry_exist = false;

  if(listptr) {
    for(rt_valuelist_t::iterator i = listptr->begin(); i != listptr->end(); i++) {
      rt_value_t* temp = *i;
      if( (temp->nexthop == val->nexthop) && (temp->iface == val->iface) )
	entry_exist = true;
    }
    if(!entry_exist)
      listptr->push_back(val);
  }
  else {
    listptr = new rt_valuelist_t();
    listptr->push_back(val);
    fib[key] = listptr;
  }
}

void RouteTable::DelRoute(rt_key_t* key, bool aggr)
{
  NewFIB_t::iterator curr = fib.find(key);
  if(curr != fib.end())
    fib.erase(curr);
}

void RouteTable::Flush(void)
{
    fib.clear() ;
    return;
}

rt_valuelist_t* RouteTable::LookupRoute(rt_key_t* key, bool aggr)
{
  NewFIB_t::iterator fibitr = fib.find(key);

  if (fibitr != fib.end())
     return fibitr->second;
  else
     return NULL;
 
}

void RouteTable::Dump(void)
{
  for(NewFIB_t::iterator fibitr = fib.begin(); fibitr != fib.end(); fibitr++) {
    rt_key_t* temp_rt_key = fibitr->first;
    rt_valuelist_t* temp_rt_value_list = fibitr->second;
    cout << "DST = " << IPAddr::ToDotted(temp_rt_key->addr);
		cout << "/" << temp_rt_key->count;
    for(rt_valuelist_t::iterator valueIt = temp_rt_value_list->begin(); valueIt != temp_rt_value_list->end(); valueIt++) {
      cout << ";";
      rt_value_t* temp_rt_value = *valueIt;
      cout << "NextHop = " << IPAddr::ToDotted(temp_rt_value->nexthop);
    }
    cout << endl;
  }

   cout << endl;
}

int RouteTable::Size(void)
{
  return fib.size();
}
