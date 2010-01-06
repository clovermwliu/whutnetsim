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



// Georgia Tech Network Simulator - Packet Callback Class
// George F. Riley.  Georgia Tech, Fall 2004

// Definitions needed for the packet callback features of GTNetS

#ifndef __packet_callbacks_h__
#define __packet_callbacks_h__

#include <list>

#include "G_common-defs.h"

class Packet;
class Node;
class Interface;

class PacketCallbacks {
public:
  // Specifies if the callback is for a packet transmission or receipt
  typedef enum { TX, RX } Type_t;
  // Defines the callback function.  Must have six parameters that
  // are passed to the callback by the GTNetS protocol layer processors:
  // Layer is the protocol layer that is calling the callback.
  // Proto is the protocol number that is calling the callback.
  // Type is TX (packet transmission) or RX (Packet receipt).
  // Packet* is the packet itself. The top of the PDU stack in the
  // packet will have the PDU from the calling layer.  In other words,
  // if the callback is from layer 2, the layer 2 pdu will be the
  // top of the stack.
  // Node* is the node that is processing the packet when the callback
  // Interface* is the interface that received or is transmitting the
  // packet (valid only for layer 2 callbacks).
  // occurs.
  // IMPORTANT:  THe callback function must return true if the packet should
  // continue to be processed by the protocol stack, and false if the
  // callback function has deleted the packet.  For example, if the
  // callback is implementing a firewall function, the firewall may
  // decide that the packet should not be forwarded for further 
  // processing. In that case, it should drop the packet and return
  // false.
  typedef bool (*Function_t)(Layer_t, Proto_t, Type_t,
                             Packet*, Node*, Interface*);
  private:
    class PacketCallback 
    {
      public:
      PacketCallback(Layer_t l, Proto_t p, Type_t t, Interface* i,Function_t f)
          : layer(l), proto(p), type(t), iface(i), cb(f) {}
      public:
        Layer_t    layer;  // Layer for this callback
        Proto_t    proto;  // Protocol number for this callback
        Type_t     type;   // Callback type (RX or TX)
        Interface* iface;  // Interface restriction
        Function_t cb;     // The callback function itself.
    };

  typedef std::list<PacketCallback> CallbackList_t;
public:
  PacketCallbacks();
  ~PacketCallbacks();
  //Doc:Method
  void Add(Layer_t, Proto_t, 
           Type_t, Interface*,
           Function_t);
    //Doc:Desc Add a callback at the specified layer and type.
    //Doc:Desc The callback is appended to the tail of the callback list.
    //Doc:Arg1 Protocol layer for the callback.
    //Doc:Arg2 Protocol number for the  callback.  If zero, indicates all.
    //Doc:Arg3 Type (RX or TX)
    //Doc:Arg4  Interface pointer.  Layer 2 callbacks can specify an additional
    //Doc:Arg4 restriction specifying a particular interface only.
    //Doc:Arg5 Callback function pointer.
  void AddHead(Layer_t, Proto_t, 
               Type_t, Interface*,
               Function_t);
  void Delete(Layer_t, Proto_t,
              Type_t, Interface*);
  // Call all callbacks for the layer and protocol
  bool Call(Layer_t, Proto_t, Type_t, Packet*, Node*, Interface* = nil);
private:
  CallbackList_t* callbacks;
};
#endif


