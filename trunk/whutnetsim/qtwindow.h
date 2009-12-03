#ifndef __qtwindow_h__
#define __qtwindow_h__

#include <vector>
#include <list>
#include "define.h"
#include "node.h"

#include "common-defs.h"
#ifdef HAVE_QT
#include <qobject.h>
#include <qapplication.h>
#include <qcanvas.h>
#include <qlcdnumber.h>
#include <qslider.h>
#include <qvbox.h>
#include <qpushbutton.h>
#include <qpainter.h>
#endif


class Node;
class BlueNode;
class Interface;
class InterfaceWireless;
class InterfaceBasic;
class Link;
class Queue;
class QApplication;
class QTChildWindow;
class QCanvasItem;
class QCanvasPolygonalItem;
class QMainWindow;
class NNIfLink { // Two nodes, link and receiving interface, and canvasitem
public:
	NNIfLink(Node* n1, Node* n2, InterfaceBasic* is, InterfaceBasic* id,
		Link* ls,Link* ld, QCanvasPolygonalItem* item)
		: s(n1), d(n2), ifaceSource(is), ifaceDest(id),
		linkSource(ls), linkDest(ld), canvasItem(item) { }
public:
	Node* s;
	Node* d;
	InterfaceBasic* ifaceSource;
	InterfaceBasic* ifaceDest;
	Link*      linkSource;
	Link*      linkDest;
	QCanvasPolygonalItem* canvasItem;
};

class NNQueue 
{
public:
	NNQueue(Queue* qq, Node* nn1, Node* nn2) : q(qq), n1(nn1), n2(nn2) {}
public:
	Queue* q;
	Node*  n1;
	Node*  n2;
};

typedef std::vector<QCanvasPolygonalItem*> CPItemVec_t;
typedef std::vector<QCanvasItem*>          CItemVec_t;
typedef std::vector<NNIfLink>              NNIfLVec_t;
typedef std::vector<NNQueue>               QueueVec_t;

#define MAX_SLIDER 30
typedef int    QTCoord_t;
typedef double QTDCoord_t;

#ifdef HAVE_QT
class QTWindow{
	/*Q_OBJECT*/
public:
	QTWindow(bool);
	~QTWindow();
public:
 void DisplayTopologyAndReturn();
 QPoint  NodeLocation(Node*); // Get the pixel coords, CENTER of the node
  void    Initialize();        // Initialize constants
  void    DrawNode(Node*, Count_t);     // Draw the node
  QPoint  LocationToPixels(const Location&); // Convert a location to pixels
  QCanvas*  Canvas()  { return canvas; }
#ifdef MOVED_TO_WLAN
	void    WirelessTxStart(Node*, InterfaceWireless*, Node*, Meters_t);
	void    WirelessTxEnd(Node*, InterfaceWireless*, Node*, Meters_t);
#endif


	/*public slots:*/

public:

private:
	void    DisplayAllNodes(bool = false);// Draw all nodes
#ifdef MOVED_TO_LINK
	void    DisplayAllPackets();          // Draw all in-flight packets
#endif
	void    DisplayAllQueues();           // Draw queues
	void    Clear();                      // Clear the display
#ifdef MOVED_TO_LINK
	void    DisplayOnePacket(Node*, Node*, Mult_t, Mult_t, const QColor&);
#endif              
#ifdef MOVED_TO_WLAN

#endif

	// Private members
	QApplication* app;
	QCanvas*      canvas;
	QCanvasView*  view;
	QLCDNumber*   updateRate;
	QSlider*      slider;
	QLCDNumber*   simTime;
	QTimer*       readyTimer;
	QMainWindow*  mw;       // Main window 
	QVBox*        box;      // The container for the other windows
	QPushButton*  quitButton;
	QPushButton*  exitButton;
	QPushButton*  record;
	QPushButton*  save;
	QPushButton*  stop;
	QPushButton*  play;
	QPushButton*  pause;
	QTCoord_t     nodePixelSize;
	QTDCoord_t    zoomMult;
	QTCoord_t     border;   // size of border, x and y, both pixels
	QTCoord_t     canvasX;
	QTCoord_t     canvasY;
	/*QTEvent*      qtEvent;*/
	/*ChildWindowList_t childWindows;*/
	// Calculated by Initialize()
	QTDCoord_t    metersToPixelsX;
	QTDCoord_t    metersToPixelsY;
	Meters_t      smallestX;
	Meters_t      smallestY;
	Meters_t      largestX;
	Meters_t      largestY;
	bool          topologyDisplayed;
	bool          updateScheduled;
	bool          ready;
	// Controlling the animation
	Time_t        currentUpdateRate;
	bool          paused;
	bool          quit;
	bool          oneShot;
	bool          playbackMode;
	bool          recording;
	Count_t       recordingFrame;
	CItemVec_t    nodeItems;
	CPItemVec_t   linkItems;
	CPItemVec_t   packetItems;
	NNIfLVec_t    displayedLinks;
	QueueVec_t    displayedQueues;
	double        updateRates[MAX_SLIDER];
};


#else
//class QTEvent;
//Doc:ClassXRef
class QTWindow {
public:
	QTWindow();
	~QTWindow();
public:
	/*void Handle(Event*, Time_t);*/
	void DisplayTopology();
	void DisplayTopologyAndReturn();
	void ProcessEvents();
	void AnimationUpdateRate(Time_t);
	/*
	QApplication* GetApp() ;
	void    AddChild(QTChildWindow*);     // Add a child
	void    DeleteChildWindow(QTChildWindow*);  // Remove a child
	*/
};
#endif

#endif