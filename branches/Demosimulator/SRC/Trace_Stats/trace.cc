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



// Georgia Tech Network Simulator - Global trace control
// George F. Riley.  Georgia Tech, Spring 2002

#include <iostream>

#include "trace.h"
#include "protograph.h"
#include "node.h"

using namespace std;

Trace*            Trace::instance;        // Single global trace object
bool              Trace::enabled = false; // Global enabled flag
Trace::Callback_t Trace::callback = nil;  // Tracing callback

Trace::Trace() 
  : trace(ProtocolGraph::N_LAYERS, DISABLED), node(0)
{
}

Trace::~Trace()
{
  Close(); // Close the file if open
}

bool Trace::Open(const char* fn)
{
  os.open(fn, ios::out);
  if (os) enabled = true;
  return os;  // True if opened successfully
}

void Trace::Close()
{
  if (os.is_open())
    {
      os.AppendEOL();
      os.close();
    }
  enabled = false;
}

void Trace::On()
{ // Enable all
  for (TSVec_t::size_type i = 0; i < trace.size(); ++i)
    {
      trace[i] = ENABLED;
    }
}

void Trace::On(Layer_t l)
{
  if (l > ProtocolGraph::N_LAYERS) return;  // Out of range
  trace[l-1] = ENABLED;
}

void Trace::Off()
{ // Disable all
  for (TSVec_t::size_type i = 0; i < trace.size(); ++i)
    {
      trace[i] = DISABLED;
    }
}

void Trace::Off(Layer_t l)
{
  if (l > ProtocolGraph::N_LAYERS) return;  // Out of range
  trace[l-1] = DISABLED;
}

bool Trace::IsOn(Layer_t l)
{ // Enquire trace status by layer
  if (l > ProtocolGraph::N_LAYERS) return false;  // Out of range
  if (!os.is_open()) return false;  // Not enabled if trace file not open
  return trace[l-1] == ENABLED;
}

bool Trace::IsEnabled()
{
  return(os.is_open());
}

void Trace::AppendEOL()
{
  if(os)os.AppendEOL(); // Add the end of line if needed
}

void Trace::NewNode(Node* n)
{ // A packet has arrived at a new node, see if log needed
  os.NewNode(n);
}

void Trace::SetNode(Node* n)
{ // Set node pointer for subsequent traces
  os.SetNode(n);
}

// Static methods
Trace* Trace::Instance()
{
  // If no instance, create one
  if (!instance) instance = new Trace();
  return instance;
}

