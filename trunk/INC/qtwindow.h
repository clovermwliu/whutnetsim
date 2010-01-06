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



// Georgia Tech Network Simulator - QT Graphics Window Interface
// George F. Riley.  Georgia Tech, Fall 2002

#ifndef __qtwindow_h__
#define __qtwindow_h__

#include <vector>
#include <list>

#include "G_common-defs.h"
#include "simulator.h"

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

typedef std::list<QTChildWindow*> ChildWindowList_t;

//Doc:ClassXRef
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


#include "handler.h"
#include "event.h"

typedef int    QTCoord_t;
typedef double QTDCoord_t;

#define MAX_SLIDER 30

// Define the QT Update events
//Doc:ClassXRef
class QTEvent : public Event {
public:
  typedef enum { START, STOP, PAUSE, UPDATE,
	  //v1.01
                 //WIRELESS_TX_START, WIRELESS_TX_END, 
                 //BASEBAND_TX_START, BASEBAND_TX_END,
                 RECORD } QTEvent_t;
  QTEvent(QTEvent_t ev) : Event(ev), bluesrc(nil), src(nil), dst(nil), iFace(nil),
      range(0), iter(0) { };
public:
  BlueNode*    bluesrc;     // Bluetooth source node  for wireless transmit animation
  Node*    src;     // Source node  for wireless transmit animation
  Node*    dst;     // Destination node  for wireless transmit animation
  InterfaceWireless* iFace; // Interface for wireless  animation
  Meters_t range;   // Range of wireless transmission
  Count_t  iter;    // Iteration for wireless tx
};


class Node;
class QTEvent;

#ifdef HAVE_QT
// Define a non-filled circle canvas item. Used for wireless animation.
class QCanvasCircle : public QCanvasEllipse
{
public:
  QCanvasCircle(int radius, QCanvas * canvas) :
      QCanvasEllipse(2 * radius, 2 * radius, canvas)
  {
  }
  QCanvasCircle(QCanvas * canvas) : QCanvasEllipse(canvas)
  {
  }

  void setDiameter(int diameter)
  {
    setSize(diameter, diameter);
  }

  void setRadius(int radius)
  {
    setSize(2 * radius, 2 * radius);
  }

protected:
  void draw(QPainter & p)
  {
    p.setPen(pen());
    drawShape(p);
  }

  void drawShape(QPainter & p)
  {
    int penWidth = pen().width();
    if (penWidth > 4)
      {
        p.drawArc((int)(x() - width()/2 + penWidth /2),
                  (int)(y() - height()/2 + penWidth/2),
                  width() - penWidth,
                  height() - penWidth , 0,16*360);
      }
    else
      {
        p.drawArc((int)(x() - width()/2), int(y() - height()/2),
                  width(), height(), 0, 16*360);
      }
  }
};


//Doc:ClassXRef
class QTWindow : public QObject, public Handler {
  Q_OBJECT
public:
  QTWindow(bool);
  ~QTWindow();
public:
  void Handle(Event*, Time_t);
  void DisplayTopology();
  void DisplayTopologyAndReturn();
  void ProcessEvents();
  void AnimationUpdateRate(Time_t);
  void UpdateTopology(bool = true); // True if forced topo redisplay
  void AddBackgroundLines(const LocationVec_t&);
  bool HasQuit() {return quit;}
  void PlaybackMode(bool);
  bool PlaybackMode(){return playbackMode;}
  Mult_t PlaybackRate(){return currentUpdateRate;}
  void PlaybackPause();
  QPoint  LocationToPixels(const Location&); // Convert a location to pixels
  QPoint  LocationToPixelsAbs(const Location&); // Convert a location to pixels
  QTCoord_t NodePixelSize() const { return nodePixelSize; }
#ifdef MOVED_TO_WLAN
  void    WirelessTxStart(Node*, InterfaceWireless*, Node*, Meters_t);
  void    WirelessTxEnd(Node*, InterfaceWireless*, Node*, Meters_t);
#endif
  //v1.01
  //void    BasebandTxStart(BlueNode*, Meters_t);
  //void    BasebandTxEnd(BlueNode*, Meters_t);
  QTCoord_t CanvasX() { return canvasX;}
  QTCoord_t CanvasY() { return canvasY;}
  QCanvas*  Canvas()  { return canvas; }
  QApplication* GetApp() { return app; }
  
public slots:
  void NewSliderValue(int);
  void Save();
  void Record(bool);
  void Play();
  void Stop();
  void Pause();
  void Quit();
  void Exit();
  void TimerDone();
public:
  void    Initialize();        // Initialize constants
  QPoint  NodeLocation(Node*); // Get the pixel coords, CENTER of the node
  void    DrawNode(Node*, Count_t);     // Draw the node
  void    DrawP2P(Node*, Node*);        // Draw a point to point link  
  // Redraw after failure/recovery
  void    RedrawP2P(InterfaceBasic*, InterfaceBasic*);
  void    AddChild(QTChildWindow*);     // Add a child
  void    DeleteChildWindow(QTChildWindow*);  // Remove a child
  void    UpdateSimTime();              // Update the simulation time display
  void    WaitWhilePaused();            // Wait until un-paused
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
  void    WirelessTxStart(Node*, InterfaceWireless*, Node*, Meters_t, Count_t);
  void    WirelessTxEnd(Node*, InterfaceWireless*, Node*, Meters_t, Count_t);
#endif
  //v1.01
  //void    BasebandTxStart(BlueNode*, Meters_t, Count_t);
  //void    BasebandTxEnd(BlueNode*, Meters_t, Count_t);
  void    RecordNextFrame(QTEvent*);
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
  QTEvent*      qtEvent;
  ChildWindowList_t childWindows;
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
class QTEvent;
//Doc:ClassXRef
    class QTWindow : public Handler {
public:
  QTWindow();
  ~QTWindow();
public:
  void Handle(Event*, Time_t);
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
