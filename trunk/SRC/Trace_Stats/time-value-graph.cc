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

// Implement a Time-Value statistics object, that graphs using Qt
// George F. Riley, Georgia Tech, Fall 2004

#include <iostream>

#ifdef HAVE_QT
#include <qcanvas.h>
#endif

#include "G_debug.h"
#include "time-value-graph.h"
#include "simulator.h"

using namespace std;

TimeValueGraph::TimeValueGraph(double xmin1, double xmax1,
                               double ymin1, double ymax1)
    : QTChildWindow(), xmin(xmin1), xmax(xmax1), ymin(ymin1), ymax(ymax1),
      lastRecordTime(0), timeLine(nil)
{
}

TimeValueGraph::~TimeValueGraph()
{
#ifdef HAVE_QT  /* not necessay but quietens compiler warnings */
  if (timeLine) delete timeLine;
#endif
}

// Interited from TimeValueStats

bool TimeValueGraph::Record(Time_t t, double v)  // Record the time and value
{
  bool r = TimeValueStats::Record(t, v);
#ifdef HAVE_QT
  // Add a new rectangle
  if (t < xmax)
    {
      double w = width * (t-lastRecordTime) / xmax;
      if (w < 1) w = 1;
      double h = height * v / ymax;
      double x = width * lastRecordTime / xmax;
      QCanvasRectangle* r = new QCanvasRectangle(canvas);
      r->setSize((int)w, (int)-h);
      r->moveBy(x, height);
      r->setPen(Qt::black);
      r->setBrush(Qt::red);
      r->show();
      items.push_back(r);
      canvas->update();
    }
#endif
  lastRecordTime = t;
  return r;
}

void TimeValueGraph::Reset()        // Clear all values
{
  TimeValueStats::Reset();
#ifdef HAVE_QT
  for (CanvasItemVec_t::size_type i = 0; i < items.size(); ++i)
    {
      delete items[i];
    }
  items.clear();
  if (timeLine) timeLine->hide();
#endif
}


void TimeValueGraph::Update()
{
#ifdef HAVE_QT
  QTChildWindow::Update();
  // Move the timeLine to appropriate spot
  Time_t now = Simulator::instance->Now();
  double x = width * now / xmax;
  if (!timeLine)
    {
      timeLine = new QCanvasLine(canvas);
      timeLine->setPen(Qt::black);
    }
  timeLine->setPoints((int)x, 0, (int)x, height);
  timeLine->show();
  canvas->update();
#endif
}


void TimeValueGraph::closeEvent(QCloseEvent* e)
{
  DEBUG0((cout << "Hello from TVG::closeEvent" << endl));
#ifdef HAVE_QT
  e->accept();
#endif
}

void TimeValueGraph::resizeEvent(QResizeEvent* e)
{
  DEBUG0((cout << "Hello from TVG::resizeEvent" << endl));
#ifdef HAVE_QT
  cout << "New size w " << e->size().width()
       << " h " << e->size().height() << endl;
#endif
}

Statistics* TimeValueGraph::Copy() const
{
  return new TimeValueGraph(xmin, xmax, ymin, ymax);
}


