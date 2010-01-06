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



// Georgia Tech Network Simulator - Protocol Graph class
// George F. Riley.  Georgia Tech, Spring 2002

// Defines the protocol graph for each node

#include <iostream>

#include "G_debug.h"
#include "protograph.h"

using namespace std;

ProtocolGraph* ProtocolGraph::common;

ProtocolGraph::ProtocolGraph() 
  : ProtoByLayer(N_LAYERS+1, ProtoMap_t())
{
}

Protocol* ProtocolGraph::Lookup(Layer_t layer, Proto_t proto)
{
  DEBUG0(cout << "ProtocolGraph::Lookup l" << layer 
         << " proto " << proto << endl);
  if (layer < 0 || layer > N_LAYERS) return nil;
  DEBUG0(cout << "ProtocolGraph::Lookup returning " 
         << ProtoByLayer[layer][proto] << endl);
  return ProtoByLayer[layer][proto];
}

Protocol* ProtocolGraph::Default(Layer_t layer)
{ // Used by some l2 protocols to find a "default" l3, if the
  // l2 proto does not have a "next layer protocol" field
  if (layer < 0 || layer > N_LAYERS) return nil;
  if (ProtoByLayer[layer].size() != 1) return nil; // zero or more than 1
  return (*ProtoByLayer[layer].begin()).second;// Only proto in specified layer
}

  
void ProtocolGraph::Insert(Layer_t layer, Proto_t proto, Protocol* p)
{
  DEBUG0((cout << "Inserting protograph, layer " << layer
          << " proto " << proto << endl));
  ProtoByLayer[layer].insert(ProtoMap_t::value_type(proto, p));
}

void ProtocolGraph::Remove(Layer_t layer, Proto_t proto)
{
  ProtoMap_t& m = ProtoByLayer[layer];
  ProtoMap_t::iterator i = m.find(proto);
  if (i == m.end()) return; // Not found
  m.erase(i);               // Remove from map
}

// Static methods
void ProtocolGraph::CreateCommon()     // Create the shared graph if needed
{
  if (!common)
    common = new ProtocolGraph();
}

