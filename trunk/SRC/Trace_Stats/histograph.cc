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


// Georgia Tech Network Simulator - Histograph Class
// George F. Riley.  Georgia Tech, Fall 2003

// Defines a Histogram and CDF statistics collection
// and displays graphically using Qt.

#include <iostream>

#include "G_debug.h"
#include "histograph.h"

#ifdef HAVE_QT
#include <qapplication.h>
#include <qcanvas.h>
#endif

using namespace std;

Histograph::Histograph()
    : Histogram(), initialized(false), showCDF(false),
      updateRate(1), updateCountdown(1)
#ifdef HAVE_QT
      , app(nil), canvas(nil), view(nil)
#endif
{
}

Histograph::Histograph(double m)
    : Histogram(m), initialized(false), showCDF(false),
      updateRate(1), updateCountdown(1), 
      canvasXY(nil), app(nil), canvas(nil), view(nil),
      textRect(nil)
{
}

Histograph::Histograph(double m, Count_t b, double mn)
    : Histogram(m, b, mn), initialized(false), showCDF(false),
      updateRate(1), updateCountdown(1),
      canvasXY(nil), app(nil), canvas(nil), view(nil),
      textRect(nil)
{
}

Histograph::Histograph(const Histograph& c) // Copy constructor
    : Histogram(c), initialized(false), showCDF(c.showCDF),
      updateRate(c.updateRate), updateCountdown(c.updateRate),
      canvasXY(nil), app(nil), canvas(nil), view(nil),
      textRect(nil)
{
}


Histograph::~Histograph()
{
#ifdef HAVE_QT
  if (app) delete app;
#endif
}

Statistics* Histograph::Copy() const   // Make a copy of the histograph
{
  return new Histograph(*this);
}

void Histograph::UpdateRate(Count_t c )
 {
   updateRate = c;
 }

// Overridden methods from Histogram
bool Histograph::Record(double v)
{
  bool r = Histogram::Record(v);
  if (!updateCountdown || --updateCountdown == 0)
    { // Time to update
      if (showCDF)
        {
          UpdateCDF();
        }
      else
        {
          UpdateHisto();
        }
      updateCountdown = updateRate;
    }
  return r;
}

void Histograph::Reset()
{
  Histogram::Reset();
  if (showCDF)
    {
      UpdateCDF();
    }
  else
    {
      UpdateHisto();
    }
}

// Private methods
void Histograph::UpdateCDF()
{
#ifdef HAVE_QT
  if (!initialized) Initialize();
  Count_t total = 0;
  Count_t nBins = bins.size();
  for (Count_t i = 0; i < nBins; ++i)
    {
      total += bins[i];
    }
  total += outOfRange;
  double  x = textRect->width(); // Initial x coordinate
  double  xAdder = (double)(canvasXY->x() - textRect->width()) / bins.size();
  Count_t cumulative = 0;
  Count_t usableY = canvasXY->y() - (int)(textRect->height() * 1.5);
  QPoint prior((int)x, canvasXY->y() - (int)(textRect->height()));

  for (Count_t i = 0; i < nBins; ++i)
    { //  Draw the cdf
      if (items.size() <= i)
        { // too small, add empty
          items.push_back(nil);
        }
      cumulative += bins[i];
      x += xAdder;
      double height = (double)cumulative / total * usableY;
      QPoint p1((int)x, (int)(canvasXY->y()-int(textRect->height())-height));
      // check if same, and if so don't change (code later)
      if (items[i]) delete items[i];
      QCanvasLine* l = new QCanvasLine(canvas);
      items[i] = l;
      l->setPoints(prior.x(), prior.y(), p1.x(), p1.y());
      l->setPen(Qt::black);
      l->show();
      prior = p1;
    }
  
  // Draw the registration lines
  const Count_t n = 10;
  for (Count_t i = 0; i <= n; ++i)
    {
      double v  = (double)i / n;
      Count_t h = (Count_t)(usableY * v);
      
      // labels and lines always same size, so we take a shortcut here
      if (i < labels.size())
        {
          delete labels[i];
          delete lines[i];
        }
      else
        {
          labels.push_back(nil);
          lines.push_back(nil);
        }
      QString qs = QString("%1").arg(v);
      labels[i] = new QCanvasText(qs, canvas);
      labels[i]->moveBy(0,
                        canvasXY->y() - (int)(textRect->height()*1.5) - h);
      labels[i]->show();
      lines[i] = new QCanvasLine(canvas);
      lines[i]->setPen(Qt::lightGray);
      lines[i]->setPoints(textRect->width(), 
                          canvasXY->y() - (int)(textRect->height()) - h,
                          canvasXY->x(), 
                          canvasXY->y() - (int)(textRect->height()) - h);
      lines[i]->show();
    }
  canvas->update(); // Show the updates
  
  while(app->hasPendingEvents())
    {
      app->processEvents(1);
    }
#endif
}

void Histograph::UpdateHisto()
{
#ifdef HAVE_QT
  if (!initialized) Initialize();
  if (bins.size())
    { // If bins not empty
      Count_t largest = 0;
      for (Count_t i = 0; i < bins.size(); ++i)
        {
          if (bins[i] > largest) largest = bins[i];
        }
      
      // Compute height of each bar
      Count_t nBins = bins.size();
      Count_t usableY = canvasXY->y() - (int)(textRect->height() * 1.5);
      for (Count_t b = 0; b < nBins; ++b)
        {
          Count_t height = (Count_t)(usableY * (bins[b]/(double)largest));
          DEBUG0((cout << "Height of bin " << b << " is " << height << endl));
          QCanvasRectangle* rect = (QCanvasRectangle*)items[b];
          rect->setSize(rect->width(), -height);
        }
      // Draw the registration lines and labels
      Count_t n = 10;
      if (largest < n) n = largest;
      double each = (double)largest / n;
      
      for (Count_t i = 0; i <= n; ++i)
        {
          double v  = each * i;
          Count_t h = (Count_t)(usableY * v / largest);
          Count_t intV = (Count_t)v;

          // labels and lines always same size, so we take a shortcut here
          if (i < labels.size())
            {
              delete labels[i];
              delete lines[i];
            }
          else
            {
              labels.push_back(nil);
              lines.push_back(nil);
            }
          QString qs = QString("%1").arg(intV);
          labels[i] = new QCanvasText(qs, canvas);
          labels[i]->moveBy(textRect->width() -
                            labels[i]->boundingRect().width(),
                            canvasXY->y() - (int)(textRect->height()*1.5) - h);
          labels[i]->show();
          lines[i] = new QCanvasLine(canvas);
          lines[i]->setPen(Qt::lightGray);
          lines[i]->setPoints(textRect->width(), 
                              canvasXY->y() - (int)(textRect->height()) - h,
                              canvasXY->x(), 
                              canvasXY->y() - (int)(textRect->height()) - h);
          lines[i]->show();
        }
      canvas->update(); // Show the updates
    }
  
  while(app->hasPendingEvents())
    {
      app->processEvents(1);
    }
#endif
}

void Histograph::Initialize()
{
#ifdef HAVE_QT
  int argc = 1;
  char* argv[] = { "GTNetS", nil};
  app = new QApplication(argc, argv);

  canvasXY = new QPoint(640, 640);
  QString qs;
  if (showCDF)
    {
      qs = QString("%1").arg(0.9);
    }
  else
    {
      qs = QString("%1").arg(12345);
    }
  // Fudge the Y size by one and a half times
  // text height.  This allows for a bottom row
  // of box labels and half of the x-axis label
  // on top
  QCanvasText txt(qs, canvas);
  textRect = new QRect(txt.boundingRect());
  canvasXY->setY(canvasXY->y() + (int)(textRect->height() * 1.5));
  // Adjust x by size of left side labels
  canvasXY->setX(canvasXY->x() + textRect->width());

  Count_t xAdder = 0;
  if (showCDF)
    { // Need to finish coding this...
    }
  else
    {
      // xAdder is width of each bar (integer pixels)
      xAdder = (canvasXY->x() - textRect->width()) / bins.size();
      // Since possibly not evenly divisible, resize canvas
      // to be correct size. Fudge two extra pixels
      xAdder+=2; // two extra pixels between bars
      //canvasXY->setX(textRect->width() + xAdder * bins.size() + xAdder);
      // And fudge the X size by the width of a 5 digit label
      canvasXY->setX(textRect->width() + xAdder * bins.size() + 4); // + 4???
    }
  
  // Create the canvas
  canvas = new QCanvas(canvasXY->x()-4, canvasXY->y());
  canvas->setDoubleBuffering(true);

  // Create the bars for the histogram or lines for CDF
  for (Count_t i = 0; i < bins.size(); ++i)
    {
      if (showCDF)
        {
          QCanvasLine* cp = new QCanvasLine(canvas);
          items.push_back(cp);
          // Will set endpoints later
        }
      else
        {
          QCanvasRectangle* cp = new QCanvasRectangle(canvas);
          // Set  zero height for now, will adjust later
          cp->setSize(xAdder, 0);
          // And move to correct position
          cp->moveBy(textRect->width() + xAdder * i,
                     canvasXY->y() - textRect->height());
          cp->setPen(Qt::black);
          cp->setBrush(Qt::red);
          cp->show();
          items.push_back(cp);
        }
    }
  
  view   = new QCanvasView(canvas);
  //view->setHScrollBarMode(QScrollView::AlwaysOff);
  //view->setVScrollBarMode(QScrollView::AlwaysOff);
  view->resize(canvasXY->x(), canvasXY->y());
  view->show();
  app->setMainWidget(view);
  while(app->hasPendingEvents())
    {
      app->processEvents(1);
    }
#endif
  initialized = true;
}
