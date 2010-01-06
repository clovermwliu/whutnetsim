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



// Georgia Tech Network Simulator - Queue base class
// George F. Riley.  Georgia Tech, Spring 2002

// Virtual Base class for all queue types.
// Provides some common functionality

#include "queue.h"
#include "droptail.h"
#include "simulator.h"
#include "link.h"
#include "interface.h"

#ifdef HAVE_QT
#include "qtwindow.h"
#include <qpoint.h>
#include <qcanvas.h>
#endif

using namespace std;

// Static members
Queue*  Queue::defaultQueue  = nil;
Count_t Queue::defaultLength = DEFAULT_DROPTAIL_LIMIT;
Count_t Queue::defaultLimitPkts = 0;  // Default is no packet limit
Count_t Queue::defaultAnimWidth = 10; // Width (pixels) of animated queues
Count_t Queue::globalQueueDrop = 0;   // Total packets dropped due to q-limit

void Queue::DummyEnque(Packet*)
{ // Over-ridden by subclasses as needed
}

Count_t Queue::DequeAllDstMac(MACAddr)
{ // Over-ridden by subclasses as needed
  return 0;
}

Count_t Queue::DequeAllDstIP(IPAddr_t)
{ // Over-ridden by subclasses as needed
  return 0;
}

Packet* Queue::DequeOneDstMac(MACAddr)
{ // Over-ridden by subclasses as needed
  return nil;
}

Packet* Queue::DequeOneDstIP(IPAddr_t)
{ // Over-ridden by subclasses as needed
  return nil;
}

DCount_t Queue::Average()
{ // Get average queue length
  Time_t now = Simulator::Now();
  if (now == startTime) return 0.0;    // Avoid divide by 0
  return totalByteSeconds / (now - startTime);
}

void     Queue::ResetAverage()
{ // Start a new averaging interval
  startTime = Simulator::Now();
  lastUpdate = startTime;
  totalByteSeconds = 0;
}

void     Queue::UpdateAverage()
{ // Queue is changing, update average
  Time_t now = Simulator::Now();
  totalByteSeconds += (now - lastUpdate) * Length();
  lastUpdate = now;
  // Also log the Time-Size history if exitst
  UpdateTimeSizeLog();
}

bool     Queue::Detailed()
{
  return detailed; // True if detailed packet enquing
}

void     Queue::Detailed(bool d)
{
  detailed = d;
}

Time_t   Queue::QueuingDelay()
{
#ifndef WIN32
  if (!interface) return 0;  // No idea if no interface assigned
  Rate_t linkBw = interface->GetLink()->Bandwidth();
  return ((Length() * 8.0) / linkBw); // Amount of time to drain queue
#else
  if (!interface_) return 0;  // No idea if no interface assigned
  Rate_t linkBw = interface_->GetLink()->Bandwidth();
  return ((Length() * 8.0) / linkBw); // Amount of time to drain queue

#endif
}

DCount_t Queue::TotalWorkload()
{
  return totalByteSeconds;
}

void Queue::SetInterface(Interface* i)
{ // Set the associated interface
#ifndef WIN32
  interface = i;
#else
  interface_ = i;
#endif
}

Count_t Queue::DropCount()
{
  return dropCount;
}

Count_t Queue::EnqueueCount()
{
  return totEnq;
}

void    Queue::CountEnq(Packet* p)
{ // Called by interface when link is not busy.
  // Packets bypass the queue, but should be counted for statistics
  // Packet pointer is needed by some queuing methods (dualqueue)
  // to determine which sub-queue should be used.
  totEnq++;
}

void    Queue::ResetStats()
{
  dropCount = 0;
  totEnq = 0;
}

void    Queue::AddForcedLoss(Time_t t, Count_t c, Time_t e)
{
  if (!forcedLosses) forcedLosses = new LossList_t();
  LossList_t::iterator i;
  for (i = forcedLosses->begin(); i != forcedLosses->end(); ++i)
    {
      if (t < i->time) break; // Insert before here
    }
  forcedLosses->insert(i, ForcedLoss(t, c, e));
}

bool    Queue::CheckForcedLoss(bool remove)
{
  if (!forcedLosses) return false; // No forced loss list
  if (forcedLosses->empty()) return false;  // list is empty
  LossList_t::iterator i;
  Time_t now = Simulator::Now();
  bool r = false;

  for (i = forcedLosses->begin(); i != forcedLosses->end(); )
    {
      if (now < i->time) return r; // Done checking
      if (now < i->expiration) r = true;
      if (0) cout << "Forced loss at " << now << " count " << i->count << endl;
      if (remove) i->count--;
      if (i->count == 0 || now >= i->expiration)
        { // Done with this one, remove it
          LossList_t::iterator j = i;
          ++i;
          if (remove) forcedLosses->erase(j);
        }
      if (r) return r; // No need to keep checking
    }
  return r; // code later
}

bool   Queue::CheckSpoofedSource(Packet*)
{
  return false; // Not checked in "normal" queues
}

// Time/Size log methods
void   Queue::EnableTimeSizeLog(bool b)
{
  if (b)
    { // Enable
      if (!timeSizeLog) timeSizeLog = new TimeSeq_t();
    }
  else
    { // Disable.  Delete any existing.
      delete timeSizeLog;
      timeSizeLog = nil;
    }
}

void   Queue::LogSizePackets(bool b)
{ // Specify logging in packets, not byets (if b)
  timeSizePackets = b;
}

void   Queue::PrintTimeSizeLog(ostream& os, Count_t div, char sep)
{
  if (!timeSizeLog) return; // no data available
  for (TimeSeq_t::size_type i = 0; i < timeSizeLog->size(); ++i)
    {
      TimeSeq& ts = (*timeSizeLog)[i];
      Count_t v = ts.seq;
      if (div) v /= div;
      os << ts.time << sep << v << endl;
    }
}

void   Queue::DisplayQueue(QTWindow* qtw, Node* s, Node* d)
{
#ifdef HAVE_QT
  if (!animate) return;
  QPoint sp = qtw->NodeLocation(s); // Get location of source
  QPoint dp = qtw->NodeLocation(d); // Get location of destination
  // Compute slope of line connecting src/dst
  double dx = dp.x() - sp.x();
  double dy = dp.y() - sp.y();
  // Compute the angle of a line from src to dst
  double theta = atan2(dy, dx);
  double sinTheta = sin(theta);
  double cosTheta = cos(theta);
  double sinThetaM2 = sin(theta + M_PI_2);
  double cosThetaM2 = cos(theta + M_PI_2);

  // Allocate the qLines vector if not already
  if (!qLines) qLines = new ItemVec_t();

#ifdef OLD_WAY
  Count_t k = 0;

  while(true)
    {
      // Display this one
      Packet* p = GetPacket(k);
      if (!p) break; // No more
      double lx = sp.x() + k * cosThetaM2; // Left x
      double ly = sp.y() + k * sinThetaM2; // Left y
      double rx = sp.x() + Queue::defaultAnimWidth * cosTheta + k * cosThetaM2;
      double ry = sp.y() + Queue::defaultAnimWidth * sinTheta + k * sinThetaM2;
      QCanvasLine* line = nil;
      if (k < qLines->size())
        {
          line = (*qLines)[k];
        }
      else
        {
          line = new QCanvasLine(qtw->Canvas());
          qLines->push_back(line);
        }
      if (p->IsColored())
        {
          line->setPen(QColor(p->R(), p->G(), p->B()));
        }
      else
        {
          line->setPen(Qt::blue);
        }
      line->setPoints((int)lx, (int)ly, (int)rx, (int)ry);
      line->show();
      ++k;
    }
#else
  ColorVec_t colors;
  GetPacketColors(colors);

  ColorVec_t::size_type i = 0;
  ItemVec_t::size_type k = 0;
  for (; i < colors.size(); ++i)
    {
      QCanvasLine* line = nil;
      if (k < qLines->size())
        {
          line = (*qLines)[k];
        }
      else
        {
          line = new QCanvasLine(qtw->Canvas());
          qLines->push_back(line);
          double lx = sp.x() + k * cosThetaM2; // Left x
          double ly = sp.y() + k * sinThetaM2; // Left y
          double rx = sp.x() + Queue::defaultAnimWidth * cosTheta +
              k * cosThetaM2;
          double ry = sp.y() + Queue::defaultAnimWidth * sinTheta +
              k * sinThetaM2;
          line->setPoints((int)lx, (int)ly, (int)rx, (int)ry);
        }
      line->show();
      line->setPen(QColor(colors[i]));
      ++k;
    }
#endif

  while(k < qLines->size())
    { // Hide any excess
      (*qLines)[k]->hide();
      ++k;
    }
#endif
}

// Private method used by subclasses to log the size vs time info
void   Queue::UpdateTimeSizeLog()
{
  // Note that we use the TimeSeq log from TCP for this, which is
  // a slight hack.  The TimeSeq object uses a type Seq_t for
  // the value being logged, but we need a Count_t value.  Luckily
  // these are both just unsigned long's so we are ok.
  if (!timeSizeLog) return;
  Count_t l = Length();
  if (timeSizePackets) l = LengthPkts();
  if (!timeSizeLog->empty())
    { // See if current timestamp is same as prior,
      // if so, overwrite last entry
      TimeSeq& last = timeSizeLog->back();
      if (last.time == Simulator::Now())
        { // Same, just overwrite
          last.seq = l;
          return;
        }
    }
  timeSizeLog->push_back(TimeSeq(Simulator::Now(), l));
}





// Static Methods
void   Queue::Default(const Queue& d)
{
  if (defaultQueue) delete defaultQueue; // Delete any existing
  defaultQueue = d.Copy();
}

Queue& Queue::Default()
{
  if (!defaultQueue) defaultQueue = new DropTail();
  return *defaultQueue;
}

void   Queue::DefaultLength(Count_t l)
{
  defaultLength = l;
  Default().SetLimit(l);
}

void   Queue::DefaultLimitPkts(Count_t l)
{
  defaultLimitPkts = l;
  Default().SetLimitPkts(l);
}


