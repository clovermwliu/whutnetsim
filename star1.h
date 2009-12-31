
#ifndef __star_h__
#define __star_h__

#include <math.h>
#include "PlatTopoBase.h"
#include "common-defs.h"

class Node;
class Linkp2p;
class Queue;
class Location;

//Doc:ClassXRef
class Star1 : public CPlatTopoBase
{
  //Doc:Class Define a star topology using point--to--point links
public:
  Star1(Count_t, IPAddr_t = IPADDR_NONE, SystemId_t = 0);// Number of nodes
 
  Star1(Count_t, const Linkp2p&, IPAddr_t = IPADDR_NONE, SystemId_t = 0);
   
  Star1(Count_t, Node*, IPAddr_t = IPADDR_NONE, SystemId_t = 0);
   
  Star1(Count_t, Node*, const Linkp2p&, IPAddr_t = IPADDR_NONE, SystemId_t = 0);
  bool GenerateTopo();  

  Node* GetHub();                     // Get the hub node
   
  Node* GetLeaf(Count_t);             // Get specified leaf
   
  Linkp2p* HubLink(Count_t);          // Get link from hub to leaf router
   
  Linkp2p* LeafLink(Count_t);         // Get link from leaf to hub
   
  Queue*   HubQueue(Count_t);         // Get queue from hub to leaf
   
  Queue*   LeafQueue(Count_t);        // Get queue from leaf to hub
   
  Count_t  LeafCount() { return leafCount;}
   
  void StarSetLocationViaBoundBox(const Location&, const Location&,
      Angle_t, Angle_t);

public:
  NodeId_t   first;      // First leaf node id
  NodeId_t   last;       // Just log first/last+1 entries
  Node*      hub;        // Points to hub node
  Count_t    leafCount;  // Number of leaf nodes
  Node* n;
private:
  void ConstructorHelper(Count_t, Node*, 
                         IPAddr_t, const Linkp2p&, SystemId_t);
};

#endif


