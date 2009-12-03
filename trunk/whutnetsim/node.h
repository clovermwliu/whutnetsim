
#ifndef __node_h__
#define __node_h__

#include <vector>
#include <deque>
#include <list>
#include "define.h"
#include "common-defs.h"
#include <qcanvas.h>
#include "location.h"
#include "mask.h"
using namespace std;
const Dist_t   INF  = 0xffffffff;

class NodeImpl;
class Node;
class  QColor;
class  QTWindow;
typedef std::vector<Node*>        NodeVec_t;
//Doc:ClassXRef
class Node {
 
public:
  typedef enum { NONE, CIRCLE, SQUARE, HEXAGON, OCTAGON,
                 CUSTOM, // Displayed by the custom callback
                 IMAGE   // Displayed with a Qt image file
  } Shape_t;
 
public:
  //Doc:Method
  Node(SystemId_t = 0); 
//protected:
 /* Node(NodeImpl *,SystemId_t = 0);*/
public:
	virtual ~Node();
	void      SetLocation(const Location&); 
	Location  UpdateLocation();                      // Get updated location
	void Color(const QColor&);
	static const NodeVec_t& GetNodes() { return nodes;}
	 QCanvasItem* Display(const QPoint&, QTWindow*);
#ifdef HAVE_QT  
	//Doc:Method
	static void DefaultPixelSize(Count_t);
	//Doc:Desc Set the default size (in pixels) of the animated nodes
	//Doc:Arg1 Default size in pixels.

	//Doc:Method
	static Count_t DefaultPixelSize();
	//Doc:Desc Query the default node pixel size
	//Doc:Return The default node pixel size.

	//Doc:Method
	static void DefaultShape(Shape_t);
	//Doc:Desc Set the default shape for nodes on the animation display
	//Doc:Arg1 Default shape.  See node.h for shape types.

	//Doc:Method
	static Shape_t DefaultShape();
	//Doc:Desc Query the default node shape.
	//Doc:Return Default shape.  See node.h for shape types.

#endif
public:
	NodeImpl* pImpl;
public:
	static NodeId_t  nextId;        // Next available node id
	static NodeVec_t nodes;         // Vector of all available nodes
#ifdef HAVE_QT
	static Count_t   defaultPixelSize; // Size in pixels of animated nodes
	static Shape_t   defaultShape;     // Default shape for all nodes
#endif
	static double    defaultMaxSpeed;  // Default maximum speed of a node
private:
	IPAddr_t    proxyIP;
	//Mask        proxyMask;
	NodeId_t  id; 

};


#endif
