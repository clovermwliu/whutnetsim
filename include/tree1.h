
#ifndef __tree1_h__ 
#define __tree1_h__ 

#include "G_common_defs.h"
#include "node.h"
#include "mask.h"
#include "PlatTopoBase.h"

class Node;
class Linkp2p;
class Queue;

#define N_FANOUT 4
#define N_DEPTH 3
#define N_TREES 5

//Doc:ClassXRef
class Tree1 : public CPlatTopoBase
{
  //Doc:Class Defines a tree topology using point--to--point links.
public:
  // Levels, Fanout, IP
  //Doc:Method
  Tree1(Count_t, Count_t,
       IPAddr_t = IPADDR_NONE, SystemId_t = 0);
   
 
  Tree1(Count_t, Count_t, const Linkp2p&,
       IPAddr_t = IPADDR_NONE, SystemId_t = 0);
  
  bool GenerateTopo();
   
  Node* GetRoot();                    // Get root node
   
  Node* GetNode(Count_t, Count_t);    // Get node at specified level, index
  
  Node* GetLeaf(Count_t);             // Get specified leaf
   
  Linkp2p* GetChildLink(Count_t, Count_t, Count_t);
    
  Linkp2p* GetParentLink(Count_t, Count_t);
   
  Queue*   GetChildQueue(Count_t, Count_t, Count_t);
   
  Queue*   GetParentQueue(Count_t, Count_t);
   
  Count_t  LeafCount() { return leafCount;}
  
  void SetLocationViaBoundBox(const Location&, const Location&,BoxType  type = DEFAULT);
  

  void BoundingBox(const Location&, const Location&, double angle);

public:
  NodeId_t   first;      // There is a global node list, we don't need one 
  NodeId_t   last;       // Just log first/last+1 entries
  Count_t    levels;     // Number of levels
  Count_t    fanout;     // Children per node
  Count_t    leafCount;  // Number of leaf nodes
  IPAddr_t   firstLeafIP;// Starting IP Address for leaf
  NodeId_t   firstLeafId;// Starting NodeId for leaf
private:
  void ConstructorHelper(Count_t, Count_t,
                         const Linkp2p&,
                         IPAddr_t,
                         SystemId_t);
  void LocateRow(Count_t, Count_t, NodeId_t,
                  const Location&, const Location&, 
                  std::vector<Meters_t>&);
};

#endif


