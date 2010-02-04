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



// Georgia Tech Network Simulator - Packet Callback Class
// George F. Riley.  Georgia Tech, Fall 2004

// Implementation of the packet callback features of GTNetS

#include "packet-callbacks.h"
#include "interface.h"

// PacketCallbacks methods

PacketCallbacks::PacketCallbacks()
    : callbacks(nil)
{ // Nothing else needed
}

PacketCallbacks::~PacketCallbacks()
{
  if (callbacks) delete callbacks;
}

void PacketCallbacks::Add(Layer_t l, Proto_t p,  Type_t t,
                          Interface* i, Function_t f)
{ // Allocate the list
  if (!callbacks) callbacks = new CallbackList_t();
  // Add the callback
  callbacks->insert(callbacks->end(), PacketCallback(l, p, t, i, f));
}

void PacketCallbacks::AddHead(Layer_t l, Proto_t p, Type_t t,
                              Interface* i, Function_t f)
{ // Allocate the list
  if (!callbacks) callbacks = new CallbackList_t();
  // Add the callback
  callbacks->insert(callbacks->begin(), PacketCallback(l, p, t, i, f));
}

void PacketCallbacks::Delete(Layer_t l, Proto_t p, Type_t t, Interface* iface)
{
  if (!callbacks) return; // No callbacks defined
  for (CallbackList_t::iterator i = callbacks->begin(); i != callbacks->end();)
    {
      CallbackList_t::iterator j = i++; // Need advance before possible erase
      // Delete if correct layer and type
      if (j->layer == l && j->proto == p && j->type == t && j->iface == iface)
        callbacks->erase(j);
    }
}

bool PacketCallbacks::Call(Layer_t l, Proto_t p, Type_t t,
                           Packet* pkt, Node* n, Interface* iface)
{
  if (!callbacks) return true; // No callbacks defined
  for (CallbackList_t::iterator i = callbacks->begin();
       i != callbacks->end(); ++i)
    {
      if ((i->layer == 0 || i->layer == l) && i->type == t)
        { // Possible match, check iface
          if (i->iface == nil || i->iface == iface)
            { // Possible match, check proto
              if (i->proto == 0 || i->proto == p)
                { // Found a match, call it
                  // If callback return false, we are done
                  if (!i->cb(l, p, t, pkt, n, iface)) return false;
                }
            }
        }
    }
  return true; // All returned true, or none found
}

  

