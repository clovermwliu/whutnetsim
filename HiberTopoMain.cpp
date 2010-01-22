////Dumbbell b(10, 5, 0.1, IPAddr("192.168.1.0"), IPAddr("192.168.2.0"), l);
//////  // Specify the bounding box
////  b.BoundingBox(Location(0,0), Location(10,10));
////// Test the tree topology with animation.
////// George F. Riley, Georgia Tech, Fall 2003
////
//#include <iostream>
//
//#include "G_common_defs.h"
//#include "simulator.h"
//#include "node.h"
//#include "tree1.h"
//#include "tcp-tahoe.h"
//#include "application-tcpserver.h"
//#include "application-tcpsend.h"
//#include "ratetimeparse.h"
//#include "linkp2p.h"
//#include "args.h"
//#include "validation.h"  
//
//#ifdef HAVE_QT
//#include <qnamespace.h>
//#include <qcolor.h>
//#endif
//
//#define N_LEVELS 5
//#define N_FANOUT 2
//
//using namespace std;
//
//int main(int argc, char** argv)
//{
//  //Validation::Init(argc, argv);
//  Simulator s;
//  Count_t nl = N_LEVELS;
//  Count_t fo = N_FANOUT;
//  //Arg("levels", nl, N_LEVELS);
//  //Arg("fanout", fo, N_FANOUT);
//  //Arg::ProcessArgs(argc, argv);
//  
//  // Set node shape to a circle for animation
//  Node::DefaultShape(Node::CIRCLE);
//  
//  //Trace* tr = Trace::Instance(); // Get a pointer to global trace object
//  //tr->Open("teststar.txt");
//  //TCP::LogFlagsText(true);       // Log TCP flags in text mode
//  Linkp2p lk(Rate("1Mb"), Time("10ms"));
//
//  // Create the tree
//  Tree tree(nl, fo, lk, IPAddr("192.168.1.1"));
//  tree.BoundingBox(Location(0,0), Location(10,5));
//  
//
//  if (!Validation::noAnimation)
//    {
//      s.StartAnimation(0, true);
//      s.AnimationUpdateInterval(Time("10us")); // 10us initial update rate
//    }
//
//  s.Progress(1);
//  s.StopAt(100);
//  s.Run();
//}
