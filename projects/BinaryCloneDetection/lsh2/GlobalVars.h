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

#ifndef GLOBALVARS_INCLUDED
#define GLOBALVARS_INCLUDED

#ifndef GLOBALVARS_CPP
#define DECLARE_EXTERN extern
#define EXTERN_INIT(x)
#else
#define DECLARE_EXTERN
#define EXTERN_INIT(x) x
#endif

// PI-64
DECLARE_EXTERN UnsT availableTotalMemory EXTERN_INIT(= DEFAULT_MEMORY_MAX_AVAILABLE);

DECLARE_EXTERN TimeVarT timeComputeULSH;
DECLARE_EXTERN TimeVarT timeGetBucket;
DECLARE_EXTERN TimeVarT timeCycleBucket;
DECLARE_EXTERN TimeVarT timeDistanceComputation;
DECLARE_EXTERN TimeVarT timeResultStoring;
DECLARE_EXTERN TimeVarT timePrecomputeHash;
DECLARE_EXTERN TimeVarT timeGBHash;
DECLARE_EXTERN TimeVarT timeChainTraversal;
DECLARE_EXTERN TimeVarT timeBucketCreation;
DECLARE_EXTERN TimeVarT timeBucketIntoUH;
DECLARE_EXTERN TimeVarT timeCycleProc;
DECLARE_EXTERN TimeVarT timeRNNQuery;
DECLARE_EXTERN TimeVarT timeCopyingULSHs;
DECLARE_EXTERN TimeVarT timeTotalBuckets;
DECLARE_EXTERN TimeVarT timeUnmarking;

DECLARE_EXTERN BooleanT timingOn EXTERN_INIT(= TRUE);
DECLARE_EXTERN TimeVarT currentTime EXTERN_INIT(= 0);
DECLARE_EXTERN TimeVarT timevSpeed EXTERN_INIT(= 0);

DECLARE_EXTERN IntT nOfDistComps EXTERN_INIT(= 0);
DECLARE_EXTERN IntT totalAllocatedMemory EXTERN_INIT(= 0);
DECLARE_EXTERN IntT nGBuckets EXTERN_INIT(= 0);
DECLARE_EXTERN IntT nBucketsInChains EXTERN_INIT(= 0);
//DECLARE_EXTERN IntT nPointsInBuckets EXTERN_INIT(= 0); // total # of points found in collinding buckets (including repetitions)
DECLARE_EXTERN IntT nAllocatedGBuckets EXTERN_INIT(= 0);
DECLARE_EXTERN IntT nAllocatedBEntries EXTERN_INIT(= 0);

DECLARE_EXTERN BooleanT noExpensiveTiming  EXTERN_INIT(= FALSE);


#endif
