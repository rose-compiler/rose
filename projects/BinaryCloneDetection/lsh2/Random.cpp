/*
 * Copyright (c) 2004-2005 Massachusetts Institute of Technology.
 * All Rights Reserved.
 *
 * MIT grants permission to use, copy, modify, and distribute this software and
 * its documentation for NON-COMMERCIAL purposes and without fee, provided that
 * this copyright notice appears in all copies.
 *
 * MIT provides this software "as is," without representations or warranties of
 * any kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability, fitness for a particular purpose, and
 * noninfringement.  MIT shall not be liable for any damages arising from any
 * use of this software.
 *
 * Author: Alexandr Andoni (andoni@mit.edu), Piotr Indyk (indyk@mit.edu)
 */

#include "headers.h"

// The state vector for generation of random numbers.
char rngState[256];

// Initialize the random number generator.
void initRandom(){
  FAILIF(NULL == initstate(2, rngState, 256));
}

// Generate a random integer in the range [rangeStart,
// rangeEnd]. Inputs must satisfy: rangeStart <= rangeEnd.
IntT genRandomInt(IntT rangeStart, IntT rangeEnd){
  ASSERT(rangeStart <= rangeEnd);
  IntT r;
  r = rangeStart + (IntT)((rangeEnd - rangeStart + 1.0) * random() / (RAND_MAX + 1.0));
  ASSERT(r >= rangeStart && r <= rangeEnd);
  return r;
}

// Generate a random 32-bits unsigned (Uns32T) in the range
// [rangeStart, rangeEnd]. Inputs must satisfy: rangeStart <=
// rangeEnd.
Uns32T genRandomUns32(Uns32T rangeStart, Uns32T rangeEnd){
  ASSERT(rangeStart <= rangeEnd);
  Uns32T r;
  if (RAND_MAX >= rangeEnd - rangeStart) {
    r = rangeStart + (Uns32T)((rangeEnd - rangeStart + 1.0) * random() / (RAND_MAX + 1.0));
  } else {
    r = rangeStart + (Uns32T)((rangeEnd - rangeStart + 1.0) * ((LongUns64T)random() * ((LongUns64T)RAND_MAX + 1) + (LongUns64T)random()) / ((LongUns64T)RAND_MAX * ((LongUns64T)RAND_MAX + 1) + (LongUns64T)RAND_MAX + 1.0));
  }
  ASSERT(r >= rangeStart && r <= rangeEnd);
  return r;
}
// PI-64
UnsT genRandomUnsT(UnsT rangeStart, UnsT rangeEnd){
  ASSERT(rangeStart <= rangeEnd);
  UnsT r;
  if (RAND_MAX >= rangeEnd - rangeStart) {
    r = rangeStart + (UnsT)((rangeEnd - rangeStart + 1.0) * random() / (RAND_MAX + 1.0));
  } else {
    r = rangeStart + (UnsT)((rangeEnd - rangeStart + 1.0) * ((LongUns64T)random() * ((LongUns64T)RAND_MAX + 1) + (LongUns64T)random()) / ((LongUns64T)RAND_MAX * ((LongUns64T)RAND_MAX + 1) + (LongUns64T)RAND_MAX + 1.0));
  }
  ASSERT(r >= rangeStart && r <= rangeEnd);
  return r;
}

// Generate a random real distributed uniformly in [rangeStart,
// rangeEnd]. Input must satisfy: rangeStart <= rangeEnd. The
// granularity of generated random reals is given by RAND_MAX.
RealT genUniformRandom(RealT rangeStart, RealT rangeEnd){
  ASSERT(rangeStart <= rangeEnd);
  RealT r;
  r = rangeStart + ((rangeEnd - rangeStart) * (RealT)random() / (RealT)RAND_MAX);
  ASSERT(r >= rangeStart && r <= rangeEnd);
  return r;
}

// Generate a random real from normal distribution N(0,1).
RealT genGaussianRandom(){
  // Use Box-Muller transform to generate a point from normal
  // distribution.
  RealT x1, x2;
  do{
    x1 = genUniformRandom(0.0, 1.0);
  } while (x1 == 0); // cannot take log of 0.
  x2 = genUniformRandom(0.0, 1.0);
  RealT z;
  z = SQRT(-2.0 * LOG(x1)) * COS(2.0 * M_PI * x2);
  return z;
}
