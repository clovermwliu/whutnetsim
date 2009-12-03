// Test animation of worm infection
// Monirul I Sharif, Georgia Institute of Technology

#include <iostream>
#include <stdio.h>
#include "define.h"
#define DEBUG_MASK 0x01

#include "debug.h"
#include "common-defs.h"
//#include "simulator.h"
#include "node.h"
//#include "linkp2p.h"
#include "qtwindow.h"

#ifdef HAVE_QT
#include <qnamespace.h>
#endif

#define N_FANOUT 4
#define N_DEPTH 3
#define N_TREES 4

//#include "linkp2p.h"
using namespace std;

int main(int argc, char** argv)
{
	/*Simulator s;*/
//
//	Count_t nd = N_DEPTH;
//	Count_t nf = N_FANOUT;
//	Count_t nt = N_TREES;
//
	Node::DefaultShape(Node::CIRCLE);
//
	Node *gateway = new Node();
	gateway->SetLocation(Location(5,8.5));
//
	Node *node2 = new Node();
	node2->SetLocation(Location(4,7.5));
//
//	// Setting colors let's see if it works
#ifdef HAVE_QT
	gateway->Color(Qt::red);
	node2->Color(Qt::blue);
	DEBUG0((cout<<"Color set"<<endl));
#endif
	/*gateway->AddDuplexLink(node2, Linkp2p::Default());*/
//	if (!Validation::noAnimation)
//	{
//		s.StartAnimation(0, true);
//		s.AnimationUpdateInterval(Time("10us")); // 10us initial update rate
//	}
//	s.Progress(1);
//	s.StopAt(100);
//	s.Run();
    QApplication a(argc, argv);

	QTWindow *qtWin = new QTWindow(true);
	qtWin->DisplayTopologyAndReturn();
	int test;
	test=2;

	a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
	return a.exec();
}