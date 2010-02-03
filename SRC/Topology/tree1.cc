
#include "tree1.h"
#include "node.h"
#include "linkp2p.h"
#include "mask.h"

#include <math.h>
#include <stdio.h>

using namespace std;

// Constructors

Tree1::Tree1(Count_t l, Count_t f, IPAddr_t i, SystemId_t id)
:CPlatTopoBase(l*f+1,i,Linkp2p::Default(),id),levels(l),fanout(f),firstLeafIP(i)
{
  //ConstructorHelper(l, f, Linkp2p::Default(), i, id);
}

Tree1::Tree1(Count_t l, Count_t f, const Linkp2p& link,
           IPAddr_t i, SystemId_t id)
:CPlatTopoBase(l*f+1,i,link,id),levels(l),fanout(f),firstLeafIP(i)
{
  //ConstructorHelper(l, f, link, i, id);
}

// Access functions
Node* Tree1::GetRoot()
{ // Get the root node
  return GetNode(0,0);
}

Node* Tree1::GetNode(Count_t l, Count_t c) // Level, index
{ // Get Specified right side leaf node
  Count_t  nodesThisLevel = 1; // Starting with root
  NodeId_t firstId = first;    // First ID for this level
  for (Count_t thisLevel = 0; thisLevel < l; ++thisLevel)
    {
      firstId += nodesThisLevel;
      nodesThisLevel *= fanout;
    }
  if (c < nodesThisLevel) return Node::GetNode(firstId + c);
  return nil; // Out of range
}

Node* Tree1::GetLeaf(Count_t c)
{ // Get specified leaf
  if (c < leafCount) return Node::GetNode(firstLeafId + c);
  return nil; // Out of range
}

Linkp2p* Tree1::GetChildLink(Count_t l,  // Level
                            Count_t i,  // Index on level
                            Count_t c)  // Child number
{ 
  if (c >= fanout) return nil;
  Node* n = GetNode(l, i);
  if (!n) return nil;
  Node* cn = GetNode(l + 1, i * fanout + c);
  if (!cn) return nil;
  return (Linkp2p*)n->GetLink(cn);
}

Linkp2p* Tree1::GetParentLink(Count_t l, Count_t i)
{ // Get link to parent from specified node
  Node* n = GetNode(l, i);
  if (!n) return nil;
  Node* pn = GetNode(l - 1, i / fanout);
  if (!pn) return nil;
  return (Linkp2p*)n->GetLink(pn);
}

Queue* Tree1::GetChildQueue(Count_t l,  // Level
                           Count_t i,  // Index on level
                           Count_t c)  // Child number
{ 
  if (c >= fanout) return nil;
  Node* n = GetNode(l, i);
  if (!n) return nil;
  Node* cn = GetNode(l + 1, i * fanout + c);
  if (!cn) return nil;
  return n->GetQueue(cn);
}

Queue* Tree1::GetParentQueue(Count_t l, Count_t i)
{ // Get link to parent from specified node
  Node* n = GetNode(l, i);
  if (!n) return nil;
  Node* pn = GetNode(l - 1, i / fanout);
  if (!pn) return nil;
  return n->GetQueue(pn);
}

void Tree1::SetLocationViaBoundBox(const Location& ll, const Location& ur,BoxType  type)
{
  vector<Meters_t> myX;
  LocateRow(0, 1, first, ll, ur, myX);
}

void Tree1::BoundingBox(const Location& ll, const Location& rr, double angle)
{
  Meters_t xx,yy,x,y,nx,ny;
  Count_t thisLevelCount = 1;

  angle = M_PI/180.0 * angle;  // convert to radians

  SetLocationViaBoundBox(ll, rr);
  xx = (ll.X()+rr.X())/2;
  yy = rr.Y();

  for (Count_t lev = 0; lev < levels; ++lev) {
    DEBUG0((cout<<"Nodes in this level "<<thisLevelCount<<endl));
    for (Count_t count = 0; count < thisLevelCount; ++count) {
      Node *node = GetNode(lev, count);

      DEBUG0((cout<<"In level "<<lev<<" count "<<count<<endl));
      
      x = node->GetLocation().X()-xx;
      y = node->GetLocation().Y()-yy;

      nx = x* cos(angle)-y*sin(angle);
      ny = x* sin(angle)+y*cos(angle);

      nx = nx + xx;
      ny = ny + yy;

      DEBUG0((printf("Location %6.2lf %6.2lf\n", nx,ny)));
      
      node->SetLocation(Location(nx,ny));
      
    }
    thisLevelCount *=fanout;
    DEBUG0((cout<<"Going to the next level"<<endl));
  }

}

bool Tree1::GenerateTopo(/*Count_t l, Count_t f,
                             const Linkp2p& link,
                             IPAddr_t leafIP,
                             SystemId_t id*/)
{
  first = Node::nextId;
 /* levels = l;
  fanout = f;*/
  Node* n = new Node(sid);
  Count_t   lastLevelCount = 1;
  NodeId_t  lastLevelFirstId = n->Id();
  IPAddr_t  nextIP = ip;

  for (Count_t lev = 1; lev < levels; ++lev)
    { // Create each subsequent level
      bool     lastLevel = (lev == (levels-1));
      Count_t  thisLevelCount = lastLevelCount * fanout;
      NodeId_t thisLevelFirstId = Node::nextId;
      if (lastLevel)
        {
          firstLeafId = Node::nextId;
          leafCount = thisLevelCount;
        }
      for (Count_t c = 0; c < thisLevelCount; ++c)
        { // Create each child
          Node* parent = Node::GetNode(lastLevelFirstId + c / fanout);
          Node* child = new Node(sid);
          if (lastLevel)
            { // Create link with IP Address
              if (nextIP == IPADDR_NONE)
                { // No IP specified
                  child->AddDuplexLink(parent, link);
                }
              else
                {
                  child->AddDuplexLink(parent, link,
                                       nextIP++, Mask(32),
                                       IPADDR_NONE, Mask(32));
                }
            }
          else
            { // Create anonymous (no IPAddr) link
              child->AddDuplexLink(parent, link);
            }
          // Add a default route from child to parent
          child->DefaultRoute(parent);
        }
      lastLevelCount = thisLevelCount;
      lastLevelFirstId = thisLevelFirstId;
    }
  last = Node::nextId;

  return true;
}

void Tree1::LocateRow(Count_t level, Count_t leafThisRow, NodeId_t firstId, 
                      const Location& ll, const Location& ur,
                      std::vector<Meters_t>& parentX)
{ // LocateRow recurses through the rows, since each row needs to know
  // the locations of the children to properly position the parent.
  vector<Meters_t> myX;
  if ((level + 1) < levels)
    {
      LocateRow(level+1, leafThisRow * fanout, firstId + leafThisRow,
                 ll, ur, myX);
    }
  else
    { // This is the bottom row.  Calculate X Locations
      Meters_t xSpacing = (ur.X() - ll.X()) / (leafThisRow + 1);
      Meters_t xLoc = xSpacing;
      for (Count_t i = 0; i < leafThisRow; ++i)
        {
          myX.push_back(xLoc);
          xLoc += xSpacing;
        }
    }
  
  const NodeVec_t& nodes = Node::GetNodes();
  Meters_t minX = 0;
  Meters_t ySpacing = (ur.Y() - ll.Y()) / levels;
  Meters_t yLoc = ySpacing / 2.0 + level * ySpacing;
  
  for (Count_t i = 0; i < leafThisRow; ++i)
    {
      Meters_t xLoc = myX[i];
      Node* n = nodes[firstId + i];
      n->SetLocation(ll.X() + xLoc, ur.Y() - yLoc);
      // Maintain stats for higher level x-location
      if ((i % fanout) == 0)
        {
          minX = xLoc;
        }
      if (((i+1) % fanout) == 0)
        {
          Meters_t upX = minX + (xLoc - minX) / 2;
          parentX.push_back(upX);
        }
    }
}

