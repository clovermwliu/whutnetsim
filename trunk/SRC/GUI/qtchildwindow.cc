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

// Implement a qt sub-window, used for various statistics
// George F. Riley, Georgia Tech, Fall 2004

#include <iostream>

#include "qtchildwindow.h"
#include "gui_ctrl.h"
#include <QtGui/qevent.h>

using namespace std;

QTChildWindow::QTChildWindow()
: 
#ifdef HAVE_QT
QMainWindow(),
#endif
canvas(nil), view(nil), width(0), height(0)
{
#ifdef HAVE_QT
	gui_ctrl* qtw = gui_ctrl::qtWin;
	if (qtw) qtw->AddChild(this);
#endif
}

QTChildWindow::QTChildWindow(Count_t w, Count_t h, const string& c)
: 
#ifdef HAVE_QT
QMainWindow(), 
#endif
canvas(nil), view(nil), width(w), height(h), caption(c)
{
#ifdef HAVE_QT
	gui_ctrl* qtw = gui_ctrl::qtWin;
	if (qtw)
	{
		qtw->AddChild(this);
		setCaption(QString(caption.c_str()));
	}
#endif
}

QTChildWindow::QTChildWindow(const QTChildWindow& cw)
: 
#ifdef HAVE_QT
QMainWindow(), 
#endif
canvas(nil), view(nil), width(cw.width), height(cw.height),
caption(cw.caption)
{
#ifdef HAVE_QT
	setCaption(QString(caption.c_str()));
#endif
}

QTChildWindow::~QTChildWindow()
{
#ifdef HAVE_QT
	gui_ctrl* qtw = gui_ctrl::qtWin;
	if (qtw) qtw->DeleteChildWindow(this);
	hide();
#endif
}

void QTChildWindow::Initialize()
{
#ifdef HAVE_QT
	if (canvas) return; // Already initilized
	gui_ctrl* qtw = gui_ctrl::qtWin;
	if (!qtw) return; // Nothing to do if no animation
	//QApplication* app = qtw->GetApp();
	QApplication* app;
	if (!app) return; // Should never happen
	// Need more here
	if (!width || !height)
	{
		width  = 256; // Take default w/h
		height = 256;
	}

	canvas = new Q3Canvas(width, height);
	view = new Q3CanvasView(canvas, this);
	view->resize(width+16, height+16);
	resize(width+16, height+16);
	canvas->update();
	view->show();
	show();
#endif
}


void QTChildWindow::Update()
{
	//cout << "Hello from Update...Initializing" << endl;
	Initialize();
}

void QTChildWindow::closeEvent ( QCloseEvent* e)
{
	cout << "Hello from QTChildWindow::closeEvent" << endl;
#ifdef HAVE_QT
	e->accept();
#endif
}

void QTChildWindow::resizeEvent ( QResizeEvent* )
{
	cout << "Hello from resizeEvent" << endl;
}

void QTChildWindow::SetCaption(const string& c)
{
	caption = c;
#ifdef HAVE_QT
	setCaption(QString(caption.c_str()));
#endif
}

  
  
