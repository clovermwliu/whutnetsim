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


// Georgia Tech Network Simulator - Protocol Data Unit base class
// George F. Riley.  Georgia Tech, Spring 2002

// Class PDU is the base class for all packet headers

#ifndef __pdh_h__
#define __pdh_h__

#include <vector>

#include "G_debug.h"
#include "G_common_defs.h"
#include "tfstream.h"
#include "G_globalstats.h"
#include "serializable.h"
#include "memreuse.h"

//Doc:ClassXRef
class PDU : public Serializable, public ReuseBase {
//Doc:Class Class {\tt PDU} serves as the base class for all the 
//Doc:Class protocol data units. Protocol headers and data chunks that
//Doc:Class that form the packets are derived from this class.
	
public:
  //Doc:Method
  PDU() { DBG((Stats::pdusAllocated++));}
  //Doc:Desc This method is the default constructor for this class. It
  //Doc:Desc updates a static variable to reflect the number of PDUs
  //Doc:Desc allocated
  
  //Doc:Method
  PDU(const PDU&) { DBG((Stats::pdusAllocated++));}
  //Doc:Desc This constructs a PDU from the reference to an already existing
  //Doc:Desc PDU.
  //Doc:Arg1 The const reference to the existing PDU
  
  //Doc:Method
  virtual ~PDU() { DBG((Stats::pdusDeleted++));}
  
public:
  //Doc:Method
  virtual Size_t Size() const = 0;    // PDU's must have a size
  //Doc:Desc This method returns the size of the PDU
  //Doc:Return the size of the PDU
  
  //Doc:Method
  virtual PDU* Copy() const = 0;            // Create a copy of this header
  //Doc:Desc This method creates a copy of this PDU
  //Doc:Return A pointer to the copy of this PDU
  
  //Doc:Method  
  virtual Layer_t Layer() { return 0;}// Some PDU's specify their layer
  //Doc:Desc Each PDU must also return a layer numer as to which protocol layer
  //Doc:Desc it belongs to according to the ISO model
  //Doc:Return the layer number
  
  //Doc:Method  
  virtual Layer_t SubLayer() { return 0;}// Some layers have sublayers
  //Doc:Desc Each PDU may be part of a sub-layer.  For example, LLCSNAP
  //Doc:Desc is layer "2a", sitting between the real layer 2 and l3 PDUs.
  //Doc:Return The sub-layer number
  
  //Doc:Method  
  virtual Count_t Version() { return 0;} // Some PDU's report version number
  //Doc:Desc If we are using multiple versions of the same PDU we may want
  //Doc:Desc to return the version number of the PDU we are using. Also useful
  //Doc:Desc when tracing
  //Doc:Return the version number
  
  //Doc:Method  
  virtual Proto_t Proto() { return 0;}   // Some PDU's report protocol number
  //Doc:Desc Most of the protocols are identified by the numeric values as
  //Doc:Desc listed by IANA. thie method returns that protocol number
  //Doc:Return the protocol number

  //Doc:Method
  virtual Priority_t Priority() { return 0;} // Some PDU's have priority
    //Doc:Desc For queues that support priorities, packets need some way to
    //Doc:Desc claim what the priority is.  This is done by querying the
    //Doc:Desc PDU list.  When a non-zero priority is found, that
    //Doc:Desc is the claimed priority.
    //Doc:Return Priority associated with this PDU (zero if none.
  
  // Trace the contents of this pdu
  //Doc:Method  
  virtual void Trace(Tfstream&, Bitmap_t,
                     Packet* = nil, const char* = nil) { }
  //Doc:Desc This method traces the contents of the PDU
  //Doc:Arg1 File output stream descriptor
  //Doc:Arg2 Bitmap that specifies the level of detail of the trace
  //Doc:Arg3 Packet that contains this PDU (optional)
  //Doc:Arg4 Extra text information for trace file (optional)
  
  //Doc:Method  
  bool Detail(Bitmap_t d, Bitmap_t b) { return (1L << d) & b;}
  // Include the serializable interface here as a temporary patch
  // until all PDU subclasses implement this
  //Doc:Method  
  virtual Size_t SSize() { return 0;}      // Size needed for serialization
  //Doc:Desc The actual size of all the PDU contents in terms of bytes for
  //Doc:Desc serialization of the contents.
  //Doc:Return the size
  
  //Doc:Method  
  virtual char*  Serialize(char* b, Size_t&) { return b;}// Serialize to buffer
  //Doc:Desc This method is used to serialize the contents of the PDU
  //Doc:Desc into a serialized character buffer b . the size of this buffer is
  //Doc:Desc in the size arguement
  //Doc:Arg1 This is the pointer to the character buffer
  //Doc:Arg2 a reference to the size variable
  //Doc:Return the pointer to the updated character buffer
  
  //Doc:Method  
  virtual char*  Construct(char* b, Size_t&) { return b;}// Const. from buffer
  //Doc:Desc This method is the reverse of the Serialize . It constructs the
  //Doc:Desc PDU object from the character buffer that has been received over
  //Doc:Desc the network
  //Doc:Arg1 This is the pointer to the character buffer
  //Doc:Arg2 a reference to the size variable
  //Doc:Return the pointer to the character buffer of the remaining data
};

typedef std::vector<PDU*> PDUVec_t;     // Vector of packet headers

#endif


