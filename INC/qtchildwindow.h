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


// Define a qt sub-window, used for various statistics
// George F. Riley, Georgia Tech, Fall 2004

#include <vector>
#include <string>
#include <Qt3Support/q3canvas.h>
#include "G_common_defs.h"

class QApplication;
class QCloseEvent;
class QResizeEvent;

typedef std::vector<Q3CanvasItem*> CanvasItemVec_t;

#ifdef HAVE_QT
#include <qmainwindow.h>

class QTChildWindow : public QMainWindow {
#else
class QTChildWindow {
#endif
public:
  QTChildWindow();
  QTChildWindow(Count_t, Count_t, const std::string& = std::string());
  QTChildWindow(const QTChildWindow&);
  virtual ~QTChildWindow();
  void         Initialize(); // Create the window, items, and show
  virtual void Update() = 0; // Update display data
  virtual void closeEvent ( QCloseEvent* );
  virtual void resizeEvent ( QResizeEvent*);
  Q3Canvas*     GetCanvas() { return canvas;}
  void         SetCaption(const std::string&);
public:
  Q3Canvas*        canvas;
  Q3CanvasView*    view;
  CanvasItemVec_t items;
  Count_t         width;
  Count_t         height;
  std::string     caption;
};


  



