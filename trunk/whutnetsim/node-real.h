

#ifndef __nodereal_h__
#define __nodereal_h__

#include <map>
#include <deque>
#include <string>

#include "common-defs.h"
#include "node-impl.h"
#include "node.h"
#include "define.h"
#define DEFAULT_SW_TIMEOUT 10   // default timeout for the switch map  secs
#include <qcanvas.h>

#include "qtwindow.h"
class Link;
class Node;
class Interface;
class Routing;
class Protocol;
class ProtocolGraph;
class PortDemux;
class Linkp2p;
class Queue;
class Application;
class RoutingNixVector;
class Mobility;
class Node;
class Image;
class QPixmap;
class QTWindow;
class QCanvasPolygonalItem;
class QCanvasSprite;
class QCanvasItem;


typedef unsigned int Port_t;

class PortPair {
public:
  PortPair() : port(0), time(0) {}
  PortPair(Port_t p, Time_t t) : port(p), time(t) {}

public:
  Port_t  port;
  Time_t  time;
};

//#ifdef HAVE_QT
#include <qcolor.h>
class NodeAnimation {
public:
	NodeAnimation() : show(false),
		pixelSizeX(Node::DefaultPixelSize()),
		pixelSizeY(Node::DefaultPixelSize()),
		shape(Node::DefaultShape()),
		hasColor(false), placeImage(false)
		 {}

public:
	bool          show;         // True if animation display desired
	Count_t       pixelSizeX;   // Size of node in pixels (X)
	Count_t       pixelSizeY;   // Size of node in pixels (Y)
	Node::Shape_t shape;        // Shape of node
	//CustomShape_t csCallback;   // Custom Shape Callback
	QColor        color;        // Color for this node on animation display
	bool          hasColor;     // True if color specified
	bool          placeImage;   // True if image sprite needs to be moved
	QCanvasPolygonalItem*  nodePolygon;// Polygonal item (if not image)
	QCanvasSprite*         nodeSprite; // Image item (for IMAGE shape only)
};
//typedef std::map<MACAddr, PortPair> MACPort_t;
//
//// Define a map for cacheing nixvector route lookups
//typedef std::map<Count_t, RoutingEntry> RENixMap_t;
// And a vector of RoutingEntries for Nix lookup (obviates above)
//typedef std::vector<RoutingEntry>       RENixVec_t;
//#endif

class NodeImpl;

//Doc:ClassXRef
class NodeReal : public NodeImpl {
public:
  NodeReal(Node*);
  ~NodeReal();

    Node::Shape_t  Shape();
  NodeImpl* Copy() const;                // All nodeimpl's must be copy'able
  void      SetLocation(const Location&); 
   void      SetLocation(Meters_t, Meters_t, Meters_t);  // Set x/y/z location
   Location  UpdateLocation();                      // Get updated location
   void  Color(const QColor& c);
    Location  GetLocation(); 
   QColor&        Color();
    QCanvasItem*   Display(const QPoint&, QTWindow*);
  // Data Members
private:
	//Routing*   pRouting;    // Points to Routing Object for this node
	//RENixMap_t nixCache;    // Cache for nix lookups
	//RENixVec_t nixLookup;   // Cache for nix lookups

	//specially for AODV
	//RoutingAODV*  pRoutingAODV;


	Meters_t x;             // x location
	Meters_t y;             // y location
	Meters_t z;             // z location
	bool     hasLocation;   // True if location assigned
//#ifndef COMPACT
//	ProtocolGraph* graph;   // Private protocol graph (if not null)
//#endif

	//Trace::TraceStatus traceStatus; // Trace level for this node
	//Mobility* mobility;     // Current mobility model


public:
	void*       userInfo;           // User specified  information

	//PortDemux*       demux;           // Port Demultiplexer map
	//Meters_t         radioRange;      // this belongs in intefaces, not nodes
	//Joules_t         battery;         // in case node is powered by battery
	//double           computePower;    // CPU power cocnsumption
	//RadioVec_t       radioInterfaces; // List of nodes in range
	//RadioVec_t       radioInf2;       // List of nodes in range potentially
	//Time_t           radioMarginTime;
	//PacketCallbacks  callbacks;       // Packet callbacks for this node

#ifdef HAVE_QT  
	//CanvasItemDeq_t  canvasItems;     // The circles for animating  wireless tx
	NodeAnimation*   animInfo;  
	QColor           wirelessColor;   // Color for wireless tx animation
#endif

	bool             icmpEnabled;     // True if ICMP enabled here, false if not

	//RouteTable*      rtable;          // the routing table object

private:
	// switch members
	/*MACPort_t        portmap;
	Time_t           timeout;*/
	bool             isswitch;
	/*WormContainment* wormcontainment;*/

public:
	static Count_t count;    // debug
	static Count_t count1;   // debug
	static Count_t count2;   // debug
	static Count_t count3;   // debug
	static Count_t count4;   // debug

};


#endif
