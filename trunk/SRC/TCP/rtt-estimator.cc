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

#include <iostream>

//#define DEBUG_MASK 0x01
#include "G_debug.h"
#include "rtt-estimator.h"
#include "simulator.h"
#include "statistics.h"

using namespace std;

RTTHistory::RTTHistory(Seq_t s, Count_t c, Time_t t)
    : seq(s), count(c), time(t), retx(false)
{
}

RTTHistory::RTTHistory(const RTTHistory& h)
    : seq(h.seq), count(h.count), time(h.time), retx(h.retx)
{
}

// RTTEstimator Static Member variables
Time_t        RTTEstimator::initialEstimate = 1.0; // Default initial estimate
RTTEstimator* RTTEstimator::defaultRTT = new RTTMDev(0.1, 1.0);
Statistics*   RTTEstimator::globalStats = nil; // No global stats
Mult_t        RTTEstimator::maxMultiplier = 64.0;  // Maximum value of multiplier

// Base class methods

RTTEstimator::RTTEstimator(const RTTEstimator& c)
  : next(c.next), history(c.history), est(c.est), nSamples(c.nSamples),
    multiplier(c.multiplier), privateStats(nil)
{
  if (c.privateStats) privateStats = c.privateStats->Copy();
}

RTTEstimator::~RTTEstimator()
{
  if (privateStats) delete privateStats;
}

void RTTEstimator::SentSeq(Seq_t s, Count_t c)
{ // Note that a particular sequence has been sent
  if (s == next)
    { // This is the next expected one, just log at end
      history.push_back(RTTHistory(s, c, Simulator::Now()));
      next = s + c; // Update next expected
    }
  else
    { // This is a retransmit, find in list and mark as re-tx
      for (RTTHistory_t::iterator i = history.begin(); i != history.end(); ++i)
        {
          if ((s >= i->seq) && (s < (i->seq + i->count)))
            { // Found it
              i->retx = true;
              // One final test..be sure this re-tx does not extend "next"
              if ((s + c) > next)
                {
                  next = s + c;
                  i->count = ((s + c) - i->seq); // And update count in hist
                }
              break;
            }
        }
    }
}

Time_t RTTEstimator::AckSeq(Seq_t a)
{ // An ack has been received, calculate rtt and log this measurement
  // Note we use a linear search (O(n)) for this since for the common
  // case the ack'ed packet will be at the head of the list
  Time_t m = 0.0;
  if (history.size() == 0) return(m);    // No pending history, just exit
  RTTHistory& h = history.front();
  if (!h.retx && a >= (h.seq + h.count))
    { // Ok to use this sample
      m = Simulator::Now() - h.time; // Elapsed time
      DEBUG0((cout << "RTT logging estimate of " << m << " seconds" 
              << " ack " << a << " h.seq " << h.seq 
              << " h.count " << h.count << endl));
      Measurement(m);                // Log the measurement
      ResetMultiplier();             // Reset multiplier on valid measurement
      if (globalStats) globalStats->Record(m);   // Log statistics if present
      if (privateStats) privateStats->Record(m); // Log private stats
    }
  // Now delete all ack history with seq <= ack
  while(history.size() > 0)
    {
      RTTHistory& h = history.front();
      if ((h.seq + h.count) > a) break;                // Done removing
      history.pop_front(); // Remove
    }
  return m;
}

void RTTEstimator::ClearSent()
{ // Clear all history entries
  next = 0;
  history.clear(); 
}

void RTTEstimator::IncreaseMultiplier()
{
  multiplier = min(multiplier * 2.0, maxMultiplier);
}

void RTTEstimator::ResetMultiplier()
{
  multiplier = 1.0;
}

void RTTEstimator::Reset()
{ // Reset to initial state
  next = 0;
  est = initialEstimate;
  history.clear();         // Remove all info from the history
  if(privateStats) privateStats->Reset();   // Clear statistics
  nSamples = 0;
  ResetMultiplier();
}

void RTTEstimator::SetPrivateStatistics(const Statistics& s)
{
  privateStats = s.Copy();
}

  
// Base class, static methods
void RTTEstimator::InitialEstimate(Time_t e)
{ // Set a new default initial estimate
  initialEstimate = e;
  defaultRTT->est = e;
}

void RTTEstimator::Default(const RTTEstimator& rtt)
{
  if (defaultRTT) delete defaultRTT;
  defaultRTT = rtt.Copy();
}

RTTEstimator* RTTEstimator::Default()
{
  return defaultRTT;
}

void RTTEstimator::SetStatistics(Statistics* s)
{
  globalStats = s;
}

// Mean-Deviation Estimator methods

RTTMDev::RTTMDev(const RTTMDev& c)
  : RTTEstimator(c), gain(c.gain), variance(c.variance)
{
}

void RTTMDev::Measurement(Time_t m)
{
  if (nSamples)
    { // Not first
      Time_t err = m - est;
      est = est + gain * err;         // estimated rtt
      if (err < 0) err = -err;        // absolute value of error
      variance = variance + gain * (err - variance); // variance of rtt
    }
  else
    { // First sample
      est = m;                        // Set estimate to current
      //variance = m / 2;               // And variance to current / 2
      variance = m; // try this
    }
  nSamples++;
}

Time_t RTTMDev::RetransmitTimeout()
{
  // If not enough samples, justjust return 2 times estimate   
  //if (nSamples < 2) return est * 2;
  if (variance < est / 4.0)
    return est * 2 * multiplier;            // At least twice current est
  return (est + 4 * variance) * multiplier; // As suggested by Jacobson
}

RTTEstimator* RTTMDev::Copy() const
{
  return new RTTMDev(*this);
}

void RTTMDev::Reset()
{ // Reset to initial state
  variance = 0.0;
  RTTEstimator::Reset();
}
