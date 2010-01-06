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



// Georgia Tech Network Simulator - Packet/PDU Serializaton
// George F. Riley.  Georgia Tech, Spring 2002

// Serialization interface

#include <iostream>
#include <string.h>

#include "G_debug.h"
#include "serializable.h"
#include "hex.h"
#include "packet.h"

using namespace std;

// Helpers

char* Serializable::SerializeToBuffer(char* b, Size_t& r, size_t& l)
{ // Serialize an unsigned long 
  CheckSize(r, sizeof(l));
  memcpy(b, &l, sizeof(l));
  return b + sizeof(l);
}

#if defined (Linux) || defined (SunOS) || defined (CYGWIN_NT_5_1)
char* Serializable::SerializeToBuffer(char* b, Size_t& r, unsigned long& l)
{ // Serialize an unsigned long
  CheckSize(r, sizeof(l));
  memcpy(b, &l, sizeof(l));
  return b + sizeof(l);
}
#endif

char* Serializable::SerializeToBuffer(char* b, Size_t& r, unsigned short& s)
{ // Serialize an unsigned short
  CheckSize(r, sizeof(s));
  memcpy(b, &s, sizeof(s));
  return b + sizeof(s);
}

char* Serializable::SerializeToBuffer(char* b, Size_t& r, float& f)
{ // Serialize a four byte float
  CheckSize(r, sizeof(f));
  memcpy(b, &f, sizeof(f));
  return b + sizeof(f);
}

char* Serializable::SerializeToBuffer(char* b, Size_t& r, double& d)
{ // Serialize an eight byte float
  CheckSize(r, sizeof(d));
  memcpy(b, &d, sizeof(d));
  return b + sizeof(d);
}

char* Serializable::SerializeToBuffer(char* b, Size_t& r, MACAddr& m)
{ // Serialize a 48 bit MAC Address
  MACAddr_t l;
  l = m.macAddr;
  CheckSize(r, sizeof(l));
  memcpy(b, &l, sizeof(l));
  return b + sizeof(l);
}

char* Serializable::SerializeToBuffer(char* b, Size_t& r, char& d)
{ // Serialize an eight byte float
  CheckSize(r, sizeof(d));
  memcpy(b, &d, sizeof(d));
  return b + sizeof(d);
}

char* Serializable::ConstructFromBuffer(char* b, Size_t& r, size_t& s)
{
  CheckSize(r, sizeof(s));
  memcpy(&s, b, sizeof(s));
  return b + sizeof(s);
}

#if defined (Linux) || defined (SunOS) || defined (CYGWIN_NT_5_1)
char*  Serializable::ConstructFromBuffer(char* b, Size_t& r, unsigned long& l)
{
  CheckSize(r, sizeof(l));
  memcpy(&l, b, sizeof(l));
  return b + sizeof(l);
}
#endif

char*  Serializable::ConstructFromBuffer(char* b, Size_t& r, unsigned short& s)
{
  CheckSize(r, sizeof(s));
  memcpy(&s, b, sizeof(s));
  return b + sizeof(s);
}

char*  Serializable::ConstructFromBuffer(char* b, Size_t& r, float& f)
{
  CheckSize(r, sizeof(f));
  memcpy(&f, b, sizeof(f));
  return b + sizeof(f);
}

char*  Serializable::ConstructFromBuffer(char* b, Size_t& r, double& d)
{
  CheckSize(r, sizeof(d));
  memcpy(&d, b, sizeof(d));
  return b + sizeof(d);
}

char*  Serializable::ConstructFromBuffer(char* b, Size_t& r, MACAddr& m)
{
  MACAddr_t l;
  CheckSize(r, sizeof(l));
  memcpy(&l, b, sizeof(l));
  m.macAddr = l;
  return b + sizeof(l);
}

char*  Serializable::ConstructFromBuffer(char* b, Size_t& r, char& l)
{
  CheckSize(r, sizeof(l));
  memcpy(&l, b, sizeof(l));
  return b + sizeof(l);
}

char* Serializable::GetSize(char* b, Size_t& r, Size_t& s)
{ // Get the size of the next size field
  CheckSize(r, sizeof(s));
  memcpy(&s, b, sizeof(s));
  return b + sizeof(s);
}

char* Serializable::PutSize(char* b, Size_t& r, Size_t& s)
{
  DEBUG0((cout << "Putting size " << Hex8n(s) << endl));
  Size_t ss = s;
  CheckSize(r, sizeof(s));
  memcpy(b, &ss, sizeof(ss));
  return b + sizeof(s);
}


// Private methods
void Serializable::CheckSize(Size_t& r, Size_t t)
{ // r = remaining, t = thissize
  if (t > r)
    {
      cout << "Serialization error; remaining " << r
           << " thissize " << t << endl;
      exit(1);
    }
  r -= t; // Reduce remaining for next time
}

// Static members

char* Serializable::Dump(char* b, Size_t s)
{ // Dump the serialized buffer for debugging
  cout << "Dumping " << s << " bytes" << endl;
  for (Size_t t = 0; t < s; ++t)
    {
      cout << Hex2n((unsigned char)b[t]);
      if (((t+1)%16 == 0)) cout << endl;
      else                 cout << " ";
    }
  if (s % 16) cout << endl;
  return b;
}

void Serializable::Validate(Packet* p)
{
  Size_t sz = p->SSize() + sizeof(Size_t); // Include size for length hdr
  char*  b = new char[sz];                 // Allocate serialization buffer
  char*  sb = b;                           // Save for reconstruction
  Size_t ssz = sz;                         // Save for reconstruction
  b = Serializable::PutSize(b, sz, sz);    // Store the total length
  // Copy to serialization buffer
  p->Serialize(b, sz);
  if (sz)
    cout << "Serializeable::Validate, expected non, got " << sz << endl;

  // Now reconstruct
  Size_t nsz = 0;
  Size_t sssz = ssz;
  char* nb = Serializable::GetSize(sb, sssz, nsz);
  Packet* np = new Packet(nb, sssz);

  // And serialize again
  Size_t sz1 = np->SSize() + sizeof(Size_t);  // Include size for length hdr
  char*  b1 = new char[sz];                  // Allocate serialization buffer
  char*  sb1 = b1;                           // Save for reconstruction
  Size_t ssz1 = sz1;                         // Save for reconstruction
  if (ssz1 != ssz)
    { // Oops, not same size
      cout << "Serializeable::Validate, expected identical sizes, got "
           << ssz1 << " " << ssz << endl;
    }
  b = Serializable::PutSize(b1, sz1, sz1);   // Store the total length
  // Copy to serialization buffer
  np->Serialize(b1, sz1);
  if (sz1)
    cout << "Serializeable::Validate, expected zero, got " << sz1 << endl;

  // At this point sb and sb1 buffers should be identical
  if (!memcmp(sb, sb1, ssz1))
    { // Oops, no match
      cout << "Serializable::Validate buffer mismatch " << endl;
      Serializable::Dump(sb,ssz);   
      Serializable::Dump(sb1,ssz1); 
    }
  delete [] sb;
  delete [] sb1;
}
