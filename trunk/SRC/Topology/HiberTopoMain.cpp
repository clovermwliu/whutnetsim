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

//更改人：李玉
//更改时间：2010-1-4

////Dumbbell b(10, 5, 0.1, IPAddr("192.168.1.0"), IPAddr("192.168.2.0"), l);
//////  // Specify the bounding box
////  b.BoundingBox(Location(0,0), Location(10,10));
////// Test the tree topology with animation.
////// George F. Riley, Georgia Tech, Fall 2003
////
//#include <iostream>
//
//#include "G_common-defs.h"
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
