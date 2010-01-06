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

// Georgia Tech Network Simulator - UDP Base Class
// George F. Riley.  Georgia Tech, Spring 2002

// Base class for all UDP variants

#include <iostream>

// Uncomment below to enable debug level 0
// #define DEBUG_MASK 0x01
#define DEBUG_MASK 0x02
#include "G_debug.h"
#include "udp.h"
#include "application.h"
#include "datapdu.h"
//#include "portdemux.h"
#include "hex.h"
//#include "simulator.h"

using namespace std;

// UDPHeader methods

UDPHeader::UDPHeader()
  : sourcePort(0), destPort(0),
    messageLength(0), checksum(0), fid(0), seq(0)
{
  UDP::totalUDP++;
}

UDPHeader::UDPHeader(PortId_t s, PortId_t d, Word_t m, Word_t c)
  : sourcePort(s), destPort(d),
    messageLength(m), checksum(c), fid(0), seq(0)
{
  UDP::totalUDP++;
}

UDPHeader::UDPHeader(PortId_t s, PortId_t d, Word_t m, Word_t c, FId_t f)
  : sourcePort(s), destPort(d),
    messageLength(m), checksum(c), fid(f), seq(0)
{
  UDP::totalUDP++;
}

UDPHeader::UDPHeader(char* b, Size_t& sz, Packet* p)
{
  cout << "Constructing udp, initial size " << sz << endl;
  Size_t s = 0;
  b = Serializable::GetSize(b, sz, s);
  cout << "l4pdu size is " << s << " (" << Hex8(s) << ") " << endl;
  sz -= s;
  b = Construct(b, s);
  cout << "Constructing udp, final  size " << sz << endl;
  p->PushPDUBottom(this);  // Add to packet
  if (sz)
    { // Remaining size, must be data
      new Data(b, sz, p);
    }
}

UDPHeader::UDPHeader(const UDPHeader& c)
  :  sourcePort(c.sourcePort),
     destPort(c.destPort),
     messageLength(c.messageLength),
     checksum(c.checksum),
     fid(c.fid),
     seq(c.seq)
{
  UDP::totalUDP++;
}

UDPHeader::~UDPHeader()
{
  UDP::totalUDPDel++;
}

Size_t UDPHeader::Size() const
{
  return 8;
}


PDU*   UDPHeader::Copy() const
{
  return new UDPHeader(*this);
}

void   UDPHeader::Trace(Tfstream& tos, Bitmap_t b, Packet* p, const char* s)
{ // Trace the contents of this pdu
  tos << " ";
  if (s)
    tos << s;
  tos << "L4-UDP";
  if (Detail(UDP::SOURCE_PORT, b))     tos << " " << sourcePort;
  if (Detail(UDP::DEST_PORT, b))       tos << " " << destPort;
  if (Detail(UDP::MSG_LENGTH, b))      tos << " " << messageLength;
  if (Detail(UDP::CHECKSUM, b))        tos << " " << Hex4(checksum);
  if (Detail(UDP::FID, b))             tos << " " << fid;
  if (Detail(UDP::SEQ, b))             tos << " " << seq;
}

// Serialization
Size_t UDPHeader::SSize()
{ // Size needed for serialization
  return
    sizeof(sourcePort) + 
    sizeof(destPort) + 
    sizeof(messageLength) + 
    sizeof(checksum) + 
    sizeof(fid) + 
    sizeof(seq);
}

char*  UDPHeader::Serialize(char* b, Size_t& sz)
{ // Serialize to a buffer
  b = SerializeToBuffer(b, sz, sourcePort);
  b = SerializeToBuffer(b, sz, destPort);
  b = SerializeToBuffer(b, sz, messageLength);
  b = SerializeToBuffer(b, sz, checksum);
  b = SerializeToBuffer(b, sz, fid);
  b = SerializeToBuffer(b, sz, seq);
  return b;
}

char*  UDPHeader::Construct(char* b, Size_t& sz)
{ // Construct from buffer
  b = ConstructFromBuffer(b, sz, sourcePort);
  b = ConstructFromBuffer(b, sz, destPort);
  b = ConstructFromBuffer(b, sz, messageLength);
  b = ConstructFromBuffer(b, sz, checksum);
  b = ConstructFromBuffer(b, sz, fid);
  b = ConstructFromBuffer(b, sz, seq);
  return b;
}


// UDPPending Methods - Manage the pending data queue
UDPPending::UDPPending() 
  : data(nil), ip(IPADDR_NONE), port(NO_PORT), sent(0)
{
}

UDPPending::UDPPending(Data* d, IPAddr_t i, PortId_t p) 
    : data(d), ip(i), port(p), sent(0)
{
}

UDPPending::UDPPending(const UDPPending& c) // Copy constructor
    : data(c.data), ip(c.ip), port(c.port), sent(c.sent)
{
  DEBUG(0,(cout << "Hello from UDPPending cctor" << endl));
}

UDPPending::~UDPPending()
{
  DEBUG(0,(cout << "Hello from UDPPending dtor" << endl));
  // Data is specifically not deleted...users of UDPPending must do this
}



// UDP methods

// Static members
Size_t UDP::defaultPacketSize = 512;
Count_t UDP::totalUDP = 0;
Count_t UDP::totalUDPDel = 0;

UDP::UDP() : L4Protocol(), pktSize(defaultPacketSize), seq(0),
             pendingData(nil)
{
  DetailOff(CHECKSUM); // By default, don't trace checksum
  DetailOff(SEQ);      // By default, don't trace seq
}

UDP::UDP(Node* n) : L4Protocol(n), pktSize(defaultPacketSize), seq(0),
                    pendingData(nil)
{
  DetailOff(CHECKSUM); // By default, don't trace checksum
  DetailOff(SEQ);      // By default, don't trace seq
}

UDP::UDP(const UDP& c) 
    : L4Protocol(c), pktSize(c.PacketSize()), seq(c.seq), pendingData(nil)
{
  DEBUG0((cout << "UDP CopyConst " << this
          << " localApp " << localApplication << endl));
}

UDP::~UDP()
{
  if (localPort != NO_PORT)
    { // Remove binding
      Unbind(ProtoNumber, localPort);
    }
}

// Notifyer Methods
void UDP::Notify(void*)
{ // We have been notified by the interface that queue space is available
  DEBUG(0,(cout << "Hello from UDP::Notify" << endl));
  SendPending();
}

void UDP::DataIndication(Node* n, Packet* p, IPAddr_t ip, Interface*)
{ // From lower layer
  UDPHeader* h = (UDPHeader*)p->PopPDU();
  if (localNode)
    { // Should always exist, but just be sure
      Trace::Instance()->NewNode(localNode);
      localNode->TracePDU(this, h, p, "+");
    }

  if (localApplication)
    { // Application exists, pass the packet up the stack
      localApplication->Receive(p, this, h->seq );
    }
  else
    { // End of the line, delete the packet
      delete p;
    }
}

// L4Protocol methods
NCount_t  UDP::Send(Size_t s)            // Send empty data
{
  if (peerIP == IPADDR_NONE)
    { // Oops, no connect peer, can't do this
      return -1; 
    }
  DEBUG0((cout << "UDP Sending " << s << " bytes" << endl));
  pendingData.push_back(UDPPending(new Data(s), peerIP, peerPort));
  SendPending();
  return s;
}

NCount_t  UDP::Send(char* d, Size_t s)   // Send with data
{
  if (peerIP == IPADDR_NONE)
    { // Oops, no connect peer, can't do this
      return -1; 
    }
  DEBUG0((cout << "UDP Sending " << s << " bytes" << endl));
  pendingData.push_back(UDPPending(new Data(s,d), peerIP, peerPort));
  SendPending();
  return s;
}

NCount_t UDP::Send(Data& d)
{
  if (peerIP == IPADDR_NONE)
    { // Oops, no connect peer, can't do this
      return -1; 
    }
  DEBUG0((cout << "UDP Sending " << d.Size() << " bytes" << endl));
  Size_t s = d.Size();
  pendingData.push_back(UDPPending((Data*)d.Copy(), peerIP, peerPort));
  SendPending();
  return s;
}

NCount_t  UDP::SendTo(Size_t s, IPAddr_t ip, PortId_t port)
{
  pendingData.push_back(UDPPending(new Data(s), ip, port));
  SendPending();
  return s;
}

NCount_t  UDP::SendTo(char* d, Size_t s, IPAddr_t ip, PortId_t port)
{
  pendingData.push_back(UDPPending(new Data(s,d), ip, port));
  SendPending();
  return s;
}

NCount_t  UDP::SendTo(Data& d, IPAddr_t ip, PortId_t port)
{
  Size_t s = d.Size();
  pendingData.push_back(UDPPending((Data*)d.Copy(), ip, port));
  SendPending();
  return s;
}

bool        UDP::Connect(IPAddr_t ip, PortId_t p)
{ // Connect to peer.  For UDP just update remote ip/port
  DEBUG0((cout << "UDP Connecting to peer " << (string)IPAddr(ip)
          << " port " << p << endl));
  peerIP = ip;
  peerPort =  p;
  if (localApplication) localApplication->ConnectionComplete(this);
  return true;  // Success
}

bool        UDP::Close()
{
  return true; // UDP Close, no action needed
}

L4Protocol* UDP::Copy() const
{
  return new UDP(*this);
}

Proto_t     UDP::ProtocolNumber()
{ // Return UDP protocol number
  return ProtoNumber;
}

bool        UDP::IsTCP()
{
  return false;
}

void        UDP::PacketSize(Size_t s)
{ // Set pkt size for this instance
  pktSize = s;
}

// Private methods

NCount_t UDP::Send(Packet* p, IPAddr_t ip, PortId_t port)
{
  UDPHeader* udp = new UDPHeader(localPort, port, p->Size(), 0);
  udp->seq = seq++;  // Set sequence number
  p->PushPDU(udp);   // Push the UDP header on the packet header list
  // Trace this PDU
  if (localNode)
    { // Should always exist, but just be sure
      // Call the callbacks first, since callback might delete
      if (!localNode->CallCallbacks(Layer(), Proto(), PacketCallbacks::TX, p))
        return 0;// Callback deleted the packet
      // Now trace
      Trace::Instance()->NewNode(localNode);
      localNode->TracePDU(this, udp, p, "-");
    }
  // Pass to layer 3 protocol
  IPV4ReqInfo r(ip, IPADDR_NONE, ttl, ProtoNumber, tos); // Info for l3
  DEBUG0((cout << "UDP::Send to ip " << (string)IPAddr(ip)
          << " port " << port << endl));
  if (extraTxDelay != 0)
    { // Need to schedule an event for later
      Scheduler::Schedule(
          new L4DelayedTxEvent(l3Proto, localNode,
                               p, r,
                               udp->messageLength,
                               true),
          extraTxDelay, this);
    }
  else
    {
      l3Proto->DataRequest(localNode, p, &r);
      if (localApplication)
        { // Application exists, notify data sent
          localApplication->Sent(udp->messageLength, this);
        }
    }
  return udp->messageLength;
}

void UDP::SendPending()
{ // Send pending data (as much as possible)
  while(pendingData.size())
    { // While there is pending data
      DEBUG(0,(cout << "SendPending, pdsize " << pendingData.size() << endl));
      Data*  d = nil;
      Size_t s = 0;
      UDPPending& pd = pendingData.front(); // Get earliest pending 
      Size_t t = pd.data->Size();           // Total bytes in data pdu
      DEBUG(0,(cout << "SendPending, frontsize " << t 
               << "  frontsent " << pd.sent << endl));
      if (t <= PacketSize() && pd.sent == 0)
        { // Special case for efficiency, just use the pointer
          d = pd.data;
          pd.data = nil; // Do this so it's not deleted later
          s = d->Size();
        }
      else
        { // Calculate remaining size, bounded by packet size
          s = min(PacketSize(), pd.data->SizeFromOffset(pd.sent));
          d = new Data(s);
          DEBUG(0,(cout << "SendPending, minsize " << s 
                   << " pktsize " << PacketSize()
                   << " sfo " << pd.data->SizeFromOffset(pd.sent)
                   << endl));
        }
      if (BufferAvailable(s))
        { // Buffer space available on output link, process this
          Trace::Instance()->AppendEOL(); // Each on separate line
          Packet* p = NewPacket();      // Create a new packet
          if (pd.data)
            d = pd.data->CopyFromOffset(s, pd.sent);
          p->PushPDU(d);                 // Add the data
    
          DEBUG0((cout << "Advancing sent by " << s << endl));
          pd.sent += s;
          DEBUG0((cout << "UDP NewPacket, notif " << p->notification << endl));
          
          // delete deq entry if finished...code later
          if (pd.sent == t)
            { 
              if (pd.data)
                {
                  delete pd.data; // Delete data if still there
                  pd.data = nil;
                }
              pendingData.pop_front();     // Remove from deque
            }
          Send(p, pd.ip, pd.port);       // Send the packet
        }
      else
        { // Ask for notification when buffer space becomes available
          DEBUG(0,(cout << "SendPending, NBS" << endl));
          if(!pd.data) pd.data = d;       // Put back the data pointer
          RequestNotification(this, nil); // Notify when space available
          break;
        }
    }
}


// Static methods
void UDP::DefaultPacketSize(Size_t s)
{
  defaultPacketSize = s;
}
