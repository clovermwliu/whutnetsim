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



// Georgia Tech Network Simulator - Histogram Class
// George F. Riley.  Georgia Tech, Spring 2002

// Defines a Histogram and CDF statistics collection

#include "G_debug.h"
#include "histogram.h"

using namespace std;

// Constructors
Histogram::Histogram()
    : min(0), max(0), outOfRange(0)
{
}

Histogram::Histogram(double m)
    : min(0), max(m), outOfRange(0)
{
}

Histogram::Histogram(double m, Count_t n, double mn)
    : bins(n), min(mn), max(m), outOfRange(0)
{
  Bins(m, n, mn);
}

Histogram::~Histogram()
{
}

void Histogram::Bins(double m, Count_t n, double mn)
{
  bins.clear();                         // Insure empty
  bins.insert(bins.begin(), n, 0);      // Insert "n" empty elements
  outOfRange = 0;
  max = m;
  min = mn;
}

bool Histogram::Record(double v)
{ // Add an entry to histogram with value v
  if ((max-min) == 0)
    {
      outOfRange++;
      return false;
    }
  if (v < min || v > max)
    {
      outOfRange++;
      return false;
    }
  
  int binnum = (int)((bins.size()/(max-min))*(v-min));
  DEBUG0((cout << "Histo::Count, v " << v << " binnum " << binnum << endl));
  if ((vector<Count_t>::size_type)binnum >= bins.size() || binnum < 0)
    {
      outOfRange++;
      return false;
    }
  bins[binnum]++; // Add to this bin
  return true;
}

bool Histogram::Record(Time_t, double v)
{ // Add an entry to histogram with value v
  // Ignore time value and just record
  return Record(v);
}

void Histogram::Reset()
{ // Reset to initial state
  for (vector<Count_t>::size_type i = 0; i < bins.size(); ++i)
    {
      bins[i] = 0;
    }
  outOfRange = 0;
}

void Histogram::Log(ostream& os, const char* hdr, const char sep)
{
  double perBin = (max-min) / bins.size();
  double f = min;
  if (hdr) os << hdr << endl;
  for (vector<Count_t>::size_type i = 0; i < bins.size(); ++i)
    {
      os << f << sep << f + perBin << sep << bins[i] << endl;
      f += perBin;
    }
}

Statistics* Histogram::Copy() const
{
  return new Histogram(*this);
}

void Histogram::CDF(ostream& os, const char* hdr, const char sep)
{
  double perBin = (max-min) / bins.size();
  double f = min;
  // Find total number of observations
  Count_t tot = outOfRange;
  for (vector<Count_t>::size_type i = 0; i < bins.size(); ++i)
    {
      tot += bins[i];
    }
  Count_t cum = 0;
  if (hdr) os << hdr << endl;
  os << "0" << sep << "0.0" << endl; // always 0 prob. < 0
  for (vector<Count_t>::size_type i = 0; i < bins.size(); ++i)
    {
      f += perBin;
      cum += bins[i]; // Cumulative total
      double pr = (double)cum/(double)tot;
      os << f << sep << pr << endl;
    }
  os << "Out of Range " << outOfRange << endl;
}

