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



// Georgia Tech Network Simulator - Global trace control
// George F. Riley.  Georgia Tech, Spring 2002

#ifndef __trace_h__
#define __trace_h__

#include <fstream>
#include <vector>

#include "G_common_defs.h"
#include "tfstream.h"

class Node;
class Protocol;
class PDU;

//Doc:ClassXRef
class Trace {
  //Doc:Class Class {\tt Trace} defines the behavior of the packet tracing
  //Doc:Class feature of \GTNS.
public:
  typedef enum { ENABLED, DISABLED, DEFAULT } TraceStatus;
  typedef std::vector<TraceStatus> TSVec_t;
  typedef bool (*Callback_t)(Node*,Protocol*,PDU*,bool);
public:
  //Doc:Method
  Trace();
    //Doc:Desc Default constructor, no arguments.

  ~Trace();
  //Doc:Method
  bool Open(const char*);  // Open the trace file
    //Doc:Desc Create a trace file.
    //Doc:Return     //Doc:Arg1 File name.
    //Doc:Return True if successfully opened.

  //Doc:Method
  void Close();            // Close the trace file
    //Doc:Desc Close the trace file.  Should be called when the simulation completes.

  //Doc:Method
  void On();               // Enable all traces
    //Doc:Desc Enable tracing for all protocol layers.

  //Doc:Method
  void On(Layer_t);        // Enable for specific layer
    //Doc:Desc Enable tracing for a specified protocol layer.
    //Doc:Arg1 Layer to enable (2, 3, or 4).

  //Doc:Method
  void Off();              // Disable all traces
    //Doc:Desc Enable tracing for all protocol layers.

  //Doc:Method
  void Off(Layer_t);       // Disable for specific layer
    //Doc:Desc Disable tracing for a specified protocol layer.
    //Doc:Arg1 Layer to disable (2, 3, or 4).

  //Doc:Method
  bool IsOn(Layer_t);      // Enquire trace status (on/off) by layer
    //Doc:Desc Test whether tracing is on or off by layer.
    //Doc:Arg1 Layer to test.
    //Doc:Return True if tracing enabled for this layer.

  //Doc:Method
  bool IsEnabled();        // Enquire trace file opened
    //Doc:Desc Check if trace file globally enabled.
    //Doc:Return True if enabled.

  Tfstream& GetTS() { return os;} // The trace file object
  void AppendEOL();        // Add the end of line if needed
  void NewNode(Node*);     // Log a new node for trace line
  void SetNode(Node*);     // Set the node for subsequent traces

  //Doc:Method
  void IPDotted(bool d = true) { os.IPDotted(d);}
    //Doc:Desc Specify that all \IPA{s} are to be logged in dotted notation.
    //Doc:Desc If not, trace in 32 bit hex.
    //Doc:Arg1 True if dotted desired.

  //Doc:Method
  void TimePrecision(Count_t d) { os.TimePrecision(d);}
    //Doc:Desc Specify the digits of precision in the time field for
    //Doc:Desc the trace file.
    //Doc:Arg1 Number of digits of precision.

  //Doc:Method
  static Trace* Instance();// Get the global trace object
    //Doc:Desc Get a pointer to the single, global trace object.
    //Doc:Return Pointer to trace object.

  //Doc:Method
  static bool   Enabled() { return enabled;} // True if tracing enabled
    //Doc:Desc Test if tracing is enabled.
    //Doc:Return True if tracing enabled.

  //Doc:Method
  static void AddCallback(Callback_t cb) { callback = cb;}
    //Doc:Desc Add a callback to be called on every trace decision.
    //Doc:Desc The callback is called AFTER the normal decisioning process,
    //Doc:Desc and the result of that is passsed to the callback.  Also
    //Doc:Desc passed are the node pointer, protocol pointer, and pdu pointer.
    //Doc:Desc The callback must be a funtion returning a bool (which is the
    //Doc:Desc final decision on whether the PDU is traced or not) and
    //Doc:Desc with four arguments, Node*, Protocol*, PDU*, bool.
    //Doc:Desc See definition of Callback_t in trace.h.
    //Doc:Arg1 Callback function to add.

public:
  static Callback_t callback;

private:
  static Trace* instance;  // The single global trace object
  static bool   enabled;   // Global enabled flag
  TSVec_t  trace;          // On/Off status by layer
  Tfstream os;             // File for logging
  Node*    node;           // Current node
};

#endif

