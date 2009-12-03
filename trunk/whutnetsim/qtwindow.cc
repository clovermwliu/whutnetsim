
#include "qtwindow.h"
#include <qmainwindow.h   >
#include "debug.h"
#include <iostream>
using namespace std;
//QSelectableView::QSelectableView(
//								 QCanvas* c, QWidget* parent,
//								 const char* name, WFlags f) :
//QCanvasView(c,parent,name,f)
//{
//}

// Constructor
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
QTWindow::QTWindow(bool fullScreen)
: app(nil), canvas(nil), view(nil), 
updateRate(nil), slider(nil), simTime(nil), readyTimer(nil),
mw(nil), box(nil), quitButton(nil), exitButton(nil), 
record(nil), save(nil), stop(nil), play(nil), pause(nil),
nodePixelSize(10), zoomMult(1.0), border(10),
//canvasX(640), canvasY(640),
canvasX(3640), canvasY(3640),
//qtEvent(nil),
topologyDisplayed(false), updateScheduled(false),
ready(true), paused(false), quit(false), oneShot(false),
playbackMode(false), recording(false), recordingFrame(0)
{
#ifdef HUH
	int argc = 1;
	char* argv[] = { "GTNetS", nil};
	app = new QApplication(argc, argv);
#endif
	// Reduce window size to 512x512 if animation recorder enabled
	/*if (Simulator::recorderEnabled)
	{
	canvasX = 512;
	canvasY = 512;
	}*/

	int argc = 1;
	char* argv = "GTNetS";
	app = new QApplication(argc, &argv);
	QDesktopWidget* qd = QApplication::desktop();
	cout << "canvX " << canvasX << " canvY " << canvasY
		<< " qdw " << qd->width() << " qdh " << qd->height()
		<< endl;
	//if (canvasX > (qd->width()-20))  canvasX = qd->width() - 20;
	//if (canvasY > (qd->height()-100)) canvasY = qd->height() - 100;
	if (canvasX > (qd->width()-40))  canvasX = qd->width() - 40;
	if (canvasY > (qd->height()-150)) canvasY = qd->height() - 150;
	if (!fullScreen)
	{ // Not full screen, use square region
		if (canvasX > canvasY) canvasX = canvasY;
		if (canvasY > canvasX) canvasY = canvasX;
	}
	// cout << "canvX " << canvasX << " canvY " << canvasY << endl;
	// Set the mapping from slider values to display update rate
	// For playback mode, these are "fraction of real time"
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
}

class QSelectableView : public QCanvasView 
{
public:
	QSelectableView(QCanvas*, QWidget* parent=0, const char* name=0, WFlags f=0);
//protected:
//	void contentsMousePressEvent(QMouseEvent*);
//	void contentsMouseMoveEvent(QMouseEvent*);
};

QSelectableView::QSelectableView(
								 QCanvas* c, QWidget* parent,
								 const char* name, WFlags f) :
QCanvasView(c,parent,name,f)
{
}

// Destructor
QTWindow::~QTWindow()
{
	QApplication::exit(); // Stop processing and close windows
	delete app;           // Delete the application object
}

void QTWindow::DisplayTopologyAndReturn()
{
	DEBUG0((cout << "Display TopologyAndReturn" << endl));
	if (canvas) return;            // Already initialized
	Initialize();                  // Pre-compute some coordinates
	// Create the main window
	mw = new QMainWindow(0, "GTNetS");
	mw->resize(canvasX+8, canvasY+32 ); // Size of canvas plus a fudge
	box = new QVBox(mw);          // Get a vertical box to hold quit and canvas
	box->resize(canvasX+8, canvasY+32 ); // Size of canvas plus a fudge
	//box->setWFlags(Qt::WType_TopLevel | Qt::WNoAutoErase);
	QHBox* hbox = new QHBox(box);

	// Allocate a quitButton button and connect to application quit
	quitButton = new QPushButton("Quit", hbox);
	quitButton->setFont(QFont("Times", 12, QFont::Bold));
	//quitButton->show();
	/*connect(quitButton, SIGNAL(clicked()), SLOT(Quit()));*/

	// Allocate a exitButton button and connect to application quit
	exitButton = new QPushButton("Exit", hbox);
	exitButton->setFont(QFont("Times", 12, QFont::Bold));
	exitButton->show();
	/*connect(exitButton, SIGNAL(clicked()), SLOT(Exit()));*/

	// Put the update rate lcd and slider
	updateRate  = new QLCDNumber( 6, hbox, "UpdateRate" );
	updateRate->setMode(QLCDNumber::Dec); // Decimal mode
	updateRate->setNumDigits(7);
	updateRate->setSegmentStyle(QLCDNumber::Filled);
	updateRate->setPaletteForegroundColor(Qt::white);
	updateRate->setPaletteBackgroundColor(Qt::black);

	//updateRate->display(updateRates[MAX_SLIDER-1]);
	// Create the slider
	slider = new QSlider( QWidget::Horizontal, hbox);
	/*connect( slider, SIGNAL(valueChanged(int)), SLOT(NewSliderValue(int)));*/
	// And insure the slider is longer than the pushbuttons
	hbox->setStretchFactor(slider, 4);

	// Create the simtime display
	simTime = new QLCDNumber(6, hbox, "SimTime");
	simTime->setMode(QLCDNumber::Dec); // Decimal mode
	simTime->setNumDigits(7);
	simTime->setSegmentStyle(QLCDNumber::Filled);
	simTime->display(0.0);
	simTime->setPaletteForegroundColor(Qt::white);
	simTime->setPaletteBackgroundColor(Qt::black);
	slider->setRange( 0, MAX_SLIDER-1 );
	slider->setValue( MAX_SLIDER-1 );

	// Put the record button if enabled
	//if (Simulator::recorderEnabled)
	//{
	//	record = new QPushButton(QPixmap(record_xpm), 0,  hbox);
	//	record->setToggleButton(true);
	//	connect(record, SIGNAL(toggled(bool)), SLOT(Record(bool)));
	//}
	//else
	//{ // Put the save button
	save = new QPushButton(hbox);
	/*connect(save, SIGNAL(clicked()), SLOT(Save()));*/
	save->setPixmap(QPixmap( filesave_xpm ));
	/*}*/


	// Put the vcr controls

	stop = new QPushButton(hbox);
	/*connect(stop, SIGNAL(clicked()), SLOT(Stop()));*/
	play = new QPushButton(hbox);
	/*connect(play, SIGNAL(clicked()), SLOT(Play()));*/
	pause = new QPushButton(hbox);
	/*connect(pause, SIGNAL(clicked()), SLOT(Pause()));*/

	stop->setPixmap(QPixmap( stop_xpm ));
	play->setPixmap(QPixmap( play_xpm ));
	pause->setPixmap(QPixmap( pause_xpm ));
	// Allocate the canvas and canvas view
	canvas = new QCanvas(canvasX, canvasY);
	canvas->setDoubleBuffering(true);

	view   = new QSelectableView(canvas, box);
	view->resize(canvasX, canvasY);
	//if (Simulator::pCustomBackground) Simulator::pCustomBackground(canvas);
        DisplayAllNodes(); // Display the topology
	mw->show();
}  
void QTWindow::Initialize()
{
	// Compute usage pixel w/h in canvas, allowing for border and node pixel size
	QTCoord_t cw = canvasX - 2 * border - nodePixelSize;
	QTCoord_t ch = canvasY - 2 * border - nodePixelSize;

	// Get range of node coordinates (meters)
	/*smallestX = Simulator::instance->SmallestX();
	smallestY = Simulator::instance->SmallestY();
	largestX = Simulator::instance->LargestX();
	largestY = Simulator::instance->LargestY();*/

	smallestX = 4.0000000000000000;
	smallestY = 7.5000000000000000;
	largestX = 5.0000000000000000;
	largestY = 8.5000000000000000;

	Meters_t deltaX = 1.0;
	Meters_t deltaY = 1.0;

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
//
//
//void QTWindow::Quit()
//{
//	Exit();
//	/*cout << "Quit button pushed" << endl;
//	Simulator::instance->Halt();*/
//	//app->quit();
//	//if (paused)
//	//  { // We need an update event to complete the exit
//	//    if (!qtEvent) qtEvent = new QTEvent(QTEvent::UPDATE);
//	//    Scheduler::Schedule(qtEvent, 0.0, this);
//	//  }
//}
//
//void QTWindow::Exit()
//{ // Exit causes the animation window to close, but the simulation continues.
//	quit = true;
//	//app->quit();
//	//delete app;
//	//app = nil;
//	//if (qtEvent)
//	//  {
//	//    Scheduler::Cancel(qtEvent);
//	//    delete qtEvent;
//	//    qtEvent = nil;
//	//  }
//}
//
////void QTWindow::NewSliderValue(int v)
////{
////	currentUpdateRate = updateRates[v];
////	updateRate->display(currentUpdateRate);
////	if (!paused && qtEvent)
////	{ // Need to cancel pending update event and reschedule
////		Scheduler::Cancel(qtEvent);
////		updateScheduled = true;
////		Scheduler::Schedule(qtEvent, currentUpdateRate, this);
////	}
////}
//
void QTWindow::DisplayAllNodes(bool forced)
{
	/*DEBUG0((cout << "QTWindow::DAN, hm " << Simulator::instance->HasMobility()
		<< " td " << topologyDisplayed
		<< " forced " << forced 
		<< "  nodes.size " << Node::GetNodes().size()
		<< endl));*/
	const NodeVec_t& nodes = Node::GetNodes();
	// Check if called before topology created..if so just return
	if (nodes.empty()) return;
	if (!topologyDisplayed) Initialize(); // Initialize some canvas values
	/*showCount = 0;*/
	//if (!topologyDisplayed || forced) displayedLinks.clear();
	for (NodeVec_t::size_type i = 0; i < nodes.size(); ++i)
	{
		Node* n = nodes[i];
		//if (Simulator::instance->HasMobility() ||
		//	!topologyDisplayed || forced)
		//{ // Need to re-display
		//	if (n->Show())
		//	{
		DrawNode(n, i);
		//showCount++;
		///*	}
		//}*/
		//// Draw the links to neighbors
		//NodeWeightVec_t nwv;
		//n->Neighbors(nwv); // Get the neighbors
		//for (NodeWeightVec_t::size_type j = 0; j < nwv.size(); ++j)
		//{
		//	Node* nn = nwv[j].node;
		//	Interface* ifaceSrc = n->GetIfByNode(nn);
		//	Link* linkSrc = ifaceSrc->GetLink();
		//	if (nn->Id() > n->Id() && nn->Show())
		//	{ // Only draw to higher nodes (lower already drawn)
		//		if (!topologyDisplayed || forced)
		//		{ // Only do this once
		//			DEBUG0((cout << "Drawing p2p link" << endl));
		//			linkSrc->AnimateLink();
		//			//DrawP2P(n, nn); // Deprecated
		//		}
		//	}
		//	// Find queues at each and save if displayed
		//	Queue* q1 = n->GetQueue(nn);
		//	if (q1 && q1->Animated())
		//		displayedQueues.push_back(NNQueue(q1, n, nn));
		//	Queue* q2 = nn->GetQueue(n);
		//	if (q2 && q2->Animated())
		//		displayedQueues.push_back(NNQueue(q2, nn, n));
		//	// Tell the link to display the packets
		//	linkSrc->AnimatePackets();
		//}
	}
	topologyDisplayed = true;
	/*DEBUG0((cout << "Displayed " << showCount << " nodes" << endl));*/
	//DisplayAllPackets();
	//DisplayAllQueues();
	canvas->update(); // Show the updates
}
void QTWindow::DrawNode(Node* n, Count_t i)
{
	QPoint p = NodeLocation(n); // Get location
    /*QPoint p(20,20);*/
	QCanvasItem* r;
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
QPoint QTWindow::LocationToPixels(const Location& l)
{
	return QPoint((QTCoord_t)((l.X() - smallestX)*metersToPixelsX) 
		- nodePixelSize / 2 + border * 2,
		(QTCoord_t)((largestY - l.Y())*metersToPixelsY)
		+ nodePixelSize / 2 + border * 2);
}
QPoint QTWindow::NodeLocation(Node* n)
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