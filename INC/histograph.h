//Copyright (c) 2010, Information Security Institute of Wuhan Universtiy(ISIWhu)
//All rights reserved.
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
//Construct Data:
//Modify Author:
//Author Organization:
//Modify Data:




// Georgia Tech Network Simulator - Histogram Class
// George F. Riley.  Georgia Tech, Spring 2002

// Defines a Histogram and CDF statistics collection
// that dynamically draws he histogram using Qt

#ifndef __histograph_h__
#define __histograph_h__

#include <vector>

#include "G_common_defs.h"
#include "histogram.h"
#include "GUI_Defs.h"

typedef  std::vector<MyCanvasPolygonalItem*> ItemVec_t;
typedef  std::vector<MyCanvasText*>          TextVec_t;
typedef  std::vector<MyCanvasLine*>          LineVec_t;

//Doc:ClassXRef
class Histograph : public Histogram {
  //Doc:Class Class {\tt Histograph} defines a statistics collection
  //Doc:Class object that tracks samples in fixed size buckets
  //Doc:Class and counts the number of samples falling in each
  //Doc:Class bucket interval.  A graphical display is produced
  //Doc:Class and updated on each sample.
public:
  //Doc:Method
  Histograph();
    //Doc:Desc Default constructor, no arguments.  Not particularly
    //Doc:Desc useful, since the default constructor sets the maximum
    //Doc:Desc measurement to zero, resulting in all measurements
    //Doc:Desc being out of range.

  //Doc:Method
  Histograph(double);
    //Doc:Desc Construct with specified maximum measurement.
    //Doc:Arg1 Maximum measurement.

  //Doc:Method
  Histograph(double, Count_t, double = 0);
    //Doc:Desc Construct with specified maximum value and bin count.
    //Doc:Desc Optional third parameter with minimum value
    //Doc:Desc This is the preferred constuctor for Histographs.
    //Doc:Arg1 Maximum measurement.
    //Doc:Arg2 Desired bin count.
    //Doc:Arg2 Minimum measurement

  Histograph(const Histograph&); // Copy constructor
  virtual ~Histograph();

  //Doc:Method
  void UpdateRate(Count_t c);
    //Doc:Desc By default, the histograph is re-drawn on every new
    //Doc:Desc "Record" call.
    //Doc:Desc Specifying an UpdateRate will re-draw less often
    //Doc:Desc (every "c" Record calls), improving performance
    //Doc:Arg1 Desired update rate.

  //Doc:Method
  void ShowCDF(bool s) { showCDF = s;}
    //Doc:Desc Specify that the graph should be displayed in CDF format.
    //Doc:Arg1 True if CDF format desired.
  
  // Over-ridden methods from histogram
  bool Record(double);
  void Reset();
  Statistics* Copy() const;   // Make a copy of the statistics
private:
  void UpdateCDF();
  void UpdateHisto();
  void Initialize();
private:
  bool    initialized;
  bool    showCDF;
  Count_t updateRate;
  Count_t updateCountdown;
  MyPoint*       canvasXY;
  MyApplication* app;
  MyCanvas*      canvas;
  MyCanvasView*  view;
  MyRect*        textRect;
  ItemVec_t     items;
  TextVec_t     labels;
  LineVec_t     lines;
};
#endif



