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



// Georgia Tech Network Simulator - Packet class
// George F. Riley.  Georgia Tech, Spring 2002

#ifndef __packet_h__
#define __packet_h__

#include <iostream>
#include <vector>

#include "G_common_defs.h"
#include "pdu.h"
#include "serializable.h"
//#include "encryption.h"
#include "memreuse.h"

class NotifyHandler;

class NixVectorOption;
//Doc:ClassXRef
//class Packet : public Serializable, public ReuseBase, public Encryption {
class Packet : public Serializable, public ReuseBase {
//Doc:Class Class {\tt Packet} defines a generic packet class which
//Doc:Class is composed of a vector of protocol data units (PDUs). It also
//Doc:Class provides a flexible interface to convert a packet into a
//Doc:Class serialized buffer for transmission over a network. ( to be used
//Doc:Class in distributed simulations )

public :
  //Doc:Method
  Packet();
  //Doc:Desc Default constructor 

  //Doc:Method
  Packet(const Packet& p); 
  //Doc:Desc This constructor takes a reference to a packet and constructs a
  //Doc:Desc new packet with identical contents.
  //Doc:Arg1 reference to a packet p
  
  //Doc:Method
  Packet(char*, Size_t);   // Construct from serialized buffer
  //Doc:Desc This constructor constructs a packet object out of the contents
  //Doc:Desc of a serialized character buffer of a given size.
  //Doc:Arg1 pointer to the character buffer
  //Doc:Arg2 size of the buffer
  
  virtual ~Packet();

  void operator=(const Packet& rhs);

public:
  
  //Doc:Method
  Size_t  Size() { return size;} // Get total size of packet
  //Doc:Desc This method returns the size of the packet contents. It adds the
  //Doc:Desc contents of all its PDUs and returns the sum.
  //Doc:Return the sum

  Size_t  SizeUpdate();
  
  //Doc:Method
  void    PushPDU(PDU* pdu)           // Add a new PDU header
  //Doc:Desc This method is used to inset a protocol data unit (PDU) into
  //Doc:Desc the packet. The PDU is added at the end of the PDU vector.
  //Doc:Arg1 pointer to the pdu object
    {
      if (top == PDUs.size())
        { // Need to add new one
          PDUs.push_back(pdu);
          top++;
        }
      else
        { // Can reuse existing, after deleting
          if (PDUs[top] == pdu)
            { // Replacing with original    
              top++;
            }
          else
            { // New one, delete and re-insert      
              delete PDUs[top];    // Delete previous pdu   
              PDUs[top++] = pdu;   // Replace with this one 
            }
        }
      size += pdu->Size();
    }

  //Doc:Method
  void    PushPDUBottom(PDU*);   // Add a new PDU header at bottom of stack
  //Doc:Desc This method is used to inset a protocol data unit (PDU) into
  //Doc:Desc the packet at the bottom of the stack. The PDU is added at the
  //Doc:Desc beginning of the PDU vector.
  //Doc:Arg1 pointer to the pdu object

  //Doc:Method
  void    InsertPDU(PDU*, NCount_t offset = 0);
    //Doc:Desc Insert a new PDU in the middle of the PDU stack. 
    //Doc:Desc It goes the specified offset
    //Doc:Desc ABOVE the current pdu.  For example, if we
    //Doc:Desc are in the L2 processor and the next unprocessed PDU is the
    //Doc:Desc L2802.3 header, an insert with offset = 0 will insert this
    //Doc:Desc between the L2802.3 header and the one above it in the stack.
    //Doc:Desc An offset of 1 will insert it above the next PDU above
    //Doc:Desc the L2802.3 header.  Negative offsets insert below  the current.
    //Doc:Arg1 PDU to insert.
    //Doc:Arg2 Insertion offset.

  //Doc:Method 
  PDU*    PopPDU()               // Get last PDU header
  //Doc:Desc This method is used when we want to pop out a PDU from the
  //Doc:Desc PDU stack and delete it from the packet.
  //Doc:Return A pointer to the PDU object
    {
      if (top == 0) return nil;  // Nope, nothing there
      PDU* p =  PDUs[--top];     // Get the pointer
      size -= p->Size();         // Reduce size
      return p;
    }

  //Doc:Method
  PDU*    PeekPDU()              // Get last PDU header without removal
  //Doc:Desc This method returns the last PDU without removing it from the
  //Doc:Desc packet's vector of PDUs.
  //Doc:Return A pointer to the PDU  object
    {
      if (top == 0) return nil;   // Nope, nothing there
      return PDUs[top - 1];       // Return without removing
    }
  
  //change Count_t to NCount_t in order to PEEK forward or backward.
  //type conversion for avoiding the warnings.
  //but potentially have problems here!
  //
  //Doc:Method
  PDU*    PeekPDU(NCount_t offset) // Peek at last + offset without removal
  //Doc:Desc This method returns the PDU at 'offset' from the last without
  //Doc:Desc removing it from the packet's vector of PDUs.
  //Doc:Desc NOTE.  A negative offset peeks at PDUS already popped.
  //Doc:Desc NOTE.  A positive offset peeks at PDUS not yet popped.
  //Doc:Return A pointer to the PDU  object	  
    {
      if ((NCount_t)top <= offset) return nil;  // Nope, nothing there
      return PDUs[top - offset - 1];   // Return without removing
    }

  //Doc:Method
  void    SkipPDU()              // Skip the current pdu
  //Doc:Desc This method skips the current pdu that's at the top of the
  //Doc:Desc PDU stack without deleting the PDU
    {
      if (top == 0) return;      // Nope, nothing there
      PDU* p =  PDUs[--top];     // Get the pointer
      size -= p->Size();         // Reduce size
    }

  //Doc:Method
  PDU*    PushExisting()
  //Doc:Desc This method returns the existing PDUs that have been skipped int
  //Doc:Desc in the {\tt SkipPDU } method
  //Doc:Return pointer to the skipped PDU
    {
      if (top == PDUs.size()) return nil; // None existing
      return PDUs[top++];
    }


  //Doc:Method
  PDU*    FindPDU(Layer_t);
    //Doc:Desc Finds a pdu in the pdu list for the specified layer.
    //Doc:Arg1 Layer to find.
    //Doc:Return PDU for specified layer, nil if none

  //Doc:Method
  PDU*    FindPDU(Layer_t, Proto_t);
    //Doc:Desc Finds a pdu in the pdu list for the specified layer and protocol
    //Doc:Arg1 Layer to find.
    //Doc:Arg1 Protocol to find.
    //Doc:Return PDU for specified layer and protocol, nil if none



  //Doc:Method
  virtual Packet* Copy() const;  // Make a copy of this packet
  //Doc:Desc This method is a generic copy function that makes a copy of
  //Doc:Desc itself and returns the pointer to the copy.
  //Doc:Return pointer to the copy of the packet
  
  // Serialization interface

  //Doc:Method
  Size_t  SSize();               // Size needed for serialization
  //Doc:Desc This method returns the total size of the payload in this
  //Doc:Desc that needs to be serialized for transmisison over the
  //Doc:Desc network to a remote simulator instance. ( for distributed
  //Doc:Desc simulations )
  //Doc:Return Payload size
  
  //Doc:Method
  char*   Serialize(char*, Size_t&); // Serialize to a working buffer
  //Doc:Desc This method serializes the contents of this packet into
  //Doc:Desc a serial character buffer
  //Doc:Arg1 charater pointer to the buffer
  //Doc:Arg2 reference to the size of the buffer
  //Doc:Return the serialized buffer
  
  //Doc:Method
  char*   Construct(char*, Size_t&); // Construct from serialized buffer
  //Doc:Desc This method does the reverse job of constructing the packet
  //Doc:Desc from a character buffer of a give size
  //Doc:Arg1 the serialized character buffer
  //Doc:Arg2 the size of the serialized buffer
  //Doc:Return the pointer to the buffer

  //Doc:Method
  virtual Priority_t Priority();
    //Doc:Desc Determine the priority of this packet.
    //Doc:Return Packet priority

  //Doc:Method
  virtual MACAddr GetSrcMac();
    //Doc:Desc Return the source MAC address for this packet
    //Doc:Return Source MAC Address

  //Doc:Method
  virtual MACAddr GetDstMac();
    //Doc:Desc Return the destination MAC address for this packet
    //Doc:Return destination MAC Address

  //Doc:Method
  virtual IPAddr_t GetSrcIP();
    //Doc:Desc Return the source IP address for this packet
    //Doc:Return source IP Address

  //Doc:Method
  virtual IPAddr_t GetDstIP();
    //Doc:Desc Return the destination IP address for this packet
    //Doc:Return destination IP Address

  //Doc:Method
  virtual bool IsColored() { return false; }
    //Doc:Desc Determine if a packet has an associated color (for animation).
    //Doc:Desc We define this and the three color component methods (below) as
    //Doc:Desc virtual, and always return {\tt false} here.
    //Doc:Desc There is a subclass
    //Doc:Desc called {\tt ColoredPacket} that will define these values.
    //Doc:Desc This is
    //Doc:Desc to save memory in packet definitions, as only those packets
    //Doc:Desc assigned will have memory to store it.
    //Doc:Return True if colored.
  
  //Doc:Method
  virtual Color_t R() { return 0; }
    //Doc:Desc Return the Red component of the color.
    //Doc:Return The Red component.

  //Doc:Method
  virtual Color_t G() { return 0; }
    //Doc:Desc Return the Green component of the color.
    //Doc:Return The Green component.
  
  //Doc:Method
  virtual Color_t B() { return 0; }
    //Doc:Desc Return the Blue component of the color.
    //Doc:Return The Blue component.

  // Debugging
  //Doc:Method
  void DBPrint();          // Print packet info for debugging
    //Doc:Desc Prints some debugging information about the pacekt

public:
  //Doc:Member
  PDUVec_t PDUs;           // Vector of protocol data unit headers
    //Doc:Desc A vector of protocol data unit (PDU) pointers for this packet.

  //Doc:Member
  PDUVec_t::size_type top; // Current top of PDUs stack
    //Doc:Desc The vector containing the PDU's does not shrink when a PDU is
    //Doc:Desc removed, so we need a value specifying what is the index
    //Doc:Desc of the current {\em top} of the stack.

  //Doc:Member
  Count_t  retx;            // Retransmission counter
  //Doc:Desc Counter specifying how many times this packet was retransmitted
 
  //Doc:Member
  Count_t  uid;            // Each packet has a unique id
    //Doc:Desc Every packet in \GTNS\ has a unique 32 bit identifier,
    //Doc:Desc  allowing the
    //Doc:Desc packet to be easily identified in the trace file.

  //Doc:Member
  Size_t   size;           // Total size in bytes, all pdu's
    //Doc:Desc Total size of this packet.  This size is measured in number 
    //Doc:Desc of bytes
    //Doc:Desc that will be transmitted on a link for this packet.  In \GTNS,
    //Doc:Desc the PDU representing an IPV4 header (for example)
    //Doc:Desc  is substantially larger than
    //Doc:Desc the 20 bytes in a normal IPV4 PDU.  But the size for the IPV4
    //Doc:Desc PDU is claimed to be 20 bytes for purposes of computing
    //Doc:Desc transmission time on a lnk.

  //Doc:Member
  Time_t   time;           // Time this packet was created
    //Doc:Desc Timestamp for when this packet was originally created.

  //Doc:Member
  NixVectorOption* nixVec; // Nix vector (if present)
    //Doc:Desc Pointer to associated \NV\ for this packet (if present).

  //
  //specially for aodv, maybe others also need it
  //
  Time_t   expire;          //the time the packet expires!

  // NotifyHandler object to inform when this packet is transmitted
  NotifyHandler*  notification;  
private:
  //Doc:Member
  static Count_t nextUid;  // Next available uid
    //Doc:Desc Next available unique identifier for a packet.

};

// Define a "ColoredPacket" that has an associated color.
class ColoredPacket : public Packet {
public:
  ColoredPacket();
  ColoredPacket(const MyColor&);
  ColoredPacket(const ColoredPacket&);
  virtual ~ColoredPacket();
  Packet* Copy() const;    // Make a copy of this packet
  bool IsColored();
  Color_t R();
  Color_t G();
  Color_t B();
  void    SetColor(const MyColor&);
  MyColor* Color() const;
private:
  MyColor* color;
};

#endif
