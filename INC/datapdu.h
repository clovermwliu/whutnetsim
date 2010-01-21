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



// Georgia Tech Network Simulator - Data Descriptors
// George F. Riley.  Georgia Tech, Spring 2002

#ifndef __datapdu_h__
#define __datapdu_h__

#include "pdu.h"
#include "seq.h"

class Packet;
//Doc:ClassXRef
class Data : public PDU {
public:
  Data();
  Data(Size_t s, char* d = nil, Count_t msg = 0, Count_t resp = 0);
  Data(const std::string&); // Construct from string
  Data(char*, Size_t&, Packet*); // Construct from serialized buffer
  Data(const Data&);   // Copy constructor
  virtual ~Data();     // Destructor
  Size_t Size() const { return size;}
  // Serialization
  Size_t SSize();                   // Size needed for serialization
  char*  Serialize(char*, Size_t&); // Serialize to a buffer
  char*  Construct(char*, Size_t&); // Construct from buffer
  virtual void Clear();// Remove all associated data
  virtual void Add(Size_t s, const char* d = 0);// Add some data to end
  virtual void Remove(Size_t);      // Remove data from head
  //  virtual Data* RemovePointer(Size_t);      // Remove, return pointer
  // Inquire available data from (f,o) sequence pair
  virtual Size_t SizeFromSeq(const Seq&, const Seq&);
  // Inquire available data from offset
  virtual Size_t SizeFromOffset(Size_t);
  // Available size from sequence difference 
  virtual Size_t OffsetFromSeq(const Seq&, const Seq&);
  virtual Data* CopyFromOffset(Size_t, Size_t);  // Size, offset, ret pointer
  // Copy data, size, offset specified by sequence difference
  virtual Data* CopyFromSeq(Size_t, const Seq&, const Seq&);
  PDU*   Copy() const;          // Create a copy of this header
  PDU*   CopyS(Size_t);         // Copy with new size
  PDU*   CopySD(Size_t, char*); // Copy with new size, new data
  Layer_t Layer() { return 5;}  // Data is at layer 5
  

  // Checksum management
  // Since we may or may not have actual data for this pdu, we
  // provide a way to specify what the checksum is.  When calling
  // the second checksum method, the specified value is returned
  // if there is no data, otherwise the checksum is calculated from
  // the actual data.
  void   Checksum(Word_t);      // Set checksum to specified value
  Word_t Checksum();            // Calculate checksum (or use preset value)
  virtual char* Contents() const { return data;}
public:
  Size_t size;        // Number of data bytes
  char* data;         // Corresponding data (may be null)
  // The next two fields allow simulated applications to exchange some info
  Size_t msgSize;     // Total size of message
  Size_t responseSize;// Size of response requested
  Word_t checksum;    // Checksum value
};


#endif

