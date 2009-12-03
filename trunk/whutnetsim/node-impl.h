#ifndef __node_impl_h__
#define __node_impl_h__

#include "node.h"
#include "define.h"
class Node;
class QTWindow;
class NodeImpl{
public:
	NodeImpl(Node*);
	virtual ~NodeImpl();
    virtual void   SetLocation(const Location&) = 0;
	virtual Location  UpdateLocation() = 0;    // Get updated location
    virtual void   Color(const QColor&) = 0;
    virtual QCanvasItem*   Display(const QPoint&, QTWindow*) = 0;
protected:
	Node*    pNode;         // Points to corresponding node interface object
	bool     usewormcontainment;      // Does this node use worm containment
	// Data items common between real and ghost nodes
	IPAddr_t ipAddr;                  // IP Address for this node
	//IFVec_t  interfaces;              // All defined network interfaces (may be ghosts)
	Count_t  neighborCount;           // Total number of neighbors
	bool     down;                    // True if node is down
};

#endif