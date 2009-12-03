#include "node-real.h"
//NodeVec_t Node::nodes;

Count_t NodeReal::count = 0;
Count_t NodeReal::count1 = 0;
Count_t NodeReal::count2 = 0;
Count_t NodeReal::count3 = 0;
Count_t NodeReal::count4 = 0;

NodeReal::NodeReal(Node* n)
: NodeImpl(n),
x(0.0), y(0.0), hasLocation(false),
userInfo(nil),
#ifdef HAVE_QT
animInfo(nil), wirelessColor(Qt::black), 
#endif
icmpEnabled(true),isswitch(false)

{
	count++;
}

NodeReal::~NodeReal()
{
	count--;
}

void NodeReal::SetLocation(const Location& l)
{
	SetLocation(l.X(), l.Y(), l.Z());
}
void NodeReal::SetLocation(Meters_t xl, Meters_t yl, Meters_t zl)
{ // Set x/y/z location
	x = xl;
	y = yl;
	z = zl;
	//if (Simulator::instance)
	//{ // Simulator object tracks min/max for displaying
	//	Simulator::instance->NewLocation(x,y);
	//}
	hasLocation = true;
#ifdef HAVE_QT  
	if (animInfo) animInfo->show = true;
#endif
}
void NodeReal::Color(const QColor& c)
{
	if (!animInfo)
	{
		animInfo = new NodeAnimation();
		animInfo->show = hasLocation;
	}
	animInfo->color = c;
	animInfo->hasColor = true;
	//if (animInfo->nodePolygon)
	//{ // Already displayed, update the canvas item
	//	animInfo->nodePolygon->setBrush(c);
	//}
}
Location NodeReal::UpdateLocation()
{
	/*if (mobility) mobility->UpdateLocation(pNode);*/
	return GetLocation();
}
Location NodeReal::GetLocation()
{
	return Location(x, y, z);
}

QCanvasItem* NodeReal::Display(const QPoint& p, QTWindow* qtw)
{
	if (!qtw) return nil; // Not animating
	QCanvas* canvas = qtw->Canvas();
	/*CustomShape_t cs;*/
	if (!animInfo)
	{
		animInfo = new NodeAnimation(); // Allocate the animation info
		animInfo->show = hasLocation;
	}
	//if (Shape() != Node::IMAGE && !animInfo->nodePolygon)
	//{ // No existing polygon, make one
	//	switch(Shape()) {
	//	case Node::NONE:
	//		return nil;
	//	case Node::CIRCLE:
			animInfo->nodePolygon =
				new QCanvasEllipse(animInfo->pixelSizeX, animInfo->pixelSizeY,
				canvas);
			animInfo->nodePolygon->move(p.x(), p.y());
		//	break;
		//case Node::SQUARE:
		//	animInfo->nodePolygon = new QCanvasRectangle(
		//		p.x() - animInfo->pixelSizeX / 2,
		//		p.y() - animInfo->pixelSizeY / 2,
		//		animInfo->pixelSizeX,
		//		animInfo->pixelSizeY,
		//		canvas);
		//	break;
		//case Node::HEXAGON:
		//	break;
		//case Node::OCTAGON:
		//	break;
		///*case Node::CUSTOM:
		//	cs = CustomShape();
		//	if (cs)
		//	{
		//		animInfo->nodePolygon = cs(canvas, pNode, p, animInfo->nodePolygon);
		//	}
		//	break;*/
		//default:
		//	break;
		//}
		if (!animInfo->nodePolygon) return nil; // Oops!  No known shape
		animInfo->nodePolygon->show();
	/*}*/
	//if (IsMobile() || animInfo->placeImage)
	//{ // Need to (possibly) move to new location
	//	if (Shape() == Node::CUSTOM)
	//	{ // Custom items may redraw depending on motion, we need to call
	//		cs = CustomShape();
	//		if (cs)
	//		{
	//			animInfo->nodePolygon = cs(canvas,pNode,p, animInfo->nodePolygon);
	//		}
	//	}
	//	else
	//	{
	//		QCanvasItem* pi;
	//		if (Shape() == Node::IMAGE)
	//			pi = animInfo->nodeSprite;
	//		else
	//			pi = animInfo->nodePolygon;
	//		pi->move( p.x() - animInfo->pixelSizeX / 2,
	//			p.y() - animInfo->pixelSizeY / 2);
	//		animInfo->placeImage = false;
	//		if (Shape() == Node::IMAGE)
	//		{ // We want images behind links and packets
	//			pi->setZ(-1);
	//		}
	//		else
	//		{
	//			pi->setZ(1.0); // We want nodes "in front of" links
	//		}
	//	}
	//}
	// Choose color of node object based on wireless conditions
	// or user specified color.
	if (Shape() != Node::CUSTOM && Shape() != Node::IMAGE)
	{ // Custom shapes and images set their own color
		QCanvasPolygonalItem* pi = (QCanvasPolygonalItem*)animInfo->nodePolygon;
		//if (IsDown())
		//{
		//	pi->setBrush(Qt::red);
		//}
		//else if (HasColor())
		//{ // Use user specified color if one is specified
			pi->setBrush(Color());
		/*}
		else if (WirelessCx())
		{
			pi->setBrush(Qt::red);
		}
		else if (WirelessTx())
		{
			pi->setBrush(Qt::cyan);
		}
		else if (WirelessRx())
		{
			pi->setBrush(Qt::green);
		}
		else if (WirelessRxMe())
		{
			pi->setBrush(Qt::blue);
		}
		else if (WirelessRxZz())
		{
			pi->setBrush(Qt::yellow);
		}*/
#ifdef DONT_DO_THIS
		else if (mobility && mobility->IsMoving())
		{
			pi->setBrush(Qt::magenta);
		}
#endif
		/*else
		{
			pi->setBrush(Qt::black);
		}*/
	}
	return animInfo->nodePolygon;

}

Node::Shape_t NodeReal::Shape()
{
	if (!animInfo)
	{
		animInfo = new NodeAnimation(); // Allocate the animation info
		animInfo->show = hasLocation;
	}
	return animInfo->shape;
}
QColor& NodeReal::Color()
{
	if (!animInfo)
	{
		animInfo = new NodeAnimation();
		animInfo->show = hasLocation;
	}
	return animInfo->color;
}