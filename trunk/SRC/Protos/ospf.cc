#ifndef WIN32
#include <unistd.h>
#endif
#define  DEBUG_MASK		0x0000

#include "debug.h"
#include "simulator.h"
#include "node.h"
#include "interface.h"
#include "interface-basic.h"
#include "ospf.h"
#include "linkp2p.h"

using namespace std;

// OSPFLSAHeader methods
OSPFLSAHeader::OSPFLSAHeader (
  Bitmap_t option, 
  OSPFLSA_t type, 
  OSPFID_t lid, 
  OSPFRouter_t rid,
  OSPFLSSeq_t sequence
  )
  : age (0),
    option (option),
    type (type),
    lid (lid),
    rid (rid),
    sequence (sequence),
    length (Size ()),
    receive (0)
{
  DEBUG (0, (cout << "OSPFLSAHeader::OSPFLSAHeader" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created with args" << endl));
}

OSPFLSAHeader::OSPFLSAHeader (
  const OSPFLSAHeader& instance
  )
  : age (instance.age),
    option (instance.option),
    type (instance.type),
    lid (instance.lid),
    rid (instance.rid),
    sequence (instance.sequence),
    chksum (instance.chksum),
    length (instance.length),
    receive (instance.receive)
{
  DEBUG (0, (cout << "OSPFLSAHeader::OSPFLSAHeader" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created from " << &instance << endl));
}

Word_t OSPFLSAHeader::CheckSum (void)
{
  DEBUG (0, (cout << "OSPFLSAHeader::CheckSum" << endl));

  UINT32 sum = 0;
  sum += option << 8 + type;
  sum += lid >> 16 + lid & 0xffff;
  sum += rid >> 16 + rid & 0xffff;
  sum += sequence >> 16 + sequence & 0xffff;
  sum += length;
  return (sum & 0xffff);
}

int OSPFLSAHeader::Compare (
  OSPFLSAHeader* lsa
  )
{
  DEBUG (0, (cout << "OSPFLSAHeader::Compare" << endl));

  Word_t age1 = (age > OSPFMAXAGE) ? age : OSPFMAXAGE;
  Word_t age2 = (lsa->age > OSPFMAXAGE) ? lsa->age : OSPFMAXAGE;

  if (sequence > lsa->sequence) {
    return (1);
  }
  if (sequence < lsa->sequence) {
    return (-1);
  }
  if (chksum > lsa->chksum) {
    return (1);
  }
  if (chksum < lsa->chksum) {
    return (-1);
  }
  if ((age1 == OSPFMAXAGE) && (age2 != OSPFMAXAGE)) {
    return (1);
  }
  if ((age1 != OSPFMAXAGE) && (age2 == OSPFMAXAGE)) {
    return (-1);
  }
  if ((age1 + OSPFMAXAGEDIFF) < age2) {
    return (1);
  }
  if (age1 > (age2 + OSPFMAXAGEDIFF)) {
    return (-1);
  }
  return (0);
}

bool OSPFLSAHeader::IsDifferent (
  OSPFLSAHeader* lsa
  )
{
  DEBUG (0, (cout << "OSPFLSAHeader::IsDifferent" << endl));

  Word_t age1 = (age > OSPFMAXAGE) ? age : OSPFMAXAGE;
  Word_t age2 = (lsa->age > OSPFMAXAGE) ? lsa->age : OSPFMAXAGE;

  if (!IsIt (lsa) ||
    ((age1 != age2) && ((age1 == OSPFMAXAGE) || (age2 == OSPFMAXAGE))) ||
    (option != lsa->option) || (length != lsa->length))
  {
    return (true);
  }
  else {
    return (false);
  }
}

void OSPFLSAHeader::dump (void)
{
  cout << "LSAHEADER: ";
  cout << "OPT = " << option;
  cout << "; TYPE = " << type;
  cout << "; LID = " << (string)IPAddr (lid);
  cout << "; RID = " << (string)IPAddr (rid);
  cout << "; SEQ = " << hex << sequence << dec;
  cout << "; LEN = " << length;
  cout << "; SUM = " << hex << chksum << dec;
  cout << endl;
}

// OSPFTOS methods

// OSPFLink methods
OSPFLink::OSPFLink (
  Type_t type,
  OSPFID_t lid,
  Long_t data,
  OSPFCost_t metric
  )
  : type (type),
    lid (lid),  
    data (data),
    number (0),
    metric (metric)
{
  DEBUG (0, (cout << "OSPFLink::OSPFLink" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created with args" << endl));
}

OSPFLink::OSPFLink (
  const OSPFLink& instance
  )
  : type (instance.type),
    lid (instance.lid),  
    data (instance.data),
    number (instance.number),
    metric (instance.metric)
{
  DEBUG (0, (cout << "OSPFLink::OSPFLink" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created from " << &instance << endl));

  for (OSPFTOSsConstIt it = instance.toss.begin (); it != instance.toss.end (); it++) {
    OSPFTOS* tos = *it;
    OSPFTOS* copy = tos->Copy ();
    toss.push_back (copy);
  }
}

OSPFLink::~OSPFLink (void)
{
  DEBUG (0, (cout << "OSPFLink::~OSPFLink" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " Destroyed" << endl));

  for (OSPFTOSsIt it = toss.begin (); it != toss.end (); it++) {
    OSPFTOS* tos = *it;
    delete tos;
  }
  toss.clear ();
}

bool OSPFLink::IsDifferent (
  OSPFLink* link
  )
{
  DEBUG (0, (cout << "OSPFLink::IsDifferent" << endl));

  if ((type != link->type) ||
    (lid != link->lid) ||
    (data != link->data) ||
    (number != link->number) ||
    (metric != link->metric))
  {
    return (true);
  }
  OSPFTOSsIt it1 = toss.begin ();
  OSPFTOSsIt it2 = link->toss.begin (); 
  for (int i = 0; i < number; i++, it1++, it2++) {
    OSPFTOS* tos1 = *it1;
    OSPFTOS* tos2 = *it2;
    if (tos1->IsDifferent (tos2)) {
      return (true);
    }
  }
  return (false);
}

void OSPFLink::dump (void)
{
  cout << type;
  cout << "/" << (string)IPAddr (lid);
  cout << "/" << (string)IPAddr (data);
  cout << "/" << metric;
}

// OSPFLSARouter methods
OSPFLSARouter::OSPFLSARouter (
  OSPFArea* area,
  OSPFLSSeq_t sequence
  )
  : OSPFLSAHeader (area->GetOption (), OSPFROUTER, area->GetRouterID (), area->GetRouterID (), sequence),
    veb (area->GetRouterType ()),
    number (0)
{
  DEBUG (0, (cout << "OSPFLSARouter::OSPFLSARouter" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created with args" << endl));

  length += 4;
  AddLinks (area);
  chksum = CheckSum ();
}

OSPFLSARouter::OSPFLSARouter (
  const OSPFLSARouter& instance
  )
  : OSPFLSAHeader (instance),
    veb (instance.veb),
    number (instance.number)
{
  DEBUG (0, (cout << "OSPFLSARouter::OSPFLSARouter" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created from " << &instance << endl));

  for (OSPFLinksConstIt it = instance.links.begin (); it != instance.links.end (); it++) {
    OSPFLink* link = *it;
    OSPFLink* copy = link->Copy ();
    links.push_back (copy);
  }
}

OSPFLSARouter::~OSPFLSARouter (void)
{
  DEBUG (0, (cout << "OSPFLSARouter::~OSPFLSARouter" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " Destroyed" << endl));

  ClearLinks ();
}

void OSPFLSARouter::AddLinks (
  OSPFArea* area
  )
{
  DEBUG (0, (cout << "OSPFLSARouter::AddLinks" << endl));

  if (area != nil) {
    area->ClearMapping ();

    OSPFInterfaces& ifaces = area->GetInterfaces ();
    for (OSPFInterfacesIt it = ifaces.begin (); it != ifaces.end (); it++) {
      OSPFInterface* iface = *it;
      if (iface->IsDown ()) {
        continue;
      }
  
      OSPFLink* link;
      IPAddr_t addr = iface->GetAddr ();
      Mask_t mask = iface->GetMask ();
      OSPFCost_t cost = iface->GetCost ();
      if (iface->IsLoopback () && (addr != IPADDR_NONE)) {
        link = new OSPFLink (OSPFLink::STUB, addr, MASK_ALL, 0);
        area->AddMapping (iface->GetAddr ());
        AddLink (link);
        continue;
      }
  
      OSPFNeighbor* neighbor;
      switch (iface->GetType ()) {
        case OSPFP2P:
          if ((neighbor = iface->GetPeer ()) != nil) {
            if (neighbor->IsFull ()) {
              if (addr == IPADDR_NONE) {
// Do it later
// Unnumbered case
              }
              else {
                link = new OSPFLink (OSPFLink::P2P, neighbor->GetID (), addr, cost);
                AddLink (link);
              }
            }
            if (iface->IsP2P ()) {
              link = new OSPFLink (OSPFLink::STUB, neighbor->GetAddr (), MASK_ALL, cost);
              area->AddMapping (iface->GetAddr ());
              AddLink (link);
            }
          }
          break;
        case OSPFBROADCAST:
        case OSPFNBMA:
          if (iface->IsDR ()) {
            OSPFNeighbors& neighbors = iface->GetNeighbors ();
            for (OSPFNeighborsIt it = neighbors.begin (); it != neighbors.end (); it++) {
              neighbor = *it;
              if (neighbor->IsFull ()) {
                link = new OSPFLink (OSPFLink::TRANSIT, addr, addr, cost);
                AddLink (link);
                break;
              }
            }
          }
          else {
            if (iface->IsBDR () || iface->IsOther ()) {
              neighbor = iface->GetDesignated ();
              if (neighbor->IsFull ()) {
                link = new OSPFLink (OSPFLink::TRANSIT, addr, addr, cost);
                AddLink (link);
                break;
              }
            }
          }
          link = new OSPFLink (OSPFLink::STUB, addr & mask, mask, cost);
          area->AddMapping (iface->GetAddr ());
          AddLink (link);
          break;
        case OSPFVLINK:
        case OSPFP2M:
        default:
          break;
      }
    }
  }
}

void OSPFLSARouter::ClearLinks (void)
{
  DEBUG (0, (cout << "OSPFLSARouter::ClearLinks" << endl));

  for (OSPFLinksIt it = links.begin (); it != links.end (); it++) {
    OSPFLink* link = *it;
    delete link;
  }
  links.clear ();
}

Word_t OSPFLSARouter::CheckSum (void)
{
  DEBUG (0, (cout << "OSPFLSARouter::CheckSum" << endl));

  UINT32 sum = OSPFLSAHeader::CheckSum ();
  sum += veb + number;
  for (OSPFLinksIt it = links.begin (); it != links.end (); it++) {
    OSPFLink* link = *it;
    OSPFID_t lid = link->GetLinkID ();
    Long_t data = link->GetData ();
    sum += lid >> 16 + lid & 0xffff;
    sum += data >> 16 + data & 0xffff;
    sum += link->GetType () << 8;
    sum += (UINT32)link->GetCost ();
  }
  return (~(sum >> 16 + sum & 0xffff));
}

bool OSPFLSARouter::IsDifferent (
  OSPFLSAHeader* lsa
  )
{
  DEBUG (0, (cout << "OSPFLSARouter::IsDifferent" << endl));

  if (OSPFLSAHeader::IsDifferent (lsa) == true) {
    return (true);
  }
  if (length <= OSPFLSAHeader::Size ()) {
    return (false);
  }
  OSPFLSARouter* rlsa = (OSPFLSARouter*)lsa;
  if ((veb != rlsa->veb) || (number != rlsa->number)) {
    return (true);
  }
  OSPFLinksIt it1 = links.begin ();
  OSPFLinksIt it2 = rlsa->links.begin (); 
  for (Long_t i = 0; i < number; i++, it1++, it2++) {
    OSPFLink* link1 = *it1;
    OSPFLink* link2 = *it2;
    if (link1->IsDifferent (link2)) {
      return (true);
    }
  }
  return (false);
}

bool OSPFLSARouter::IsConnected (
  OSPFID_t vid
  )
{
  DEBUG (0, (cout << "OSPFLSARouter::IsConnected" << endl));

  for (OSPFLinksIt it = links.begin (); it != links.end (); it++) {
    OSPFLink* link = *it;
    if (link->GetLinkID () == vid) {
      return (true);
    }
  }
  return (false);
}

void OSPFLSARouter::dump (void)
{
  cout << "LSARouter: ";
  cout << "AGE = " << age;
  cout << "; OPT = " << option;
  cout << "; TYPE = " << type;
  cout << "; LID = " << (string)IPAddr (lid);
  cout << "; RID = " << (string)IPAddr (rid);
  cout << "; SEQ = " << hex << sequence << dec;
  cout << "; LEN = " << length;
  cout << "; NUM = " << number;
  cout << (string)IPAddr (rid);
  if (number != 0) {
    cout << "; LINK = ";
    for (OSPFLinksIt it = links.begin (); it != links.end (); it++) {
      OSPFLink* link = *it;
      link->dump ();
      cout << " ";
    }
  }
  cout << endl;
}

// OSPFLSANetwork methods

// OSPFLSASummary methods

// OSPFLSAASExt methods

// OSPFPacket methods
OSPFPacket::OSPFPacket (
  Type_t type,
  OSPFInterface* iface
  )
  : type (type), 
    length (OSPFPacket::Size ()),
    rid (iface->GetRouterID ()),
    aid (iface->GetAreaID ()),
    atype (iface->GetAType ())
{
  DEBUG (0, (cout << "OSPFPacket::OSPFPacket" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created with args" << endl));
}

OSPFPacket::OSPFPacket (
  const OSPFPacket& instance
  )
  : type (instance.type), 
    length (instance.length), 
    rid (instance.rid), 
    aid (instance.aid), 
    atype (instance.atype)
{
  DEBUG (0, (cout << "OSPFPacket::OSPFPacket" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created from " << &instance << endl));
}

bool OSPFPacket::CheckPacket (
  IPAddr_t src,
  IPAddr_t des,
  OSPFInterface* iface
  ) const
{
  DEBUG (0, (cout << "OSPFPacket::CheckPacket" << endl));

  if ((Layer () != OSPFLAYER) || (Proto () != OSPFPROTO) || (version != OSPFVERSION)) {
    return (false);
  }

  if (aid == iface->GetAreaID ()) {
    if (iface->GetType () != OSPFP2P) {
      IPAddr_t addr = iface->GetAddr ();
      Mask_t mask = iface->GetMask ();
      if ((src & mask) != (addr & mask)) {
        return (false);
      }
    }
  }
  else {
    if (aid == OSPFBACKBONE) {
// Do it alter
    }
    else {
      return (false);
    }
  }

//  if (des == OSPFALLDROUTERS) {
//    if (!iface->IsDR () && !iface->IsBDR ()) {
//      return (false);
//    }
//  }

  if (atype != iface->GetAType ()) {
    return (false);
  }

// Do it later
// Authentication check
  
  return (true);
}

// OSPFHelloHead methods
OSPFHello::OSPFHello (
  OSPFInterface* iface
  )
  : OSPFPacket (OSPFPacket::HELLO, iface),
    helloInterval (iface->GetHelloInterval ()),
    option (iface->GetOption ()),
    priority (iface->GetPriority ()),
    deadInterval (iface->GetDeadInterval ()),
    designated (iface->GetDR ()),
    backup (iface->GetBDR ())
{
  DEBUG (0, (cout << "OSPFHello::OSPFHello" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created with args" << endl));

  switch (iface->GetType ()) {
    case OSPFP2P:
    case OSPFVLINK:
      mask = IPADDR_NONE; 
      break;
    default:
      mask = iface->GetMask ();
      break;
  }

  length += 20;
  OSPFNeighbors& nbrs = iface->GetNeighbors ();
  for (OSPFNeighborsIt it = nbrs.begin (); it != nbrs.end (); it++) {
    OSPFNeighbor* neighbor = *it;
    if (!neighbor->IsDown ()) {
      neighbors.push_back (neighbor->GetID ());
      length += sizeof (OSPFRouter_t);
    }
  }
}

OSPFHello::OSPFHello (
  const OSPFHello& instance
  )
  : OSPFPacket (instance),
    mask (instance.mask),
    helloInterval (instance.helloInterval),
    option (instance.option),
    priority (instance.priority),
    deadInterval (instance.deadInterval),
    designated (instance.designated),
    backup (instance.backup),
    neighbors (instance.neighbors)
{
  DEBUG (0, (cout << "OSPFHello::OSPFHello" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created from " << &instance << endl));
}

OSPFHello::~OSPFHello (void)
{
  DEBUG (0, (cout << "OSPFHello::~OSPFHello" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " Destroyed" << endl));

  neighbors.clear ();
}

bool OSPFHello::CheckPacket (
  OSPFInterface* iface
  ) const
{
  DEBUG (0, (cout << "OSPFHello::CheckPacket" << endl));

  OSPFNetwork_t itype = iface->GetType ();
  if (((itype != OSPFP2P) && (itype != OSPFVLINK)) && (mask != iface->GetMask ())) {
    return (false);
  }

  if ((helloInterval != iface->GetHelloInterval ()) || (deadInterval != iface->GetDeadInterval ())) {
    return (false);
  }

  if (((option & OSPFOPT_E) == 0) != iface->IsStub ()) {
    return (false);
  }

  return (true);
}

bool OSPFHello::FindRouter (
  OSPFRouter_t rid
  ) const
{
  DEBUG (0, (cout << "OSPFHello::FindRouter" << endl));

  return (find (neighbors.begin (), neighbors.end (), rid) != neighbors.end ());
}

void OSPFHello::dump (void)
{
  cout << "HELLO: ";
  cout << "LEN = " << length;
  cout << "; RID = " << (string)IPAddr (rid);
  cout << "; DR = " << (string)IPAddr (designated);
  cout << "; BDR = " << (string)IPAddr (backup);
  if (!neighbors.empty ()) {
    cout << "; NEI = ";
    for (OSPFRoutersIt it = neighbors.begin (); it != neighbors.end (); it++) {
      OSPFRouter_t router = *it;
      cout << (string)IPAddr (router) << " ";
    }
  }
  cout << endl;
}

// OSPFDataD methods
OSPFDataD::OSPFDataD (
  Seq_t sequence,
  OSPFInterface* iface
  )
  : OSPFPacket (OSPFPacket::DATAD, iface),
    mtu (iface->GetMTU ()),
    option (iface->GetOption ()),
    imms (IBIT | MBIT | MSBIT),
    sequence (sequence)
{
  DEBUG (0, (cout << "OSPFDataD::OSPFDataD" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created with args" << endl));

  length += 8;
}

OSPFDataD::OSPFDataD (
  OSPFNeighbor* neighbor 
  )
  : OSPFPacket (OSPFPacket::DATAD, neighbor->GetInterface ()),
    mtu (neighbor->GetMTU ()),
    option (neighbor->GetInterface ()->GetOption ()),
    sequence (neighbor->GetSequence ())
{
  DEBUG (0, (cout << "OSPFDataD::OSPFDataD" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created with args" << endl));

  length += 8;
  OSPFLSAHeaders& database = neighbor->GetDatabase ();
  OSPFLSAHeadersIt it = database.begin ();
  while (it != database.end ()) {
    OSPFLSAHeader* header = *it;
    if ((length + header->Size ()) <= mtu) {
      headers.push_back (header);
      length += header->Size ();
      it = database.erase (it);
    }
    else {
      break;
    }
  }

  imms = 0;
  if (!database.empty ()) {
    imms |= MBIT;
  }
  if (neighbor->IsMaster ()) {
    imms |= MSBIT;
  }
}

OSPFDataD::OSPFDataD (
  const OSPFDataD& instance
  )
  : OSPFPacket (instance),
    mtu (instance.mtu),
    option (instance.option),
    imms (instance.imms),
    sequence (instance.sequence)
{
  DEBUG (0, (cout << "OSPFDataD::OSPFDataD" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created from " << &instance << endl));

  for (OSPFLSAHeadersConstIt it = instance.headers.begin (); it != instance.headers.end (); it++) {
    OSPFLSAHeader* header = *it;
    OSPFLSAHeader* copy = header->OSPFLSAHeader::Copy ();
    headers.push_back (copy);
  }
}

OSPFDataD::~OSPFDataD (void)
{
  DEBUG (0, (cout << "OSPFDataD::~OSPFDataD" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " Destroyed" << endl));

  for (OSPFLSAHeadersIt it = headers.begin (); it != headers.end (); it++) {
    OSPFLSAHeader* header = *it;
    delete (header);
  }
  headers.clear ();
}

void OSPFDataD::dump (void)
{
  cout << "DATAD: ";
  cout << "SEQ = 0x" << hex << sequence << dec;
  if (imms & IBIT) {
    cout << "; INIT";
  }
  if (imms & MBIT) {
    cout << "; MORE";
  }
  if (imms & MSBIT) {
    cout << "; MASTER";
  }
  else {
    cout << "; SLAVE";
  }
  cout << "; LEN = " << length;
  if (!headers.empty ()) {
    cout << "; HEAD = ";
    for (OSPFLSAHeadersIt it = headers.begin (); it != headers.end (); it++) {
      OSPFLSAHeader* header = *it;
      cout << header->GetType () << "/";
      cout << (string)IPAddr (header->GetLinkID ()) << "/";
      cout << (string)IPAddr (header->GetAdvID ()) << " ";
    }
  }
  cout << endl;
}

//OSPFRequest methods

// OSPFLSReq methods
OSPFLSReq::OSPFLSReq (
  OSPFNeighbor* neighbor
  )
  : OSPFPacket (OSPFPacket::LSREQ, neighbor->GetInterface ())
{
  DEBUG (0, (cout << "OSPFLSReq::OSPFLSReq" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created with args" << endl));

  Size_t mtu = neighbor->GetMTU ();
  OSPFLSAHeaders& requested = neighbor->GetRequested ();
  for (OSPFLSAHeadersIt it = requested.begin (); it != requested.end (); it++) {
    OSPFLSAHeader* header = *it;
    if ((length + OSPFRequest::Size ()) > mtu) {
      break;
    }
    OSPFRequest* request = new OSPFRequest (header);
    requests.push_back (request);
    length += request->Size ();
  }
}

OSPFLSReq::OSPFLSReq (
  const OSPFLSReq& instance
  )
  : OSPFPacket (instance)
{
  DEBUG (0, (cout << "OSPFLSReq::OSPFLSReq" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created from " << &instance << endl));

  for (OSPFRequestsConstIt it = instance.requests.begin (); it != instance.requests.end (); it++) {
    OSPFRequest* request = *it;
    OSPFRequest* copy = request->Copy ();
    requests.push_back (copy);
  }
}

OSPFLSReq::~OSPFLSReq (void)
{
  DEBUG (0, (cout << "OSPFLSReq::~OSPFLSReq" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " destroyed" << endl));

  for (OSPFRequestsIt it = requests.begin (); it != requests.end (); it++) {
    OSPFRequest* request = *it;
    delete request;
  }
  requests.clear ();
}

void OSPFLSReq::dump (void)
{
  cout << "LSREQ: ";
  cout << "LEN = " << length;
  if (!requests.empty ()) {
    cout << "; REQ = ";
    for (OSPFRequestsIt it = requests.begin (); it != requests.end (); it++) {
      OSPFRequest* request = *it;
      cout << request->GetType () << "/";
      cout << (string)IPAddr (request->GetLinkID ()) << "/";
      cout << (string)IPAddr (request->GetAdvID ()) << " ";
    }
  }
  cout << endl;
}

// OSPFLSUpd methods
OSPFLSUpd::OSPFLSUpd (
  OSPFInterface* iface
  )
  : OSPFPacket (OSPFPacket::LSUPD, iface)
{
  DEBUG (0, (cout << "OSPFLSUpd::OSPFLSUpd" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created with args" << endl));

  length += 4;
}

OSPFLSUpd::OSPFLSUpd (
  const OSPFLSUpd& instance
  )
  : OSPFPacket (instance)
{
  DEBUG (0, (cout << "OSPFLSUpd::OSPFLSUpd" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created from " << &instance << endl));

  for (OSPFLSAHeadersConstIt it = instance.lsas.begin (); it != instance.lsas.end (); it++) {
    OSPFLSAHeader* lsa = *it;
    AddLSA (lsa, 0);
  }
}

OSPFLSUpd::~OSPFLSUpd (void)
{
  DEBUG (0, (cout << "OSPFLSUpd::~OSPFLSUpd" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " destroyed" << endl));

  for (OSPFLSAHeadersIt it = lsas.begin (); it != lsas.end (); it++) {
    OSPFLSAHeader* lsa = *it;
    delete lsa;
  }
  lsas.clear ();
}

void OSPFLSUpd::AddLSA (
  OSPFLSAHeader* lsa,
  Time_t delay
  )
{
  DEBUG (0, (cout << "OSPFLSUpd::AddLSA" << endl));

  if (lsa != nil) {
    OSPFLSAHeader* copy = lsa->Copy ();
    copy->IncAge ((Word_t)delay);
    lsas.push_back (copy);
    length += copy->Size ();
  }
}

void OSPFLSUpd::dump (void)
{
  cout << "LSUPD: ";
  cout << "LEN = " << length;
  if (!lsas.empty ()) {
    cout << "; LSA = ";
    for (OSPFLSAHeadersIt it = lsas.begin (); it != lsas.end (); it++) {
      OSPFLSAHeader* lsa = *it;
      cout << lsa->GetAge ();
      cout << "/" << lsa->GetType ();
      cout << "/" << (string)IPAddr (lsa->GetLinkID ());
      cout << "/" << (string)IPAddr (lsa->GetAdvID ());
      cout << " ";
    }
  }
  cout << endl;
}

// OSPFLSAck methods
OSPFLSAck::OSPFLSAck (
  OSPFInterface* iface
  )
  : OSPFPacket (OSPFPacket::LSACK, iface)
{
  DEBUG (0, (cout << "OSPFLSAck::OSPFLSAck" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created with args" << endl));
}

OSPFLSAck::OSPFLSAck (
  const OSPFLSAck& instance
  )
  : OSPFPacket (instance)
{
  DEBUG (0, (cout << "OSPFLSAck::OSPFLSAck" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created from " << &instance << endl));

  for (OSPFLSAHeadersConstIt it = instance.headers.begin (); it != instance.headers.end (); it++) {
    OSPFLSAHeader* header = *it;
    AddLSA (header);
  }
}

OSPFLSAck::~OSPFLSAck (void)
{
  DEBUG (0, (cout << "OSPFLSAck::~OSPFLSAck" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " destroyed" << endl));

  for (OSPFLSAHeadersIt it = headers.begin (); it != headers.end (); it++) {
    OSPFLSAHeader* header = *it;
    delete header;
  }
  headers.clear ();
}

void OSPFLSAck::AddLSA (
  OSPFLSAHeader* lsa
  )
{
  DEBUG (0, (cout << "OSPFLSAck::AddLSA" << endl));

  if (lsa != nil) {
    OSPFLSAHeader* copy = lsa->OSPFLSAHeader::Copy ();
    headers.push_back (copy);
    length += copy->Size ();
  }
}

void OSPFLSAck::dump (void)
{
  cout << "LSACK: ";
  cout << "LEN = " << length;
  if (!headers.empty ()) {
    cout << "; LSA = ";
    for (OSPFLSAHeadersIt it = headers.begin (); it != headers.end (); it++) {
      OSPFLSAHeader* header = *it;
      cout << header->GetAge ();
      cout << "/" << header->GetType ();
      cout << "/" << (string)IPAddr (header->GetLinkID ());
      cout << "/" << (string)IPAddr (header->GetAdvID ());
      cout << " ";
    }
  }
  cout << endl;
}

#ifdef WIN32
long int lrand48()
{
    return rand();
}
#endif

// OSPFRange methods

// OSPFLastDD methods

// OSPFNeighbor methods
OSPFNeighbor::OSPFNeighbor (
  OSPFRouter_t nid,
  Byte_t priority,
  IPAddr_t addr,
  Bitmap_t option,
  IPAddr_t designated,
  IPAddr_t backup,
  OSPFInterface* iface
  )
  : status (DOWN),
    inactiveEvent (nil),
    master (false),
    sequence (lrand48 ()),
    nid (nid),
    priority (priority),
    addr (addr),
    option (option),
    designated (designated), backup (backup),
    // Internal data structures
    iface (iface),
    ddrxmtEvent (nil),
    reqrxmtEvent (nil),
    updrxmtEvent (nil),
    last (nil)
{
  DEBUG (0, (cout << "OSPFNeighbor::OSPFNeighbor" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created with args" << endl));
}

OSPFNeighbor::~OSPFNeighbor (void)
{
  DEBUG (0, (cout << "OSPFNeighbor::~OSPFNeighbor" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " destroyed" << endl));

  ClearInactive ();
  ClearLSAs ();
  ClearDDRxmt ();
  ClearREQRxmt ();
  ClearUPDRxmt ();
  ClearLast ();
}

void OSPFNeighbor::Handle (
  Event* event, 
  Time_t time
  )
{
  DEBUG (0, (cout << "OSPFNeighbor::Handle" << endl));
  DEBUG (1, (cout << "\tEvent " << event->event << " at " << time << endl));

  HandleEvent ((Event_t)event->event);

  delete event;
}

void OSPFNeighbor::HandleEvent (
  Event_t event
  )
{
  DEBUG (0, (cout << "OSPFNeighbor::ChangeStatus" << endl));

  Status_t prev_status = status;
  switch (event) {
    case HELLORECEIVED:
      if ((status == DOWN) || (status == ATTEMPT)) {
        status = INIT;
      }
      TriggerInactive ();
      break;
    case START:
      if (status == DOWN) {
        status = ATTEMPT;
// Do it later
// For NBMA, send hello to this neighbor, start inactive timer
      }
      break;
    case TWOWAYRECEIVED:
      if (status == INIT) {
        if (iface->IsAdjacency (addr)) {
          status = EXSTART;
          sequence++;
          master = true;
          SendDataD ();
        }
        else {
          status = TWOWAY;
        }
      }
      break;
    case NEGOTIATIONDONE:
      if (status == EXSTART) {
        status = EXCHANGE;
        BuildDatabase ();
      }
      break;
    case EXCHANGEDONE:
      if (status == EXCHANGE) {
        if (requested.empty ()) {
          status = FULL;
        }
        else {
          status = LOADING;
          SendLSReq ();
        }
      }
      break;
    case LOADINGDONE:
      if (status == LOADING) {
        status = FULL;
      }
      break;
    case ADJOK:
      if (status == TWOWAY) {
        if (iface->IsAdjacency (addr)) {
          status = EXSTART;
          sequence++;
          master = true;
          SendDataD ();
        }
      }
      else {
        if (status >= EXSTART) {
          if (iface->IsAdjacency (addr) == false) {
            status = TWOWAY;
            ClearLSAs ();
          }
        }
      }
      break;
    case SEQMISMATCH:
    case BADLSREQ:
      if (status >= EXCHANGE) {
        status = EXSTART;
        ClearLSAs ();
        sequence++;
        master = true;
        SendDataD ();
      }
      break;
    case ONEWAYRECEIVED:
      if (status >= TWOWAY) {
        status = INIT;
        ClearLSAs ();
      }
      break;
    case KILLNBR:
    case INACTIVETIMER:
    case LLDOWN:
      status = DOWN;
      ClearLSAs ();
      ClearInactive ();
      break;
    case DDRXMTTIMER:
      ddrxmtEvent = nil;
      SendDDRxmt ();
      break;
    case REQRXMTTIMER:
      reqrxmtEvent = nil;
      SendREQRxmt ();
      break;
    case UPDRXMTTIMER:
      updrxmtEvent = nil;
      SendUPDRxmt ();
      break;
    default:
      break;
  }
  if (prev_status != status) {
//cout << "\t" << (string)IPAddr (iface->GetRouterID ()) << "'s " << (string)IPAddr (nid) << " status " << status << endl;
    if ((prev_status == FULL) || (status == FULL)) {
      OSPFArea* area = iface->GetArea ();
      area->FlushRouterLSA ();
    }
  }
}

Size_t OSPFNeighbor::GetMTU (void) const
{
  DEBUG (0, (cout << "OSPFNeighbor::GetMTU" << endl));

  return (iface->GetMTU ());
}

bool OSPFNeighbor::IsMaster (void) const
{ 
  DEBUG (0, (cout << "OSPFNeighbor::IsMaster" << endl));

  return (iface->GetRouterID () > nid); 
}

void OSPFNeighbor::ClearInactive (void)
{
  DEBUG (0, (cout << "OSPFNeighbor::ClearInactive" << endl));

  if (inactiveEvent != nil) {
    Scheduler::Cancel (inactiveEvent);
    delete inactiveEvent;
    inactiveEvent = nil;
  }
}

void OSPFNeighbor::TriggerInactive (void)
{
  DEBUG (0, (cout << "OSPFNeighbor::TriggerInactive" << endl));

  ClearInactive ();
  inactiveEvent = new Event (INACTIVETIMER);
  Scheduler::Schedule (inactiveEvent, iface->GetDeadInterval (), this);
}

void OSPFNeighbor::ClearLSAs (void)
{
  DEBUG (0, (cout << "OSPFNeighbor::ClearLSAs" << endl));

  for (OSPFLSAHeadersIt it = noacked.begin (); it != noacked.end (); it++) {
    OSPFLSAHeader* lsa = *it;
    delete lsa;
  }
  noacked.clear ();

  for (OSPFLSAHeadersIt it = database.begin (); it != database.end (); it++) {
    OSPFLSAHeader* header = *it;
    delete header;
  }
  database.clear ();

  for (OSPFLSAHeadersIt it = requested.begin (); it != requested.end (); it++) {
    OSPFLSAHeader* request = *it;
    delete request;
  }
  requested.clear ();
}

void OSPFNeighbor::BuildDatabase (void)
{
  DEBUG (0, (cout << "OSPFNeighbor::BuildDatabase" << endl));

  OSPFArea* area = iface->GetArea ();
  OSPFLSARouters& rlsas = area->GetRouterLSAs ();
  for (OSPFLSARoutersIt it = rlsas.begin (); it != rlsas.end (); it++) {
    OSPFLSAHeader* rlsa = *it;
    if (rlsa->GetAge () < OSPFMAXAGE) {
      OSPFLSAHeader* header = rlsa->OSPFLSAHeader::Copy ();
      database.push_back (header);
    }
    else {
      AddNoacked (rlsa);
    }
  }
}

void OSPFNeighbor::ClearDDRxmt (void)
{
  DEBUG (0, (cout << "OSPFNeighbor::ClearDDRxmt" << endl));

  if (ddrxmtEvent != nil) {
    Scheduler::Cancel (ddrxmtEvent);
    delete ddrxmtEvent;
    ddrxmtEvent = nil;
  }
}

void OSPFNeighbor::TriggerDDRxmt (void)
{
  DEBUG (0, (cout << "OSPFNeighbor::TriggerDDRxmt" << endl));

  ClearDDRxmt ();
  ddrxmtEvent = new Event (DDRXMTTIMER);
  Scheduler::Schedule (ddrxmtEvent, iface->GetRxmtInterval (), this);
}

void OSPFNeighbor::ClearREQRxmt (void)
{
  DEBUG (0, (cout << "OSPFNeighbor::ClearREQRxmt" << endl));

  if (reqrxmtEvent != nil) {
    Scheduler::Cancel (reqrxmtEvent);
    delete reqrxmtEvent;
    reqrxmtEvent = nil;
  }
}

void OSPFNeighbor::TriggerREQRxmt (void)
{
  DEBUG (0, (cout << "OSPFNeighbor::TriggerREQRxmt" << endl));

  ClearREQRxmt ();
  reqrxmtEvent = new Event (REQRXMTTIMER);
  Scheduler::Schedule (reqrxmtEvent, iface->GetRxmtInterval (), this);
}

void OSPFNeighbor::ClearUPDRxmt (void)
{
  DEBUG (0, (cout << "OSPFNeighbor::ClearUPDRxmt" << endl));

  if (updrxmtEvent != nil) {
    Scheduler::Cancel (updrxmtEvent);
    delete updrxmtEvent;
    updrxmtEvent = nil;
  }
}

void OSPFNeighbor::TriggerUPDRxmt (void)
{
  DEBUG (0, (cout << "OSPFNeighbor::TriggerUPDRxmt" << endl));

  ClearUPDRxmt ();
  updrxmtEvent = new Event (UPDRXMTTIMER);
  Scheduler::Schedule (updrxmtEvent, iface->GetRxmtInterval (), this);
}

void OSPFNeighbor::SendPacket (
#ifdef HAVE_QT
  QColor color,
#endif
  OSPFPacket* msg
  )
{
  DEBUG (0, (cout << "OSPFNeighbor::SendPacket" << endl));

#ifdef HAVE_QT
  ColoredPacket* packet = new ColoredPacket (color);
#else
  Packet* packet = new Packet ();
#endif
  packet->PushPDU (msg);
  IPV4ReqInfo info (addr, iface->GetAddr (), OSPFTTL, OSPFPROTO);
  IPV4::Instance ()->DataRequest (iface->GetNode (), packet, &info);
}

bool OSPFNeighbor::ReceiveHello (
  OSPFHello* hello
  )
{
  DEBUG (0, (cout << "OSPFNeighbor::ReceiveHello" << endl));

  priority = hello->priority;
  designated = hello->designated;
  backup = hello->backup;

  HandleEvent (HELLORECEIVED);

  if (hello->FindRouter (iface->GetRouterID ())) {
    HandleEvent (TWOWAYRECEIVED);
    return (true);
  }
  else {
    HandleEvent (ONEWAYRECEIVED);
    ReplyHello ();
    return (false);
  }
}

void OSPFNeighbor::ReplyHello (void)
{
  DEBUG (0, (cout << "OSPFNeighbor::ReplyHello" << endl));

  OSPFHello* hello = new OSPFHello (iface);
#ifdef HAVE_QT
    SendPacket (HELLOCOLOR, hello);
#else
    SendPacket (hello);
#endif
}

void OSPFNeighbor::SendDataD (void)
{
  DEBUG (0, (cout << "OSPFNeighbor::SendDataD" << endl));

  OSPFDataD* dd;
  bool main = IsMaster ();
  if (status == EXSTART) {
    dd = new OSPFDataD (sequence, iface);
    last = dd->Copy ();
#ifdef HAVE_QT
    SendPacket (DATADCOLOR, dd);
#else
    SendPacket (dd);
#endif
    TriggerDDRxmt ();
  }
  else {
    dd = new OSPFDataD (this);
    last = dd->Copy ();
#ifdef HAVE_QT
    SendPacket (DATADCOLOR, dd);
#else
    SendPacket (dd);
#endif
    if (main) {
      TriggerDDRxmt ();
    }
    else {
      if (database.empty () && ((imms & OSPFDataD::MBIT) == 0)) {
        HandleEvent (EXCHANGEDONE);
      }
    }
  }
}

void OSPFNeighbor::ReceiveDataD (
  OSPFDataD* dd
  )
{
  DEBUG (0, (cout << "OSPFNeighbor::ReceiveDataD" << endl));

  if (iface->GetMTU () >= dd->GetMTU ()) {

    bool main = IsMaster ();
    bool init = dd->IsInit ();
    bool more = dd->IsMore ();
    bool slave = !dd->IsMaster ();
    bool empty = (dd->GetHeaders ()).empty ();
    Byte_t opt = dd->GetOption ();
    Seq_t seq = dd->GetSequence ();

    switch (status) {
      case INIT: 
        HandleEvent (TWOWAYRECEIVED);
        if (status != EXSTART) {
          break;
        }
      case EXSTART: 
        if (slave == main) {
          if ((init && more && (!slave) && empty) || ((!init) && main && (sequence == seq))) {
            if ((init && more && (!slave) && empty)) {
              master = false;
              sequence = seq;
            }
            HandleEvent (NEGOTIATIONDONE);
            option = opt;
            ProcessDataD (dd);
          }
        }
        break;
      case EXCHANGE: 
        if ((main != slave) || init || (option != opt)) {
          HandleEvent (SEQMISMATCH);
          break;
        }
        if (main) {
          if ((sequence - 1) == seq) {
            break;
          }
          if (sequence == seq) {
            ProcessDataD (dd);
            break;
          }
        }
        else {
          if (sequence == seq) {
            SendDDRxmt ();
            break;
          }
          if ((sequence + 1) == seq) {
            ProcessDataD (dd);
            break;
          }
        }
        HandleEvent (SEQMISMATCH);
        break;
      case LOADING: 
      case FULL:
        if ((main != slave) || init || (option != opt)) {
          HandleEvent (SEQMISMATCH);
          break;
        }
        if (main) {
          if ((sequence - 1) == seq) {
            break;
          }
        }
        else {
          if (sequence == seq) {
          SendDDRxmt ();
            break;
          }
        }
        HandleEvent (SEQMISMATCH);
        break;
      case DOWN: 
      case ATTEMPT: 
      case TWOWAY: 
      default:
        break;
    }
  }
}

void OSPFNeighbor::ProcessDataD (
  OSPFDataD* dd
  )
{
  DEBUG (0, (cout << "OSPFNeighbor::ProcessDataD" << endl));

  imms = dd->GetIMMS ();
  ClearDDRxmt ();
  ClearLast ();
  BuildRequested (dd);

  if (IsMaster ()) {
    sequence++;
    if (!database.empty () || dd->IsMore ()) {
      SendDataD ();
    }
    else {
      HandleEvent (EXCHANGEDONE);
    }
  }
  else {
    sequence = dd->GetSequence ();
    SendDataD ();
  }
}

void OSPFNeighbor::BuildRequested (
  OSPFDataD* dd
  )
{
  DEBUG (0, (cout << "OSPFNeighbor::BuildRequestList" << endl));

  OSPFLSAHeaders& headers = dd->GetHeaders ();
  for (OSPFLSAHeadersIt it = headers.begin (); it != headers.end (); it++) {
    OSPFLSAHeader* header = *it;
    if (header->CheckType ()) {
      OSPFLSAHeader* instance = iface->FindLSA (header);
      if ((instance == nil) || (instance->Compare (header) == -1)) {
        OSPFLSAHeader* request = header->OSPFLSAHeader::Copy ();
        requested.push_back (request);
      }
    }
    else {
      HandleEvent (SEQMISMATCH);
      break;
    }
  }
}

OSPFLSAHeader* OSPFNeighbor::FindRequest (
  OSPFLSAHeader* lsa
  )
{
  DEBUG (0, (cout << "OSPFNeighbor::FindRequest" << endl));

  if (lsa != nil) {
    for (OSPFLSAHeadersIt it = requested.begin (); it != requested.end (); it++) {
      OSPFLSAHeader* header = *it;
      if (lsa->IsIt (header)) {
        return (header);
      }
    }
  }
  return (nil);
}

void OSPFNeighbor::EraseRequest (
  OSPFLSAHeader* lsa
  )
{
  DEBUG (0, (cout << "OSPFNeighbor::EraseRequest" << endl));

  if (lsa != nil) {
    for (OSPFLSAHeadersIt it = requested.begin (); it != requested.end (); it++) {
      OSPFLSAHeader* request = *it;
      if (lsa->IsIt (request)) {
        delete request;
        requested.erase (it);
        break;
      }
    }
    if (requested.empty ()) {
      ClearREQRxmt ();
      if (IsLoading ()) {
        HandleEvent (LOADINGDONE);
      }
    }
  }
}

void OSPFNeighbor::AddNoacked (
  OSPFLSAHeader* lsa
  )
{
  DEBUG (0, (cout << "OSPFNeighbor::AddNoacked" << endl));

  if (lsa != nil) {
    OSPFLSAHeader* copy = lsa->Copy ();
    noacked.push_back (copy);
    if (updrxmtEvent == nil) {
      TriggerUPDRxmt ();
    }
  }
}

OSPFLSAHeader* OSPFNeighbor::FindNoacked (
  OSPFLSAHeader* lsa
  )
{
  DEBUG (0, (cout << "OSPFNeighbor::FindNoacked" << endl));

  if (lsa != nil) {
    for (OSPFLSAHeadersIt it = noacked.begin (); it != noacked.end (); it++) {
      OSPFLSAHeader* noack = *it;
      if (lsa->IsIt (noack) && (lsa->Compare (noack) == 0)) {
        return (noack);
      }
    }
  }
  return (nil);
}

void OSPFNeighbor::EraseNoacked (
  OSPFLSAHeader* lsa
  )
{
  DEBUG (0, (cout << "OSPFNeighbor::EraseNoacked" << endl));

  if (lsa != nil) {
    for (OSPFLSAHeadersIt it = noacked.begin (); it != noacked.end (); it++) {
      OSPFLSAHeader* noack = *it;
      if (lsa->IsIt (noack) && (lsa->Compare (noack) == 0)) {
        delete noack;
        noacked.erase (it);
        break;
      }
    }
    if (noacked.empty ()) {
      ClearUPDRxmt ();
    }
  }
}

void OSPFNeighbor::SendDDRxmt (void)
{
  DEBUG (0, (cout << "OSPFNeighbor::SendDDRxmt" << endl));

  if (status == FULL) {
    HandleEvent (SEQMISMATCH);
  }
  else {
#ifdef HAVE_QT
    SendPacket (DATADCOLOR, last);
#else
    SendPacket (last);
#endif
    TriggerDDRxmt ();
  }
}

void OSPFNeighbor::SendLSReq (void)
{
  DEBUG (0, (cout << "OSPFNeighbor::SendLSReq" << endl));

  if (requested.empty ()) {
    ClearREQRxmt ();
    if (IsLoading ()) {
      HandleEvent (LOADINGDONE);
    }
  }
  else {
    OSPFLSReq* lsreq = new OSPFLSReq (this);
#ifdef HAVE_QT
    SendPacket (LSREQCOLOR, lsreq);
#else
    SendPacket (lsreq);
#endif
    TriggerREQRxmt ();
  }
}

void OSPFNeighbor::ReceiveLSReq (
  OSPFLSReq* lsreq 
  )
{
  DEBUG (0, (cout << "OSPFNeighbor::ReceiveLSReq" << endl));
  
  if (IsFlooding ()) {
    OSPFLSUpd* lsupd = nil;
    OSPFRequests& requests = lsreq->GetRequests ();
    for (OSPFRequestsIt it = requests.begin (); it != requests.end (); it++) {
      OSPFRequest* request = *it;
      OSPFLSAHeader* lsa = iface->FindLSA (request);
      if (lsa == nil) {
        HandleEvent (BADLSREQ);
        if (lsupd != nil) {
          delete lsupd;
        }
        return;
      }
      if (lsupd == nil) {
        lsupd = new OSPFLSUpd (iface);
      }
      if ((lsupd->Size () + lsa->Size ()) > GetMTU ()) {
#ifdef HAVE_QT
        SendPacket (LSUPDCOLOR, lsupd);
#else
        SendPacket (lsupd);
#endif
        lsupd = new OSPFLSUpd (iface);
      }
      lsupd->AddLSA (lsa, iface->GetDelay ());
    }
    if (lsupd != nil) {
#ifdef HAVE_QT
      SendPacket (LSUPDCOLOR, lsupd);
#else
      SendPacket (lsupd);
#endif
    }
  }
}

void OSPFNeighbor::SendUPDRxmt (void)
{
  DEBUG (0, (cout << "OSPFNeighbor::SendUPDRxmt" << endl));

  OSPFLSUpd* lsupd = nil;
  for (OSPFLSAHeadersIt it = noacked.begin (); it != noacked.end (); it++) {
    OSPFLSAHeader* lsa = *it;
    if (lsupd == nil) {
      lsupd = new OSPFLSUpd (iface);
    }
    if ((lsupd->Size () + lsa->Size ()) > GetMTU ()) {
      break;
    }
    lsupd->AddLSA (lsa, iface->GetDelay ());
  }
  if (lsupd != nil) {
#ifdef HAVE_QT
    SendPacket (LSUPDCOLOR, lsupd);
#else
    SendPacket (lsupd);
#endif
    TriggerUPDRxmt ();
  }
}

void OSPFNeighbor::ReceiveLSUpd (
  OSPFLSUpd* lsupd
  )
{
  DEBUG (0, (cout << "OSPFNeighbor::ReceiveLSUpd" << endl));

  if (IsFlooding ()) {
    OSPFLSAHeaders& lsas = lsupd->GetLSAs ();
    for (OSPFLSAHeadersIt it = lsas.begin (); it != lsas.end (); it++) {
      OSPFLSAHeader* lsa = *it;
      if (lsa->CheckChksum () && lsa->CheckType () &&
        (!iface->IsStub () || (lsa->GetType () != OSPFASEXT))) 
      {
        OSPFLSAHeader* instance = iface->FindLSA (lsa);
        if ((lsa->GetAge () >= OSPFMAXAGE) && (instance == nil) && !iface->FindNeighbor ()) {
          SendLSAck (lsa);
        }
        else {
          int compare = (instance == nil) ? 1 : lsa->Compare (instance);
          if (compare == 1) {
            if ((instance == nil) || (instance->FromReceive () > OSPFMINARRIVAL)) {
              iface->InstallLSA (lsa);
              bool sendback = iface->FloodOut (lsa, this);
              if (sendback == false) {
                if (instance != nil) {
                  if (iface->IsBDR ()) {
                    if (this == iface->GetDesignated ()) {
                      iface->AddAck (lsa);
                    }
                  }
                  else {
                    iface->AddAck (lsa);
                  }
                }
              }
              if (IsSelfOriginated (lsa)) {
// Do it later
// 13.4
                continue;
              }
            }
          }
          else {
            OSPFLSAHeader* request = FindRequest (lsa);
            if (request != nil) {
              EraseRequest (request);
              HandleEvent (BADLSREQ);
            }
            else {
              if (compare == 0) {
                OSPFLSAHeader* noack = FindNoacked (lsa);
                if (noack != nil) {
                  EraseNoacked (noack);
                  if (iface->IsBDR ()) {
                    if (this == iface->GetDesignated ()) {
                      iface->AddAck (lsa);
                    }
                  }
                }
                else {
                  SendLSAck (lsa);
                }
              }
              else {
                if ((instance->GetAge () < OSPFMAXAGE) || (instance->GetSequence () < OSPFMAXSEQUENCE)) {
                  OSPFLSUpd* upd = new OSPFLSUpd (iface);
                  upd->AddLSA (instance, iface->GetDelay ());
#ifdef HAVE_QT
                  SendPacket (LSUPDCOLOR, upd);
#else
                  SendPacket (upd);
#endif
                }
              }
            }
          }
        }
      }
    }  
  }
}

bool OSPFNeighbor::IsSelfOriginated (
  OSPFLSAHeader* lsa
  ) const
{
  DEBUG (0, (cout << "OSPFNeighbor::IsSelfOriginated" << endl));

  if ((lsa->GetAdvID () == iface->GetRouterID ()) ||
    ((lsa->GetType () == OSPFNETWORK) && (lsa->GetLinkID () == addr)))
  {
    return (true);
  }
  else {
    return (false);
  }
}

void OSPFNeighbor::SendLSAck (
  OSPFLSAHeader* lsa
  )
{
  DEBUG (0, (cout << "OSPFNeighbor::SendLSAck" << endl));

  OSPFLSAck* lsack = new OSPFLSAck (iface);
  lsack->AddLSA (lsa);
#ifdef HAVE_QT
  SendPacket (LSACKCOLOR, lsack);
#else
  SendPacket (lsack);
#endif
}

void OSPFNeighbor::ReceiveLSAck (
  OSPFLSAck* lsack
  )
{
  DEBUG (0, (cout << "OSPFNeighbor::ReceiveLSAck" << endl));

  if (IsFlooding ()) {
    OSPFLSAHeaders& headers = lsack->GetHeaders ();
    for (OSPFLSAHeadersIt it = headers.begin (); it != headers.end (); it++) {
      OSPFLSAHeader* ack = *it;
      EraseNoacked (ack);
    }
  }
}

void OSPFNeighbor::debug (void)
{
  iface->debug();
}

// OSPFInterface methods
OSPFInterface::OSPFInterface (
  OSPFNetwork_t type,
  IPAddr_t addr,
  Mask_t mask,
  Byte_t priority,
  OSPFCost_t cost,
  OSPFArea* area
  )
  : type (type),
    status (DOWN),
    addr (addr), mask (mask),
    aid (area->GetAreaID ()),
    helloInterval (OSPFHELLOINTERVAL), deadInterval (OSPFDEADINTERVAL),
    delay (1),
    priority (priority),
    helloEvent (nil), waitEvent (nil), 
    designated (IPADDR_NONE), backup (IPADDR_NONE),
    cost (cost),
    rxmtInterval (OSPFRXMTINTERVAL),
    atype (0),
    // Internal data structures
    area (area),
    outaddr (OSPFALLROUTERS),
    rxmtEvent (nil)
{
  DEBUG (0, (cout << "OSPFInterface::OSPFInterface" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created with args" << endl));

  if (type == OSPFVLINK) {
    mtu = 0;
  }
  else {
    mtu = 1500;
  }
}

OSPFInterface::~OSPFInterface (void)
{
  DEBUG (0, (cout << "OSPFInterface::~OSPFInterface" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " destroyed" << endl));

  ClearHello ();
  ClearWait ();
  ClearNeighbors ();
  ClearRxmt ();
  ClearAcks ();
}

void OSPFInterface::Handle (
  Event* event, 
  Time_t time
  )
{
  DEBUG (0, (cout << "OSPFInterface::Handle" << endl));
  DEBUG (1, (cout << "\tEvent " << event->event << " at " << time << endl));

  Status_t prev_status = status;
  switch ((Event_t)event->event) {
    case INTERFACEUP:
      if (status == DOWN) {
        if ((type == OSPFP2P) || (type == OSPFP2M) || (type == OSPFVLINK)) {
          status = P2P;
        }
        else {
          if (priority == OSPFNODR) {
            status = DROTHER;
          }
          else {
            status = WAITING;
            TriggerWait ();
            if (type == OSPFNBMA) {
// Do it later
            }
          }
        }
        SendHello ();
      }
      break;
    case WAITTIMER:
      if (status == WAITING) {
        SelectRouter ();
      }
      break;
    case BACKUPSEEN:
      if (status == WAITING) {
        ClearWait ();
        SelectRouter ();
      }
      break;
    case NEIGHBORCHANGE:
      if ((status == DROTHER) || (status == BDR) || (status == DR)) {
        SelectRouter ();
      }
      break;
    case LOOPIND:
      status = LOOPBACK;
      ClearHello ();
      ClearWait ();
      ClearNeighbors ();
      ClearRxmt ();
      ClearAcks ();
      break;
    case UNLOOPIND:
      if (status == LOOPBACK) {
        status = DOWN;
      }
      break;
    case INTERFACEDOWN:
      status = DOWN;
      ClearHello ();
      ClearWait ();
      ClearNeighbors ();
      ClearRxmt ();
      ClearAcks ();
      break;
    case HELLOTIMER:
      helloEvent = nil;
      SendHello ();
      break;
    case RXMTTIMER:
      rxmtEvent = nil;
      SendAck ();
      break;
    default:
      break;
  }
  delete event;

  if (prev_status != status) {
    if (prev_status == DOWN) {
      area->ChangeInterface (1);
    }
    else {
      if (status == DOWN) {
        area->ChangeInterface (-1);
      }
    }
    area->FlushRouterLSA ();
  }
}

bool OSPFInterface::IsInterface (
  Node* node,
  IPAddr_t addr
  )
{
  DEBUG (0, (cout << "OSPFInterface::IsInterface" << endl));
  DEBUG (1, (cout << "\tNode " << node->Id () << "; IP " << (string)IPAddr (addr) << endl));

  IFVec_t ifaces = node->Interfaces ();
  for (IFVec_t::iterator it = ifaces.begin (); it != ifaces.end (); it++) {
    Interface* iface = *it;
    if (addr == iface->GetIPAddr ()) {
      return (true);
    }
  }
  return (false);
}

Node* OSPFInterface::GetNode (void) const
{
  DEBUG (0, (cout << "OSPFInterface::GetNode" << endl));

  return (area->GetNode ());
}

bool OSPFInterface::IsStub (void) const
{
  DEBUG (0, (cout << "OSPFInterface::IsStub" << endl));

  return (area->IsStub ());
}

Bitmap_t OSPFInterface::GetOption (void) const
{
  DEBUG (0, (cout << "OSPFInterface::GetOption" << endl));

  return (area->GetOption ());
}

void OSPFInterface::InstallLSA (
  OSPFLSAHeader* lsa
  )
{
  DEBUG (0, (cout << "OSPFInterface::InstallLSA" << endl));

  area->InstallLSA (lsa);
}

OSPFLSAHeader* OSPFInterface::FindLSA (
  OSPFLSAHeader* lsa
  )
{
  DEBUG (0, (cout << "OSPFInterface::FindLSA" << endl));

  if (lsa != nil) {
    return (area->FindLSA (lsa->GetType (), lsa->GetLinkID (), lsa->GetAdvID ()));
  }
  else {
    return (nil);
  }
}

OSPFLSAHeader* OSPFInterface::FindLSA (
  OSPFRequest* request
  )
{
  DEBUG (0, (cout << "OSPFInterface::FindLSA" << endl));

  if (request != nil) {
    return (area->FindLSA (request->GetType (), request->GetLinkID (), request->GetAdvID ()));
  }
  else {
    return (nil);
  }
}

void OSPFInterface::TriggerEvent (
  Event_t type
  )
{
  DEBUG (0, (cout << "OSPFInterface::TriggerEvent" << endl));
  DEBUG (1, (cout << "\tEvent " << type << endl));

  Event* event = new Event (type);
  Scheduler::Schedule (event, 0, this);
}

void OSPFInterface::ClearHello (void)
{
  DEBUG (0, (cout << "OSPFInterface::ClearHello" << endl));

  if (helloEvent != nil) {
    Scheduler::Cancel (helloEvent);
    delete helloEvent;
    helloEvent = nil;
  }
}

void OSPFInterface::TriggerHello (void)
{
  DEBUG (0, (cout << "OSPFInterface::TriggerHello" << endl));

  ClearHello ();
  helloEvent = new Event (HELLOTIMER);
  Scheduler::Schedule (helloEvent, helloInterval, this);
}

void OSPFInterface::ClearWait (void)
{
  DEBUG (0, (cout << "OSPFInterface::ClearWait" << endl));

  if (waitEvent != nil) {
    Scheduler::Cancel (waitEvent);
    delete waitEvent;
    waitEvent = nil;
  }
}

void OSPFInterface::TriggerWait (void)
{
  DEBUG (0, (cout << "OSPFInterface::TriggerWait" << endl));

  ClearWait ();
  waitEvent = new Event (WAITTIMER);
  Scheduler::Schedule (waitEvent, deadInterval, this);
}

OSPFNeighbor* OSPFInterface::AddNeighbor (
  IPAddr_t src,
  OSPFHello* hello
  )
{
  DEBUG (0, (cout << "OSPFInterface::AddNeighbor" << endl));

  if (hello != nil) {
    OSPFNeighbor* neighbor = new OSPFNeighbor (hello->rid, hello->priority,
      src, hello->option, hello->designated, hello->backup, this);
    neighbors.push_back (neighbor);
    return (neighbor);
  }
  else {
    return (nil);
  }
}

OSPFNeighbor* OSPFInterface::FindNeighbor (
  IPAddr_t addr,
  OSPFRouter_t rid
  )
{
  DEBUG (0, (cout << "OSPFInterface::FindNeighbor" << endl));

  for (OSPFNeighborsIt it = neighbors.begin (); it != neighbors.end (); it++) {
    OSPFNeighbor* neighbor = *it;
    switch (type) {
      case OSPFP2P:
      case OSPFVLINK:
        if (neighbor->nid == rid) {
          return (neighbor);
        }
        break;
      case OSPFBROADCAST:
      case OSPFP2M:
      case OSPFNBMA:
        if (neighbor->addr == addr) {
          return (neighbor);
        }
        break;
      default:
        break;
    }
  }
  return (nil);
}

OSPFNeighbor* OSPFInterface::GetPeer (void) const
{
  DEBUG (0, (cout << "OSPFInterface::GetPeer" << endl));
  
  OSPFNeighbor* neighbor = nil;
  if ((type == OSPFP2P) || (type == OSPFVLINK)) {
    if (!neighbors.empty ()) {
      neighbor = *neighbors.begin ();
    }
  }
  return (neighbor);
}

OSPFNeighbor* OSPFInterface::GetDesignated (void)
{
  DEBUG (0, (cout << "OSPFInterface::GetDesignated" << endl));
  
  if ((type == OSPFBROADCAST) || (type == OSPFNBMA)) {
    OSPFNeighbor* neighbor = FindNeighbor (designated, IPADDR_NONE);
    return (neighbor);
  }
  return (nil);
}

bool OSPFInterface::FindNeighbor (void)
{
  DEBUG (0, (cout << "OSPFInterface::FindNeighbor" << endl));

  for (OSPFNeighborsIt it = neighbors.begin (); it != neighbors.end (); it++) {
    OSPFNeighbor* neighbor = *it;
    if (neighbor->IsExchange () || neighbor->IsLoading ()) {
      return (true);
    }   
  }
  return (false);
}

void OSPFInterface::ClearNeighbors (void)
{
  DEBUG (0, (cout << "OSPFInterface::ClearNeighbors" << endl));

  for (OSPFNeighborsIt it = neighbors.begin (); it != neighbors.end (); it++) {
    OSPFNeighbor* neighbor = *it;
    neighbor->HandleEvent (OSPFNeighbor::KILLNBR);
    delete neighbor;
  }
  neighbors.clear ();
}

void OSPFInterface::SendToAdjacency (
#ifdef HAVE_QT
  QColor color,
#endif
  OSPFPacket* msg
  )
{ 
  DEBUG (0, (cout << "OSPFInterface::SendToAdjacency" << endl));

  if (status >= P2P) {
    switch (type) {
      case OSPFP2P:
      case OSPFP2M:
      case OSPFVLINK:
        for (OSPFNeighborsIt it = neighbors.begin (); it != neighbors.end (); it++) {
          OSPFNeighbor* neighbor = *it;
          if (neighbor->IsFlooding ()) {
#ifdef HAVE_QT
            neighbor->SendPacket (color, msg);
#else
            neighbor->SendPacket (msg);
#endif
          }
        }
        break;
      case OSPFBROADCAST:
      case OSPFNBMA:
        {
          IPAddr_t out;
          if ((status == DR) || (status == BDR)) {
            out = OSPFALLROUTERS;
          }
          else {
            out = OSPFALLDROUTERS;
          }
#ifdef HAVE_QT
          ColoredPacket* packet = new ColoredPacket (color);
#else
          Packet* packet = new Packet ();
#endif
          packet->PushPDU (msg);
          IPV4ReqInfo info (out, addr, OSPFTTL, OSPFPROTO);
          IPV4::Instance ()->DataRequest (area->GetNode (), packet, &info);
        }
        break;
      default:
        break;
    }
  }
}

void OSPFInterface::SendHello (void)
{ 
  DEBUG (0, (cout << "OSPFInterface::SendHello" << endl));

  if ((status != DOWN) && (status != LOOPBACK)) {
    OSPFHello* hello = new OSPFHello (this);
#ifdef HAVE_QT
    ColoredPacket* packet = new ColoredPacket (HELLOCOLOR);
#else
    Packet* packet = new Packet ();
#endif
    packet->PushPDU (hello);
    IPV4ReqInfo* info;
    switch (type) {
      case OSPFP2P:
      case OSPFBROADCAST:
        info = new IPV4ReqInfo (OSPFALLROUTERS, addr, OSPFTTL, OSPFPROTO);
        IPV4::Instance ()->DataRequest (area->GetNode (), packet, info);
        delete info;
        break;
      case OSPFVLINK:
// Do it later
        break;
      case OSPFP2M:
// Do it later
        break;
      case OSPFNBMA:
// Do it later
        break;
      default:
        break;
    }
    TriggerHello ();
  }
}

void OSPFInterface::ReceiveHello (
  IPAddr_t src,
  OSPFHello* hello
  )
{
  DEBUG (0, (cout << "OSPFInterface::ReceiveHello" << endl));

  if ((status != DOWN) && (status != LOOPBACK)) {
    OSPFNeighbor* neighbor;
    if ((neighbor = FindNeighbor (src, hello->rid)) == nil) {
      neighbor = AddNeighbor (src, hello);
    }
    bool claimedDR = neighbor->ClaimDR ();
    bool claimedBDR = neighbor->ClaimBDR ();
    Byte_t old_pri = neighbor->priority;
    if (neighbor->ReceiveHello (hello) == true) {
      if (hello->priority != old_pri) {
        TriggerEvent (NEIGHBORCHANGE);
      }
      if (neighbor->ClaimDR () && (hello->backup == IPADDR_NONE) && (status == WAITING)) {
        TriggerEvent (BACKUPSEEN);
      }
      else {
        if (claimedDR != neighbor->ClaimDR ()) {
          TriggerEvent (NEIGHBORCHANGE);
        }
      }
      if (neighbor->ClaimBDR () && (status == WAITING)) {
        TriggerEvent (BACKUPSEEN);
      }
      else {
        if (claimedBDR != neighbor->ClaimBDR ()) {
          TriggerEvent (NEIGHBORCHANGE);
        }
      }

      if (type == OSPFNBMA) {
// Do it later
// 9.5.1
      }
    }
  }
}

bool OSPFInterface::IsAdjacency (
  IPAddr_t ip
  ) const
{
  DEBUG (0, (cout << "OSPFInterface::IsAdjacency" << endl));

  bool adj;
  switch (type) {
    case OSPFP2P:
    case OSPFP2M:
    case OSPFVLINK:
      adj = true;
      break;
    case OSPFBROADCAST:
    case OSPFNBMA:
      if ((addr == designated) || (addr == backup) ||
        (ip == designated) || (ip == backup))
      {
        adj = true;
      }
      else {
        adj = false;
      }
      break;
    default:
      adj = false;
      break;
  }
  return (adj);
}

void OSPFInterface::SelectRouter (void)
{
  DEBUG (0, (cout << "OSPFInterface::SelectRouter" << endl));

  IPAddr_t prev_dr = designated;
  IPAddr_t prev_bdr = backup;

  for (int i = 0; i < 2; i++) {
    OSPFRouter_t crid;
    Byte_t cpriority;
    bool cdeclared;

    if ((priority != OSPFNODR) && (addr != designated)) {
      crid = GetRouterID ();
      cpriority = priority;
      cdeclared = (addr == backup);
      backup = addr;
    }
    else {
      crid = IPADDR_NONE;
      cpriority = OSPFNODR;
      cdeclared = false;
      backup = IPADDR_NONE;
    }
    for (OSPFNeighborsIt it = neighbors.begin (); it != neighbors.end (); it++) {
      OSPFNeighbor* neighbor = *it;
      if ((neighbor->status >= OSPFNeighbor::TWOWAY) && 
         (neighbor->priority != OSPFNODR) &&
         (neighbor->addr != neighbor->designated)) 
      {
        if (cdeclared) {
          if ((neighbor->addr != neighbor->backup) ||
            (neighbor->priority < cpriority) ||
            ((neighbor->priority == cpriority) && (neighbor->nid < crid)))
          {
            continue;
          }
        }
        else {
          if ((neighbor->addr != neighbor->backup) &&
            ((neighbor->priority < cpriority) ||
            ((neighbor->priority == cpriority) && (neighbor->nid < crid))))
          {
            continue;
          }
        }
        crid = neighbor->nid;
        cpriority = neighbor->priority;
        cdeclared = (neighbor->addr == neighbor->backup);
        backup = neighbor->addr;
      }
    }

    if ((priority != OSPFNODR) && (addr == designated)) {
      crid = GetRouterID ();
      cpriority = priority;
    }
    else {
      cpriority = OSPFNODR;
      designated = IPADDR_NONE;
    }
    for (OSPFNeighborsIt it = neighbors.begin (); it != neighbors.end (); it++) {
      OSPFNeighbor* neighbor = *it;
      if ((neighbor->status >= OSPFNeighbor::TWOWAY) &&
        (neighbor->priority != OSPFNODR) &&
        (neighbor->addr == neighbor->designated))
      {
        if ((neighbor->priority > cpriority) ||
          ((neighbor->priority == cpriority) && (neighbor->nid > crid)))
        {
          crid = neighbor->nid;
          cpriority = neighbor->priority;
          designated = neighbor->addr;
        }
      }
    }

    if (designated == IPADDR_NONE) {
      designated = backup;
    }

    if (((designated == prev_dr) || ((addr != designated) && (addr != prev_dr))) &&
       ((backup == prev_bdr) || ((addr != backup) && (addr != prev_bdr)))) 
    {
      break;
    }
  }

  if (addr == designated) {
    status = DR;
  }
  else {
    if (addr == backup) {
      status = BDR;
    }
    else {
      status = DROTHER;
    }
  }

  if (type == OSPFNBMA) {
    if ((addr == designated) || (addr == backup)) {
      for (OSPFNeighborsIt it = neighbors.begin (); it != neighbors.end (); it++) {
        OSPFNeighbor* neighbor = *it;
        if (neighbor->priority == OSPFNODR) {
          neighbor->HandleEvent (OSPFNeighbor::START);
        }
      }
    }
  }

  if ((designated != prev_dr) || (backup != prev_bdr)) {
    for (OSPFNeighborsIt it = neighbors.begin (); it != neighbors.end (); it++) {
      OSPFNeighbor* neighbor = *it;
      if (neighbor->status >= OSPFNeighbor::TWOWAY) {
        neighbor->HandleEvent (OSPFNeighbor::ADJOK);
      }
    }
  }
}

bool OSPFInterface::FloodOut (
  OSPFLSAHeader* lsa,
  OSPFNeighbor* sender
  )
{
  DEBUG (0, (cout << "OSPFInterface::FloodOut" << endl));

  return (area->FloodOut (lsa, this, sender));
}

bool OSPFInterface:: ReceiveLSA (
  OSPFNeighbor* sender,
  OSPFLSAHeader* lsa
  )
{
  DEBUG (0, (cout << "OSPFInterface::ReceiveLSA" << endl));

  bool flood = false;
  for (OSPFNeighborsIt it = neighbors.begin (); it != neighbors.end (); it++) {
    OSPFNeighbor* neighbor = *it;
    if (neighbor->IsFlooding ()) {
      if (!neighbor->IsFull ()) {
        OSPFLSAHeader* request = neighbor->FindRequest (lsa);
        if (request != nil) {
          int compare = lsa->Compare (request);
          if (compare == -1) {
            continue;
          }
          if (compare == 0) {
            neighbor->EraseRequest (request);
            continue;
          }
          if (compare == 1) {
            neighbor->EraseRequest (request);
          }
        }
      }
      if ((sender != nil) && (neighbor == sender)) {
        continue;
      }
      neighbor->AddNoacked (lsa);
      flood = true;
    }
  }
  return (flood);
}

void OSPFInterface::EraseNoacked (
  OSPFLSAHeader* lsa
  )
{
  DEBUG (0, (cout << "OSPFInterface::EraseNoacked" << endl));

  for (OSPFNeighborsIt it = neighbors.begin (); it != neighbors.end (); it++) {
    OSPFNeighbor* neighbor = *it;
    neighbor->EraseNoacked (lsa);
  }
}

void OSPFInterface::ClearRxmt (void)
{
  DEBUG (0, (cout << "OSPFInterface::ClearRxmt" << endl));

  if (rxmtEvent != nil) {
    Scheduler::Cancel (rxmtEvent);
    delete rxmtEvent;
    rxmtEvent = nil;
  }
}

void OSPFInterface::TriggerRxmt (void)
{
  DEBUG (0, (cout << "OSPFInterface::TriggerRxmt" << endl));

  ClearRxmt ();
  rxmtEvent = new Event (RXMTTIMER);
  Scheduler::Schedule (rxmtEvent, rxmtInterval / 2, this);
}

void OSPFInterface::ClearAcks (void)
{
  DEBUG (0, (cout << "OSPFInterface::ClearAcks" << endl));

  for (OSPFLSAHeadersIt it = acks.begin (); it != acks.end (); it++) {
    OSPFLSAHeader* header = *it;
    delete header;
  }
  acks.clear ();
}

void OSPFInterface::AddAck (
  OSPFLSAHeader* lsa
  )
{
  DEBUG (0, (cout << "OSPFInterface::AddAck" << endl));

  if (lsa != nil) {
    OSPFLSAHeader* header = lsa->OSPFLSAHeader::Copy ();
    acks.push_back (header);
    if (rxmtEvent == nil) {
      TriggerRxmt ();
    }
  }
}

void OSPFInterface::SendAck (void)
{
  DEBUG (0, (cout << "OSPFInterface::SendAck" << endl));

  OSPFLSAck* lsack = nil;
  for (OSPFLSAHeadersIt it = acks.begin (); it != acks.end (); it++) {
    OSPFLSAHeader* ack = *it;
    if (lsack == nil) {
      lsack = new OSPFLSAck (this);
    }
    if ((lsack->Size () + ack->Size ()) > GetMTU ()) {
#ifdef HAVE_QT
      SendToAdjacency (LSACKCOLOR, lsack);
#else
      SendToAdjacency (lsack);
#endif

      lsack = new OSPFLSAck (this);
    }
    lsack->AddLSA (ack);
  }
  if (lsack != nil) {
#ifdef HAVE_QT
    SendToAdjacency (LSACKCOLOR, lsack);
#else
    SendToAdjacency (lsack);
#endif
  }
}

void OSPFInterface::debug (void)
{
  area->debug();
}

// OSPFVertex methods
OSPFVertex::OSPFVertex (
  OSPFVertex* parent,
  OSPFLink* link
  )
  : vid (link->GetLinkID ()),
    cost (parent->cost + link->GetCost())
{
  DEBUG (0, (cout << "OSPFArea::OSPFArea" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created" << endl));

  switch (link->GetType()) {
    case OSPFLink::P2P:
      type = OSPFROUTER;
      out = parent->out != IPADDR_NONE ? parent->out : link->GetData ();
      break;
    case OSPFLink::TRANSIT:
// Do it later
      break;
    default:
      break;
  }
}

bool OSPFVertex::operator < (
  const OSPFVertex& vertex
  ) const
{
  DEBUG (0, (cout << "OSPFVertex::operator<" << endl));

  if (cost > vertex.cost) {
    return (true);
  }
  if (cost < vertex.cost) {
    return (false);
  }
  if (type < vertex.type) {
    return (true);
  }
  return (false);
}

// OSPFHop methods

// OSPFRouting methods
OSPFRouting::OSPFRouting (
  OSPFLSA_t type,
  OSPFID_t id,
  Mask_t mask,
  Bitmap_t option,
  OSPFArea_t aid,
  Path_t path,
  OSPFCost_t cost
  )
  : type (type),
    id (id),
    aid (aid),
    path (path),
    cost (cost),
    cost2 (0),
    origin (nil)
{
  DEBUG (0, (cout << "OSPFRouting::OSPFRouting" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created" << endl));

  switch (type) {
    case OSPFROUTER:
      this->mask = MASK_ALL;
      this->option = option;
      break;
    case OSPFNETWORK:
      this->mask = mask;
      this->option = 0;
      break;
    default:
      this->mask = MASK_ALL;
      this->option = 0;
      break;
  }
}

OSPFRouting::~OSPFRouting (void)
{
  DEBUG (0, (cout << "OSPFRouting::~OSPFRouting" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " destroyed" << endl));

  for (OSPFHopsIt it = hops.begin (); it != hops.end (); it ++) {
    OSPFHop* hop = *it;
    delete hop;
  }
  hops.clear ();
}

bool OSPFRouting::IsIt (
  OSPFRouting* entry
  )
{
  DEBUG (0, (cout << "OSPFRouting::IsIt" << endl));

  if ((type == entry->type) &&
    (id == entry->id) &&
    (mask == entry->mask) &&
    (aid == entry->aid) &&
    (path == entry->path))
  {
    return (true);
  }
  else {
    return (false);
  }
}

void OSPFRouting::AddNexthop (
  IPAddr_t out,
  IPAddr_t next,
  OSPFRouter_t rid
  )
{
  DEBUG (0, (cout << "OSPFRouting::AddNexthop" << endl));

  for (OSPFHopsIt it = hops.begin (); it != hops.end (); it++) {
    OSPFHop* hop = *it;
    if ((out == hop->GetOutput ()) && (next == hop->GetNext ()) && (rid == hop->GetAdvID ())) {
      return;
    }
  }
  OSPFHop* entry = new OSPFHop (out, next, rid);
  hops.push_back (entry);
}

void OSPFRouting::dump (void)
{
  // print only the router LSAs
  if(type == OSPFROUTER) {
    cout << "TYPE = " << type;
    cout << "; ID = " << (string)IPAddr (id);
    if (type == OSPFNETWORK) {
      cout << "; MASK = " << (string)IPAddr (mask);
    }
    cout << "; AID = " << (string)IPAddr (aid);
    cout << "; PATH = " << path;
    cout << "; COST = " << cost;
    if (!hops.empty ()) {
      cout << "; HOP = ";
      for (OSPFHopsIt it = hops.begin (); it != hops.end (); it++) {
	OSPFHop* hop = *it;
	hop->dump ();
	cout << " ";
      }
    }
    cout << endl;
  }
}

// OSPFArea methods
OSPFArea::OSPFArea (
  OSPFArea_t aid,
  OSPF* ospf
  )
  : aid (aid),
    transitCapability (false), 
    extRoutingCapability (false),
    stubDefaultCost (1),
    // Internal data structures
    ospf (ospf),
    number (0)
{
  DEBUG (0, (cout << "OSPFArea::OSPFArea" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created" << endl));
}

OSPFArea::~OSPFArea (void)
{
  DEBUG (0, (cout << "OSPFArea::~OSPFArea" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " destroyed" << endl));

  ClearRanges ();
  ClearInterfaces ();
  ClearRouterLSAs ();
  ClearNetworkLSAs ();
  ClearSummaryLSAs ();
  ClearMapping ();
}

Node* OSPFArea::GetNode (void) const
{
  DEBUG (0, (cout << "OSPFArea::GetNode" << endl));

  return (ospf->GetNode ());
}

OSPFRouter_t OSPFArea::GetRouterID (void) const
{
  DEBUG (0, (cout << "OSPFArea::GetRouterID" << endl));

  return (ospf->GetRouterID ());
}

Bitmap_t OSPFArea::GetRouterType (void) const
{
  DEBUG (0, (cout << "OSPFArea::GetRouterType" << endl));

  Bitmap_t type = 0;
  if (ospf->IsBorder ()) {
    type |= OSPFLSARouter::BBIT;
  }
  if (ospf->IsBoundary ()) {
    type |= OSPFLSARouter::EBIT;
  }
  if (ospf->IsVLinkEnd ()) {
    type |= OSPFLSARouter::VBIT;
  }
  return (type);
}

void OSPFArea::ClearRanges (void)
{
  DEBUG (0, (cout << "OSPFArea::ClearRanges" << endl));

  for (OSPFRangesIt it = ranges.begin (); it != ranges.end (); it++) {
    OSPFRange* range = *it;
    delete (range);
  }
  ranges.clear ();
}

OSPFInterface* OSPFArea::AddInterface (
  OSPFNetwork_t type,
  IPAddr_t addr,
  Mask_t mask,
  Byte_t priority,
  OSPFCost_t cost
  )
{
  DEBUG (0, (cout << "OSPFArea::AddInterface" << endl));
  DEBUG (1, (cout << "\tType " << type << "; IP " << (string)IPAddr (addr)));
  DEBUG (1, (cout << "; Mask " << (string)IPAddr (mask) << "; Priority " << priority));
  DEBUG (1, (cout << "; Cost " << cost << endl));

  OSPFInterface* iface = new OSPFInterface (type, addr, mask, priority, cost, this);
  ifaces.push_back (iface);
  return (iface);
}

OSPFInterface* OSPFArea::FindInterface (
  IPAddr_t addr
  )
{
  DEBUG (0, (cout << "OSPFArea::FindInterface" << endl));
  DEBUG (1, (cout << "\tIP " << (string)IPAddr (addr) << endl));

  for (OSPFInterfacesIt it = ifaces.begin (); it != ifaces.end (); it++) {
    OSPFInterface* iface = *it;
    if (iface->GetAddr () == addr) {
      return (iface);
    }
  }
  return (nil);
}

void OSPFArea::ClearInterfaces (void)
{
  DEBUG (0, (cout << "OSPFArea::ClearInterfaces" << endl));

  for (OSPFInterfacesIt it = ifaces.begin (); it != ifaces.end (); it++) {
    OSPFInterface* iface = *it;
    delete (iface);
  }
  ifaces.clear ();
}

void OSPFArea::ClearRouterLSAs (void)
{
  DEBUG (0, (cout << "OSPFArea::ClearRouterLSAs" << endl));

  for (OSPFLSARoutersIt it = rlsas.begin (); it != rlsas.end (); it++) {
    OSPFLSARouter* rlsa = *it;
    delete (rlsa);
  }
  rlsas.clear ();
}

void OSPFArea::ClearNetworkLSAs (void)
{
  DEBUG (0, (cout << "OSPFArea::ClearNetworkLSAs" << endl));

  for (OSPFLSANetworksIt it = nlsas.begin (); it != nlsas.end (); it++) {
    OSPFLSANetwork* nlsa = *it;
    delete (nlsa);
  }
  nlsas.clear ();
}

void OSPFArea::ClearSummaryLSAs (void)
{
  DEBUG (0, (cout << "OSPFArea::ClearSummaryLSAs" << endl));

  for (OSPFLSASummariesIt it = slsas.begin (); it != slsas.end (); it++) {
    OSPFLSASummary* slsa = *it;
    delete (slsa);
  }
  slsas.clear ();
}

Bitmap_t OSPFArea::GetOption (void) const
{
  DEBUG (0, (cout << "OSPFArea::GetOption" << endl));

  Bitmap_t option = 0;
  if (extRoutingCapability == true) {
    option |= OSPFOPT_E;
  }
  return (option);
}

void OSPFArea::InstallLSA (
  OSPFLSAHeader* lsa
  )
{
  DEBUG (0, (cout << "OSPFArea::InstallLSA" << endl));
  
  if (lsa != nil) {
    OSPFLSAHeader* instance = FindLSA (lsa->GetType (), lsa->GetLinkID (), lsa->GetAdvID ());
    if ((instance == nil) || instance->IsDifferent (lsa)) {
      if (instance != nil) {
        EraseNoacked (instance);
      }
      lsa->SetReceive ();
      switch (lsa->GetType ()) {
        case OSPFROUTER:
          AddRouterLSA ((OSPFLSARouter*)lsa);
          break;
        case OSPFNETWORK:
        case OSPFSUMIP:
        case OSPFSUMASBR:
        case OSPFASEXT:
        default:
          break;
      }
    }
    BuildTree ();
  }
}

OSPFLSAHeader* OSPFArea::FindLSA (
  OSPFLSA_t type,
  OSPFID_t lid,
  OSPFRouter_t rid
  )
{
  DEBUG (0, (cout << "OSPFArea::FindLSA" << endl));
  
  switch (type) {
    case OSPFROUTER:
      return (FindRouterLSA (lid));
    case OSPFNETWORK:
    case OSPFSUMIP:
    case OSPFSUMASBR:
    case OSPFASEXT:
    default:
      break;
  }
  return (nil);
}

void OSPFArea::AddRouterLSA (
  OSPFLSARouter* lsa
  )
{
  DEBUG (0, (cout << "OSPFArea::AddRouterLSA" << endl));

  if (lsa != nil) {
    EraseRouterLSA (lsa->GetLinkID ());
    OSPFLSAHeader* copy = lsa->Copy ();
    rlsas.push_back ((OSPFLSARouter*)copy);
  }
}

OSPFLSARouter* OSPFArea::FindRouterLSA (
  OSPFRouter_t rid
  )
{
  DEBUG (0, (cout << "OSPFArea::FindRouterLSA" << endl));

  for (OSPFLSARoutersIt it = rlsas.begin (); it != rlsas.end (); it++) {
    OSPFLSARouter* rlsa = *it;
    if ((rlsa->GetLinkID () == rid) && (rlsa->GetAdvID () == rid)) {
      return (rlsa);
    }
  }
  return (nil);
}

void OSPFArea::EraseRouterLSA (
  OSPFRouter_t rid
  )
{
  DEBUG (0, (cout << "OSPFArea::EraseRouterLSA" << endl));

  for (OSPFLSARoutersIt it = rlsas.begin (); it != rlsas.end (); it++) {
    OSPFLSARouter* rlsa = *it;
    if ((rlsa->GetLinkID () == rid) && (rlsa->GetAdvID () == rid)) {
      delete rlsa;
      rlsas.erase (it);
      break;
    }
  }
}

void OSPFArea::FlushRouterLSA (void)
{
  DEBUG (0, (cout << "OSPFArea::FlushLSARouter" << endl));

  if (number == 0) {
    EraseRouterLSA (GetRouterID ());
  }
  else {
    OSPFLSARouter* rlsa = FindRouterLSA (GetRouterID ());
    OSPFLSARouter* flush;
    if (rlsa == nil) {
      flush = new OSPFLSARouter (this);
    }
    else {
      flush = new OSPFLSARouter (this, rlsa->GetSequence () + 1);
    }
    InstallLSA (flush);
    FloodOut (flush);
    delete flush;
  }
}

bool OSPFArea::IsOnTree (
  OSPFVertexes& tree,
  OSPFVertex* vertex
  )
{
  DEBUG (0, (cout << "OSPFArea::IsOnTree" << endl));

  for (OSPFVertexesIt it = tree.begin (); it != tree.end (); it++) {
    OSPFVertex* node = *it;
    if ((node->type == vertex->type) &&
      (node->vid == vertex->vid) &&
      (node->cost < vertex->cost))
    {
      return (true);
    }
  }
  return (false);
}

void OSPFArea::ClearTree (
  OSPFVertexes& tree
  )
{
  DEBUG (0, (cout << "OSPFArea::ClearTree" << endl));

  for (OSPFVertexesIt it = tree.begin (); it != tree.end (); it++) {
    OSPFVertex* vertex = *it;
    delete vertex;
  }
  tree.clear ();
}

void OSPFArea::DumpTree (
  OSPFVertexes& tree
  )
{
  DEBUG (0, (cout << "OSPFArea::DumpTree" << endl));

  for (OSPFVertexesIt it = tree.begin (); it != tree.end (); it++) {
    OSPFVertex* vertex = *it;
    cout << "\ttype = " << vertex->type << "; ";
    cout << "\tvid = " << (string)IPAddr (vertex->vid) << "; ";
    cout << "\tcost = " << vertex->cost << "; " ;
    cout << "\tout = " << (string)IPAddr (vertex->out) << endl;
  }
}

void OSPFArea::BuildTree (void)
{
  DEBUG (0, (cout << "OSPFArea::BuildTree" << endl));

  OSPFVertexes tree;
  OSPFVertexQueue queue;
  OSPFVertex* root = new OSPFVertex (OSPFROUTER, GetRouterID ());
  queue.push (root);
  transitCapability = false;

  while (!queue.empty ()) {
    OSPFVertex* v = queue.top ();
    queue.pop ();
    if (!IsOnTree (tree, v)) {
      tree.push_back (v);
      if (v->type == OSPFROUTER) {
        OSPFLSARouter* vlsa = FindRouterLSA (v->vid);
        if (vlsa != nil) {
          if (vlsa->IsVLinkEnd ()) {
            transitCapability = true;
          }
          Count_t index = 0;
          OSPFLinks& links = vlsa->GetLinks ();
          for (OSPFLinksIt it = links.begin (); it != links.end (); it++) {
            OSPFLink* link = *it;
            OSPFVertex* w;
            if (link->GetType () == OSPFLink::STUB) {
              IPAddr_t ip = GetMapping (index++);
              OSPFID_t wid = link->GetLinkID ();
              Mask_t mask = link->GetData ();
              OSPFCost_t cost = v->cost + link->GetCost ();
              IPAddr_t out = v->out != IPADDR_NONE ? v->out : ip;
              ospf->AddRouting (OSPFNETWORK, wid, mask, 0, aid, OSPFRouting::INTRA, cost, out, false);
	      continue;
            }
            OSPFID_t wid = link->GetLinkID ();
            if (link->GetType () == OSPFLink::P2P) {
              OSPFLSARouter* wlsa = FindRouterLSA (wid);
              if ((wlsa == nil) || (wlsa->GetAge () == OSPFMAXAGE) || (!wlsa->IsConnected (v->vid))) {
                continue;
              }
            }
            else {
// Do it later
            }
            w = new OSPFVertex (v, link);
            queue.push (w);
            ospf->AddRouting (OSPFROUTER, v->vid, MASK_ALL, GetOption (), aid, OSPFRouting::INTRA, v->cost, v->out, true);
	  }
        }
      }
      else {
// Do it later
      }
    }
    else {
      delete v;
    }
  }
  ClearTree (tree);

  ospf->DumpRouting ();
}

bool OSPFArea::FloodOut (
  OSPFLSAHeader* lsa,
  OSPFInterface* receiver,
  OSPFNeighbor* sender
  )
{
  DEBUG (0, (cout << "OSPFArea::FloodOut" << endl));

  bool sendback = false;
  if (lsa->GetType () == OSPFASEXT) {
// Do it later
// 13.3    
  }
  else {
    for (OSPFInterfacesIt it = ifaces.begin (); it != ifaces.end (); it++) {
      OSPFInterface* iface = *it;
      if (iface->ReceiveLSA (sender, lsa) == false) {
        continue;
      }
      if ((receiver != nil) && (iface == receiver)) {
        if (((sender != nil) && ((sender->GetAddr () == iface->GetDR ()) || (sender->GetAddr () == iface->GetBDR ()))) || iface->IsBDR ()) {
          continue;
        }
      }
      sendback = true;
      OSPFLSUpd* lsupd = new OSPFLSUpd (iface);
      if ((lsupd->Size () + lsa->Size ()) <= iface->GetMTU ()) {
        lsupd->AddLSA (lsa, iface->GetDelay ());
#ifdef HAVE_QT
        iface->SendToAdjacency (LSUPDCOLOR, lsupd);
#else
        iface->SendToAdjacency (lsupd);
#endif
      }
    }
  }
  return (sendback);
}

OSPFRouter_t OSPFInterface::GetRouterID (void) const
{
  DEBUG (0, (cout << "OSPFInterface::GetRouterID" << endl));

  return (area->GetRouterID ());
}

void OSPFArea::IncAge (void)
{
  DEBUG (0, (cout << "OSPFArea::IncAge" << endl));

  for (OSPFLSARoutersIt it = rlsas.begin (); it != rlsas.end (); it++) {
    OSPFLSARouter* rlsa = *it;
    if (rlsa->IncAge () >= OSPFMAXAGE) {
      FloodOut (rlsa);
    }
  }
}

void OSPFArea::EraseNoacked (
  OSPFLSAHeader* lsa
  )
{
  DEBUG (0, (cout << "OSPFArea::EraseNoacked" << endl));

  for (OSPFInterfacesIt it = ifaces.begin (); it != ifaces.end (); it++) {
    OSPFInterface* iface = *it;
    iface->EraseNoacked (lsa);
  }
}

void OSPFArea::debug (void)
{
  cout << (string)IPAddr (GetRouterID ()) << " Database" << endl;
  for (Long_t i = 0; i < rlsas.size (); i++) {
    rlsas[i]->dump ();
  }
}

// OSPFBackbone methods
bool OSPFBackbone::ExistVLink (void)
{
  DEBUG (0, (cout << "OSPFBackbone::ExistVLink" << endl));

  for (OSPFInterfacesIt it = ifaces.begin (); it != ifaces.end (); it++) {
    OSPFInterface* iface = *it;
    if (iface->GetType () == OSPFVLINK) {
      return (true);
    }
  }
  return (false);
}

// OSPFEXRouter methods

// OSPF methods
OSPF::OSPF (
  OSPFRouter_t rid
  )
  : rid (rid), 
    // Internal data structures
    node (nil),
    status (DISABLED),
    boundary (false),
    timerEvent (nil)
{
  DEBUG (0, (cout << "OSPF::OSPF" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " created with Router ID " << (string)IPAddr (rid) << endl));
}

OSPF::~OSPF (void)
{
  DEBUG (0, (cout << "OSPF::~OSPF" << endl));
  DEBUG (1, (cout << "\tInstance " << this << " destroyed" << endl));

  for (OSPFAreasIt it = areas.begin (); it != areas.end (); it++) {
    OSPFArea* area = *it;
    delete (area);
  }
  areas.clear ();

  for (OSPFEXRoutersIt it = exrouters.begin (); it != exrouters.end (); it++) {
    OSPFEXRouter* exrouter = *it;
    delete (exrouter);
  }
  exrouters.clear ();

  for (OSPFLSAASExtsIt it = asexts.begin (); it != asexts.end (); it++) {
    OSPFLSAASExt* asext = *it;
    delete (asext);
  }
  asexts.clear ();

  ClearRouting ();

  ClearTimer ();
}

void OSPF::AttachNode (
  Node* node
  )
{
  DEBUG (0, (cout << "OSPF::AttachNode" << endl));
  DEBUG (1, (cout << "\tNode " << node->Id () << " attached" << endl));

  if (node->IsReal ()) {
    this->node = node;
    this->node->InsertProto (OSPFLAYER, OSPFPROTO, this);
  }
}

void OSPF::Handle (
  Event* event, 
  Time_t time
  )
{
  DEBUG (0, (cout << "OSPF::Handle" << endl));
  DEBUG (1, (cout << "\tEvent " << event->event << " at " << time << endl));

  switch ((Event_t)event->event) {
    case START:
      if (status == DISABLED) {
        status = ENABLED;
        for (OSPFAreasIt ait = areas.begin (); ait != areas.end (); ait++) {
          OSPFArea* area = *ait;
          OSPFInterfaces& ifaces = area->GetInterfaces ();
          for (OSPFInterfacesIt iit = ifaces.begin ();  iit != ifaces.end (); iit++) {
            OSPFInterface* iface = *iit;
            iface->TriggerEvent (OSPFInterface::INTERFACEUP);
          }
        }
        TriggerTimer ();
      }
      break;
    case STOP:
      if (status == ENABLED) {
        ClearTimer ();
        for (OSPFAreasIt ait = areas.begin (); ait != areas.end (); ait++) {
          OSPFArea* area = *ait;
          OSPFInterfaces& ifaces = area->GetInterfaces ();
          for (OSPFInterfacesIt iit = ifaces.begin (); iit != ifaces.end (); iit++) {
            OSPFInterface* iface = *iit;
            iface->TriggerEvent (OSPFInterface::INTERFACEDOWN);
          }
        }
        status = DISABLED;
      }
      break;
    case TIMER:
      timerEvent = nil;
      IncAge ();
      break;
    default:
      break;
  }

  delete event;
}

void OSPF::DataIndication (
  Node* node, 
  Packet* packet, 
  IPAddr_t src,
  L3Protocol&,
  Interface* in
  )
{
  DEBUG (0, (cout << "OSPF::DataIndication" << endl));
  DEBUG (1, (cout << "\tNode " << node->Id () << endl));

  if (status == ENABLED) {
    OSPFInterface* iface;
    IPAddr_t des = IPAddrBroadcast; // Should be destination address
    IPAddr_t ifaddr = in->GetIPAddr ();
    if ((iface = CheckPacket (src, des, ifaddr)) != nil) {
      OSPFPacket* header = (OSPFPacket*)packet->PeekPDU ();
      if (header->CheckPacket (src, des, iface)) {
//cout << (string)IPAddr (rid) + " rece ";
        switch (header->GetType ()) {
          case OSPFPacket::HELLO:
            {
              OSPFHello* hello = (OSPFHello*)header;
//hello->dump ();
              if (hello->CheckPacket (iface)) {
                iface->ReceiveHello (src, hello);
              }
            }
            break;
          case OSPFPacket::DATAD:
            {
              OSPFDataD* dd = (OSPFDataD*)header;
//dd->dump ();
              OSPFNeighbor* neighbor;
              if ((neighbor = iface->FindNeighbor (src, dd->GetAdvID ())) != nil) {
                neighbor->ReceiveDataD (dd);
              }
            }
            break;
          case OSPFPacket::LSREQ:
            {
              OSPFLSReq* lsreq = (OSPFLSReq*)header;
//lsreq->dump ();
              OSPFNeighbor* neighbor;
              if ((neighbor = iface->FindNeighbor (src, lsreq->GetAdvID ())) != nil) {
                neighbor->ReceiveLSReq (lsreq);
              }
            }
            break;
          case OSPFPacket::LSUPD:
            {
              OSPFLSUpd* lsupd = (OSPFLSUpd*)header;
//lsupd->dump ();
              OSPFNeighbor* neighbor;
              if ((neighbor = iface->FindNeighbor (src, lsupd->GetAdvID ())) != nil) {
                neighbor->ReceiveLSUpd (lsupd);
              }
            }
            break;
          case OSPFPacket::LSACK:
            {
              OSPFLSAck* lsack = (OSPFLSAck*)header;
//lsack->dump ();
              OSPFNeighbor* neighbor;
              if ((neighbor = iface->FindNeighbor (src, lsack->GetAdvID ())) != nil) {
                neighbor->ReceiveLSAck (lsack);
              }
            }
            break;
          default:
            break;
        }
      }
    }
  }

  delete packet; 
}

OSPFArea* OSPF::AddArea (
  OSPFArea_t aid
  )
{
  DEBUG (0, (cout << "OSPF::AddArea" << endl));
  DEBUG (1, (cout << "\tArea ID " << (string)IPAddr (aid) << endl));

  OSPFArea* area;
  if (aid == OSPFBACKBONE) {
    area = new OSPFBackbone (this);
  }
  else {
    area = new OSPFArea (aid, this);
  }
  areas.push_back (area);
  return (area);
}

OSPFArea* OSPF::FindArea (
  OSPFArea_t aid
  )
{
  DEBUG (0, (cout << "OSPF::FindArea" << endl));
  DEBUG (1, (cout << "\tArea ID " << (string)IPAddr (aid) << endl));

  for (OSPFAreasIt it = areas.begin (); it != areas.end (); it++) {
    OSPFArea* area = *it;
    if (area->GetAreaID () == aid) {
      return (area);
    }
  }
  return (nil);
}

OSPFInterface* OSPF::FindInterface (
  IPAddr_t addr
  )
{
  DEBUG (0, (cout << "OSPF::FindInterface" << endl));
  DEBUG (1, (cout << "\tIP " << (string)IPAddr (addr) << endl));

  for (OSPFAreasIt it = areas.begin (); it != areas.end (); it++) {
    OSPFArea* area = *it;
    OSPFInterface* iface = area->FindInterface (addr);
    if (iface != nil) {
      return (iface);
    }
  }
  return (nil);
}

bool OSPF::IsVLinkEnd (void)
{
  DEBUG (0, (cout << "OSPF::IsVLinkEnd" << endl));

  OSPFBackbone* backbone = FindBackbone ();
  if ((backbone != nil) && backbone->ExistVLink ()) {
    return (true);
  }
  else {
    return (false);
  }
}

OSPFInterface* OSPF::CheckPacket (
  IPAddr_t src,
  IPAddr_t des,
  IPAddr_t ifaddr
  )
{
  DEBUG (0, (cout << "OSPF::CheckPacket" << endl));

  OSPFInterface* iface;
  if ((iface = FindInterface (ifaddr)) != nil) {
    if ((des == OSPFALLROUTERS) || (des == OSPFALLDROUTERS) || (des == ifaddr)) { 
      if (src != ifaddr) {
        return (iface);
      }
    }
  }
  return (nil);
}

void OSPF::ClearTimer (void)
{
  DEBUG (0, (cout << "OSPF::ClearTimer" << endl));

  if (timerEvent != nil) {
    Scheduler::Cancel (timerEvent);
    delete timerEvent;
    timerEvent = nil;
  }
}

void OSPF::TriggerTimer (void)
{
  DEBUG (0, (cout << "OSPF::TriggerTimer" << endl));

  ClearTimer ();
  timerEvent = new Event (TIMER);
  Scheduler::Schedule (timerEvent, 1.0, this);
}

void OSPF::IncAge (void)
{
  DEBUG (0, (cout << "OSPF::TimerAction" << endl));

  for (OSPFAreasIt it = areas.begin (); it != areas.end (); it++) {
    OSPFArea* area = *it;
    area->IncAge ();
  }
  IncASExtAge ();

  TriggerTimer ();
}

void OSPF::IncASExtAge (void)
{
  DEBUG (0, (cout << "OSPF::IncASExtAge" << endl));
// Do it later
}

void OSPF::ClearRouting (void)
{
  DEBUG (0, (cout << "OSPF::ClearRouting" << endl));

  for (OSPFRoutingsIt it = table.begin (); it != table.end (); it++) {
    OSPFRouting* routing = *it;
    delete routing;
  }
  table.clear ();
}

void OSPF::AddRouting (
  OSPFLSA_t type,
  OSPFID_t id,
  Mask_t mask,
  Bitmap_t option,
  OSPFArea_t aid,
  OSPFRouting::Path_t path,
  OSPFCost_t cost,
  IPAddr_t out,
  bool stub
  )
{
  DEBUG (0, (cout << "OSPF::AddRouting" << endl));
  // T.J.
  Interface* out_If = node->GetIfByIP(out);

  if(out_If != nil) {

    OSPFRouting* entry = new OSPFRouting (type, id, mask, option, aid, path, cost);
    // T.J. :
    RouteTable* rTable = node->getRouteTable();
    rt_key_t* rt_Key = new rt_key_t(id, Mask( mask).NBits());
    //rt_value_t* rt_Value = new rt_value_t(out, out_If);
    Linkp2p* link = (Linkp2p*)out_If->GetLink();
    Interface* outifPeer = (Interface*)link->pPeer;
    
    rt_value_t *rt_Value = new rt_value_t(outifPeer->GetIPAddr(),
                                          out_If);
    // T.J.

    
    for (OSPFRoutingsIt it = table.begin (); it != table.end (); it++) {
      OSPFRouting* routing = *it;
      if (routing->IsIt (entry)) {
	if (stub == true) {
	  if (routing->GetCost () > cost) {
	    delete routing;
	    table.erase (it);          // better path, delete the old one, and add the new one (down later on)
	    // T.J. :delete the entry from the FIB
	    if(type == OSPFROUTER)
	      rTable->DelRoute(rt_Key);
	    // T.J.
	    
	  }
	  else {
	    if (routing->GetCost () == cost) {
	      routing->AddNexthop (out);  // equal cost path, just add the new next hop
	      // T.J. : Add the equal-cost next hop to the FIB
	      if(type == OSPFROUTER)
                  rTable->AddRoute(rt_Key, rt_Value);
	      // T.J.
	    }
	    delete entry;
	    return;
	  }
	}
	else {
	  if (routing->GetCost () != cost) {
	    delete routing;
	    table.erase (it);
	    // T.J. :delete the entry from the FIB
	    if(type == OSPFROUTER)
	      rTable->DelRoute(rt_Key);
	    // T.J.
	  }
	  else {
	    routing->AddNexthop (out);
	    delete entry;
	    // T.J.
	    if(type == OSPFROUTER)
	      rTable->AddRoute(rt_Key, rt_Value);
	    // T.J.
	    return;
	  }
	}
      }
    }
    entry->AddNexthop (out);
    table.push_back (entry);
    // T.J. :add the new path to the FIB
    if(type == OSPFROUTER)
      rTable->AddRoute(rt_Key, rt_Value);
    // T.J.
  }
}

void OSPF::DumpRouting (void)
{
  DEBUG (0, (cout << "OSPF::DumpRouting" << endl));

  cout << (string)IPAddr (GetRouterID ()) << " Routing table" << endl;
  for (OSPFRoutingsIt it = table.begin (); it != table.end (); it++) {
    OSPFRouting* routing = *it;
    routing->dump ();
  }

  // Dump the FIB
  cout << endl << (string)IPAddr (GetRouterID ()) << " FIB Table" << endl;
  RouteTable* rTable = node->getRouteTable();
  rTable->Dump();

  cout << endl;
}

