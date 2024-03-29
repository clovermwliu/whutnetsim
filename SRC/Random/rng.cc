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



// Georgia Tech Network Simulator - Random Number class
// George F. Riley.  Georgia Tech, Spring 2002

#include <iostream>

#include <math.h>
#include <stdlib.h>
#ifndef WIN32
#include <sys/time.h>			// for gettimeofday
#include <unistd.h>
#else
#include <time.h>			// for gettimeofday
#include <windows.h>
#include <winsock.h>
#endif

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>       

//#define DEBUG_MASK 0x01
#include "G_debug.h"
#include "rng.h"

using namespace std;

// Seed methods
ConstantSeed::ConstantSeed(Seed_t s)
{
  seeds[0] = s;
  seeds[1] = s;
  seeds[2] = s;
  seeds[3] = s;
  seeds[4] = s;
  seeds[5] = s;
}

ConstantSeed::ConstantSeed(Seed_t s0, Seed_t s1, Seed_t s2,
                           Seed_t s3, Seed_t s4, Seed_t s5)
{
  seeds[0] = s0;
  seeds[1] = s1;
  seeds[2] = s2;
  seeds[3] = s3;
  seeds[4] = s4;
  seeds[5] = s5;
}

// Base class methods

bool          Random::useDevRandom = false;  // True if use /dev/random desired
bool          Random::globalSeedSet = false; // True if GlobalSeed called
int           Random::devRandom = -1;
Seed_t        Random::globalSeed[6];
unsigned long Random::heuristic_sequence;
RngStream*    Random::globalRNG;

Random::Random() 
    : privateStream(nil)
{ // Nothing to be done here
  DEBUG0((cout << "Hello from Random::Random()" << endl));
}
  
Random_t Random::Value()
{
  Initialize();
  // Check if private stream, use it if so
  if (privateStream) return privateStream->RandU01();
  // Otherwise use the common global one
  return globalRNG->RandU01(); // Uniform [0..1)
}

Random* Random::Copy() const
{
  return new Random(*this);
}

bool Random::SetSeed(const Seed& s)
{
  // Get a private stream for this generator if not already created
  if (s.IsRandom())
    {
      Seed_t seeds[6];
      GetRandomSeeds(seeds);
      if (!privateStream) privateStream = new RngStream();
      return privateStream->SetSeed(seeds);
    }
  // Not random seed, use specified
  const ConstantSeed& cs = (ConstantSeed&)s;
  if (!RngStream::CheckSeed(cs.seeds)) return false; // Seed is not valid
  if (!privateStream) privateStream = new RngStream();
  return privateStream->SetSeed(cs.seeds);
}

// Uniform methods
Random_t Uniform::Value()
{
  return m_min + Random::Value() * (m_max - m_min);
}

Random* Uniform::Copy() const
{
  return new Uniform(*this);
}

// Constant methods
Random_t Constant::Value()
{
  return m_const;
}

IRandom_t Constant::IntValue()
{
  return (IRandom_t)m_const;
}

Random* Constant::Copy() const
{
  return new Constant(*this);
}

// Sequential methods
Sequential::Sequential(Random_t f, Random_t l, Random_t i, Count_t c)
  : m_min(f), m_max(l), m_increment(Constant(i).Copy()), m_consecutive(c),
    m_current(f), m_currentConsecutive(0)
{
}

Sequential::Sequential(Random_t f, Random_t l, const Random& i, Count_t c)
  : m_min(f), m_max(l), m_increment(i.Copy()), m_consecutive(c),
    m_current(f), m_currentConsecutive(0)
{
}

Sequential::Sequential(const Sequential& c)
  : m_min(c.m_min), m_max(c.m_max),
    m_increment(c.m_increment->Copy()), m_consecutive(c.m_consecutive),
    m_current(c.m_current), m_currentConsecutive(c.m_currentConsecutive)
{
}

Random_t Sequential::Value()
{ // Return a sequential series of values
  Random_t r = m_current;
  if (++m_currentConsecutive == m_consecutive)
    { // Time to advance to next
      m_currentConsecutive = 0;
      m_current += m_increment->Value();
      if (m_current >= m_max)
        m_current = m_min + (m_current - m_max);
    }
  return r;
}

Random* Sequential::Copy() const
{
  return new Sequential(*this);
}

// Exponential methods
Random_t Exponential::Value()
{
  Random_t r = -m_mean*log(Random::Value());
  if (m_bound != 0 && r > m_bound) return m_bound;
  return r;
}

Random* Exponential::Copy() const
{
  return new Exponential(*this);
}

// Pareto methods
Random_t Pareto::Value()
{
Random_t scale = m_mean * ( m_shape - 1.0) / m_shape;

  Random_t r = (scale * ( 1.0 / pow(Random::Value(), 1.0 / m_shape)));
  DEBUG0((cout << "Pareto, scale " << scale << " r " << r << endl));
  if (m_bound != 0 && r > m_bound) return m_bound;
  return r;
}

Random* Pareto::Copy() const
{
  return new Pareto(*this);
}

// Weibull methods
Random_t Weibull::Value()
{
Random_t exponent = 1.0 / m_alpha;

  Random_t r = m_mean * pow( -log(Random::Value()), exponent);
  if (m_bound != 0 && r > m_bound) return m_bound;
  return r;
}

Random* Weibull::Copy() const
{
  return new Weibull(*this);
}

// Normal methods
Random_t Normal::Value()
{
  if (m_nextValid)
    { // use previously generated
      m_nextValid = false;
      return m_next;
    }
  while(1)
    { // See Simulation Modeling and Analysis p. 466 (Averill Law)
      // for algorithm
      Random_t u1 = Random::Value();
      Random_t u2 = Random::Value();
      Random_t v1 = 2 * u1 - 1;
      Random_t v2 = 2 * u2 - 1;
      Random_t w = v1 * v1 + v2 * v2;
      if (w <= 1.0)
        { // Got good pair
          Random_t y = sqrt((-2 * log(w))/w);
          m_next = m_mean + v2 * y * sqrt(m_variance);
          if (fabs(m_next) > m_bound) m_next = m_bound * (m_next)/fabs(m_next);
          m_nextValid = true;
          Random_t x1 = m_mean + v1 * y * sqrt(m_variance);
          if (fabs(x1) > m_bound) x1 = m_bound * (x1)/fabs(x1);
          return x1;
        }
    }
}

Random* Normal::Copy() const
{
  return new Normal(*this);
}

// Empirical methods
Random_t Empirical::Value()
{ // Return a value from the empirical distribution
  // This code based (loosely) on code by Bruce Mah (Thanks Bruce!)
  if (emp.size() == 0) return 0.0; // HuH? No empirical data
  if (!validated) Validate();      // Insure in non-decreasing
  Random_t r = Random::Value();
  if (r <= emp.front().cdf)return emp.front().value; // Less than first
  if (r >= emp.back().cdf) return emp.back().value;  // Greater than last
  // Binary search
  ValueCDFVec_t::size_type bottom = 0;
  ValueCDFVec_t::size_type top = emp.size() - 1;
  while(1)
    {
      ValueCDFVec_t::size_type c = (top + bottom) / 2;
      if (r >= emp[c].cdf && r < emp[c+1].cdf)
        { // Found it
          return Interpolate(emp[c].cdf, emp[c+1].cdf,
                             emp[c].value, emp[c+1].value,
                             r);
        }
      // Not here, adjust bounds
      if (r < emp[c].cdf) top    = c - 1;
      else                bottom = c + 1;
    }
}

Random* Empirical::Copy() const
{
  return new Empirical(*this);
}

void Empirical::CDF(Random_t v, CDF_t c)
{ // Add a new empirical datapoint to the empirical cdf
  // NOTE.   These MUST be inserted in non-decreasing order
  emp.push_back(ValueCDF(v, c));
}

void Empirical::Validate()
{
  ValueCDF prior;
  for (ValueCDFVec_t::size_type i = 0; i < emp.size(); ++i)
    {
      ValueCDF& current = emp[i];
      if (current.value < prior.value || current.cdf < prior.cdf)
        { // Error
          cout << "Empirical Dist error,"
               << " current value " << current.value
               << " prior value "   << prior.value
               << " current cdf "   << current.cdf
               << " prior cdf "     << prior.cdf << endl;
          exit(1);
        }
      prior = current;
    }
  validated = true;
}

Random_t Empirical::Interpolate(CDF_t c1, CDF_t c2,
                                Random_t v1, Random_t v2, CDF_t r)
{ // Interpolate random value in range [v1..v2) based on [c1 .. r .. c2)
  return (v1 + ((v2 - v1) / (c2 - c1)) * (r - c1));
}

// Integer Empirical methods

IRandom_t IntEmpirical::IntValue()
{
  return (IRandom_t)Value();
}

Random* IntEmpirical::Copy() const
{
  return new IntEmpirical(*this);
}


Random_t IntEmpirical::Interpolate(CDF_t c1, CDF_t c2,
                                   Random_t v1, Random_t v2, CDF_t r)
{ // Interpolate random value in range [v1..v2) based on [c1 .. r .. c2)
  return ceil(v1 + ((v2 - v1) / (c2 - c1)) * (r - c1));
}

// Deterministic
Deterministic::Deterministic(Random_t* d, Count_t c)
    : count(c), next(c), data(d)
{ // Nothing else needed
}

  
Random_t Deterministic::Value()
{
  if (next == count) next = 0;
  return data[next++];
}

Random* Deterministic::Copy() const
{
  return new Deterministic(*this);
}

// Static Methods
bool Random::GlobalSeed(Seed_t s0, Seed_t s1, Seed_t s2,
                Seed_t s3, Seed_t s4, Seed_t s5)
{
  if (globalRNG)
    {
      cout << "Random number generator already initialized!" << endl;
      cout << "Call to Random::GlobalSeed() ignored" << endl;
      return false;
    }
  globalSeed[0] = s0;
  globalSeed[1] = s1;
  globalSeed[2] = s2;
  globalSeed[3] = s3;
  globalSeed[4] = s4;
  globalSeed[5] = s5;
  if (!RngStream::CheckSeed(globalSeed)) return false; // Invalid  seed
  globalSeedSet = true;
  return true;
}

bool Random::GlobalSeed(const Seed& s)
{
  if (globalRNG)
    {
      cout << "Random number generator already initialized!" << endl;
      cout << "Call to Random::GlobalSeed() ignored" << endl;
      return false;
    }
  if (s.IsRandom()) return true; // Random seed is the default
  const ConstantSeed& cs = (ConstantSeed&)s;
  return GlobalSeed(cs.seeds[0], cs.seeds[1], cs.seeds[2],
                    cs.seeds[3], cs.seeds[4], cs.seeds[5]);
}

void Random::Initialize()
{ 
  if (globalRNG) return; // Already initialized and seeded
  if (!globalSeedSet)
    { // No global seed, try a random one
      GetRandomSeeds(globalSeed);
    }
  // See the RNG package
  //RngStream::SetPackageSeed(globalSeed); // No longer needed
  // Create the base  RNG
  globalRNG = new RngStream();
  globalRNG->SetSeed(globalSeed);
}
#ifdef WIN32
void          gettimeofday(struct timeval *tv,int dummy)
{
SYSTEMTIME SystemTime;
GetSystemTime(&SystemTime);
tv->tv_sec = SystemTime.wSecond ;
tv->tv_usec = SystemTime.wMilliseconds ;
}

#endif

void Random::GetRandomSeeds(Seed_t seeds[6])
{
  // Check if /dev/random exists
  if (useDevRandom && devRandom < 0)
    {
#ifndef WIN32   
		devRandom = open("/dev/random", O_RDONLY);
#endif
    }
  if (devRandom > 0)
    { // Use /dev/random
      DEBUG0((cout << "Seeding with /dev/random" << endl));
      while(true)
        {
          for (int i = 0; i < 6; ++i)
            {
#ifndef WIN32
              read(devRandom, &seeds[i], sizeof(seeds[i]));
#endif
            }
          if (RngStream::CheckSeed(seeds)) break; // Got a valid one
        }
    }
  else
    { // Seed from time of day (code borrowed from ns2 random seeding)
      // Thanks to John Heidemann for this technique
      while(true)
        {

          DEBUG0((cout << "Seeding with time of day" << endl));
          timeval tv;
          gettimeofday(&tv, 0);
          seeds[0] = (tv.tv_sec^tv.tv_usec^(++heuristic_sequence <<8))
              & 0x7fffffff;
          gettimeofday(&tv, 0);
          seeds[1] = (tv.tv_sec^tv.tv_usec^(++heuristic_sequence <<8))
              & 0x7fffffff;
          gettimeofday(&tv, 0);
          seeds[2] = (tv.tv_sec^tv.tv_usec^(++heuristic_sequence <<8))
              & 0x7fffffff;
          gettimeofday(&tv, 0);
          seeds[3] = (tv.tv_sec^tv.tv_usec^(++heuristic_sequence <<8))
              & 0x7fffffff;
          gettimeofday(&tv, 0);
          seeds[4] = (tv.tv_sec^tv.tv_usec^(++heuristic_sequence <<8))
              & 0x7fffffff;
          gettimeofday(&tv, 0);
          seeds[5] = (tv.tv_sec^tv.tv_usec^(++heuristic_sequence <<8))
              & 0x7fffffff;
          if (RngStream::CheckSeed(seeds)) break; // Got a valid one
        }
    }
}

  

