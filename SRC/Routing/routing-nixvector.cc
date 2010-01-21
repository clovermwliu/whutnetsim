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




// Georgia Tech Network Simulator - NixVector Routing class
// George F. Riley.  Georgia Tech, Spring 2002

// NixVector routing method

// NixVector routing is described in two research papers:
//    "Stateless Routing in Network Simulations", MASCOTS 2000
//    "Efficient Routing using NixVectors", HPSR 2001
// both by George F. Riley

//2010.01.04 modified by ��Цӯ

//#define DEBUG_MASK 0x4

#include "routing-nixvector.h"

// Constructor (empty)
RoutingNixVector::RoutingNixVector() : Routing()
/*
���������췽�����̳�Routing��
*/
{
}

// Destructor
RoutingNixVector::~RoutingNixVector()
/*
�������������������cache
*/
{ // Delete all the cached nix vectors
  cache.clear();
}

void RoutingNixVector::Default(RoutingEntry r)
/*
����������Ĭ��·�ɣ�Ĭ����һ����Ϣ��
*/
{ // Save default route to use when no NixVec can be calculated
  defaultRoute = r;
}

RoutingEntry RoutingNixVector::GetDefault()
/*
����������Ĭ��·��
*/
{
  return defaultRoute;
}

void RoutingNixVector::Add( IPAddr_t a, Count_t m,
                            Interface* i, IPAddr_t ip)
/*
��������Routing���м̳����ķ�����δʹ��
*/
{ // Ignored for nixvector routing
}

RoutingEntry RoutingNixVector::Lookup(Node*, IPAddr_t t)
/*
��������Routing���м̳����ķ��������ҽ�㵽Ŀ��IP����һ����Ϣ�����ﷵ��һ����RoutingEntry����
*/
{ // Lookup routing entry for specified target address
  // Return an empty one to force a NV creation
  return RoutingEntry();
}

RoutingEntry RoutingNixVector::LookupFromPDU(PDU*)
/*
����:��PDU�в�����һ��·�ɣ�δʵ��
*/
{// Find from PDU (NixVector)..not coded yet
  return RoutingEntry();
}

Routing* RoutingNixVector::Clone()
/*
����������һ���µ�RoutingNixVector����
*/
{
  return new RoutingNixVector();
}

Routing::RType_t RoutingNixVector::Type()
/*
���������ص�ǰ·�����ͣ���NIXVECTOR
*/
{
  return NIXVECTOR;
}

void    RoutingNixVector::ReInitializeRoutes(Node* n, bool up)
/*
�����������˸ı�ʱ�����߸���·��ʱ�����³�ʼ��·��cache
������[IN]n���Ӹý�㿪ʼ�����ж�
      [IN]up�������n״̬ΪupʱΪtrue������Ϊfalse
*/
{ // Validate the existing nix-vectors
  if (cache.empty()) return; // Nothing to do
  if (up)
    { // Since "up" actions mean there may be a better route
      // we simply delete all existing nix-vectors and rebuild later.
      cache.clear();
      return;
    }
  // For "down" actions, we can simply check each existing
  // NV to make sure it does not cross down nodes or interfaces
  for (NixMap_t::iterator i = cache.begin(); i != cache.end(); )
    {
      NixMap_t::iterator next = i;
      next++;
      DEBUG0((cout << "Node " << n->Id() 
              << " routing " << this
              << " dest " << (string)IPAddr(i->first)
              << " nvaddr " << &i->second
              << " nvlth " << i->second.Remaining()
              << " used " << i->second.used
              << " size " << i->second.size << endl));
	  //�жϸ���㡢Interface��Link�Ƿ���Ч
      if (!ValidateNixVector(n, i->first, i->second))
        { // No loner valid, delete
          cache.erase(i);//ȥ����Ч����
        }
      i = next;
    }
}
   
bool    RoutingNixVector::NeedReInit()
/*
�������Ƿ���Ҫ���³�ʼ��·�����ݽṹ���޾����жϣ�ֱ�ӷ���true
*/
{ // Re-Initialization is needed for nixvector routing
  return true;
}

Size_t  RoutingNixVector::Size() const
/*
��������Routing��̳����ķ���������FIB�ṹ�Ĵ�С����RoutingNixVector��δʹ�øýṹ������0
*/
{ // For statistics, return the size (bytes) of the FIB
  return 0;
}

NixVectorOption* RoutingNixVector::GetCachedNixVector(IPAddr_t i)
/*
����������cache��ָ��Ŀ��IP��Ӧ��NixVectorOptionָ�룬��ָ��IP�����ڣ��򷵻ؿ�
*/
{
  // First see if we have a cached NixVector to the specified target
  NixMap_t::iterator it = cache.find(i);
  if (it != cache.end())
    { // Found a cached entry, return a new copy of it
      Stats::nvReused++;
      return new NixVectorOption(it->second);
    }
  return nil;
}

NixVectorOption* RoutingNixVector::GetNixVector(Node* r, IPAddr_t i)
/*
��������cache�в���Ŀ��IP������ҵ����򷵻ض�Ӧ��NixVectorOptionָ�룻
      ���û���ҵ��������BFS�����鵽���r��Ŀ��IP��·�ɣ���ͨ������BuildNixVector�������ض�Ӧ��NixVectorOptionָ��
������[IN]r�����ø÷����Ľ��ָ��
      [IN]i��Ŀ��IP
����ֵ��NixVectorOptionָ��
*/
{
  // First see if we have a cached NixVector to the specified target
  //���Ȳ���cache���Ƿ��Ѿ�����Ŀ��IP�������ڣ���ֱ�ӷ������Ӧ��NixVectorOptionָ��
  NixMap_t::iterator it = cache.find(i);
  if (it != cache.end())
    { // Found a cached entry, return a new copy of it
      Stats::nvReused++;
      DEBUG0((cout << "Node " << node->Id()
              << " routing " << this
              << " destip " << (string)IPAddr(it->first)
              << " reusing nvaddr " << &it->second
              << " size " << it->second.size
              << endl));
      return new NixVectorOption(it->second);
    }
  // No cached vector, create a new one
  //û���ҵ�Ŀ��IP��Ӧ��cache����´���һ��
  NodeIfVec_t      nextHop;      // Next hop vector
  NodeVec_t        parent;       // Parent vector
  IPAddrVec_t      aliases;      // List of IP's with same path
  const NodeVec_t& nodes = Node::GetNodes();
  Stats::nvCreated++;
  DEBUG0((cout << "Calculating nv from " << (string)IPAddr(r->GetIPAddr())
          << " to target " << (string)IPAddr(i) << endl));
  //����BFS�������ҽ��r��Ŀ��IP��·��
  NodeId_t targetId = BFS(nodes, r, nextHop, parent, i, aliases);
  DEBUG0((cout << "Found BFS target id " << targetId
          << " to target " << (string)IPAddr(i) << endl));
  DEBUG(1,(cout << "BFS Found targetid " << targetId << endl));
  if (targetId == NODE_NONE)
    {
      DEBUG(2,(cout << "HuH?  Can't find NV to destination "
               << (string)IPAddr(i) << endl));
      return nil;
    }
#ifdef VERVOSE_DEBUG
  // verbose debug...remove
  for (NodeVec_t::size_type j = 0; j < parent.size(); ++j)
    {
      Node* nn = parent[j];
      cout << "Parent " << j << " is ";
      if (nn) cout << nn->Id() << endl;
      else    cout << "nil" << endl;
    }
#endif
  DEBUG0((cout << "Found target Id " << targetId << endl));
  NixVectorOption* nv = new NixVectorOption();
  // Create the NixVector by recursing backwards through the parent list
  //
  BuildNixVector(r, nodes[targetId], parent, *nv);
  nv->Reset();  // Reset used and size
  // Add to the cache
  DEBUG(1,(cout << "Inserting new nv in cache " << nv 
           << " nv->nixVec " << nv->nixVector << endl));
  //���뵽cache��
  cache.insert(NixMap_t::value_type(i, *nv));
#undef  USE_ALIASES
#ifdef  USE_ALIASES
  cout << "Adding aliases" << endl;
  // And NV's for all IP's with same path
  for (IPAddrVec_t::size_type j = 0; j < aliases.size(); ++j)
    {
      if (aliases[j] != i)
        { // Not the target already logged
          DEBUG0((cout << "Inserting alias for "
                  << (string)IPAddr(aliases[j])
                  << endl));
          cache.insert(NixMap_t::value_type(aliases[j], *nv));
        }
    }
#endif
  DEBUG0((cout << "Build NixVector is "));
  DEBUG0((nv->Dump()));
  DEBUG(1,(cout << "Done inserting" << endl));
  DEBUG0((cout << "Done Calculating nv to target " << (string)IPAddr(i)
          << endl));
  return nv;
}

bool RoutingNixVector::ValidateNixVector(Node* n, IPAddr_t d,
                                         NixVectorOption nv)
/*
�������ӽ��n��ʼ��ͨ��Ѱ�Ҹ������ھӽ�㣬�ж������и���㡢Interface��Link�Ƿ��Ѿ���Ч��Down��
������[IN]n:ָ�����ָ��
      [IN]d:δʹ��
	  [IN]nv:NixVectorOption����ͷ
����ֵ������ҵ���ĳ������Interface��Link�Ѿ���Ч���򷵻�false�����򷵻�true
*/
{ // Insure the NixVector does not cross "down" nodes or interfaces
  // Note pass by value for the NixVectorOption.  This is necessary
  // since  we change the "remaining" value, and we don't want to
  // change  it in the actual nv cache.
  while(nv.Remaining())
    {
      NixBits_t       nix = nv.Extract(nv.BitCount(n->NeighborCount()));
      InterfaceBasic* iface = nil;
      Count_t         cnc = 0;      // Cumulative neighbor count
      const IFVec_t&  interfaces = n->Interfaces();
      // Find which interface has the specified nix
      for (IFVec_t::size_type i = 0; i < interfaces.size(); ++i)
        {
          Count_t nc = interfaces[i]->NeighborCount(n);
          Count_t priorCnc = cnc;
          cnc += nc; // Advance cumulative neighbor count
          if (nix < cnc)
            { // Yes, it's on this interface
              iface = interfaces[i];
              if (iface->IsDown()) return false;
              Link* link = iface->GetLink();
              iface = link->GetPeer(nix - priorCnc);
              break;
            }
        }
      if (!iface) return false;
      if (iface->IsDown()) return false;
      // Advance to next node
      n = iface->GetNode();
      if (n->IsDown()) return false;
    }
  return true;
}

  
void RoutingNixVector::DBDump(Node* n)
/*
�����������ã������ǰ����ip��ַ�͵�ǰcache�Ĵ�С
*/
{ // Dump debug info
  if (cache.size())
    {
      cout << "NixVector info for node " << (string)IPAddr(n->GetIPAddr()) 
           << endl;
      cout << "  Number Cached NV's " << cache.size() << endl;
    }
}
// Static Methods
void RoutingNixVector::BuildNixVector(Node* src, Node* current,
                                       NodeVec_t& parentVec,
                                       NixVectorOption& nv)
/*
������ͨ���ݹ鷽�������ƴӽ��src��currentʱ������ʹ�õ�NixVectorOption
������[IN]src:���ø÷����Ľ��ָ��
      [IN]current:��ǰ��Ŀ����
	  [IN]parentVec���ӽ��src��������ʱ��Ŀ�����ǰһ�����
	  [IN][OUT]nv:�ӽ��src��Ŀ���㣬����ʹ�õ�NixVectorOptionָ��
����ֵ����
*/
{ // Build the nix vector
  if (src == current) return; //��ǰ����Ŀ�Ľ����ͬʱ�˳����ݹ����
  if (parentVec[current->Id()] == nil)
    {
      cout << "RoutingNixVector::BuildNixVector..no path to target" << endl;
      return;
    }
  // Recurse until we get to the source
  if (current == parentVec[current->Id()])
    {
      cout << "HuH?   Node " << current->Id() << " is own parent" << endl;
      exit(1);
    }
  //�ݹ���ã���һ��currentΪ��ǰcurrent��parentVec�е�Parent 
  BuildNixVector(src, parentVec[current->Id()], parentVec, nv);
  // Now append the neighbor bits to the nix vector
  //��ǰcurrent��parentVec�е�Parent
  Node* n = parentVec[current->Id()];  // Parent node
  //���current�ǽ��n�ĵڼ����ھӽ��
  Count_t nix = n->GetNix(current);    // Get Nix from parent to this
  if (nix == MAX_COUNT)
    { // HuH? Can get the nix value
      cout << "RoutingNixVector::BuildNixVector..No nix value" << endl;
      return;
    }
  Count_t nc  = n->NeighborCount();    // Total number of neighbors 
  DEBUG(4,(cout << "nix " << nix << " nc " << nc << endl));
  //
  nv.Add(nix, nv.BitCount(nc));        // Add the bits to the nix vector
}

// NixVerouting-static.hctorOption methods

NixVectorOption::NixVectorOption() 
/*
���������캯������ʼ������ͷ�Ĵ�СΪNIX_BPW
*/
  : PDU(), nixVector(0), used(0), size(NIX_BPW)
{
  DEBUG(1,(cout << "Constructing NV " << this << endl));
  Stats::nvAllocated++;
}

// Copy Constructor
NixVectorOption::NixVectorOption(const NixVectorOption& c)
  : PDU(c), used(c.used), size(c.size)
/*
���������ƹ��캯��
*/
{
  DEBUG(1,(cout << "Constructing nv (Copy) " << this 
           << " c.nv " << c.nixVector << endl));
  if (size <= NIX_BPW)
    {
      nixVector = c.nixVector;
    }
  else
    { // Need to realloc and copy
      Size_t s = WSize();
      nixVector = new NixBits_t[s];
      DEBUG(1,(cout << "Allocated NV (copy const) " << nixVector 
               << " WSize " << WSize() << endl));
      memcpy(nixVector, c.nixVector, s * sizeof(NixBits_t));
    }
  DEBUG(1,(cout << "Exit CC, nv " << nixVector << endl));
  Stats::nvAllocated++;
}

NixVectorOption::NixVectorOption(char* b, Size_t& sz)
  : PDU()
/*
���������캯����ָ������ͷ����
*/
{
  DEBUG0((cout << "Constructing nixvector, initial size " << sz << endl));
  Construct(b, sz);
  DEBUG0((cout << "Constructing nixvector, final size " << sz << endl));
  Stats::nvAllocated++;
}

// Destructor
NixVectorOption::~NixVectorOption()
/*
�����������������ͷ�һЩ�ռ�
*/
{
  if (size > NIX_BPW)
    {
      DEBUG(1,(cout << "Deleting nv (Detruct) " << this
               << " nv " << nixVector << endl));
      delete [] nixVector;
    }
  Stats::nvDeleted++;
}

// Assignment operator
NixVectorOption& NixVectorOption::operator=(const NixVectorOption& r)
/*
����������=����������ʾ��=�ұߵ�NixVectorOption����һ�ݸ���=�ұߵ�NixVectorOption�Ͷ���
*/
{
  DEBUG0((cout << "Hello from NV Assignment" << endl));
  if (&r != this)
    { // Not self assignment
      if (size > NIX_BPW)
        { // Delete existing vector
          DEBUG(1,(cout << "Deleting nv (Assignment) " << nixVector << endl));
          delete [] nixVector;
        }
      size = r.size;
      used = r.used;
      if (size <= NIX_BPW)
        { // Not a pointer, just the value
          nixVector = r.nixVector;
        }
      else
        { // Need to realloc and copy
          Size_t s = WSize();
          nixVector = new NixBits_t[s];
          DEBUG(1,(cout << "Allocated nv (Assignment) " << nixVector 
                   << " WSize() " << WSize() << endl));
          memcpy(nixVector, r.nixVector, s * sizeof(NixBits_t));
        }
    }
  return *this;
}

Size_t NixVectorOption::Size() const
/*
��������PDU��̳����ķ�����δʵ�֣�ֱ�ӷ���0
*/
{
  return 0;
}

PDU* NixVectorOption::Copy() const
/*
���������ص�ǰNixVectorOption����Ŀ���
*/
{ // Make a copy of this NixVector
  return new NixVectorOption(*this);
}

// Serialization
Size_t    NixVectorOption::SSize()
/*
���������ش��л�ʱ���õ��ı�ͷ����ʵ���ȣ����PDU��
*/
{ // Size needed for serialization
  //Size_t r = sizeof(optionNumber) + sizeof(used) + sizeof(size);
  Size_t r = sizeof(used) + sizeof(size);
  // Add size of actual NV
  r += WSize() * sizeof(NixBits_t);
  return r;
}

char*     NixVectorOption::Serialize(char* b, Size_t& sz)
/*
���������л������PDU��
*/
{ // Serialize to a buffer
  //b = SerializeToBuffer(b, sz, optionNumber);
  b = SerializeToBuffer(b, sz, used);
  b = SerializeToBuffer(b, sz, size);
  if (WSize() == 1)
    { // just one word
      NixBits_t nb = (NixBits_t)nixVector;
      b = SerializeToBuffer(b, sz, nb);
    }
  else
    { // Need to serialize each one
      for (Count_t i = 0; i < WSize(); ++i)
        {
          b = SerializeToBuffer(b, sz, nixVector[i]);
        }
    }
  return b;
}

char*     NixVectorOption::Construct(char* b, Size_t& sz)
/*
������ָ�����ݣ����챨��ͷ�����캯�����ã����PDU��
*/
{ // Construct from buffer
  //b = ConstructFromBuffer(b, sz, optionNumber);
  //����serializable.cc�еĺ��� 
  b = ConstructFromBuffer(b, sz, used);
  b = ConstructFromBuffer(b, sz, size);
  if (WSize() == 1)
    { // just one word
      NixBits_t nb = 0;
      b = ConstructFromBuffer(b, sz, nb);
      nixVector = (NixBits_t*)nb;
    }
  else
    {
      nixVector = new NixBits_t[WSize()];
      DEBUG(1,(cout << "Allocated NV (Construct) " << nixVector
               << " WSize() " << WSize() << endl));
      for (Count_t i = 0; i < WSize(); ++i)
        {
          b = ConstructFromBuffer(b, sz, nixVector[i]);
        }
    }
  return b;
}

void NixVectorOption::Add(NixBits_t b, Count_t n)
/*
��������b�е�used+1��n+usedλ�����뵽nb�Ķ�Ӧλ�У�
      �����ǰ��ͷʣ��ռ䲻��������չ��32λ�����ٲ���
*/
{ // Add the n bits specified by b (b is right justified)
  if (WSize() == 1 && (used + n) <= NIX_BPW)//��ǰʣ��ռ��㹻
    { // Simplest case, still less than one word
      NixBits_t nb = (NixBits_t)nixVector;
      InsertBits(nb, used, b, n);//����b��nb�У���b�е�used+1��n+usedλ�����뵽nb�Ķ�Ӧλ��
      nixVector = (NixBits_t*)nb;   // Replace in the pointer field
      used += n;                    // Adjust used
      DEBUG(4,(cout << "Added " << b << " count " << n 
               << " used " << used << endl));
      DEBUG(4,(Dump()));
      return;
    }
  if (WSize() == 1)//��ǰ����ͷ�ռ䲻������Ҫ��չ
    { // Size is less than one word, need to expand size
      Expand();
    }
  //��չ֮���ٲ���b
  // At this point we are sure that we have an array in nixVector
  Count_t startWord = used / NIX_BPW;
  Count_t endWord = (used + n - 1) / NIX_BPW;
  Count_t usedWord = used % 32;
  while(startWord >= WSize()) 
	  Expand();
  if (startWord == endWord)//��ͬһ�֣�32λ����
    { // Simple case, fits in same word
      InsertBits(nixVector[startWord], usedWord, b, n);
      used += n;                   // Adjust used and size
      return;
    }
  // Spans two words
  Count_t   firstCount = NIX_BPW - usedWord;
  NixBits_t fb = b >> (n - firstCount);   // Upper bits that fit in first
  InsertBits(nixVector[startWord], usedWord, fb, firstCount);
  Count_t   endCount = n - firstCount;
  while (endWord >= WSize())
	  Expand();    // Expand until big enough
  InsertBits(nixVector[endWord], 0, b, endCount); //����b��nb�У���b�е�1��endCountλ�����뵽nixVector[endWord]�Ķ�Ӧλ��
  used += n;                        // Adjust used
}

NixBits_t NixVectorOption::Extract(Count_t n)
/*
��������ȡ��ǰ����ͷ�д�used+1��ʼ��nλ
*/
{
  DEBUG0((cout << "Extracting " << n << " bits from nv, used " << used 
          << " remaining " << Remaining() << endl));
  if ((used + n) > size) return NIX_NONE;     // Error, out of range
  Count_t startWord = used / NIX_BPW;
  Count_t endWord = (used + n - 1) / NIX_BPW;
  Count_t usedWord = used % 32;
  NixBits_t b = 0;
  if (startWord == endWord)//��ͬһ�֣�32λ����
    { // Simple case, both in same word
      Mask_t m = BitMask(usedWord, n);//32-usedWordλ������������n��1��32-n-usedWord��0���
      if (WSize() == 1)//����ͷֻ��һ���֣�32λ��
        { // Not a pointer, just the value
          b = ((NixBits_t)nixVector) & m;//��ȡnixVector�д�usedWord+1��ʼ��nλ
        }
      else//����ͷ���Ȳ�ֹһ���֣���Ҫ��ȷ����ʼ���ֵ�λ��
        {
          DEBUG0((cout << "NV Extracting, startword " << startWord
                  << " nv[sw] " << Hex8(nixVector[startWord])
                  << " m " << Hex8(m) << endl));
          b = nixVector[startWord] & m; //��ȡnixVector[startWord]�д�usedWord+1��ʼ��nλ
        }
      DEBUG0((cout << "Shifting " << Hex8(b)
              << " by " << NIX_BPW - (usedWord + n) << endl));
      b >>= NIX_BPW - (usedWord + n);         // Position to lower bits
    }
  else//����ͬһ�֣�32λ����
    {
      Count_t thisBits = NIX_BPW - usedWord;
      Mask_t m = BitMask(usedWord, thisBits);
      NixBits_t ub = nixVector[startWord] & m;
      Count_t nextBits = n - thisBits;
      ub <<= nextBits;
      m = BitMask(0, nextBits);
      NixBits_t lb = nixVector[endWord] & m;
      lb >>= NIX_BPW - nextBits;
      b = ub | lb;
    }
  used += n;
  DEBUG0((cout << "Done Extracting " << n << " bits from nv, used " << used 
          << " remaining " << Remaining() << endl));
  return b;
}

Count_t NixVectorOption::Remaining()
/*
���������ظ�NixVectorOption����ͷ��δʹ�õĳ���
*/
{
  if (used <= size) return size - used;
  return 0; // Should never get here
}

void NixVectorOption::Reset()
/*
���������ñ�ͷ���ѵ�ǰ���õĴ�С��Ϊ�µ��ܴ�С�����õĴ�С��Ϊ0
*/
{ // Reset size and used
  size = used;
  used = 0;
}

void NixVectorOption::Dump()
/*
������������,���Э��ͷ��Ϣ
*/
{ // For debugging, dump the nix vector
  cout << "nv";
  if (WSize() == 1)
    { // only one word
      NixBits_t nb = (NixBits_t)nixVector;
      cout << " " << Hex8(nb) << endl;
      return;
    }
  for (Size_t i = 0; i < WSize(); ++i)
    {
      cout << " " << Hex8(nixVector[i]);
    }
  cout << endl;
}

// Private methods

Count_t NixVectorOption::WSize()
/*
�������ѱ���ͷ�ܳ���size�ĵ�λת��Ϊword��������
*/
{ // Get size rounded up to words
  return (size + NIX_BPW - 1) / NIX_BPW;
}

Mask_t NixVectorOption::BitMask(Count_t pos, Count_t nbits)
/*
��������λ����������0x80000000��������nbits-1λ(ǰ�油1)������߼�����posλ��ǰ�油0�����ڲ������ұ���ͷʱʹ��
���أ�32-posλ������������nbits��1��32-nbit-pos��0���
*/
{ // Get a mask at pos, nbits.  Bits in NixBits_t are 0 - 31 from MSB
  long m = 0x80000000;      // one bit mask at position 0
  m >>= (nbits - 1);//��������        // nbits bit mask with upper bit at position 0
  unsigned long um = (unsigned long)m;
  um >>= pos;//�߼�����               // nbits starting at position pos
  return um;
}

void NixVectorOption::Expand()
/*
��������ǰ����ͷ����ռ䲻��ʱ����չ32λ
*/
{ // Expand the size by one word
  DEBUG(1,(cout << "Expanding NV, WSize() " << WSize() << endl));
  if (WSize() == 1)
    { // Currently only one
      NixBits_t nb = (NixBits_t)nixVector;
      size += NIX_BPW;
      nixVector = new NixBits_t[WSize()];
      nixVector[0] = nb;
      nixVector[1] = 0;
    }
  else
    { // Realloc, copy, and delete old
      NixBits_t* nb = nixVector;
      size += NIX_BPW;
      nixVector = new NixBits_t[WSize()];
      DEBUG(1,(cout << "Allocated NV (Expand) " << nixVector << endl));
      memcpy(nixVector, nb, (WSize() -1 ) * sizeof(NixBits_t));
      DEBUG(1,(cout << "Deleting nv (expand) " << nb << endl));
      delete [] nb;
    }
}

void NixVectorOption::InsertBits(
                                 NixBits_t& nb,
                                 Count_t    bitNum,
                                 NixBits_t  b,
                                 Count_t    n)
/*
����������b��nb�У���b�е�bitNum+1��n+bitNumλ�����뵽nb�Ķ�Ӧλ��
*/
{
  Mask_t m = BitMask(bitNum, n);//���أ�32-bitNumλ������������n��1��32-n-bitNum��0���
  nb &= ~m; //��nb��bitNum+1��n+bitNumλ��������nλ
  b <<= 32 - bitNum - n;//b����32 - bitNum - nλ����32 - bitNum - nλ��0
  DEBUG(4,(cout << "IB m " << Hex8(m)
           << " b " << Hex8(b)
           << " b & m " << Hex8(b & m) << endl));
  nb |= (b & m);//��b�е�bitNum+1��n+bitNumλ�����뵽nb�Ķ�Ӧλ��
}

Count_t NixVectorOption::BitCount(Count_t m)
/*
���������ش���m����ı�����
*/
{ // m is the number of neighbors, returns number of bits needed
  if ( m <= 2) return 1;
  if ( m <= 4) return 2;
  if ( m <= 8) return 3;
  if ( m <= 16) return 4;
  if ( m <= 32) return 5;
  if ( m <= 64) return 6;
  if ( m <= 128) return 7;
  if ( m <= 256) return 8;
  if ( m <= 512) return 9;
  if ( m <= 1024) return 10;
  if ( m <= 2048) return 11;
  if ( m <= 4096) return 12;
  if ( m <= 8192) return 13;
  if ( m <= 16384) return 14;
  if ( m <= 32768) return 15;
  return 16;
}
