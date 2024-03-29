//Copyright (c) 2010, Information Security Institute of Wuhan Universtiy(ISIWhu)
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


// Georgia Tech Network Simulator - Random Number class
// George F. Riley.  Georgia Tech, Spring 2002

// Random is base class for all random number generators

#ifndef __random_h__
#define __random_h__

#include <vector>
#include <algorithm>

#include "G_common_defs.h"
#include "RngStream.h"

typedef double        Random_t;  // Real Random Variable
typedef unsigned long IRandom_t; // Integer Random Variable
typedef double        CDF_t;     // Cumulative distribution Function value
typedef unsigned long Seed_t;

class Uniform;

class Seed {
  // Seed is used to seed the random number generator(s)
  // This is a base class for RandomSeed and ConstantSeed
  // No constructors supplied, since we can't create any of these
public:
  virtual ~Seed() {}
  virtual bool IsRandom() const = 0;
};

class RandomSeed : public Seed {
public:
  RandomSeed() {};
  ~RandomSeed() {};
  bool IsRandom() const { return true; }
};

class ConstantSeed : public Seed 
{
public:
  ConstantSeed(Seed_t); // Use six copies of the specified value
  ConstantSeed(Seed_t,Seed_t,Seed_t,Seed_t,Seed_t,Seed_t);  // Six seeds
  bool IsRandom() const { return false; }
  ~ConstantSeed() {};
public:
  Seed_t seeds[6];
};
  
  
//Doc:ClassXRef
class Random { 
  //Doc:Class \GTNS\ has a rich set of  random number generators.
  //Doc:Class Class {\tt Random} defines the base class functionalty
  //Doc:Class required for all random number generators.
  //Doc:Class Note: The underlying random number generation method used
  //Doc:Class by \GTNS\ is the {\tt RngStream} code by Pierre L'Ecuyer at
  //Doc:Class the University of Montreal.
public:
  //Doc:Meth od
  Random();
    //Doc:Desc Constructor for a random number generator with a random seed.

  virtual ~Random() { }
  //Doc:Method
  virtual Random_t  Value();     // Returns a value [0.0 .. 1.0)
    //Doc:Desc Return a floating point random value
    //Doc:Return Floating point random value.

  // Returns integer value
  //Doc:Method
  virtual IRandom_t IntValue() { return (IRandom_t)Value();}
    //Doc:Desc Return an integer random value.
    //Doc:Return Integer random value.

  virtual Random*   Copy() const;

  // Static methods

  //Doc:Method
  bool SetSeed(const Seed&);
    //Doc:Desc Use a private seed and private stream for this generator.
    //Doc:Arg1 Seed to use.  Can either be a RandomSeed or ConstantSeed.
    //Doc:Return True if valid seed.


  //Doc:Method
  static  void UseDevRandom(bool udr = true) { useDevRandom = udr;}
    //Doc:Desc Specify whether the linux device {\tt /dev/random} is to
    //Doc:Desc be used for a random seed.
    //Doc:Arg1 True if {\tt /dev/random} desired.

  // Use the global seed to force precisely reproducible results.
  //Doc:Method
  static bool GlobalSeed(Seed_t,Seed_t,Seed_t,Seed_t,Seed_t,Seed_t) ;
    //Doc:Desc It is often desirable to create a simulation that uses random
    //Doc:Desc numbers, while at the same time is completely reproducible.
    //Doc:Desc Specifying this set of six random seeds initializes the
    //Doc:Desc random number generator with the specified seed.
    //Doc:Desc This leads to precisely reproducible results.
    //Doc:Desc Note. This constructor is deprecated.  New code should use 
    //Doc:Desc the GlobalSeed call with a Seed class argument (below).
    //Doc:Arg1 Seed to use (32 bit unsigned)
    //Doc:Arg2 Seed to use (32 bit unsigned)
    //Doc:Arg3 Seed to use (32 bit unsigned)
    //Doc:Arg4 Seed to use (32 bit unsigned)
    //Doc:Arg5 Seed to use (32 bit unsigned)
    //Doc:Arg6 Seed to use (32 bit unsigned)
    //Doc:Return True if seed is valid.

  //Doc:Method
  static bool GlobalSeed(const Seed&);
    //Doc:Desc It is often desirable to create a simulation that uses random
    //Doc:Desc numbers, while at the same time is completely reproducible.
    //Doc:Desc Specifying this set of six random seeds initializes the
    //Doc:Desc random number generator with the specified seed.
    //Doc:Desc This leads to precisely reproducible results.
    //Doc:Arg1 Seed to use.  Can either be a RandomSeed or ConstantSeed.
    //Doc:Return True if seed is valid.

private:
  RngStream* privateStream;    // Private stream for generator (if specified)
  static void Initialize();    // Initialize  the RNG system
  static void GetRandomSeeds(Seed_t seeds[6]);
private:
  static bool useDevRandom;    // True if using /dev/random desired
  static bool globalSeedSet;   // True if global seed has been specified
  static int  devRandom;       // File handle for /dev/random
  static Seed_t globalSeed[6]; // The global seed to use
protected:
  static unsigned long heuristic_sequence;
  static RngStream* globalRNG;
public:
};  


//Doc:ClassXRef
class Uniform : public Random { // Uniformly distributed random variable
  //Doc:Class Creates a uniformly distributed random number generator.
public:
  //Doc:Method
  Uniform() : m_min(0), m_max(1.0) { }
    //Doc:Desc Creates a uniform random number generator in the
    //Doc:Desc range [0.0 .. 1.0)

  //Doc:Method
  Uniform(Random_t s, Random_t l) : m_min(s), m_max(l) { };
    //Doc:Desc Creates a uniform random number generator with the specified
    //Doc:Desc range.

  Uniform(const Uniform& c) : m_min(c.m_min), m_max(c.m_max) { }
  virtual Random_t Value();
  virtual Random*  Copy() const;
private:
  Random_t m_min;
  Random_t m_max;
};

//Doc:ClassXRef
class Constant : public Random { // A random variable that returns a constant
  //Doc:Class Class {\tt Constant} defines a random number generator that
  //Doc:Class returns the same value every sample.  It's not immediately
  //Doc:Class obvious
  //Doc:Class why such a generator is useful, but it is.  A number of other
  //Doc:Class \GTNS\ objects require a random number generator as parameters
  //Doc:Class to describe the behavior.  For example, application {\tt TCPSend}
  //Doc:Class uses a random number generator to determine how much data
  //Doc:Class to send.  By passing a {\tt Constant} RNG to the {\tt TCPSend}
  //Doc:Class application, it will send a known constant amount.

public:
  //Doc:Method
  Constant() : m_const(0) { }
    //Doc:Desc Construct a {\tt Constant} RNG that return zero every sample.

  //Doc:Method
  Constant(Random_t c) : m_const(c) { };
    //Doc:Desc Construct a {\tt Constant} RNG that returns the specified value
    //Doc:Desc every sample.
    //Doc:Arg1 Constant value for this RNG.

  Constant(const Constant& c) : m_const(c.m_const) { }
  //Doc:Method
  void    NewConstant(Random_t c) { m_const = c;}
    //Doc:Desc Specify a new constant RNG for this generator.
    //Doc:Arg1 Constant value for this RNG.

  virtual Random_t  Value();
  virtual IRandom_t IntValue();
  virtual Random*   Copy() const;
private:
  Random_t m_const;
};

//Doc:ClassXRef
class Sequential : public Random { // Return a sequential list of values
  //Doc:Class Class {\tt Sequential} defines a random number generator that
  //Doc:Class returns a sequential sequence.
public:
  // first, last, increment, consecutive
  //Doc:Method
  Sequential(Random_t f, Random_t l, Random_t i = 1, Count_t c = 1);
    //Doc:Desc Constructor for the {\tt Sequential} RNG.  The four parameters
    //Doc:Desc define the sequence.  For example {\tt Sequential(0, 5, 1, 2)}
    //Doc:Desc returns the sequence 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 0, 0 \ldots
    //Doc:Arg1 First value returned.
    //Doc:Arg2 Last + 1 value returned.
    //Doc:Arg3 Increment.
    //Doc:Arg4 Repeat count for  each  value.

  //Doc:Method
  Sequential(Random_t f, Random_t l, const Random& i, Count_t c = 1);
    //Doc:Desc Constructor for the {\tt Sequential} RNG.
    //Doc:Desc Differs from the first constructor only in the fact that the
    //Doc:Desc increment paramenter is a random variable.
    //Doc:Arg1 First value returned.
    //Doc:Arg2 Last + 1 value returned.
    //Doc:Arg3 Reference to a random variable for the sequence increment.
    //Doc:Arg4 Repeat count for  each  value.

  Sequential(const Sequential& c);
  virtual Random_t Value();
  virtual Random*  Copy() const;
private:
  Random_t m_min;
  Random_t m_max;
  Random*  m_increment;
  Count_t  m_consecutive;
  Random_t m_current;
  Count_t  m_currentConsecutive;
};

//Doc:ClassXRef
class Exponential : public Random { // Exponentially Distributed random var
  //Doc:Class Class {\tt Exponential} defines a random variable with an
  //Doc:Class  exponential distribution.
public:
  //Doc:Method
  Exponential() : m_mean(1.0), m_bound(0) { }
    //Doc:Desc Constructs an exponential random variable  with a mean
    //Doc:Desc value of 1.0.

  //Doc:Method
  explicit Exponential(Random_t m) : m_mean(m), m_bound(0) { };
    //Doc:Desc Constructs an exponential  random variable with the specified
    //Doc:Desc mean value.
    //Doc:Arg1 Mean value for the random variable.

  //Doc:Method
  Exponential(Random_t m, Random_t b) : m_mean(m), m_bound(b) { };
    //Doc:Desc Constructs an exponential randome variable  with the specified
    //Doc:Desc mean value and specified upper limit.  Since exponential
    //Doc:Desc distributions can theoretically return unbounded values,
    //Doc:Desc it is sometimes useful to specify  a fixed upper limit.
    //Doc:Desc Note however that when the upper limit is specified, the
    //Doc:Desc true mean of the distribution is slightly smaller than
    //Doc:Desc the mean value specified.
    //Doc:Arg1 Mean value for the random variable.
    //Doc:Arg2 Upper limit on returned values.

  Exponential(const Exponential& c) : m_mean(c.m_mean), m_bound(c.m_bound) { }
  virtual Random_t Value();
  virtual Random* Copy() const;
private:
  Random_t m_mean;  // Mean value of RV
  Random_t m_bound; // Upper bound on value (if non-zero)
};

//Doc:ClassXRef
class Pareto : public Random { // Pareto distributed random var
  //Doc:Class Class {\tt Pareto} defines a random variable with a
  //Doc:Class pareto distribution.
public:
  //Doc:Method
  Pareto() : m_mean(1.0), m_shape(1.5), m_bound(0) { }
    //Doc:Desc Constructs a pareto random variable  with a mean
    //Doc:Desc value of 1.0 and a shape (alpha) parameter of 1.5.

  //Doc:Method
  explicit Pareto(Random_t m) 
    : m_mean(m), m_shape(1.5), m_bound(0) { };
    //Doc:Desc Constructs a pareto random variable with the specified mean
    //Doc:Desc value and a shape (alpha) parameter of 1.5.
    //Doc:Arg1 Mean value for the distribution.

  //Doc:Method
  Pareto(Random_t m, Random_t s) 
    : m_mean(m), m_shape(s), m_bound(0) { };
    //Doc:Desc Constructs a pareto random variable with the specified mean
    //Doc:Desc value and shape (alpha).
    //Doc:Arg1 Mean value for the distribution.
    //Doc:Arg2 Shape (alpha) parameter for the distribution.

  //Doc:Method
  Pareto(Random_t m, Random_t s, Random_t b) 
    : m_mean(m), m_shape(s), m_bound(b) { };
    //Doc:Desc Constructs a pareto random variable with the specified mean
    //Doc:Desc value, shape (alpha), and upper bound.
    //Doc:Desc Since pareto 
    //Doc:Desc distributions can theoretically return unbounded values,
    //Doc:Desc it is sometimes useful to specify  a fixed upper limit.
    //Doc:Desc Note however that when the upper limit is specified, the
    //Doc:Desc true mean of the distribution is slightly smaller than
    //Doc:Desc the mean value specified.
    //Doc:Arg1 Mean value for the random variable.
    //Doc:Arg2 Shape (alpha) parameter for the distribution.
    //Doc:Arg3 Upper limit on returned values.

  Pareto(const Pareto& c) 
    : m_mean(c.m_mean), m_shape(c.m_shape), m_bound(c.m_bound) { }
  virtual Random_t Value();
  virtual Random* Copy() const;
private:
  Random_t m_mean;  // Mean value of RV
  Random_t m_shape; // Shape parameter
  Random_t m_bound; // Upper bound on value (if non-zero)
};

//Doc:ClassXRef
class Weibull : public Random { // Weibull distributed random var
  //Doc:Class Class {\tt Weibull} defines a random variable with a
  //Doc:Class weibull distribution.
public:
  //Doc:Method
  Weibull() : m_mean(1.0), m_alpha(1), m_bound(0) { }
    //Doc:Desc Constructs a weibull random variable  with a mean
    //Doc:Desc value of 1.0 and a shape (alpha) parameter of 1

  //Doc:Method
  explicit Weibull(Random_t m) 
    : m_mean(m), m_alpha(1), m_bound(0) { };
    //Doc:Desc Constructs a weibull random variable with the specified mean
    //Doc:Desc value and a shape (alpha) parameter of 1.5.
    //Doc:Arg1 Mean value for the distribution.

  //Doc:Method
  Weibull(Random_t m, Random_t s) 
    : m_mean(m), m_alpha(s), m_bound(0) { };
    //Doc:Desc Constructs a weibull random variable with the specified mean
    //Doc:Desc value and a shape (alpha).
    //Doc:Arg1 Mean value for the distribution.
    //Doc:Arg2 Shape (alpha) parameter for the distribution.

  //Doc:Method
  Weibull(Random_t m, Random_t s, Random_t b) 
    : m_mean(m), m_alpha(s), m_bound(b) { };
    //Doc:Desc Constructs a weibull random variable with the specified mean
    //Doc:Desc value, alpha, and upper bound.
    //Doc:Desc Since weibull 
    //Doc:Desc distributions can theoretically return unbounded values,
    //Doc:Desc it is sometimes useful to specify  a fixed upper limit.
    //Doc:Desc Note however that when the upper limit is specified, the
    //Doc:Desc true mean of the distribution is slightly smaller than
    //Doc:Desc the mean value specified.
    //Doc:Arg1 Mean value for the random variable.
    //Doc:Arg2 Shape (alpha) parameter for the distribution.
    //Doc:Arg3 Upper limit on returned values.

  Weibull(const Weibull& c) 
    : m_mean(c.m_mean), m_alpha(c.m_alpha), m_bound(c.m_bound) { }
  virtual Random_t Value();
  virtual Random* Copy() const;
private:
  Random_t m_mean;  // Mean value of RV
  Random_t m_alpha; // Shape parameter
  Random_t m_bound; // Upper bound on value (if non-zero)
};

//Doc:ClassXRef
class Normal : public Random { // Normally Distributed random var
  //Doc:Class Class {\tt Normal} defines a random variable with an
  //Doc:Class  exponential distribution.
public:
  //Doc:Method
  Normal() : m_mean(0.0), m_variance(1.0), m_bound(INFINITE_VALUE),
                     m_nextValid(false) { }
    //Doc:Desc Constructs an normal random variable  with a mean
    //Doc:Desc value of 0 and variance of 1.

  //Doc:Method
  Normal(Random_t m, Random_t v, Random_t b = INFINITE_VALUE)
      : m_mean(m), m_variance(v), m_bound(b), m_nextValid(false) { };
    //Doc:Desc Constructs an normal random variable with the specified
    //Doc:Desc mean and variance values.
    //Doc:Arg1 Mean value for the random variable.
    //Doc:Arg2 Variance for the random variable.
    //Doc:Arg1 Bound value for the random variable.  The bound is
    //Doc:Arg1 positive.  The Normal is bounded within +- bound.

  Normal(const Normal& c)
      : m_mean(c.m_mean), m_variance(c.m_variance), m_bound(c.m_bound) { }
  virtual Random_t Value();
  virtual Random* Copy() const;
private:
  Random_t m_mean;      // Mean value of RV
  Random_t m_variance;  // Mean value of RV
  Random_t m_bound;     // Bound on value (absolute value)
  bool     m_nextValid; // True if next valid
  Random_t m_next;      // The algorithm produces two values at a time
};

// Value/CDF pair class for Emiprical Distributions
//Doc:ClassXRef
class ValueCDF {
public:
  ValueCDF() : value(0.0), cdf(0.0){ }
  ValueCDF(Random_t v, CDF_t c) : value(v), cdf(c) { }
  ValueCDF(const ValueCDF& c) : value(c.value), cdf(c.cdf) { }
  Random_t value;
  CDF_t    cdf;
};

typedef std::vector<ValueCDF> ValueCDFVec_t;

//Doc:ClassXRef
class Empirical : public Random { // Empirical distribution random var
  //Doc:Class Defines a random variable  that has a specified, empirical 
  //Doc:Class distribution.  The distribution is specified by a
  //Doc:Class series of calls the the {\tt CDF} member function, specifying a
  //Doc:Class value and the probability that the function value is less than
  //Doc:Class the specified value.  When values are requested,
  //Doc:Class a uniform random variable is used to select a probabililty,
  //Doc:Class and the return value is interpreted linerarly between the
  //Doc:Class two appropriate points in the CDF.
public:
  //Doc:Method
  explicit Empirical() : validated(false) { }
    //Doc:Desc Constructor for the {\tt Empirical} random variables.

  virtual ~Empirical() { }
  Empirical(const Empirical& c) : validated(c.validated), emp(c.emp) { }
  virtual Random_t Value();
  virtual Random* Copy() const;
  //Doc:Method
  virtual void     CDF(Random_t, CDF_t);  // Value, prob <= Value
    //Doc:Desc Specifies  a point in the empirical distribution.
    //Doc:Arg1 The function value for this point.
    //Doc:Arg2 Probability that the function is less than or equal
    //Doc:Arg2 to the specified value.
private:
  virtual void     Validate();  // Insure non-decreasing emiprical values
  virtual Random_t Interpolate(CDF_t, CDF_t, Random_t, Random_t, CDF_t);
  bool               validated; // True if non-decreasing validated
  ValueCDFVec_t      emp;       // Empicical CDF
};

//Doc:ClassXRef
class IntEmpirical : public Empirical { // Empirical distribution, integer
  //Doc:Class Defines an empirical distribution where all values are integers.
  //Doc:Class Indentical to {\tt Empirical}, but with slightly different
  //Doc:Class interpolation between points.
  // Identical to Empirical, but slightly different interpolation
public:
  //Doc:Method
  IntEmpirical() { }
    //Doc:Desc Constructor for {\tt IntEmpirical}

  virtual Random* Copy() const;
  virtual IRandom_t IntValue();
  virtual Random_t Interpolate(CDF_t, CDF_t, Random_t, Random_t, CDF_t);
};

//Doc:ClassXRef
class Deterministic : public Random { // Deterministic distribution random var
  //Doc:Class Defines a random variable  that has a specified, deterministic
  //Doc:Class distribution.  This would be useful when trying to force
  //Doc:Class the RNG to return a known sequence, perhaps to
  //Doc:Class compare GTNetS to some other simulator
public:
  //Doc:Method
  explicit Deterministic(Random_t*, Count_t);
    //Doc:Desc Constructor for the {\tt Deterministic} random variables.
    //Doc:Arg1 Pointer to array of random values to return in sequence
    //Doc:Arg2 Number of values in the array.

  virtual ~Deterministic() { }
  virtual Random_t Value();
  virtual Random* Copy() const;
private:
  Count_t   count;
  Count_t   next;
  Random_t* data;
};


#endif
