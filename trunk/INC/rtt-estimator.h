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



// Georgia Tech Network Simulator - Round Trip Time Estimation Class
// George F. Riley.  Georgia Tech, Spring 2002

// Implements several variations of round trip time estimators

#ifndef __rtt_estimator_h__
#define __rtt_estimator_h__

#include "G_common-defs.h"

#include <deque>

//Doc:ClassXRef
class RTTHistory {
public:
  RTTHistory(Seq_t s, Count_t c, Time_t t);
  RTTHistory(const RTTHistory& h); // Copy constructor
public:
  Seq_t   seq;    // First sequence number in packet sent
  Count_t count;  // Number of bytes sent
  Time_t  time;   // Time this one was sent
  bool    retx;   // True if this has been retransmitted
};

typedef std::deque<RTTHistory> RTTHistory_t;

class Statistics;
//Doc:ClassXRef
class RTTEstimator {  //  Base class for all RTT Estimators
  //Doc:Class Class {\tt RTTEstimator} is a virtual base class which
  //Doc:Class defines the behavior of a round trip time estimator used
  //Doc:Class by {\tt TCP}.
public:
  // Default constructor  
  //Doc:Method
  RTTEstimator() : next(0), history(), est(initialEstimate),
    nSamples(0), multiplier(1.0), privateStats(nil) { } 
    //Doc:Desc {\tt RTTEstimator} constructor, no arguments.

  //Doc:Method
  RTTEstimator(Time_t e) : next(0), history(), est(e),
    nSamples(0), multiplier(1.0), privateStats(nil) { }
    //Doc:Desc {\tt RTTEstimator} constructor specifying
    //Doc:Desc the initial RTT estimate.
    //Doc:Arg1 Initial RTT Estimate.

  //Doc:Method
  RTTEstimator(const RTTEstimator&); // Copy constructor
    //Doc:Desc Copy constructor.
    //Doc:Arg1 {\tt RTTEstimator} object to copy.

  virtual ~RTTEstimator();

  // Note that a particular sequence was sent  
  //Doc:Method
  virtual void SentSeq(Seq_t, Count_t);
    //Doc:Desc Notify the RTT estimator that a particular sequence number
    //Doc:Desc was sent.
    //Doc:Arg1 First sequence number in the block of data sent.
    //Doc:Arg2 Length of block sent (bytes).

  // Note a particular seq was acknowledged
  // Return measured rtt (0 if invalid)
  //Doc:Method
  virtual Time_t AckSeq(Seq_t);
    //Doc:Desc Notify the RTT estimator that a particular sequence number
    //Doc:Desc has been acknowledged.
    //Doc:Arg1 Sequence number acknowledged.
    //Doc:Return Reasured RTT for this sample.

  // Clear the sent-seq list
  //Doc:Method
  virtual void ClearSent();
    //Doc:Desc Clear the list of pending sequence numbers sent.

  // Add a measurement to the filter
  //Doc:Method
  virtual void   Measurement(Time_t t) = 0;
    //Doc:Desc Add a round trip time measurement to the filter.
    //Doc:Arg1 Individual measurement (seconds).

  // Get the current estimate
  //Doc:Method
  virtual Time_t Estimate() = 0;
    //Doc:Desc Return the current RTT estimate.
    //Doc:Return Current RTT estimate (seconds).

  // Get the timeout period
  //Doc:Method
  virtual Time_t RetransmitTimeout() = 0;
    //Doc:Desc Return the computed retransmit timeout period.
    //Doc:Return Retransmit timeout period (seconds).

  // Set the initial sequence number
  //Doc:Method
  void Init(Seq_t s) { next = s;}
    //Doc:Desc Set the initial sequence number for this estimator.
    //Doc:Arg1 Initial sequence number.

  // Make a copy of this estimator
  //Doc:Method
  virtual RTTEstimator* Copy() const = 0;
    //Doc:Desc Make a copy of this estimator.
    //Doc:Return Pointer to copy of this estimator.

  //Doc:Method
  virtual void IncreaseMultiplier();
    //Doc:Desc Increase the multiplier for the retransmit timeout calculation.
    //Doc:Desc Many {\tt TCP} implementations exponentially increase
    //Doc:Desc the retransmit timeout period on each timeout action.

  //Doc:Method
  virtual void ResetMultiplier();
    //Doc:Desc Reset the retransmit multiplier to the initial value.

  // Reset to initial state
  //Doc:Method
  virtual void Reset();
    //Doc:Desc Reset this RTT estimator to its initial state.

  //Doc:Method
  void SetPrivateStatistics(const Statistics&);
    //Doc:Desc Specify a statistics object to track the behavior  of this
    //Doc:Desc estimator object.
    //Doc:Arg1 Reference to any statistics object.

  //Doc:Method
  Statistics* GetPrivateStatistics() { return privateStats;}
    //Doc:Desc Get a pointer to the statistics object for this estimator.
    //Doc:Return Statistics pointer to current private statistics object,
    //Doc:Return {\tt nil} if none.

private:
  Seq_t        next;    // Next expected sequence to be sent
  RTTHistory_t history; // List of sent packet
public:
  Time_t       est;     // Current estimate
  Count_t      nSamples;// Number of samples
  Mult_t       multiplier;   // RTO Multiplier
  Statistics*  privateStats; // Statistics for this rtt only
public:
  //Doc:Method
  static void InitialEstimate(Time_t);
    //Doc:Desc Set a global default initial estimate.
    //Doc:Arg1 Initial estimate for all rtt estimators.

  //Doc:Method
  static void Default(const RTTEstimator&);  // Set new default
    //Doc:Desc Set a new global default RTT estimator.
    //Doc:Arg1 Reference to an RTT estimator to use for global default.

  //Doc:Method
  static RTTEstimator* Default();      // Retrieve current default
    //Doc:Desc Get a pointer to the current default RTT estimator.
    //Doc:Return Pointer to current default RTT estimator.

  //Doc:Method
  static void SetStatistics(Statistics*); // Set global statistics object
    //Doc:Desc Set a statistics object for global statistics.  This
    //Doc:Desc collects the statistics from all RTT estimators in a
    //Doc:Desc single statistics object.
    //Doc:Arg1 Reference to a statistics object for global data collection.

private:
  static Time_t initialEstimate;       // Default initial estimate
  static RTTEstimator* defaultRTT;     // Default RTT Estimator
  static Statistics*   globalStats;    // Non-null if global statistics kept
  static Mult_t        maxMultiplier;  // Maximum value of multiplier
};

// The "Mean-Deviation" estimator, as discussed by Van Jacobson
// "Congestion Avoidance and Control", SIGCOMM 88, Appendix A

//Doc:ClassXRef
class RTTMDev : public RTTEstimator {
  //Doc:Class Class {\tt RTTMDev} implements the "Mean--Deviation" estimator
  //Doc:Class as described by Van Jacobson 
  //Doc:Class "Congestion Avoidance and Control", SIGCOMM 88, Appendix A
public :
  //Doc:Method
  RTTMDev(Mult_t g) : gain(g), variance(0) { }
    //Doc:Desc Constructor for {\tt RTTMDev} specifying the gain factor for the
    //Doc:Desc estimator.
    //Doc:Arg1 Gain factor.

  //Doc:Method
  RTTMDev(Mult_t g, Time_t e) : RTTEstimator(e), gain(g), variance(0) { }
    //Doc:Desc Constructor for {\tt RTTMDev} specifying the gain factor
    //Doc:Desc and the initial estimate.
    //Doc:Arg1 Gain factor.
    //Doc:Arg2 Initial estimate.

  //Doc:Method
  RTTMDev(const RTTMDev&); // Copy constructor
    //Doc:Desc Copy constructor.
    //Doc:Arg1 {\tt RTTMDev} object to copy.

  void   Measurement(Time_t);
  Time_t Estimate() { return est;}
  Time_t Variance() { return variance;}
  Time_t RetransmitTimeout();
  RTTEstimator* Copy() const;
  void   Reset();
  // Set the filter gain
  //Doc:Method
  void Gain(Mult_t g) { gain = g;}
    //Doc:Desc Set the filter gain for this estimator.
    //Doc:Arg1 Gain factor.

public:
  Mult_t       gain;       // Filter gain
  Time_t       variance;   // Current variance
};

#endif



