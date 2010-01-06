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



// Georgia Tech Network Simulator - Queue base class
// George F. Riley.  Georgia Tech, Spring 2002

// Virtual Base class for all queue types.

#ifndef __queue_h__
#define __queue_h__

#include <iostream>
#include <deque>
#include <list>
#include <vector>

#include "packet.h"
#include "tcp.h"   // For TimeSeq_t

class Interface;

typedef unsigned int Qrgb; // This is so we don't need any qt includes here

//Doc:ClassXRef
class ForcedLoss { // Information for forcing packet losses
public:
  ForcedLoss(Time_t t, Count_t c)
    : time(t), count(c), expiration(INFINITE_TIME) {}
  ForcedLoss(Time_t t, Count_t c, Time_t e)
    : time(t), count(c), expiration(e) {}
public:
  Time_t  time;  // Time for loss
  Count_t count; // Number of losses starting at (time)
  Time_t  expiration; // Expiration time
};

typedef std::list<Packet*>    PktList_t;  // List of pending packets
typedef std::list<ForcedLoss> LossList_t; // List of forced losses
typedef std::vector<Qrgb>     ColorVec_t; // Vector of colors for each packet

//Doc:ClassXRef
class NDInfo { // Information for non-detailed queues
public:
  NDInfo() : startTx(0), stopTx(0), size(0), updated(false) { }
  NDInfo(Time_t s, Time_t e, Size_t sz)
    : startTx(s), stopTx(e), size(sz), updated(false) { }
public:
  Time_t startTx; // Time transmit starts
  Time_t stopTx;  // Time transmit ends
  Size_t size;    // Size of packet in bytes
  bool   updated; // True if queue size already updated for this one
};

class QTWindow;
class QCanvasLine;
typedef std::deque<NDInfo> NDDeq_t; // Deque of Non-detailed info
typedef std::vector<QCanvasLine*> ItemVec_t;

//Doc:ClassXRef
class Queue {
//Doc:Class The class {\tt Queue} defines the base class for interface queues
//Doc:Class that enque packets before they are sent down on the link
//Doc:Class Different queuing disciplines may inherit from this class to
//Doc:Class define their own functionality
public:
  //Doc:Method
  Queue(bool d = true)
    : totalByteSeconds(0), startTime(0.0),
      lastUpdate(0.0), detailed(d), forcedLosses(nil), timeSizeLog(nil),
      timeSizePackets(false),
#ifndef WIN32
      verbose(false), dropCount(0), totEnq(0), ndDeq(nil), interface(nil),
#else
	  verbose(false), dropCount(0), totEnq(0), ndDeq(nil), interface_(nil),
#endif
      animate(false), qLines(nil)
    { }
  //Doc:Desc This method is the default constructor

  // Destructor
  virtual ~Queue() { }

  //Doc:Method
  virtual bool Enque(Packet*) = 0;    // Return true if enqued, false if not
  //Doc:Desc This method is used to enque a packet in the queue
  //Doc:Arg1 the pointer to the packet to be added to the queue
  //Doc:Return a boolean to indicate if the enque operation succeeded

  //Doc:Method
  virtual Packet* Deque() = 0;        // Return next element (NULL if none)
  //Doc:Desc This method deques a packet from the queue and returns it
  //Doc:Return Pointer to the packet, or nil if none available

  //Doc:Method
  virtual Packet* PeekDeque() = 0;
    //Doc:Desc Return a pointer to the next packet in the queue, without
    //Doc:Desc removing it.  This is used by some subclasses of
    //Doc:Desc   queue (DiffServQueue for example) to find out if the packet
    //Doc:Desc has enough tokens to be transmitted.
    //Doc:Return Pointer to the packet, or nil if none available.

  //Doc:Method
  virtual void DummyEnque(Packet*);
    //Doc:Desc This method is called by interfaces when the link is free
    //Doc:Desc     and no queuing is necessary.  However, some queuing
    //Doc:Desc     methods (for example DRR) need to know that a packet
    //Doc:Desc has been sent to manage the fairness properly.
    //Doc:Arg1 Packet being transmitted.

  //Doc:Method
  virtual Count_t DequeAllDstMac(MACAddr);
    //Doc:Desc   Remove all packets in the queue with the specified
    //Doc:Desc destination MAC address.
    //Doc:Arg1 MAC Address to remove
    //Doc:Return Count of removed packets

  //Doc:Method
  virtual Count_t DequeAllDstIP(IPAddr_t);
    //Doc:Desc   Remove all packets in the queue with the specified
    //Doc:Desc destination IP address.
    //Doc:Arg1 IP Address to remove
    //Doc:Return Count of removed packets

  //Doc:Method
  virtual Packet* DequeOneDstMac(MACAddr);
    //Doc:Desc   Remove a packet in the queue with the specified
    //Doc:Desc destination MAC address.
    //Doc:Arg1 MAC Address to remove
    //Doc:Return Pointer to the removed packet

  //Doc:Method
  virtual Packet* DequeOneDstIP(IPAddr_t);
    //Doc:Desc   Remove a packet in the queue with the specified
    //Doc:Desc destination IP address.
    //Doc:Arg1 IP Address to remove
    //Doc:Return Pointer to the removed packet

  //Doc:Method
  virtual Count_t Length() = 0;       // How many bytes are enqued?
  //Doc:Desc This method returns the size of the queue in terms of bytes
  //Doc:Return number of bytes

  //Doc:Method
  virtual Count_t LengthPkts() = 0;   // How many packets are enqued?
  //Doc:Desc This method returns the size of the queue in terms of the
  //Doc:Desc number of packets
  //Doc:Return number of packets enqued

  //Doc:Method
  virtual Queue* Copy() const = 0;    // Make a copy of this queue
  //Doc:Desc This method makes a copy of the queue and returns a pointer
  //Doc:Desc to the copy
  //Doc:Return pointer to a copy of the queue

  //Doc:Method
  virtual void SetLimit(Count_t) = 0; // Set new limit
  //Doc:Desc This method sets a limit to the size of the queue. this limit
  //Doc:Desc is in terms of bytes
  //Doc:Arg1 size of the queue in bytes

  //Doc:Method
  virtual Count_t GetLimit() const = 0;     // Get limit (bytes)
  //Doc:Desc This method returns the size of the queue in terms of the bytes
  //Doc:Return Max limit of the queue size in bytes

  //Doc:Method
  virtual void SetLimitPkts(Count_t) = 0;   // Set packets limit
  //Doc:Desc This method sets a limit to the size of the queue. this limit
  //Doc:Desc is in terms of number of packets that can be enqued
  //Doc:Arg1 size of the queue in packets

  //Doc:Method
  virtual Count_t GetLimitPkts() const = 0; // Get packets limit
  //Doc:Desc This method returns the size of the queue in terms of the packets
  //Doc:Return Max limit of the queue size in number of packets


  //Doc:Method
  DCount_t Average();                 // Get average queue length
  //Doc:Desc This method returns the average queue length
  //Doc:Return average queue length

  //Doc:Method
  void     ResetAverage();            // Start a new averaging interval
  //Doc:Desc This method resets the averages and starts a new interval
  //Doc:Desc to calculate averages

  //Doc:Method
  void     UpdateAverage();           // Queue is changing, update average
  //Doc:Desc This method updates the averages. ala moving averages

  //Doc:Method
  virtual bool Check(Size_t, Packet* = nil) = 0;// Test if buff space available
  //Doc:Desc This method checks if buffer space (of a given size) is available
  //Doc:Arg1 The size to be checked
  //Doc:Arg2 The pointer to a packet to be queued.
  //Doc:Return boolean to indicate true or false

  //Doc:Method
  virtual bool Detailed();            // True if detailed model w/ local queing
  //Doc:Desc This method returns a bool to indicate if detailed queue model
  //Doc:Desc is to be used
  //Doc:Return bool to indicate true or false

  //Doc:Method
  virtual void Detailed(bool);        // Set detailed on/off
  //Doc:Desc This method is used to set if detailed model is to be used
  //Doc:Arg1 bool to indicate if detailed model is needed

  //Doc:Method
  virtual Time_t   QueuingDelay();    // Calculate queuing delay at current lth
  //Doc:Desc This method calculates the current queuing delay
  //Doc:Return the queuing delay

  //Doc:Method
  virtual DCount_t TotalWorkload();   // Return the byte-seconds (workload)
  //Doc:Desc This method calculates the toal work load in terms of byte-secs
  //Doc:Return The workload

  //Doc:Method
  virtual void     SetInterface(Interface*);  // Let the q know which interface
  //Doc:Desc This method attaches the queue to an interface
  //Doc:Arg1 The interface to which it is to be attached

  //Doc:Method
  virtual Count_t  DropCount();       // Number of dropped packets
  //Doc:Desc This method returns the number of dropped packets.
  //Doc:Return  the number of dropped packets.

  //Doc:Method
  virtual Count_t  EnqueueCount();    // Number of enqueued packets
  //Doc:Desc This method returns the number of packets enqueued
  //Doc:Return  the number of enqueued packets.


  //Doc:Method
  virtual void     CountEnq(Packet* = nil); // Count an enque
  //Doc:Method

  virtual void     ResetStats();      // Reset the statistics to zero
  //Doc:Desc This method zeroes down all the statistics pertaining to this
  //Doc:Desc queue

  //Doc:Method
  void             AddForcedLoss(Time_t, Count_t = 1, Time_t = INFINITE_TIME);
    //Doc:Desc Adds information about a forced  loss.  Allows a simulation to
    //Doc:Desc force a packet loss at a particular time, to observe
    //Doc:Desc protocol behavior in the presence of losses.
    //Doc:Arg1 Time of the loss.
    //Doc:Arg2 Number of consecutive packets to drop, stating at the time.
    //Doc:Arg3 Expiration time for this loss.  If all the requested losses
    //Doc:Arg3 are not accomplished by this time, then the remainder
    //Doc:Arg3 are not enforced.

  //Doc:Method
  bool             CheckForcedLoss(bool = true);
    //Doc:Desc Checks if a forced loss should be enforced.
    //Doc:Desc Called by each subclass
    //Doc:Desc prior to enquing a packet.
    //Doc:Desc The forced loss will be counted, and the entry
    //Doc:Desc removed if all forced losses have been enforced.
    //Doc:Arg1 True if the forced loss entry should be removed by this call.
    //Doc:Return True if time for a forced loss.

  //Doc:Method
  virtual bool CheckSpoofedSource(Packet*);
    //Doc:Desc Some queue types enforce egress filtering and drop spoofed
    //Doc:Desc  source addresses.  This checks if the packet should
    //Doc:Desc be dropped for spoofing.  Returns false for all queue
    //Doc:Desc types that don't check this, and true for those that
    //Doc:Desc do check and that find a spoofed source address.
    //Doc:Arg1 Packet to check.
    //Doc:Return True if packet should be dropped due to spoofing.

  //Doc:Method
  void    EnableTimeSizeLog(bool = true);
    //Doc:Desc Enable or disable the logging of size vs. time history.
    //Doc:Desc NOTE.  This should be enabled with care, since the memory
    //Doc:Desc demands of the log are significant.
    //Doc:Arg1 True if Time/Size logging desired.

  //Doc:Method
  void    LogSizePackets(bool = true);
    //Doc:Desc Specify that the TimeSize log should log in units
    //Doc:Desc of packets (or not).  Default is log in bytes.
    //Doc:Arg1 True of logging in units of packets desired.

  //Doc:Method
  void    PrintTimeSizeLog(std::ostream&,
                           Count_t div = 0,
                           char sep = ' ');
    //Doc:Desc Log previously collected time/size data to a file.
    //Doc:Arg1 Output stream to log the data.
    //Doc:Arg1 If non-zero, each entry is divided by this value.
    //Doc:Arg3 Separator character between time and sequence number

  //Doc:Method
  void    Animate(bool b = true) { animate = b;}
    //Doc:Desc   Specify animation of this queue is (or is not) desired.
    //Doc:Arg1 True if animation desired.

  //Doc:Method
  bool    Animated() { return animate;}
    //Doc:Desc Query if animation enabled.
    //Doc:Return True if animation enabled.

  //Doc:Method
  virtual void    DisplayQueue(QTWindow*, Node*, Node*);
  //Doc:Desc Animate the queue occupancy.
  //Doc:Arg1 QTWindow object.
  //Doc:Arg2 Source Node.
  //Doc:Arg3 Destinattion Node.

  //Doc:Method
  virtual Packet* GetPacket(Count_t k) = 0;
    //Doc:Desc Used by the animation.  Gets the k'th packet from the head.
    //Doc:Arg1 Index of desired packet.
    //Doc:Return Packet pointer to k'th packet.  Nil if does not exist.

  //Doc:Method
  virtual void     GetPacketColors(ColorVec_t&) = 0;
    //Doc:Desc Fill in the vector of colors for each pending packet.
    //Doc:Desc This is more efficient than the GetPacket method above,
    //Doc:Desc and the above is deprecated.
    //Doc:Arg1 Vector of colors to fill in.
    //Doc:Arg1 zero'th entry is next packet to deque.

  void             Verbose(bool v = true) { verbose = v;}
protected:
  void UpdateTimeSizeLog();
public:
  // Managing the default queue
  //Doc:Method
  static void    Default(const Queue& d);
  //Doc:Desc This method sets a the default queue
  //Doc:Arg1 reference to a queue object

  //Doc:Method
  static Queue&  Default();
  //Doc:Desc This method returns a reference to the default queue object
  //Doc:Return Reference to the default queue

  //Doc:Method
  static void    DefaultLength(Count_t l);
  //Doc:Desc This method sets the default queue length in bytes
  //Doc:Arg1 the count in bytes

  //Doc:Method
  static Count_t DefaultLength() { return defaultLength;}
  //Doc:Desc This method returns the default queue size in bytes.
  //Doc:Return the default size in bytes

  //Doc:Method
  static void    DefaultLimitPkts(Count_t l);
  //Doc:Desc This method sets the default queue length in terms of the
  //Doc:Desc number of packets
  //Doc:Arg1 the count in bytes

  //Doc:Method
  static Count_t DefaultLimitPkts() { return defaultLimitPkts;}
  //Doc:Desc This method returns the default queue limit in packets.
  //Doc:Desc   Limit of zero indicates queue in bytes, not packets.
  //Doc:Return the default size in packets

  

private:
  DCount_t    totalByteSeconds;     // Area under time/occupancy curve
  Time_t      startTime;            // Start time of this measurement
  Time_t      lastUpdate;           // Time of last update
  bool        detailed;             // True if using detailed queuing
  LossList_t* forcedLosses;         // List of pending forced losses
  TimeSeq_t*  timeSizeLog;          // A log of size vs time
  bool        timeSizePackets;      // True if tims/size log is in packets
protected:
  bool        verbose;              // True if verbose debug
  Count_t     dropCount;            // Total dropped packets
  Count_t     totEnq;               // Total enqueued packets
  NDDeq_t*    ndDeq;                // Pointer to non-detailed deq info
public:
#ifndef WIN32
	Interface*  interface;            // Associated interface
#else
	Interface*  interface_;            // Associated interface
#endif
private:
  bool        animate;              // True if queue animation desired
  ItemVec_t*  qLines;               // QCanvasLines used to animate queue
  // Static Members
  static Queue*  defaultQueue;      // Default queue type
  static Count_t defaultLength;     // Default queue length
  static Count_t defaultLimitPkts;  // Default limit (packets)
public:
  static Count_t defaultAnimWidth;  // Width (in pixels) of animated queues
  static Count_t globalQueueDrop;   // Total dropped due to queue overflow
};

#endif



