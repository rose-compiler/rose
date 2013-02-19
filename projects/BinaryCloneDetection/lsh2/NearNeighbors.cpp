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

/*
  An interface to the R-NN data structure. Here are two basic functions
  -- creating the R-NN data structure (with or without data set points
  given), and a function for replying to a NN query. The parameters of
  R-NN data structure are computed automatically.
 */

#include "headers.h"

RealT lshPrecomp, uhashOver, distComp;

/*
  Checks whether the data types used are of the correct size. An
  ASSERT will fail if some data type does not have the correct size.
 */
// PI-64 TODO: not portable code
void checkDataTypes(){
  ASSERT(sizeof(IntT) == MACHINE_WORD_SIZE/8);
  ASSERT(sizeof(IntT) >= 2); // others are not quite supported (because of printf's).
  ASSERT(sizeof(Int32T) >= 4);
  ASSERT(sizeof(Uns32T) >= 4);
  ASSERT(sizeof(LongUns64T) >= 8);
}

/*
  Initializes some of the global mechanisms (timing), and makes
  necessary sanity checks (e.g., data types sizes).
 */
void initializeLSHGlobal(){
  checkDataTypes();

  // Initialize global variables
  timingOn = TRUE;

  // Initialize timings.
  tuneTimeFunctions();

  // Initialize the random number generator.
  initRandom();
}

// Creates a new R-near neighbor data structure,
// PRNearNeighborStructT, from the parameters <thresholdR> and
// <successProbability>. <nPointsEstimate> is the estimate of the
// number of points that will be in the data structure (a better
// estimate will better optimize the amount of memory used and the
// performance).
// PI-64
PRNearNeighborStructT initRNearNeighbor(RealT thresholdR, RealT successProbability, IntT nPointsEstimate){
  ASSERT(FALSE); // Not implemented yet
}

/*
  Creates a new R-near neighbor data structure (PRNearNeighborStructT)
  from the parameters <thresholdR> and <successProbability> for the
  data set <dataSet>. <nPoints> is the number of points in the data
  set and <dimension> is the dimension of the points.

  The set <sampleQueries> is a set with query sample points
  (R-NN DS's parameters are optimized for query points from the set
  <sampleQueries>). <sampleQueries> could be a sample of points from the
  actual query set or from the data set. When computing the estimated
  number of collisions of a sample query point <q> with the data set
  points, if there is a point in the data set with the same pointer
  with <q> (that is when <q> is a data set point), then the
  corresponding point (<q>) is not considered in the data set (for the
  purpose of computing the respective #collisions estimation).
*/
// PI-64
PRNearNeighborStructT initSelfTunedRNearNeighborWithDataSet(RealT thresholdR, 
							    RealT successProbability, 
							    IntT nPoints, 
							    IntT dimension, 
							    PPointT *dataSet, 
							    IntT nSampleQueries, 
							    PPointT *sampleQueries, 
							    IntT memoryUpperBound){
  initializeLSHGlobal();

  PRNearNeighborStructT nnStruct = NULL;

  RNNParametersT optParameters = computeOptimalParameters(thresholdR, successProbability, nPoints, dimension, dataSet, nSampleQueries, sampleQueries, memoryUpperBound);

  if (!optParameters.useUfunctions) {
    DPRINTF("Used L=%ld\n", optParameters.parameterL);
  }else{
    DPRINTF("Used m = %ld\n", optParameters.parameterM);
    DPRINTF("Used L = %ld\n", optParameters.parameterL);
  }

  TimeVarT timeInit = 0;
  TIMEV_START(timeInit);

  // Init the R-NN data structure.
  if (optParameters.typeHT != HT_HYBRID_CHAINS){
    nnStruct = initLSH(optParameters, nPoints);
  }else{
    printRNNParameters(DEBUG_OUTPUT, optParameters);
    nnStruct = initLSH_WithDataSet(optParameters, nPoints, dataSet);
  }

  TIMEV_END(timeInit);
  printf("Time for initializing: %0.6lf\n", timeInit);
  DPRINTF("Allocated memory: %ld\n", totalAllocatedMemory);

  TimeVarT timeAdding = 0;
  if (optParameters.typeHT != HT_HYBRID_CHAINS){
    // Add the points to the LSH buckets.
    TIMEV_START(timeAdding);
    for(IntT i = 0; i < nPoints; i++){
      addNewPointToPRNearNeighborStruct(nnStruct, dataSet[i]);
    }
    TIMEV_END(timeAdding);
    printf("Time for adding points: %0.6lf\n", timeAdding);
    DPRINTF("Allocated memory: %ld\n", totalAllocatedMemory);
  }

  DPRINTF("Time for creating buckets: %0.6lf\n", timeBucketCreation);
  DPRINTF("Time for putting buckets into UH: %0.6lf\n", timeBucketIntoUH);
  DPRINTF("Time for computing GLSH: %0.6lf\n", timeComputeULSH);
  DPRINTF("NGBuckets: %ld\n", nGBuckets);

  return nnStruct;
}
#include <iostream.h>
// PI-64
IntT getRNearNeighbors(PRNearNeighborStructT nnStruct, PPointT queryPoint, PPointT *(&result), IntT &resultSize){
  DPRINTF("Estimated ULSH comp: %0.6lf\n", lshPrecomp * nnStruct->nHFTuples * nnStruct->hfTuplesLength);
  DPRINTF("Estimated UH overhead: %0.6lf\n", uhashOver * nnStruct->nHFTuples);
//   RealT estNColls = estimateNCollisions(nnStruct->nPoints, 
// 					nnStruct->dimension, 
// 					nnStruct->points, 
// 					queryPoint, 
// 					nnStruct->parameterK, 
// 					nnStruct->parameterL, 
// 					nnStruct->parameterR);
//   DPRINTF("Estimated #collisions (query specific): %0.6lf\n", (double)estNColls);
//   estNColls = (double)estimateNDistinctCollisions(nnStruct->nPoints, 
// 						  nnStruct->dimension, 
// 						  nnStruct->points, 
// 						  queryPoint, 
// 						  nnStruct->useUfunctions, 
// 						  nnStruct->hfTuplesLength, 
// 						  nnStruct->nHFTuples, 
// 						  nnStruct->parameterR);
//   DPRINTF("Estimated #distinct collisions (query specific): %0.6lf\n", estNColls);
//   DPRINTF("Estimated Dist comp time (query specific): %0.6lf\n", distComp * estNColls);

  // reset all the timers
  timeRNNQuery = 0;
  timeComputeULSH = 0;
  timeGetBucket = 0;
  timeCycleBucket = 0;
  timeDistanceComputation = 0;
  timeResultStoring = 0;
  timeCycleProc = 0;
  timePrecomputeHash = 0;
  timeGBHash = 0;
  timeChainTraversal = 0;
  nOfDistComps = 0;
  timeTotalBuckets = 0;

  TIMEV_START(timeRNNQuery);
  noExpensiveTiming = !DEBUG_PROFILE_TIMING;
  // PI-64
  IntT nNearNeighbors = getNearNeighborsFromPRNearNeighborStruct(nnStruct, queryPoint, result, resultSize);
  //std::cerr << "Results size: " << resultSize << std::endl;
 // std::cerr << "nNearNeighbors: " << nNearNeighbors << std::endl;

  TIMEV_END(timeRNNQuery);

  DPRINTF("Time to compute LSH: %0.6lf\n", timeComputeULSH);
  DPRINTF("Time to get bucket: %0.6lf\n", timeGetBucket);
  DPRINTF("Time to cycle through buckets: %0.6lf\n", timeCycleBucket);
  DPRINTF("Time to for processing buckets (UH+examining points): %0.6lf\n", timeTotalBuckets);
  //DPRINTF("Time to copy ULSHs: %0.6lf\n", timeCopyingULSHs);
  //DPRINTF("Time to unmark points: %0.6lf\n", timeUnmarking);
  DPRINTF("Time for distance comps: %0.6lf\n", timeDistanceComputation);
  DPRINTF("Time to store result: %0.6lf\n", timeResultStoring);
  //printf("Time for cycle processing: %0.6lf\n", timeCycleProc);
  //printf("Time for precomputing hashes: %0.6lf\n", timePrecomputeHash);
  //printf("Time for GB hash: %0.6lf\n", timeGBHash);
  //printf("Time for traversal of chains: %0.6lf\n", timeChainTraversal);
  DPRINTF("Number of dist comps: %ld\n", nOfDistComps);
  DPRINTF("Number buckets in chains: %ld\n", nBucketsInChains);
  DPRINTF("Number buckets in chains / L: %0.3lf\n", (double)nBucketsInChains / nnStruct->nHFTuples);
  DPRINTF("Cumulative time for R-NN query: %0.6lf\n", timeRNNQuery);

  return nNearNeighbors;
}
