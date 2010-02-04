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

#include <iostream>
#include "math.h"

//#define DEBUG_MASK 0x02
#include "G_debug.h"
#include "G_common_defs.h"
#include "simulator.h"
#include "node.h"
#include "event.h"
#include "link.h"
#include "linkp2p.h"
#include "node-real.h"
//#include "interface-wireless.h"
#include "node-real.h"
#include "queue.h"
#include "qtchildwindow.h"
#include "gui_ctrl.h"
//#include "baseband.h"
//#include "node-blue.h"
//#include "node-blue-impl.h"
#include "ratetimeparse.h"
#include "Task.h"

#ifdef HAVE_QT

#include <QtGui/QVBoxLayout>
#include <qpushbutton.h>
#include <qfont.h>
#include <qpixmap.h>
#include <qslider.h>
#include <qlcdnumber.h>
#include <qdesktopwidget.h>
#include <qtimer.h>
#include <qpainter.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <Qt3Support/q3hbox.h>
#include <q3canvas.h>
#include <Qt3Support/q3valuelist.h>

using namespace std;

class QStandardItem;

gui_ctrl* gui_ctrl::qtWin;

#ifdef NOT_USED
// Pixmaps for vcr controls
static const char * rewind_xpm[] = {
"16 16 3 1",
" 	c None",
".	c #FFFFFF",
"+	c #000000",
"................",
".++..........++.",
".++........++++.",
".++......++++++.",
".++....++++++++.",
".++..++++++++++.",
".++++++++++++++.",
".++++++++++++++.",
".++++++++++++++.",
".++++++++++++++.",
".++..++++++++++.",
".++....++++++++.",
".++......++++++.",
".++........++++.",
".++.........+++.",
"................"};

static const char * next_xpm[] = {
"16 16 3 1",
" 	c None",
".	c #FFFFFF",
"+	c #000000",
"................",
".++.....+.......",
".+++....++......",
".++++...+++.....",
".+++++..++++....",
".++++++.+++++...",
".+++++++++++++..",
".++++++++++++++.",
".++++++++++++++.",
".+++++++++++++..",
".++++++.+++++...",
".+++++..++++....",
".++++...+++.....",
".+++....++......",
".++.....+.......",
"................"};

#endif

/* XPM */
#ifdef USE_LARGE_CAMERA
static const  char *filesave_xpm[] = {
/* width height num_colors chars_per_pixel */
"    32    32      151            2",
/* colors */
".. c None",
".# c #1f1f1f",
".a c #202020",
".b c #2f2720",
".c c #302020",
".d c #302820",
".e c #3f2720",
".f c #3f2820",
".g c #3f2f2f",
".h c #3f3730",
".i c #402820",
".j c #402f20",
".k c #403730",
".l c #4f2820",
".m c #4f302f",
".n c #4f403f",
".o c #4f484f",
".p c #50302f",
".q c #50403f",
".r c #5f2f20",
".s c #5f3020",
".t c #5f3830",
".u c #5f4f30",
".v c #5f4f50",
".w c #5f5760",
".x c #60372f",
".y c #60382f",
".z c #60482f",
".A c #604f40",
".B c #60504f",
".C c #60606f",
".D c #6f4030",
".E c #6f4730",
".F c #6f676f",
".G c #6f7790",
".H c #70402f",
".I c #704030",
".J c #704830",
".K c #704f3f",
".L c #705f50",
".M c #706860",
".N c #707780",
".O c #7f6850",
".P c #7f706f",
".Q c #804f2f",
".R c #80573f",
".S c #805f40",
".T c #806740",
".U c #807060",
".V c #807f80",
".W c #80808f",
".X c #8090b0",
".Y c #80a0d0",
".Z c #8f5730",
".0 c #8f5840",
".1 c #8f7f7f",
".2 c #8f8f90",
".3 c #905f40",
".4 c #906740",
".5 c #90684f",
".6 c #907f6f",
".7 c #908780",
".8 c #908f8f",
".9 c #9097a0",
"#. c #909fb0",
"## c #90a7cf",
"#a c #9f5f30",
"#b c #9f684f",
"#c c #9f7750",
"#d c #9f887f",
"#e c #9f8f80",
"#f c #9f9790",
"#g c #9f98a0",
"#h c #9fa0af",
"#i c #9fafd0",
"#j c #9fafdf",
"#k c #a06f4f",
"#l c #a0703f",
"#m c #a07f5f",
"#n c #a0906f",
"#o c #a09780",
"#p c #a09890",
"#q c #a09f9f",
"#r c #a0b0c0",
"#s c #a0b7e0",
"#t c #af7850",
"#u c #af875f",
"#v c #af978f",
"#w c #af9fa0",
"#x c #afa09f",
"#y c #afa0a0",
"#z c #afa7af",
"#A c #afa8a0",
"#B c #afa8b0",
"#C c #afafcf",
"#D c #afb7d0",
"#E c #b07f50",
"#F c #b07f5f",
"#G c #b0a08f",
"#H c #b0a79f",
"#I c #b0a8af",
"#J c #b0afa0",
"#K c #b0b0b0",
"#L c #b0c7ef",
"#M c #bf875f",
"#N c #bfa89f",
"#O c #bfaf9f",
"#P c #bfb0a0",
"#Q c #bfb0af",
"#R c #bfb7b0",
"#S c #bfb8bf",
"#T c #c0b09f",
"#U c #c0b7af",
"#V c #c0b8af",
"#W c #c0b8bf",
"#X c #c0bfb0",
"#Y c #c0c0bf",
"#Z c #c0d0df",
"#0 c #cf9f6f",
"#1 c #cfc0b0",
"#2 c #cfc0c0",
"#3 c #cfd7f0",
"#4 c #d0bfa0",
"#5 c #d0c8c0",
"#6 c #d0cfc0",
"#7 c #d0cfd0",
"#8 c #d0d7a0",
"#9 c #dfcfbf",
"a. c #dfd7cf",
"a# c #dfd7df",
"aa c #dfd8df",
"ab c #dfe0bf",
"ac c #dfe7ff",
"ad c #e0d7af",
"ae c #e0d7d0",
"af c #e0d8df",
"ag c #e0e0e0",
"ah c #efe0e0",
"ai c #efe7e0",
"aj c #efe7ef",
"ak c #efe88f",
"al c #efe8f0",
"am c #eff0a0",
"an c #f0e0c0",
"ao c #f0e7e0",
"ap c #f0e8e0",
"aq c #f0f0ef",
"ar c #f0f0ff",
"as c #f0f8ff",
"at c #fff7ff",
"au c #fff8f0",
/* pixels */
"............................................#o#N#N#v#P..........",
"........................................#H#U#H#N#U#5ae#Y#H......",
"......................................#H#v#e#N#U#1aeaeahah#H....",
"....................................#H#v.1#d#O#Y#5aeahapaqaa#H..",
"..................................#H#N.1.M.7#Ya#aeagapatapap#5..",
"..................................#1#y.V.7#q#I#S#Ya#agajalalah#O",
"...................8#y.V.P.h.h...1a.#1#P#W#q.V.9#.#.#Kaaatauau#1",
".............8#y#q#y#I#R#Y#q.e.c#Papapaual#B.G###r#i.X#Calauau#1",
".........p.f.8#K#S#R#K#S#S#S#q.b#1auauauau.X#rakab#L.Y#iasauau#7",
".......p.l.j.n#d#K#R#R#S#K#K#S#p#Pauauauau#D#8amac#L.Y##auauaual",
".....p.j.e.p.i.q#d#A#I#W#W#Q#S#S#Pauauauau#D#Zacac#L#j#sauauauag",
"...m.p.m.f.e.i.l.S#Raaaaagalatal#Xauauatah#K#L#3ac#3#j#3auauau#6",
"...l.m.m.e.i.y#b.Laiauauauauauau#Pauauahafaf#7#r#r#i#Dauauauauae",
"...e.f.j.b.e#b.R.haaauauauauauau#Hahaoaf#5#6#7#K#h#Ba#ajauauag#H",
".D.D.l.b.a.f#M.h.#aaauauauauauau.P#5aeae#1#Y#6#w.2.9#g#z#7#7ae..",
".p.s.s.b.a.f#F.t.baiaualagahalag.E#dae#1#P#J#Y#y.F.C.N.W.2#f#H..",
".e.p.p.a.a.f#E.D.calau.F.#.u#n#2.K.f#4#X#x.8#y#A.F.o.w.C.7#H....",
".t.x.f.a.a.e#E.D.caqau.g.z.A.A#J.R.f.O#Y#U#f#y#P#N.M.v#d#x......",
"...i.c.b.a.f#E.D.calaq.Bad#G.d#I.E.#.q#o#N#X#P#P#P#v#A#x........",
"...e.a.a.b.f#t.x.capap.M.L.k.oae.D.#.K.R.f.B#Y#1#1#V............",
"...e.b.b.b.f#t.t.calau#P.n.n#Qau.y.#.R.R.#.c....................",
"...i.e.c.b.f#t.x.haqauau#Y#Yaqau.p.#.R.R.c.h....................",
"...l.c.e.e.j#t.s.tauau#6.q.q#Nau.h.c.T.0.c.j....................",
"...c.e.i.i.j#F.x.Dauau.U.S.1.q#5.D.c.5.R.c.j....................",
"...e.i.l.l.y#b.x.taq#5.A#4an.1#9.H.c#c.Z.c.p....................",
"...e.l.l.l.3.0.r.5auae.U#N#O#e#9.D.c#m.4.r.I....................",
"...c.r.x.r.0.J#t#0arau#9.6#d#9au.J.c.R.3.s.I....................",
"...e.x.y.y.3.3.Z#uauauauauauauah.Q.l.D.0.x......................",
".....l.H.I.Z#k.y#mauauauau#9#v.4.J.r............................",
".......y.I.J#k.x.3#T#q#m.3.Q.Q.y.s..............................",
".........y.Z#t#l#a.Z.J.y.H.x....................................",
"...........y#l.I.x.x.x.........................................."
};
#else
static const char *filesave_xpm[] = {
"    16    16        4            1",
". c #040404",
"# c #808304",
"a c #bfc2bf",
"  c None",
"                ",
"..............  ",
".#.aaaaaaaa.a.  ",
".#.aaaaaaaa...  ",
".#.aaaaaaaa.#.  ",
".#.aaaaaaaa.#.  ",
".#.aaaaaaaa.#.  ",
".#.aaaaaaaa.#.  ",
".##........##.  ",
".############.  ",
".##.........#.  ",
".##......aa.#.  ",
".##......aa.#.  ",
".##......aa.#.  ",
" .............  ",
"                ",
};
#endif

static const char *record_xpm[] = {
/* width height ncolors chars_per_pixel */
"32 32 16 1",
/* colors */
"  c #000000000000",
"! c #800000000000",
"# c #000080000000",
"$ c #800080000000",
"% c #000000008000",
"& c #800000008000",
"' c #000080008000",
"( c #800080008000",
") c #c000c000c000",
"* c #ff0000000000",
"+ c #0000ff000000",
", c #ff00ff000000",
"- c #00000000ff00",
". c #ff000000ff00",
"/ c #0000ff00ff00",
"0 c #ff00ff00ff00",
/* pixels */
//"                                ",
//" 0000000000000000000000000000000",
//"  0         0       000000000000",
//" 0 --------- ------- 000000 0000",
//" 0 ---------          0000 , 000",
//"  0          --------     ,, (00",
//" 00 - --------------- - - ,, ((0",
//"000 - --------------- - - ,, ((0",
//"000 - --------------- - - ,, ((0",
//"000 - --------------- - - ,, ((0",
//"000 - --------------- - - ,, ((0",
//"000 - ---------------     ,, ((0",
//"0000  -  ----- - - -- (((( , ((0",
//"00000 --------------- ((((( (((0",
//"000000               (((0000((00",
//"0000000(((((((((((((((((00000000",
//"00000000(((((((((((((((000000000",
//"0000               0000000000000",
//"0000 ,0,0,0,0,0,0, (000000000000",
//"0000 0,0,0,0,0,0,0 ((00000000000",
//"0000 ,0,0               00000000",
//"0000 0, , ,0,0,0,0,0,0, (0000000",
//"0000 , 0  0,0,0,0,0,0,0 ((000000",
//"0000 0, , ,0 0,0,0,0 0, ((000000",
//"0000 ,0,0 0 0 0   0 0 0 ((000000",
//"0000 0,0, ,0 0,0,0,0 0, ((000000",
//"0000      0,0,0,0,0,0,0 ((000000",
//"00000(((( ,0,0,0,0,0,0, ((000000",
//"000000((( 0,0,0,0,0,0,0 ((000000",
//"000000000               ((000000",
//"0000000000((((((((((((((((000000",
//"00000000000(((((((((((((((000000"
} ;

static const char * play_xpm[] = {
"16 16 3 1",
" 	c None",
".	c #FFFFFF",
"+	c #000000",
"                ",
" ++             ",
" ++++           ",
" ++++++         ",
" ++++++++       ",
" ++++++++++     ",
" ++++++++++++   ",
" +++++++++++++  ",
" +++++++++++++  ",
" ++++++++++++   ",
" ++++++++++     ",
" ++++++++       ",
" ++++++         ",
" ++++           ",
" +++            ",
"                "};

static const char * stop_xpm[] = {
"16 16 3 1",
" 	c None",
".	c #FFFFFF",
"+	c #000000",
"                ",
" ++++++++++++++ ",
" ++++++++++++++ ",
" ++++++++++++++ ",
" +++        +++ ",
" +++        +++ ",
" +++        +++ ",
" +++        +++ ",
" +++        +++ ",
" +++        +++ ",
" +++        +++ ",
" +++        +++ ",
" ++++++++++++++ ",
" ++++++++++++++ ",
" ++++++++++++++ ",
"                "};

static const char * pause_xpm[] = {
"16 16 3 1",
" 	c None",
".	c #FFFFFF",
"+	c #000000",
"                ",
"   ++++   ++++  ",
"   ++++   ++++  ",
"   ++++   ++++  ",
"   ++++   ++++  ",
"   ++++   ++++  ",
"   ++++   ++++  ",
"   ++++   ++++  ",
"   ++++   ++++  ",
"   ++++   ++++  ",
"   ++++   ++++  ",
"   ++++   ++++  ",
"   ++++   ++++  ",
"   ++++   ++++  ",
"   ++++   ++++  ",
"                "};

#ifdef MOVED_TO_QTWINDOW_H
// Define a non-filled circle canvas item. Used for wireless animation.
class Q3CanvasCircle : public Q3CanvasEllipse
{
public:
  Q3CanvasCircle(int radius, Q3Canvas * canvas) :
      Q3CanvasEllipse(2 * radius, 2 * radius, canvas)
  {
  }
  Q3CanvasCircle(Q3Canvas * canvas) : Q3CanvasEllipse(canvas)
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
#endif

gui_ctrl::gui_ctrl(bool fullScreen, QApplication* a)
  : canvas(nil), view(nil), t(nil),
    updateRate(nil), slider(nil), simTime(nil), readyTimer(nil),
    mw(nil), box(nil), quitButton(nil), exitButton(nil), 
    record(nil), save(nil), stop(nil), play(nil), pause(nil),
    nodePixelSize(10), zoomMult(1.0), border(10),
    //canvasX(640), canvasY(640),
    canvasX(3640), canvasY(3640),
    qtEvent(nil),
    topologyDisplayed(false), updateScheduled(false),
    ready(true), paused(false), quit(false), oneShot(false),
    playbackMode(false), recording(false), recordingFrame(0)
{
  app = a;
  qtWin = this;
  // Reduce window size to 512x512 if animation recorder enabled
  if (Simulator::recorderEnabled)
    {
      canvasX = 512;
      canvasY = 512;
    }
  
  QDesktopWidget* qd = QApplication::desktop();
  if (canvasX > (qd->width()-40))  canvasX = qd->width() - 40;
  if (canvasY > (qd->height()-150)) canvasY = qd->height() - 150;
  if (!fullScreen)
    {  
      if (canvasX > canvasY) canvasX = canvasY;
      if (canvasY > canvasX) canvasY = canvasX;
    }

  updateRates[0]  = 0.000000001;
  updateRates[1]  = 0.000000002;
  updateRates[2]  = 0.000000004;
  updateRates[3]  = 0.000000008;
  updateRates[4]  = 0.000000016;
  updateRates[5]  = 0.000000032;
  updateRates[6]  = 0.000000064;
  updateRates[7]  = 0.000000128;
  updateRates[8]  = 0.000000256;
  updateRates[9]  = 0.000000512;
  updateRates[10]  = 0.000001;
  updateRates[11]  = 0.000002;
  updateRates[12]  = 0.000004;
  updateRates[13]  = 0.000008;
  updateRates[14]  = 0.000016;
  updateRates[15]  = 0.000032;
  updateRates[16]  = 0.000064;
  updateRates[17]  = 0.000125;
  updateRates[18]  = 0.000250;
  updateRates[19]  = 0.000500;
  updateRates[20] = 0.001000;
  updateRates[21] = 0.002000;
  updateRates[22] = 0.004000;
  updateRates[22] = 0.008000;
  updateRates[23] = 0.016000;
  updateRates[24] = 0.032000;
  updateRates[25] = 0.064000;
  updateRates[26] = 0.125000;
  updateRates[27] = 0.250000;
  updateRates[28] = 0.500000;
  updateRates[29] = 1.000000;

  currentUpdateRate = updateRates[MAX_SLIDER-1];

  if (canvas) return;            
  Initialize();                
  mw = new gui_frame( );
  mw->resize(canvasX+8, canvasY+32 );
  mw->setWindowState(Qt::WindowMaximized);
  mw->setIcon(QPixmap("./resources/mainframe.png"));
  box = new Q3VBox(mw);          // Get a vertical box to hold quit and canvas
  box->resize(canvasX+8, canvasY+32 ); // Size of canvas plus a fudge

  canvas = new Q3Canvas(canvasX, canvasY);
  canvas->setDoubleBuffering(true);
  view=new Q3CanvasView(canvas,box);
  view->resize(canvasX, canvasY);

  Q3HBox* hbox = new Q3HBox(box);

  updateRate  = new QLCDNumber(hbox);
  updateRate->setName("UpdateRate");    
  updateRate->setMode(QLCDNumber::Dec); // Decimal mode
  updateRate->setNumDigits(7);
  updateRate->setSegmentStyle(QLCDNumber::Filled);
  updateRate->setStyleSheet("background-color:darkgreen;");

 
  slider = new QSlider( Qt::Horizontal,hbox);
  connect( slider, SIGNAL(valueChanged(int)), SLOT(NewSliderValue(int)));
  hbox->setStretchFactor(slider, 4);

  // Create the simtime display
  simTime = new QLCDNumber(hbox);
  simTime->setName("simTime"); 
  simTime->setMode(QLCDNumber::Dec); // Decimal mode
  simTime->setNumDigits(7);
  simTime->setSegmentStyle(QLCDNumber::Filled);
  simTime->display(0.0);
  simTime->setStyleSheet("background-color:darkgreen;");
  slider->setRange( 0, MAX_SLIDER-1 );
  slider->setValue( MAX_SLIDER-1 );

  record = new QPushButton( );
  // Put the record button if enabled
  if (Simulator::recorderEnabled)
  {
	  record->setIcon(QPixmap(record_xpm));
	  //hbox->addWidget(record);
	  record->setToggleButton(true);
	  connect(record, SIGNAL(toggled(bool)), SLOT(Record(bool)));
  }
  else
  { // Put the save button
	  save = new QPushButton(hbox);
	  connect(save, SIGNAL(clicked()), SLOT(Save()));
	  save->setPixmap(QPixmap( filesave_xpm ));
  }

  //stop = new QPushButton(hbox);
   stop = new QPushButton( );
  QObject::connect(stop, SIGNAL(clicked()), SLOT(Stop()));

  play = new QPushButton(hbox);
  QObject::connect(play, SIGNAL(clicked()), SLOT(Play()));

  pause = new QPushButton(hbox);

  QObject::connect(pause, SIGNAL(clicked()), SLOT(Pause()));

  stop->setPixmap(QPixmap( stop_xpm ));
  play->setPixmap(QPixmap( play_xpm ));
  pause->setPixmap(QPixmap( pause_xpm ));
  // Allocate the canvas and canvas view

  quitButton = new QPushButton("Quit",hbox);
  quitButton->setFixedSize(40,25);

  quitButton->setFont(QFont("Times", 12, QFont::Bold));
  connect(quitButton, SIGNAL(clicked()), SLOT(Quit()));
  exitButton = new QPushButton("Exit");


  //if (Simulator::pCustomBackground) Simulator::pCustomBackground(canvas);
  //DisplayAllNodes(); // Display the topology
  setdown(true);
  mw->setCentralWidget(box);
  mw->resize(canvasX+8, canvasY+32 );
  mw->show();
  QObject::connect(mw,SIGNAL(fileready(QString)),this,SLOT(newtask(QString)));
}

// Destructor
gui_ctrl::~gui_ctrl()
{

}

// Event Handler
void gui_ctrl::Handle(Event* e, Time_t t)
{
  QTEvent* ev = (QTEvent*)e;
  if (!topologyDisplayed) DisplayAllNodes();
  switch (ev->event) {
    case QTEvent::START :
      cout << "Got start event" << endl;
      Play();
      break;
    case QTEvent::STOP :
      cout << "Got stop event" << endl;
      Stop();
      break;
    case QTEvent::PAUSE :
      cout << "Got pause event" << endl;
      Pause();
      break;
    case QTEvent::RECORD:
    case QTEvent::UPDATE :
      // Record must do everything UPDATE does
      DEBUG0((cout << "Hello from QTEvent Update time " 
              << Simulator::Now() << endl));
      updateScheduled = false;
      // Update the simtime lcd display
      UpdateSimTime();
      // Show the topology (if changed)
      DisplayAllNodes();
      ProcessEvents();
      if (ev->event == QTEvent::RECORD)
        {
          RecordNextFrame(ev);
          if (!recording) delete ev;
        }
      else
        { // UPDATE event
          WaitWhilePaused();
          // Schedule another update event in the future
          // But check for non-nil, since the quit event may delete the event
          if (qtEvent && !updateScheduled && !quit)
            {
              Scheduler::Schedule(qtEvent, currentUpdateRate, this);
              updateScheduled = true;
            }
        }
#ifdef OLD_WAY
      //if (quit)
      //  {
      //    delete app;
      //    app = nil;
      //  }
#endif
      // Update child windows
      for (ChildWindowList_t::iterator i = childWindows.begin();
           i != childWindows.end(); ++i)
        {
          QTChildWindow* cw = *i;
          cw->Update();
        }
      break;
#ifdef MOVED_TO_WLAN
    case QTEvent::WIRELESS_TX_START :
    case QTEvent::WIRELESS_TX_END :
#ifdef VERBOSE
      cout << "Wireless TX ";
      if (ev->event == QTEvent::WIRELESS_TX_START) 
        cout << "Start ";
      else
        cout << "End ";
      cout << " iter " << ev->iter
           << " time " << Simulator::Now() << endl;
#endif
      if (ev->event == QTEvent::WIRELESS_TX_START) 
        WirelessTxStart(ev->src, ev->iFace, ev->dst, ev->range, ++(ev->iter));
      else
        WirelessTxEnd(ev->src, ev->iFace, ev->dst, ev->range, ++(ev->iter));
      delete ev; // New event allocated in private subroutines
      break;
#endif
	  
    }
  
}

void gui_ctrl::DisplayTopology()
{	
  // if (canvas) return;            // Already initialized
   mw->show();
   DisplayAllNodes( );
}

void gui_ctrl::setdown(bool down)
{
  updateRate->setEnabled(!down);
  slider->setEnabled(!down);
  simTime->setEnabled(!down);
  save->setEnabled(!down);
  stop->setEnabled(!down);
  play->setEnabled(!down);
  pause->setEnabled(!down);
  quitButton->setEnabled(!down);
}

void gui_ctrl::newtask(QString s)
{
	if(t)
	{
		delete t;
	}
	mw->dirlist->currentitem->setIcon(QIcon(".\\Resources\\play.png"));
	setdown(false);
	char c[64];
	strcpy(c, s.toLatin1().data());

	const string str(c); 

	t = new CTask(0,str);
	t->InitTask();
	t->Start();
}

void gui_ctrl::ProcessEvents()
{
	app->processEvents( );
}

void gui_ctrl::AnimationUpdateRate(Time_t t)
{
  if (!slider) DisplayTopology();
  
  // Find nearest slider value
  for (Count_t i = 0; i < MAX_SLIDER; ++i)
    {
      if (t < updateRates[i])
        { // Found it
          slider->setValue(i);
          currentUpdateRate = t;
          updateRate->display(currentUpdateRate);
          cout << "Initial update rate " << currentUpdateRate << endl;
          return;
        }
    }
  slider->setValue(MAX_SLIDER - 1);

}


void gui_ctrl::UpdateTopology(bool b)
{ // Displays the topology on demand
  DEBUG0((cout << "GTWindow::UpdateTopology (forced)" << endl));
  // Next line insures pacing at appropriate rate.
  // The ready flag is set to true when the readyTimer pops.
  WaitWhilePaused();
  while(!ready && !quit) ProcessEvents();
  if (quit) return;
  if (playbackMode)
    {
      ready = false;
      readyTimer->start(1000/25, true);  // Use 25 frames per sec
    }

  // Update the simtime lcd display
  UpdateSimTime();
  DisplayAllNodes(b); // Display the topology
  ProcessEvents();   // And allow QT to update
}

Count_t lineCount; // debug

void gui_ctrl::AddBackgroundLines(const LocationVec_t& points)
{
  bool first = true;
  QPoint prior; // Prior point
  Location priorLoc; // debug
   
  if (!canvas) DisplayTopology();
  DEBUG0((cout << "QTW::ABL points.size " << points.size() << endl));
  for (LocationVec_t::size_type i = 0; i < points.size(); ++i)
    {
      DEBUG0((cout << "locX " << points[i].X() 
              << " y " << points[i].Y() << endl));
      QPoint current = LocationToPixels(points[i]);
      // debug
      if (current.x() < 0 || current.x() >= canvasX ||
          current.y() < 0 || current.y() >= canvasY)
        {
          cout << "HuH?  Conversion out of range, x " << current.x()
               << " y " << current.y()
               << " mx " << points[i].X()
               << " my " << points[i].Y() << endl;
        }
      
      bool inRange = (points[i].Y() <= 60.0 && points[i].Y() >= 20.0 &&
                      priorLoc.Y() <= 60.0 && priorLoc.Y() >= 20.0);
      
      inRange = true;
      if (!first && inRange)
        {
          Q3CanvasLine* line = new Q3CanvasLine(canvas);
          line->setPen(QPen(Qt::black));
          line->setPoints(prior.x(), prior.y(), current.x(), current.y());
          DEBUG0((cout << "Adding point x0 " << prior.x()
                  << " y0 " << prior.y()
                  << " x1 " << current.x()
                  << " y1 " << current.y() << endl));
          
          line->show();
          lineCount++;
        }
      first = false;
      prior = current;
      priorLoc = points[i];
    }
  canvas->update(); // Show the updates
  DEBUG0((cout << "Cumulative Line Count " << lineCount << endl));
}

void gui_ctrl::PlaybackMode(bool pb)
{
  if (pb)
    { // Playback mode requested, create a pacing timer for real-time display
      playbackMode = true;
      readyTimer = new QTimer(this);
      connect(readyTimer, SIGNAL(timeout()), SLOT(TimerDone()));
      readyTimer->start(1000/25, true);  // Use 25 frames per sec
    }
  else
    {
      if (readyTimer)
        {
          readyTimer->stop();
          delete readyTimer;
          readyTimer = nil;
        }
      playbackMode = true;
      ready = true;
    }
}

void gui_ctrl::PlaybackPause()
{
  paused = true;
}


// Slots
void gui_ctrl::NewSliderValue(int v)
{
  currentUpdateRate = updateRates[v];
  updateRate->display(currentUpdateRate);
  if (!paused && qtEvent)
    { // Need to cancel pending update event and reschedule
      Scheduler::Cancel(qtEvent);
      updateScheduled = true;
      Scheduler::Schedule(qtEvent, currentUpdateRate, this);
    }
}

void gui_ctrl::Save()
{
  // Get a PNG image of the view
  QPixmap pm = QPixmap::grabWidget(view);
  QString fn = QFileDialog::getSaveFileName(".", "Images(*.png)", view,
                                            "Save File Dialog", 
                                            "Choose a file");
  if (!fn.isEmpty())
    {
      // Make sure ends with .png
      if (!fn.endsWith(".png")) fn += ".png";
      if ( QFile::exists( fn ))
        {
          QMessageBox mb( "GTNetS",
                          "Saving the file will overwrite the original file on the disk.\n"
                          "Do you really want to save?",
                          QMessageBox::Information,
                          QMessageBox::Yes | QMessageBox::Default,
                          QMessageBox::No, 0);
          mb.setButtonText( QMessageBox::Yes, "Yes" );
          mb.setButtonText( QMessageBox::No, "No" );
          int r = mb.exec();
          if (r == QMessageBox::No) return;
        }
      
#ifdef NEED_NEWER_QT  
           QMessageBox::question(
               view,
               tr("Overwrite File?"),
               tr("File %1 already exists."
                  "Do you want to overwrite it?")
               .arg( fn ),
               tr("&Yes"), tr("&No"),
               QString::null, 0, 1 ) )
        {
          return;
        }
#endif
      pm.save(fn, "PNG");
    }
}

void gui_ctrl::Record(bool)
{
  if (recording)
    {
      cout << "Stop recording" << endl;
      recording = false;
      // No need to delete the event, as it will do nothing
      // when schedule and be deleted then.
    }
  else
    {
      cout << "Start recording" << endl;
      recordingFrame = 0;
      recording = true;
      // Create the event and record the first frame
      QTEvent* ev = new QTEvent(QTEvent::RECORD);
      RecordNextFrame(ev);
    }
}

void gui_ctrl::Play()
{
  paused = false;
  // Schedule update event
  if (!qtEvent)
    {
      qtEvent = new QTEvent(QTEvent::UPDATE);
    }
  else
    {
      if (updateScheduled)
        {
          Scheduler::Cancel(qtEvent);
        }
    }
  Scheduler::Schedule(qtEvent, currentUpdateRate, this);
  updateScheduled = true;
}

void gui_ctrl::Stop()
{
  if (!paused)
    {
      //cout << "Stop animation" << endl;
      paused = true;
      if (qtEvent)
        { // Should always be true, but just in case
          Scheduler::Cancel(qtEvent);
          delete qtEvent;
          qtEvent = nil;
        }
    }
}

void gui_ctrl::Pause()
{
  DEBUG0((cout << "gui_ctrl::Pause, paused " << paused 
          << " now " << Simulator::Now() << endl));
  //if (paused)
  //  { // Pressing pause while already paused is a "one-shot"
  //    oneShot = true;
  //    return;
  //  }
  paused = true;
  WaitWhilePaused();
#ifdef OLD_WAY
  //if (quit)
  //  {
  //    delete app;
  //    app = nil;
  //  }
#endif
}

void gui_ctrl::Quit()
{ 
  mw->currentfilepath = "";
  //清除画布
  Q3CanvasItemList items = canvas->allItems();
  Q3CanvasItemList::iterator iter;
  for(iter = items.begin();iter != items.end(); iter++)
  {
	  delete *iter;
  }
  canvas->update();
  //file窗口任务图标转换
  mw->dirlist->currentitem->setIcon(QIcon(".\\Resources\\pause.png"));
  setdown(true);
  Exit();
  Simulator::instance->Halt();

  qtEvent = nil;
  topologyDisplayed = false;
  updateScheduled = false;
  ready = true;
  paused = false;
  quit = false;
  oneShot = false;
  playbackMode = false; 
  recording = false;
  recordingFrame = 0;

}

void gui_ctrl::Exit()
{ // Exit causes the animation window to close, but the simulation continues.
  quit = true;
 
}

void gui_ctrl::TimerDone()
{
  ready = true;
}


// Private members
void gui_ctrl::Initialize()
{
  // Compute usage pixel w/h in canvas, allowing for border and node pixel size
  QTCoord_t cw = canvasX - 2 * border - nodePixelSize;
  QTCoord_t ch = canvasY - 2 * border - nodePixelSize;

  Meters_t deltaX = largestX - smallestX;
  Meters_t deltaY = largestY - smallestY;

  DEBUG0((cout << " SX " << smallestX << " LX " << largestX 
          << " SY " << smallestY << " LY " << largestY
          << " DX " << deltaX << " DY " << deltaY << endl));
  
  // Make sure the region is square to maintain proper aspect ratio
  if (deltaX < deltaY) 
    {
      Meters_t adder = (deltaY - deltaX) / 2;
      //cout << "Adjusting X by += " << adder << endl;
      smallestX -= adder;
      largestX += adder;
    }
  else if (deltaY < deltaX) 
    {
      Meters_t adder = (deltaX - deltaY) / 2;
      //cout << "Adjusting Y by += " << adder << endl;
      smallestY -= adder;
      largestY += adder;
    }

  DEBUG0((cout << " SX " << smallestX << " LX " << largestX 
          << " SY " << smallestY << " LY " << largestY
          << " DX " << deltaX << " DY " << deltaY << endl));
  
  DEBUG0((cout << " SX " << smallestX << " LX " << largestX 
          << " SY " << smallestY << " LY " << largestY
          << " DX " << deltaX << " DY " << deltaY 
          << " cw " << cw << " ch " << ch
          << endl));
  

  // Calculate the meters to pixels conversions
  metersToPixelsX = cw / (largestX - smallestX);
  metersToPixelsY = ch / (largestY - smallestY);
}

MyPoint gui_ctrl::NodeLocation(Node* n)
{
  Location l = n->UpdateLocation();
  DEBUG0((cout << "nx " << l.X() << " ny " << l.Y()
          << " mtpx " << metersToPixelsX
          << " mtpy " << metersToPixelsY << endl));
  return LocationToPixels(l);
#ifdef MOVED_TO_METHOD
  // Calculate coordinates of CENTER of the node
#ifdef THIS_IS_UPSIDE_DOWN
  return QPoint((QTCoord_t)((l.X() - smallestX)*metersToPixelsX) 
                 - nodePixelSize / 2 + border * 2,
                (QTCoord_t)((l.Y() - smallestY)*metersToPixelsY) 
                 - nodePixelSize / 2 + border * 2);
#endif
  return QPoint((QTCoord_t)((l.X() - smallestX)*metersToPixelsX) 
                 - nodePixelSize / 2 + border * 2,
                (QTCoord_t)((largestY - l.Y())*metersToPixelsY)
                + nodePixelSize / 2 + border * 2);
#endif
}

void gui_ctrl::DrawNode(Node* n, Count_t i)
{
  QPoint p = NodeLocation(n); // Get location
  Q3CanvasItem* r;
  if (i >= nodeItems.size())
    { // New one
      r = n->Display(p, this);
      if (!r) return; // Not displayed
      nodeItems.push_back(r);
    }
  else
    { // Use existing
      n->Display(p, this);
    }
}

// Below is deprecated.  Links now animate themselves.  See link.h
void gui_ctrl::DrawP2P(Node* f, Node* t)
{ // Draw p2p link connecting node "from" to node "to"
  QPoint fp = NodeLocation(f);
  QPoint tp = NodeLocation(t);
  // Create a new canvas item
  Q3CanvasLine* cl = new Q3CanvasLine(canvas);
  linkItems.push_back(cl);
  cl->show();
  cl->setPoints(fp.x(), fp.y(), tp.x(), tp.y());
  Interface* ifaceSrc = f->GetIfByNode(t);
  Link* linkSrc = ifaceSrc->GetLink();
  Interface* ifaceDst = t->GetIfByNode(f);
  Link* linkDst = ifaceDst->GetLink();
  displayedLinks.push_back(NNIfLink(f, t, ifaceSrc, ifaceDst,
                                    linkSrc, linkDst, cl));
  displayedLinks.push_back(NNIfLink(t, f, ifaceDst, ifaceSrc,
                                    linkDst, linkSrc, cl));
  // If interface is down in either direction color it red
  if (ifaceSrc->IsDown() || ifaceDst->IsDown())
    {
      cl->setPen(QPen(Qt::red));
    }
}

void gui_ctrl::RedrawP2P(InterfaceBasic* ifaceSrc, InterfaceBasic* ifaceDst)
{ // Used to re-draw a link during failure or recovery
  for (NNIfLVec_t::size_type i = 0; i < displayedLinks.size(); ++i)
    {
      NNIfLink& nn = displayedLinks[i];
      if ((nn.ifaceDest == ifaceDst && nn.ifaceSource == ifaceSrc) ||
          (nn.ifaceDest == ifaceSrc && nn.ifaceSource == ifaceDst))
        { // Found it
          Q3CanvasPolygonalItem* cl = nn.canvasItem;
          if (ifaceDst->IsDown() || ifaceSrc->IsDown())
            { // Color red if either interface is down
              cl->setPen(QPen(Qt::red));
            }
          else
            {
              cl->setPen(QPen(Qt::black));
            }
          canvas->update();
          return;
        }
    }
}

  
void gui_ctrl::AddChild(QTChildWindow* c)
{
  cout << "Adding child " << c << endl;
  // First make sure not duplicate
  DeleteChildWindow(c);
  // Then add
  childWindows.push_back(c);
}

void gui_ctrl::DeleteChildWindow(QTChildWindow* c)
{
  for (ChildWindowList_t::iterator i = childWindows.begin();
       i != childWindows.end(); ++i)
    {
      if (*i == c)
        { // Found it
          childWindows.erase(i);
          return;
        }
    }
}

void gui_ctrl::UpdateSimTime()
{
  simTime->display(Simulator::Now());
}

void gui_ctrl::WaitWhilePaused()
{
  if (!paused) return;
  while (app && paused && !quit && !oneShot)
  {
	  app->processEvents();
  }
  oneShot = false;
}

Count_t showCount = 0;

void gui_ctrl::DisplayAllNodes(bool forced)
{
  const NodeVec_t& nodes = Node::GetNodes();
  // Check if called before topology created..if so just return
  if (nodes.empty()) return;
  if (!topologyDisplayed)
  {
	  smallestX = Simulator::instance->SmallestX();
	  smallestY = Simulator::instance->SmallestY();
	  largestX = Simulator::instance->LargestX();
	  largestY = Simulator::instance->LargestY();
	  Initialize(); // Initialize some canvas values
  }
  showCount = 0;
  //if (!topologyDisplayed || forced) displayedLinks.clear();
  for (NodeVec_t::size_type i = 0; i < nodes.size(); ++i)
    {
      Node* n = nodes[i];
      if (Simulator::instance->HasMobility() ||
          !topologyDisplayed || forced)
        { // Need to re-display
          if (n->Show())
            {
              DrawNode(n, i);
              showCount++;
            }
        }
      // Draw the links to neighbors
      NodeWeightVec_t nwv;
      n->Neighbors(nwv); // Get the neighbors
      for (NodeWeightVec_t::size_type j = 0; j < nwv.size(); ++j)
        {
          Node* nn = nwv[j].node;
          Interface* ifaceSrc = n->GetIfByNode(nn);
          Link* linkSrc = ifaceSrc->GetLink();
          if (nn->Id() > n->Id() && nn->Show())
            { // Only draw to higher nodes (lower already drawn)
              if (!topologyDisplayed || forced)
                { // Only do this once
                  DEBUG0((cout << "Drawing p2p link" << endl));
                  linkSrc->AnimateLink();
                  //DrawP2P(n, nn); // Deprecated
                }
            }
          // Find queues at each and save if displayed
          Queue* q1 = n->GetQueue(nn);
          if (q1 && q1->Animated())
            displayedQueues.push_back(NNQueue(q1, n, nn));
          Queue* q2 = nn->GetQueue(n);
          if (q2 && q2->Animated())
            displayedQueues.push_back(NNQueue(q2, nn, n));
          // Tell the link to display the packets
          linkSrc->AnimatePackets();
        }
    }
  topologyDisplayed = true;
  DEBUG0((cout << "Displayed " << showCount << " nodes" << endl));
  //DisplayAllPackets();
  DisplayAllQueues();
  canvas->update(); // Show the updates
}

#ifdef MOVED_TO_LINK
void gui_ctrl::DisplayAllPackets()
{
  DEBUG(2,(cout << "Displaying all pkts, dl size "
           << displayedLinks.size() << endl));
  // Start by clearing existing
  for (CItemVec_t::size_type i = 0; i < packetItems.size(); ++i)
    {
      delete packetItems[i];
    }
  packetItems.clear();

  for (NNIfLVec_t::size_type i = 0; i < displayedLinks.size(); ++i)
    {
      Interface* id = displayedLinks[i].ifaceDest;
      Linkp2p*   link = (Linkp2p*)displayedLinks[i].linkSource;
      Time_t     delay = link->Delay();  // Propagation delay along link
      Rate_t     bw = link->Bandwidth(); // Link bandwidth
      
      // The destination interface has a list of pending packets
      EventCQ* pending = id->GetEventCQ();
      if (!pending) continue; // No event cq yet, so no packets
      if (pending->empty()) continue; // No packets
      while(playbackMode && !pending->empty())
        { // remove any rx events n the past
          EventPair* ep = pending->front();
          if (ep->time > Simulator::Now()) break; // Received in future, keep
          pending->pop_front();       // Remove this one
          LinkEvent* evRx = (LinkEvent*)ep->event;
          DEBUG0((cout << "QTW removing evRx time " << ep->time
                  << " now " << Simulator::Now() << endl));

          delete evRx->p;
          delete evRx;
        }


      for (Count_t j = 0; j < pending->size();  ++j)
        {
          EventPair* ev = (*pending)[j];
          LinkEvent* le = (LinkEvent*)(ev->event);
          // Ignore first bit events for satellite interfaces
          if (le->event != LinkEvent::PACKET_RX) continue;
          Packet*    p = le->p;
          QColor     c(Qt::blue);
          // See if the packet has a color assigned
          if (p->IsColored())
            {
              c = QColor(p->R(), p->G(), p->B());
            }
          else
            { // Display BER pkts in red if not other color specified
              if (le->hasBitError) c = Qt::red;
            }
          
          
          // The timestamp in the event is last bit received.
          // We also need first bit time.
          Count_t    pSize = p->Size()*8; // Size in bits
          pSize -= pSize/10; // Shorten a bit to show breaks between packets
          Time_t     lastBitTime  = ev->time;
          Time_t     firstBitTime = ev->time - (double)(pSize)/bw;
          // Check if Pkt sent in future (playback). ignore
          if ((firstBitTime - delay) > Simulator::Now())
            {
              DEBUG0((cout << "Ignoring pkt sent in future" << endl));
              break;
            }

          // Check if last bit received in the past (playback), ignore
          if (lastBitTime < Simulator::Now())
            {
              DEBUG0((cout << "Ignoring pkt received in past" << endl));
              break;
            }


          if (firstBitTime < Simulator::Now()) firstBitTime = Simulator::Now();
          Time_t fbFuture = firstBitTime - Simulator::Now();
          Time_t lbFuture = lastBitTime - Simulator::Now();
          if (lbFuture > delay) lbFuture = delay;
          // Compute fraction of the link delay for first and last bit times.
          // This is the "distance" along the link line for the display
          Mult_t dfb = 1.0 - fbFuture/delay;
          Mult_t dlb = 1.0 - lbFuture/delay;
          DEBUG0((cout << "Displaying pkt, dfb " << dfb 
                  << " dlb " << dlb << endl));
          DisplayOnePacket(displayedLinks[i].s, displayedLinks[i].d,
                           dfb, dlb, c);
        }
    }
}
#endif

void gui_ctrl::DisplayAllQueues()
{
  for (QueueVec_t::size_type i = 0; i < displayedQueues.size(); ++i)
    {
      NNQueue& nnq = displayedQueues[i];
      nnq.q->DisplayQueue(this, nnq.n1, nnq.n2);
    }
}

void gui_ctrl::Clear()
{ // Remove all items from the canvas
  // First the nodes
  for (CItemVec_t::size_type i = 0; i < nodeItems.size(); ++i)
    {
      delete nodeItems[i];
    }
  nodeItems.clear();
  // Next the links
  for (CItemVec_t::size_type i = 0; i < linkItems.size(); ++i)
    {
      delete linkItems[i];
    }
  linkItems.clear();
  // Finally the packets
  for (CItemVec_t::size_type i = 0; i < packetItems.size(); ++i)
    {
      delete packetItems[i];
    }
  packetItems.clear();
  // Note no longer displayed so it will redraw next time
  topologyDisplayed = false;
}

#ifdef MOVED_TO_LINK
void gui_ctrl::DisplayOnePacket(Node* s, Node* d,
                                Mult_t firstBit, Mult_t lastBit,
                                const QColor& c)
{
  QPoint sp = NodeLocation(s); // Get location of source
  QPoint dp = NodeLocation(d); // Get location of destination
  // Compute length of line connecting src/dst
  double dx = dp.x() - sp.x();
  double dy = dp.y() - sp.y();
  DEBUG(2,(cout << " sp.x " << sp.x() << " sp.y " << sp.y()
           << " dp.x " << dp.x() << " dp.y " << dp.y()
           << " dx " << dx << " dy " << dy << endl));
  double dist = sqrt(dx*dx + dy*dy);
  DEBUG(2,(cout << "Distance node " << s->Id() << " to " << d->Id() 
           << " is " << dist << endl));
  // Compute the angle of a line from src to dst
  double theta = atan2(dy, dx);
  DEBUG(2,(cout << "Angle is " << 360.0 * theta / ( 2.0 * M_PI)
           << " degrees" << endl));
  bool verbose = (theta == 0.0);
  verbose = false;
  
  double sinTheta = sin(theta);
  double cosTheta = cos(theta);
  double pxfb = sp.x() + dist * firstBit * cosTheta;
  double pyfb = sp.y() + dist * firstBit * sinTheta;
  double pxlb = sp.x() + dist * lastBit  * cosTheta;
  double pylb = sp.y() + dist * lastBit  * sinTheta;
  // Use Q3CanvasPolygon for packets
  QPointArray qpa(5); // Four Points for rectangle, plus closing point
  // Compute points 2 pixels left and right of specified
  // Also translate the packet so we are 4 pixels "to the right"
  // of the link line, allowing bi-directional packets to not
  // interfere with each other.
  double sinThetaM2 = sin(theta + M_PI_2);
  double cosThetaM2 = cos(theta + M_PI_2);
  double width = 2.0;

  double pxlfb = pxfb - width * sinTheta + 4 * cosThetaM2;
  double pylfb = pyfb + width * cosTheta + 4 * sinThetaM2;
  double pxrfb = pxfb + width * sinTheta + 4 * cosThetaM2;
  double pyrfb = pyfb - width * cosTheta + 4 * sinThetaM2;
  qpa.setPoint(0, (int)pxlfb, (int)pylfb);
  qpa.setPoint(1, (int)pxrfb, (int)pyrfb);
  double pxllb = pxlb - width * sinTheta + 4 * cosThetaM2;
  double pyllb = pylb + width * cosTheta + 4 * sinThetaM2;
  double pxrlb = pxlb + width * sinTheta + 4 * cosThetaM2;
  double pyrlb = pylb - width * cosTheta + 4 * sinThetaM2;
  qpa.setPoint(2, (int)pxrlb, (int)pyrlb);
  qpa.setPoint(3, (int)pxllb, (int)pyllb);
  qpa.setPoint(4, (int)pxlfb, (int)pylfb); // Close the polygon
  
  if (verbose)
    {
      cout << "cosT  " << cosTheta << " sinT " << sinTheta << endl;
      cout << "FB  x " << pxfb << " y " << pyfb << endl;
      cout << "LB  x " << pxlb << " y " << pylb << endl;
      cout << "LFB x " << qpa.point(0).x() << " y " << qpa.point(0).y() <<endl;
      cout << "RFB x " << qpa.point(1).x() << " y " << qpa.point(1).y() <<endl;
      cout << "RLB x " << qpa.point(2).x() << " y " << qpa.point(2).y() <<endl;
      cout << "LLB x " << qpa.point(3).x() << " y " << qpa.point(3).y() <<endl;
    }
  
  Q3CanvasPolygon* cp = new Q3CanvasPolygon(canvas);
  packetItems.push_back(cp);
  cp->setPen(Qt::black);
  cp->setBrush(c);
  cp->show();
  cp->setPoints(qpa);
}
#endif

MyPoint gui_ctrl::LocationToPixels(const Location& l)
{
  return QPoint((QTCoord_t)((l.X() - smallestX)*metersToPixelsX) 
                 - nodePixelSize / 2 + border * 2,
                (QTCoord_t)((largestY - l.Y())*metersToPixelsY)
                + nodePixelSize / 2 + border * 2);
}

MyPoint gui_ctrl::LocationToPixelsAbs(const Location& l)
{
  return QPoint((QTCoord_t)((l.X() - smallestX)*metersToPixelsX),
                (QTCoord_t)((largestY - l.Y())*metersToPixelsY));
}

#ifdef MOVED_TO_WLAN
void   gui_ctrl::WirelessTxStart(Node* n, InterfaceWireless* iFace, Node* d,
                                 Meters_t range)
{
  if (!n->IsReal()) return;
  DEBUG(1, (cout << "Anim Wireless Tx Start, range " << range
          << " time " << Simulator::Now() << endl));
  WirelessTxStart(n, iFace, d, range, 0);
}

void   gui_ctrl::WirelessTxEnd(Node* n, InterfaceWireless* iFace, Node* d,
                               Meters_t range)
{
  if (!n->IsReal()) return;
  DEBUG(1, (cout << "Anim Wireless Tx End, range " << range
          << " time " << Simulator::Now() << endl));
  WirelessTxEnd(n, iFace, d, range, 0);
}
#endif

#ifdef MOVED_TO_WLAN
void   gui_ctrl::WirelessTxStart(Node* n, InterfaceWireless* iFace, Node* d,
                                 Meters_t range, Count_t it)
{
  if (it == 1)
    { // First iteration, see if "pauseOnTx" set
      if (Simulator::instance->PauseOnWirelessTx())
        {
          Pause();
        }
    }

  if (it)
    {
      // Add the circle to the canvas items list (if not zeroth iteration)
      // First find the radius of the circle in pixels
      Location l = n->GetLocation();
      QPoint   lp = LocationToPixels(l);
      n->Display(lp, this);
      Meters_t r = range / 10.0 * it;
      Location rl = l;
      rl.X(rl.X() + r);
      QPoint  rp = LocationToPixels(rl);
      Q3CanvasCircle* c = new Q3CanvasCircle(rp.x() - lp.x(), canvas);
      // Set the color, with reduced intensity farther  away
      const QColor& col = n->WirelessTxColor();
      int red = col.red();
      int green = col.green();
      int blue = col.blue();
      int nred   = red   + (int)((double)(255-red)   * (it-1)/10.0);
      int ngreen = green + (int)((double)(255-green) * (it-1)/10.0);
      int nblue  = blue  + (int)((double)(255-blue)  * (it-1)/10.0);
      c->setPen(QColor(nred, ngreen, nblue));
      c->move(lp.x(), lp.y());
      c->show();
      n->PushWirelessTx(c);
      // Now re-display any nodes in the power range just processed
      Meters_t prevr = range / 10.0 * (it - 1);
      const RadioVec_t& rv = n->GetRadioInterfaceList();
      for (RadioVec_t::const_iterator i = rv.begin(); i != rv.end(); ++i)
        {
          InterfaceWireless* iFace1 = i->inf;
          Meters_t dist1 = i->dist;
          if (dist1 > prevr && dist1 <= r)
            { // Need to update this one
              Node* n1 = iFace1->GetNode();
              Location l1 = n1->GetLocation();
              QPoint   lp1 = LocationToPixels(l1);
              n1->Display(lp1, this);
            }
        }
          
      // Update the simtime
      UpdateSimTime();
      canvas->update(); // Show the changes
      ProcessEvents();
      WaitWhilePaused();
    }
  
  if (it < 10)
    {
      QTEvent* ev = new QTEvent(QTEvent::WIRELESS_TX_START);
      ev->src = n;
      ev->dst = d;
      ev->iFace = iFace;
      ev->range = range;
      ev->iter = it;
      Time_t perIteration = ev->range / SPEED_LIGHT;
      // Schedule next update event
      Scheduler::Schedule(ev, perIteration, this);
    }
}

void   gui_ctrl::WirelessTxEnd(Node* n, InterfaceWireless* iFace, Node* d,
                               Meters_t range, Count_t it)
{
  if (it)
    { // Remove the circles one-by-one to animate end of transmission
      // Must be real node here
      Location l = n->GetLocation();
      QPoint   lp = LocationToPixels(l);
      n->Display(lp, this);
      Q3CanvasItem* c = n->PopWirelessTx();
      if(c)delete c;
      // Now re-display any nodes in the power range just processed
      Meters_t r = range / 10.0 * it;
      Meters_t prevr = range / 10.0 * (it - 1);
      const RadioVec_t& rv = n->GetRadioInterfaceList();
      for (RadioVec_t::const_iterator i = rv.begin(); i != rv.end(); ++i)
        {
          InterfaceWireless* iFace1 = i->inf;
          Meters_t dist1 = i->dist;
          if (dist1 > prevr && dist1 <= r)
            { // Need to update this one
              Node* n1 = iFace1->GetNode();
              Location l1 = n1->GetLocation();
              QPoint   lp1 = LocationToPixels(l1);
              n1->Display(lp1, this);
            }
        }
          
      // Update the simtime
      UpdateSimTime();
      canvas->update(); // Show the changes
      ProcessEvents();
      WaitWhilePaused();
    }
  
  if (it < 10)
    {
      QTEvent* ev = new QTEvent(QTEvent::WIRELESS_TX_END);
      ev->src = n;
      ev->dst = d;
      ev->iFace = iFace;
      ev->range = range;
      ev->iter = it;
      Time_t perIteration = ev->range / SPEED_LIGHT;
      // Schedule next update event
      Scheduler::Schedule(ev, perIteration, this);
    }
}
#endif

void gui_ctrl::RecordNextFrame(QTEvent* e)
{ // Records the PPM file for next display frame
  if (!recording) return;
  char work[128];
  sprintf(work, "GTNetS%05u.ppm", (unsigned)recordingFrame++);
  //QPixmap pm = QPixmap::grabWidget(view);
  QPixmap pm = QPixmap::grabWidget((QWidget*)box,QRect(mw->x(),mw->y(),canvasX+8, canvasY+32));
  // For some reason the buttons do not display properly at this point.
  // and need to be repainted
  quitButton->repaint(false);
  exitButton->repaint(false);
  if (record) record->repaint(false);
  if (save)   save->repaint(false);
  stop->repaint(false);
  play->repaint(false);
  pause->repaint(false);
  // Remove above when qt fixed
  pm.save(work, "PPM");
  cout << "Saved file " << work << endl;
  // Schedule next event at the appropriate time in the future
  Time_t future = 1.0 / (25.0 * Simulator::recorderSpeedup);
  Scheduler::Schedule(e, future, this);
}

#endif

