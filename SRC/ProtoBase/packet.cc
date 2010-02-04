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

#ifdef HAVE_QT
#include <qcolor.h>
#endif
// Uncomment below to enable debug level 0
// #define DEBUG_MASK 0x01

//#define DEBUG_MASK 0x01
//#define DEBUG_MASK 0x08
#include "G_debug.h"
#include "packet.h"
#include "trace.h"
#include "simulator.h"
#include "G_globalstats.h"
#include "hex.h"
#include "l2proto802.3.h"
#include "l3protocol.h" 
//#include "mpi.h" v1.01
#include "routing-nixvector.h"

using namespace std;

Count_t Packet::nextUid = 1; // Unique id's for packets

Packet::Packet() : top(0),  retx(0), size(0), nixVec(nil), expire(0), notification(nil)
{
  top = 0; // Initialize the top of stack pointer
  uid = nextUid++;
  time = Simulator::Now();
  PDUs.reserve(4);                // Room for Data, TCP, IP and L2
  DBG((Stats::pktsAllocated++));
}

Packet::Packet(const Packet& p) :  retx(p.retx), uid(p.uid), size(0),
                                  time(p.time),
                                  nixVec(nil),
                                  expire(p.expire),
                                  notification(p.notification)
{ // Make a copy of the PDU header list
  if (p.nixVec) nixVec = (NixVectorOption*)p.nixVec->Copy();
  top = 0;
  PDUs.reserve(p.PDUs.size());
  //for (PDUVec_t::size_type i = 0; i < p.top; ++i)
  for (PDUVec_t::size_type i = 0; i < p.PDUs.size(); ++i)
    {
      PushPDU(p.PDUs[i]->Copy());
    }
  top = p.top;
  // Set size, since the above loop does ALL PDUs, even those already popped
  size = p.size;
  DBG((Stats::pktsAllocated++));
}

Packet::Packet(char* b, Size_t sz) : nixVec(nil), notification(nil)
{ // Construct from serialized buffer
  DEBUG(1,(cout << "Contructing packet, sz " << sz << endl));
  Construct(b, sz);
  DBG((Stats::pktsAllocated++));
}

Packet::~Packet()
{
  // Delete each pdu header
  for (PDUVec_t::const_iterator i = PDUs.begin(); i != PDUs.end(); ++i)
    {
      delete *i;
    }
  if (nixVec) delete nixVec;      // Delete nixvector if present
  Trace::Instance()->AppendEOL(); // End of trace line when pkt deleted
  DBG((Stats::pktsDeleted++));
}


Size_t Packet::SizeUpdate()
{
  size = 0;
  for (PDUVec_t::size_type i = 0; i < top; ++i)
    size += PDUs[i]->Size();
  return size;
}


#ifdef MOVED_TO_INLINE
Size_t Packet::Size()
{ // Return total size of all PDU's
  return size;
}

void Packet::PushPDU(PDU* pdu) // Add a new PDU header
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
#endif

void Packet::PushPDUBottom(PDU* pdu) // Add a new PDU header to bottom
{ // During packet serialization, pdu's are serialized top to bottom.
  // This is to allow the L2 pdu "first", so we can recontruct the
  // pdu list "up" the stack.  However, this means we need to add
  // pdu's to the list starting with l2, which is normally the "top"
  // of the pdu stack.  This function allows building the pdu list
  // from top to bottom.
  if (PDUs.empty())
    { // Simplest case, just push and exit
      PushPDU(pdu);
    }
  else
    { // Non-empty, move existing up
      PDU* pd = PDUs[PDUs.size() - 1];
      PDUs.push_back(pd);
      for (PDUVec_t::size_type i = PDUs.size() - 1; i != 0; --i)
        {
          PDUs[i] = PDUs[i-1];
        }
      PDUs[0] = pdu; // Put this one on bottom
      size += pdu->Size();
      top++;
    }
}

void Packet::InsertPDU(PDU* pdu, NCount_t offset)
{
  if (offset >= (NCount_t)top) return; // Can't do it
  DEBUG0((cout << "Inserting before begin + " << top - offset - 1 << endl));
  PDUVec_t::iterator i = PDUs.begin() + top - offset - 1;
  PDUs.insert(i, pdu);
  if (offset >= 0) top++; // Inserted below current
  size += pdu->Size();
}
  
#ifdef MOVED_TO_INLINE
PDU* Packet::PopPDU()      // Get last PDU header
{
  if (top == 0) return NULL; // Nope, nothing there

  PDU* p =  PDUs[--top];      // Get the pointer
  size -= p->Size();          // Reduce size
  return p;
}

PDU* Packet::PeekPDU()     // Get last PDU header without removal
{
  if (top == 0) return NULL;  // Nope, nothing there
  return PDUs[top - 1];       // Return without removing
}

//change Count_t to NCount_t in order to PEEK forward or backward.
//type conversion for avoiding the warnings.
//but potentially have problems here!
//
PDU* Packet::PeekPDU(NCount_t offset)
{ // Peek at last + offset without removal
  if ((NCount_t)top <= offset) return NULL;  // Nope, nothing there
  return PDUs[top - offset - 1];   // Return without removing
}
#endif

PDU*    Packet::FindPDU(Layer_t l)
{ // Find PDU by layer
  for (PDUVec_t::size_type i = 0; i < top; ++i)
    {
      PDU* pdu= PDUs[i];
      if (pdu->Layer() == l) return pdu; // Found
    }
  return nil; // Not found
}

PDU*    Packet::FindPDU(Layer_t l, Proto_t proto)
{ // Find PDU by layer and protocol
  for (PDUVec_t::size_type i = 0; i < top; ++i)
    {
      PDU* pdu= PDUs[i];
      if ((pdu->Layer() == l) && (pdu->Proto() == proto)) return pdu; //Found
    }
  return nil; // Not found
}


Packet* Packet::Copy() const
{
  return new Packet(*this);
}

// Serialization

Size_t Packet::SSize()
{ // Size needed for serialization
  Size_t sz = sizeof(retx) + sizeof(top) + sizeof(size) + sizeof(uid)
    + sizeof(time) + sizeof(NixVectorOption*);
  if (nixVec)
    { // NixVector present, add size
      if (nixVec->Remaining())
        { // Only if not empty
          sz += nixVec->SSize() + sizeof(Size_t);
        }
    }
  // Add the size of each pdu
  for (PDUVec_t::size_type i = 0; i < top; ++i)
    {
      sz += PDUs[i]->SSize() + sizeof(Size_t);
    }
  return sz;
}

char*  Packet::Serialize(char* b, Size_t& sz)
{ // Serialize to a working buffer
  DEBUG(2,(cout << "Packet size during serialization " << Size() << endl));
  DEBUG(2,(cout << "Size before packet members " << sz << endl));
  b = SerializeToBuffer(b, sz, top);
  b = SerializeToBuffer(b, sz, retx);
  b = SerializeToBuffer(b, sz, uid);
  b = SerializeToBuffer(b, sz, size);
  b = SerializeToBuffer(b, sz, time);
  // Note we are serializing a pointer here, which normally is not
  // correct.  However, on reconstruction, this value is just used
  // to detect the presence of a NixVector.  If present, the nix
  // vector itself is constructed and a new pointer created
  NixVectorOption* nv = nixVec;
  if (nv)
    { // Serialize the nix vector
      if (nv->Remaining())
        { // Only if not all used
          b = SerializeToBuffer(b, sz, (unsigned long&)nv);
          Size_t s = nixVec->SSize();
          DEBUG(3,(cout << "Serializing NV, size " << s << endl));
          b = Serializable::PutSize(b, sz, s);
          sz -= s;
          b = nv->Serialize(b, s);
        }
      else
        { // Don't serialize an empty nixvector
          nv = nil;
        }
    }
  if (!nv) b = SerializeToBuffer(b, sz, (unsigned long&)nv);
  DEBUG(1,(cout << "Size after packet members " << sz << endl));
  // Now serialize the PDUs
  for (PDUVec_t::size_type i = top - 1; ; --i)
    {
      Size_t s = PDUs[i]->SSize() + sizeof(Size_t);
      DEBUG(2,(cout << "Serializing pdu size " << Hex2(s) << endl));
      b = Serializable::PutSize(b, sz, s);
      DEBUG(2,(cout << "Serializing pdu, s " << s << " sz " << sz << endl));
      sz -= (s-sizeof(Size_t));
      b = PDUs[i]->Serialize(b, s);
      DEBUG(2,(cout << "Size after pdu " << sz << endl));
      if (i == 0) break;
    }

  DEBUG(1,(cout << "Remaining size is " << Hex8(sz)
           << " (should be zero)" << endl));
  return b;
}

char*  Packet::Construct(char* b, Size_t& sz)
{ // Construct from serialized buffer
  b = ConstructFromBuffer(b, sz, top);
  b = ConstructFromBuffer(b, sz, retx);
  b = ConstructFromBuffer(b, sz, uid);
  b = ConstructFromBuffer(b, sz, size);
  b = ConstructFromBuffer(b, sz, time);
  unsigned long nvul;
  b = ConstructFromBuffer(b, sz, nvul);
  if (nvul)
    { // Nix vector is present, construct it
      DEBUG(3,(cout << "Constructing NixVector" << endl));
      Size_t thisSize  = 0;
      b = Serializable::GetSize(b, sz, thisSize);
      Size_t saveSize = thisSize;
      nixVec = new NixVectorOption(b, thisSize);
      b += saveSize;
      sz -= saveSize;
    }
  // For debugging, ignore the nixVector
  //if (nixVec) delete nixVec;
  //nixVec = nil;
  // End debugging
  DEBUG0((cout << "Packet::Construct, top " << top
       << " uid " << uid
       << " size " << size
          << " time " << time << endl));
  // Constructing each pdu done slightly differently, since we can't
  // know at this point which type each pdu is.
  // Since (at least at this point in GTNetS development), L2802-3
  // are the only packet types that can be sent simulator to simulator
  // we can start by assuming l2802-3 l2 pdu is first thing.  Each
  // pdu then constructs the next layer
  top = 0;  // we need to reset top here (why did we serialize it anyway?)
  size = 0; // also reset size, as it's calculated as pdu's are pushed
  //v1.01
  //if (Simulator::instance->supercomputerInterconnect)
  //  { // Using MPI packets
  //    new MPIHeader(b, sz, this);
  //  }
  //else
    { // Normal 802.3 packets
      new L2802_3Header(b, sz, this); // all we need here!
    }
  DEBUG(1,(cout << "Packet::Construct, residual size " << sz << endl));
  return b + sz;
}

Priority_t Packet::Priority()
{
  Priority_t pr = 0;
  NCount_t offset = 0;

  while (true)
    {
      PDU* pdu = PeekPDU(offset++);
      if (!pdu) return pr; // End of pdu's
      Priority_t thisPr = pdu->Priority();
      if (thisPr) return thisPr; // Found non-zero
    }
}

MACAddr Packet::GetSrcMac()
{ // Check for l2 pdu, and get the dst macaddr if so
  for (PDUVec_t::size_type i = 0; i < PDUs.size(); ++i)
    {
      PDU* pdu = PDUs[i];
      if (!pdu) return MACAddr(); // None found
      if (pdu->Layer() == 2 && pdu->SubLayer() == 0)
        {
          return ((L2PDU*)(pdu))->GetSrcMac();
        }
    }
  return MACAddr::NONE;
}

// Get the destination MAC
MACAddr Packet::GetDstMac()
{
  for (PDUVec_t::size_type i = 0; i < PDUs.size(); ++i)
    {
      PDU* pdu = PDUs[i];
      if (!pdu) return MACAddr(); // None found
      if (pdu->Layer() == 2 && pdu->SubLayer() == 0)
        {
          return ((L2PDU*)(pdu))->GetDstMac();
        }
    }
  return MACAddr::NONE;
}


// Get the source mac address from l2 pdu
IPAddr_t Packet::GetSrcIP()
{
  for (PDUVec_t::size_type i = 0; i < PDUs.size(); ++i)
    {
      PDU* pdu = PDUs[i];
      if (!pdu) return IPADDR_NONE; // None found
      if (pdu->Layer() == 3)
        {
          return ((L3PDU*)(pdu))->GetSrcIP();
        }
    }
  return IPADDR_NONE;
}

// Get the destination MAC
IPAddr_t Packet::GetDstIP()
{
  for (PDUVec_t::size_type i = 0; i < PDUs.size(); ++i)
    {
      PDU* pdu = PDUs[i];
      if (!pdu) return IPADDR_NONE; // None found
      if (pdu->Layer() == 3)
        {
          return ((L3PDU*)(pdu))->GetDstIP();
        }
    }
  return IPADDR_NONE;
}

// Debugging
void Packet::DBPrint()
{
  cout << "Packet size " << Size() << endl;
  cout << "Packet pdu stack size " << PDUs.size() << endl;
  cout << "Packet pdu stack top  " << top << endl;
  for (PDUVec_t::size_type i = 0; i < top; ++i)
    {
      PDU* pdu = PDUs[i];
      cout << "PDU " << i << " layer " << pdu->Layer()
           << " SubLayer " << pdu->SubLayer()
           << " Proto " << pdu->Proto()
           << " Version " << pdu->Version()
           << " size " << pdu->Size() << endl;
    }
}


// Definitions for the colored packet
ColoredPacket::ColoredPacket() : Packet(), color(nil)
{
}


ColoredPacket::ColoredPacket(const QColor& c)
  : Packet(),
#ifdef HAVE_QT
    color(new QColor(c))
#else
    color(nil)
#endif
{
}

ColoredPacket::ColoredPacket(const ColoredPacket& p)
    : Packet(p), color(nil)
{
#ifdef HAVE_QT
  if (p.Color()) color = new QColor(*p.Color());
#endif
}

ColoredPacket::~ColoredPacket()
{
  delete color;
}

Packet* ColoredPacket::Copy() const
{
  return new ColoredPacket(*this);
}

bool ColoredPacket::IsColored()
{
  return true;
}

Color_t ColoredPacket::R()
{
#ifdef HAVE_QT
  return color->red();
#else
  return 0;
#endif
}

Color_t ColoredPacket::G()
{
#ifdef HAVE_QT
  return color->green();
#else
  return 0;
#endif
}

Color_t ColoredPacket::B()
{
#ifdef HAVE_QT
  return color->blue();
#else
  return 0;
#endif
}

void    ColoredPacket::SetColor(const QColor& c)
{
#ifdef HAVE_QT
  if (color) delete color;
  color = new QColor(c);
#endif
}


QColor* ColoredPacket::Color() const
{
  return color;
}
