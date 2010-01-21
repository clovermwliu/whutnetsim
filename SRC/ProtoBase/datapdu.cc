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



// Georgia Tech Network Simulator - Data PDU Headers
// George F. Riley.  Georgia Tech, Spring 2002

#include <iostream>
#include <algorithm>

#include <string.h>

#include "G_debug.h"
#include "datapdu.h"
#include "packet.h"
#include "hex.h"

using namespace std;

// NullData Methods

Data::Data() : size(0), data(nil),
               msgSize(0), responseSize(0), checksum(0)
{
  DEBUG0((cout << "DataPDU() Constructor" << this << endl));
  DBG((Stats::dataPdusAllocated++));
}

Data::Data(Size_t s, char* d, Count_t msg, Count_t resp)
  : size(s), data(nil), msgSize(msg), responseSize(resp)
{ // Make a copy of the data
  if (d)
    {
      data = new char[s];
      memcpy(data, d, s);
    }
  DEBUG0((cout << "DataPDU(s,d) Constructor" << this << endl));
  DBG((Stats::dataPdusAllocated++));
}

Data::Data(const string& s) 
  : size(s.length() + 1), data(strdup(s.c_str())),
    msgSize(0), responseSize(0), checksum(0)
{
  DEBUG0((cout << "DataPDU(string) Constructor" << this << endl));
}

Data::Data(const Data& c)
  : size(c.Size()), data(0),
    msgSize(c.msgSize), responseSize(c.responseSize),
    checksum(c.checksum)
{ // Copy constructor
  DEBUG0((cout << "DataPDU() Copy Constructor" << this << endl));
  DBG((Stats::dataPdusAllocated++));
  if (c.Contents())
    { // Has data
      data = new char[Size()];
      memcpy(data, c.Contents(), Size());
    }
}

Data::Data(char* b, Size_t& sz, Packet* p)
  : data(0)
{ // Construct from serialized buffer
  DEBUG0((cout << "DataPDU() Construct from buffer" << this << endl));
    Size_t s = 0;
  b = Serializable::GetSize(b, sz, s);
  DEBUG0((cout << "data size is " << s << " (" << Hex8(s) << ") " << endl));
  sz -= s;
  b = Construct(b, s);
  p->PushPDUBottom(this);  // Add to packet
}

Data::~Data()
{ // destructor
  DEBUG0((cout << "DataPDU() Destructor " << this << endl));
  DBG((Stats::dataPdusDeleted++));
  if (data) delete [] data;
}

// Serialization
Size_t Data::SSize()
{ // Size needed for serialization
  Size_t r = sizeof(size) + sizeof(msgSize) + sizeof(responseSize);
  if (data) r += size; // if associated data
  return r;
}

char*  Data::Serialize(char* b, Size_t& sz)
{ // Serialize to a buffer
  b = SerializeToBuffer(b, sz, size);
  b = SerializeToBuffer(b, sz, msgSize);
  b = SerializeToBuffer(b, sz, responseSize);
  b = SerializeToBuffer(b, sz, checksum);
  DEBUG0((cout << "Serializing msgSize " << msgSize 
          << " responseSize " << responseSize << endl));
  if (data)
    { // Associated data
      DEBUG0((cout << "Serializing data, size " << size 
              << " sz " << sz << endl));
      memcpy(b, data, size);
      b += size;
      sz -= size;
    }
  return b;
}

char*  Data::Construct(char* b, Size_t& sz)
{ // Construct from buffer
  b = ConstructFromBuffer(b, sz, size);
  b = ConstructFromBuffer(b, sz, msgSize);
  b = ConstructFromBuffer(b, sz, responseSize);
  b = ConstructFromBuffer(b, sz, checksum);
  DEBUG0((cout << "Constructed msgSize " << msgSize 
          << " responseSize " << responseSize << endl));
  if (sz && sz  >= size)
    { // Still have remaining size, must be associated data
      data = new char[size];
      memcpy(data, b, size);
      b += size;
      sz -= size;
    }
  if (sz)
    {
      DEBUG0((cout << "HuH?  remaining size " << sz 
              << " after data construct" << endl));
      sz = 0; // ! Major hack for now till I get this debgged
    }
  return b;
}

PDU* Data::Copy() const
{
  return new Data(*this);
};

PDU* Data::CopyS(Size_t s)
{ // Copy, but with new size (assumes no associated data);
  return new Data(s, nil, msgSize, responseSize);
}

PDU* Data::CopySD(Size_t s, char* d)
{ // Copy, but with new size (assumes no associated data);
  return new Data(s, d, msgSize, responseSize);
}

void Data::Clear()
{ // Remove all pending data
  if (data) delete [] data; // Free memory if used
  data = nil;
  size = 0;
}

void Data::Add(Size_t s, const char* d)
{
  if (data)
    { // Data exists, realloc and copy
      char* n = new char[Size() + s];
      memcpy(n, data, Size());
      if (d)
        { // New data specified
          memcpy(n + Size(), d, s); // Append the new data
        }
      else
        {
          memset(n + Size(), 0, s); // Apend zeros
        }
      delete [] data;           // Delete the old data
      data = n;                 // Points to new one
    }
  else
    { // No existing data, see if new data
      if (d)
        {
          data = new char[s];
          memcpy(data, d, s);
        }
    }
  size += s;
}

void Data::Remove(Size_t s)
{
  Size_t r = s > Size() ? Size() : s;

  size -= r;          // Reduce size from current
  if (data)
    { // data actually exists, realloc and copy
      if (size)
        {
          char* d = new char[Size()];
          memcpy(d, data, Size());
          delete [] data;
          data = d;
        }
      else
        { // Zero size, so don't need the data anymore
          delete [] data;
          data = nil;
        }
    }
}

Size_t Data::SizeFromSeq(const Seq& f, const Seq& o)
{
  Size_t o1 = OffsetFromSeq(f,o); // Offset to start of unused data
  return SizeFromOffset(o1);      // Amount of data after offset
}

Size_t Data::SizeFromOffset(Size_t o)
{ // Find out how much data is available from offset
  if (o > size) return 0;     // No data at requested offset
  return size - o;            // Available data after offset
}

Size_t Data::OffsetFromSeq(const Seq& f, const Seq& o)
{ // f is the first sequence number in this data, o is offset sequence
  if (o < f) return 0; // HuH?  Shouldn't happen
  return o - f;
}

Data* Data::CopyFromOffset(Size_t s, Size_t o)
{ // Make a copy of data from starting position "o" for "s" bytes
  // Return nil if results in zero length data
  Size_t s1 = min(s, SizeFromOffset(o)); // Insure not beyond end of data
  if (s1 == 0)    return nil;   // No data requested
  if (data)
    { // Actual data exists, make copy and return it
      char* d1 = new char[s1];  // Allocate memory for the copy
      memcpy(d1, &data[o], s1); // Copy the data
      Data* d = new Data(s1, d1, msgSize, responseSize);  // Return copy
      return d;
    }
  else
    { // No actual data, just return non-data pdu of correct size
      return new Data(s1, nil, msgSize, responseSize);
    }
}

Data* Data::CopyFromSeq(Size_t s, const Seq& f, const Seq& o)
{
  Data* d = CopyFromOffset(s, OffsetFromSeq(f,o));
  return d;
}

// Checksum management
void Data::Checksum(Word_t ck)
{ // Set a checksum for data pdus with no actual data
  checksum = ck;
}

Word_t Data::Checksum()
{
  if (!data) return checksum; // Use specified value if no data
  // Calculate the checksum
  Word_t* pData = (Word_t*)data;
  // Calculate the checksum, using 16 bit xor
  Word_t  sum = 0;
  Size_t  s2 = size / 2;
  for (Size_t i = 0; i < s2; ++i)
    {
      sum ^= pData[i];
    }
  if (s2 * 2 < size)
    { // Odd size, xor in last word
      sum ^= (data[size-1] << 8);
    }
  return sum;
}

      
  
  
  



