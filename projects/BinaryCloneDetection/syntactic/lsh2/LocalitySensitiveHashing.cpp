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
  The main functionality of the LSH scheme is in this file (all except
  the hashing of the buckets). This file includes all the functions
  for processing a PRNearNeighborStructT data structure, which is the
  main R-NN data structure based on LSH scheme. The particular
  functions are: initializing a DS, adding new points to the DS, and
  responding to queries on the DS.
 */

#include "headers.h"
#include <assert.h>

void printRNNParameters(FILE *output, RNNParametersT parameters){
  ASSERT(output != NULL);
  fprintf(output, "R\n");
  fprintf(output, "%0.9lf\n", parameters.parameterR);
  fprintf(output, "Success probability\n");
  fprintf(output, "%0.9lf\n", parameters.successProbability);
  fprintf(output, "Dimension\n");
  // PI-64: TODO: define a generic format character for 32/64 bit integers
  fprintf(output, "%ld\n", parameters.dimension);
  fprintf(output, "R^2\n");
  fprintf(output, "%0.9lf\n", parameters.parameterR2);
  fprintf(output, "Use <u> functions\n");
  fprintf(output, "%ld\n", parameters.useUfunctions);
  fprintf(output, "k\n");
  fprintf(output, "%ld\n", parameters.parameterK);
  fprintf(output, "m [# independent tuples of LSH functions]\n");
  fprintf(output, "%ld\n", parameters.parameterM);
  fprintf(output, "L\n");
  fprintf(output, "%ld\n", parameters.parameterL);
  fprintf(output, "W\n");
  fprintf(output, "%0.9lf\n", parameters.parameterW);
  fprintf(output, "T\n");
  fprintf(output, "%ld\n", parameters.parameterT);
  fprintf(output, "typeHT\n");
  fprintf(output, "%ld\n", parameters.typeHT);
}

RNNParametersT readRNNParameters(FILE *input){
  ASSERT(input != NULL);
  RNNParametersT parameters;
  char s[1000];// TODO: possible buffer overflow

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  FSCANF_REAL(input, &parameters.parameterR);

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  FSCANF_REAL(input, &parameters.successProbability);

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  fscanf(input, "%ld", &parameters.dimension);

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  FSCANF_REAL(input, &parameters.parameterR2);

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  fscanf(input, "%ld", &parameters.useUfunctions);

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  fscanf(input, "%ld", &parameters.parameterK);

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  fscanf(input, "%ld", &parameters.parameterM);

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  fscanf(input, "%ld", &parameters.parameterL);

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  FSCANF_REAL(input, &parameters.parameterW);

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  fscanf(input, "%ld", &parameters.parameterT);

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  fscanf(input, "%ld", &parameters.typeHT);

  return parameters;
}

// Creates the LSH hash functions for the R-near neighbor structure
// <nnStruct>. The functions fills in the corresponding field of
// <nnStruct>.
void initHashFunctions(PRNearNeighborStructT nnStruct){
  ASSERT(nnStruct != NULL);
  LSHFunctionT **lshFunctions;
  // allocate memory for the functions
  FAILIF(NULL == (lshFunctions = (LSHFunctionT**)MALLOC(nnStruct->nHFTuples * sizeof(LSHFunctionT*))));
  for(IntT i = 0; i < nnStruct->nHFTuples; i++){
    FAILIF(NULL == (lshFunctions[i] = (LSHFunctionT*)MALLOC(nnStruct->hfTuplesLength * sizeof(LSHFunctionT))));
    for(IntT j = 0; j < nnStruct->hfTuplesLength; j++){
      FAILIF(NULL == (lshFunctions[i][j].a = (RealT*)MALLOC(nnStruct->dimension * sizeof(RealT))));
    }
  }

  // initialize the LSH functions
  for(IntT i = 0; i < nnStruct->nHFTuples; i++){
    for(IntT j = 0; j < nnStruct->hfTuplesLength; j++){
      // vector a
      for(IntT d = 0; d < nnStruct->dimension; d++){
	lshFunctions[i][j].a[d] = genGaussianRandom();
      }
      // b
      lshFunctions[i][j].b = genUniformRandom(0, nnStruct->parameterW);
    }
  }

  nnStruct->lshFunctions = lshFunctions;
}

// Initializes the fields of a R-near neighbors data structure except
// the hash tables for storing the buckets.
// PI-64
PRNearNeighborStructT initializePRNearNeighborFields(RNNParametersT algParameters, IntT nPointsEstimate){
  PRNearNeighborStructT nnStruct;
  FAILIF(NULL == (nnStruct = (PRNearNeighborStructT)MALLOC(sizeof(RNearNeighborStructT))));
  nnStruct->parameterR = algParameters.parameterR;
  nnStruct->parameterR2 = algParameters.parameterR2;
  nnStruct->useUfunctions = algParameters.useUfunctions;
  nnStruct->parameterK = algParameters.parameterK;
  if (!algParameters.useUfunctions) {
    // Use normal <g> functions.
    nnStruct->parameterL = algParameters.parameterL;
    nnStruct->nHFTuples = algParameters.parameterL;
    nnStruct->hfTuplesLength = algParameters.parameterK;
  }else{
    // Use <u> hash functions; a <g> function is a pair of 2 <u> functions.
    nnStruct->parameterL = algParameters.parameterL;
    nnStruct->nHFTuples = algParameters.parameterM;
    nnStruct->hfTuplesLength = algParameters.parameterK / 2;
  }
  nnStruct->parameterT = algParameters.parameterT;
  nnStruct->dimension = algParameters.dimension;
  nnStruct->parameterW = algParameters.parameterW;

  nnStruct->nPoints = 0;
  nnStruct->pointsArraySize = nPointsEstimate;

  FAILIF(NULL == (nnStruct->points = (PPointT*)MALLOC(nnStruct->pointsArraySize * sizeof(PPointT))));

  // create the hash functions
  initHashFunctions(nnStruct);

  // init fields that are used only in operations ("temporary" variables for operations).

  // init the vector <pointULSHVectors> and the vector
  // <precomputedHashesOfULSHs>
  // PI-64
  FAILIF(NULL == (nnStruct->pointULSHVectors = (UnsT**)MALLOC(nnStruct->nHFTuples * sizeof(UnsT*))));
  for(IntT i = 0; i < nnStruct->nHFTuples; i++){
    FAILIF(NULL == (nnStruct->pointULSHVectors[i] = (UnsT*)MALLOC(nnStruct->hfTuplesLength * sizeof(UnsT))));
  }
  FAILIF(NULL == (nnStruct->precomputedHashesOfULSHs = (UnsT**)MALLOC(nnStruct->nHFTuples * sizeof(UnsT*))));
  for(IntT i = 0; i < nnStruct->nHFTuples; i++){
    FAILIF(NULL == (nnStruct->precomputedHashesOfULSHs[i] = (UnsT*)MALLOC(N_PRECOMPUTED_HASHES_NEEDED * sizeof(UnsT))));
  }
  // init the vector <reducedPoint>
  FAILIF(NULL == (nnStruct->reducedPoint = (RealT*)MALLOC(nnStruct->dimension * sizeof(RealT))));
  // init the vector <nearPoints>
  nnStruct->sizeMarkedPoints = nPointsEstimate;
  FAILIF(NULL == (nnStruct->markedPoints = (BooleanT*)MALLOC(nnStruct->sizeMarkedPoints * sizeof(BooleanT))));
  for(IntT i = 0; i < nnStruct->sizeMarkedPoints; i++){
    nnStruct->markedPoints[i] = FALSE;
  }
  // init the vector <nearPointsIndeces>
  FAILIF(NULL == (nnStruct->markedPointsIndeces = (IntT*)MALLOC(nnStruct->sizeMarkedPoints * sizeof(IntT))));

  nnStruct->reportingResult = TRUE;

  return nnStruct;
}

// Constructs a new empty R-near-neighbor data structure.
// PI-64
PRNearNeighborStructT initLSH(RNNParametersT algParameters, IntT nPointsEstimate){
  ASSERT(algParameters.typeHT == HT_LINKED_LIST || algParameters.typeHT == HT_STATISTICS);
  PRNearNeighborStructT nnStruct = initializePRNearNeighborFields(algParameters, nPointsEstimate);

  // initialize second level hashing (bucket hashing)
  FAILIF(NULL == (nnStruct->hashedBuckets = (PUHashStructureT*)MALLOC(nnStruct->parameterL * sizeof(PUHashStructureT))));
  // PI-64
  UnsT *mainHashA = NULL, *controlHash1 = NULL;
  BooleanT uhashesComputedAlready = FALSE;
  for(IntT i = 0; i < nnStruct->parameterL; i++){
    nnStruct->hashedBuckets[i] = newUHashStructure(algParameters.typeHT, nPointsEstimate, nnStruct->parameterK, uhashesComputedAlready, mainHashA, controlHash1, NULL);
    uhashesComputedAlready = TRUE;
  }

  return nnStruct;
}

void preparePointAdding(PRNearNeighborStructT nnStruct, PUHashStructureT uhash, PPointT point);


// Construct PRNearNeighborStructT given the data set <dataSet> (all
// the points <dataSet> will be contained in the resulting DS).
// Currenly only type HT_HYBRID_CHAINS is supported for this
// operation.
// PI-64
PRNearNeighborStructT initLSH_WithDataSet(RNNParametersT algParameters, IntT nPoints, PPointT *dataSet){
  ASSERT(algParameters.typeHT == HT_HYBRID_CHAINS);
  ASSERT(dataSet != NULL);
  ASSERT(USE_SAME_UHASH_FUNCTIONS);

  PRNearNeighborStructT nnStruct = initializePRNearNeighborFields(algParameters, nPoints);

  // Set the fields <nPoints> and <points>.
  nnStruct->nPoints = nPoints;
  // PI-64
  for(IntT i = 0; i < nPoints; i++){
    nnStruct->points[i] = dataSet[i];
  }
  
  // initialize second level hashing (bucket hashing)
  FAILIF(NULL == (nnStruct->hashedBuckets = (PUHashStructureT*)MALLOC(nnStruct->parameterL * sizeof(PUHashStructureT))));
  // PI-64
  UnsT *mainHashA = NULL, *controlHash1 = NULL;
  PUHashStructureT modelHT = newUHashStructure(HT_LINKED_LIST, nPoints, nnStruct->parameterK, FALSE, mainHashA, controlHash1, NULL);
  
  // PI-64
  UnsT **(precomputedHashesOfULSHs[nnStruct->nHFTuples]);
  for(IntT l = 0; l < nnStruct->nHFTuples; l++){
    FAILIF(NULL == (precomputedHashesOfULSHs[l] = (UnsT**)MALLOC(nPoints * sizeof(UnsT*))));
    for(IntT i = 0; i < nPoints; i++){
      FAILIF(NULL == (precomputedHashesOfULSHs[l][i] = (UnsT*)MALLOC(N_PRECOMPUTED_HASHES_NEEDED * sizeof(UnsT))));
    }
  }

  for(IntT i = 0; i < nPoints; i++){
    preparePointAdding(nnStruct, modelHT, dataSet[i]);
    for(IntT l = 0; l < nnStruct->nHFTuples; l++){
      for(IntT h = 0; h < N_PRECOMPUTED_HASHES_NEEDED; h++){
	precomputedHashesOfULSHs[l][i][h] = nnStruct->precomputedHashesOfULSHs[l][h];
      }
    }
  }

  //DPRINTF("Allocated memory(modelHT and precomputedHashesOfULSHs just a.): %ld\n", totalAllocatedMemory);

  // Initialize the counters for defining the pair of <u> functions used for <g> functions.
  IntT firstUComp = 0;
  IntT secondUComp = 1;
  for(IntT i = 0; i < nnStruct->parameterL; i++){
    // build the model HT.
    for(IntT p = 0; p < nPoints; p++){
      // Add point <dataSet[p]> to modelHT.
      if (!nnStruct->useUfunctions) {
	// Use usual <g> functions (truly independent; <g>s are precisly
	// <u>s).
	addBucketEntry(modelHT, 1, precomputedHashesOfULSHs[i][p], NULL, p);
      } else {
	// Use <u> functions (<g>s are pairs of <u> functions).
	addBucketEntry(modelHT, 2, precomputedHashesOfULSHs[firstUComp][p], precomputedHashesOfULSHs[secondUComp][p], p);
      }
    }

    //ASSERT(nAllocatedGBuckets <= nPoints);
    //ASSERT(nAllocatedBEntries <= nPoints);

    // compute what is the next pair of <u> functions.
    secondUComp++;
    if (secondUComp == nnStruct->nHFTuples) {
      firstUComp++;
      secondUComp = firstUComp + 1;
    }

    // copy the model HT into the actual (packed) HT. copy the uhash function too.
    nnStruct->hashedBuckets[i] = newUHashStructure(algParameters.typeHT, nPoints, nnStruct->parameterK, TRUE, mainHashA, controlHash1, modelHT);

    // clear the model HT for the next iteration.
    clearUHashStructure(modelHT);
  }

  freeUHashStructure(modelHT, FALSE); // do not free the uhash functions since they are used by nnStruct->hashedBuckets[i]

  // freeing precomputedHashesOfULSHs
  for(IntT l = 0; l < nnStruct->nHFTuples; l++){
    for(IntT i = 0; i < nPoints; i++){
      FREE(precomputedHashesOfULSHs[l][i]);
    }
    FREE(precomputedHashesOfULSHs[l]);
  }

  return nnStruct;
}



// // Packed version (static).
// PI-64
// PRNearNeighborStructT buildPackedLSH(RealT R, BooleanT useUfunctions, IntT k, IntT LorM, RealT successProbability, IntT dim, IntT T, IntT nPoints, PPointT *points){
//   ASSERT(points != NULL);
//   PRNearNeighborStructT nnStruct = initializePRNearNeighborFields(R, useUfunctions, k, LorM, successProbability, dim, T, nPoints);

//   // initialize second level hashing (bucket hashing)
//   FAILIF(NULL == (nnStruct->hashedBuckets = (PUHashStructureT*)MALLOC(nnStruct->parameterL * sizeof(PUHashStructureT))));
//   UnsT *mainHashA = NULL, *controlHash1 = NULL;
//   PUHashStructureT modelHT = newUHashStructure(HT_STATISTICS, nPoints, nnStruct->parameterK, FALSE, mainHashA, controlHash1, NULL);
//   for(IntT i = 0; i < nnStruct->parameterL; i++){
//     // build the model HT.
//     for(IntT p = 0; p < nPoints; p++){
//       // addBucketEntry(modelHT, );
//     }



//     // copy the model HT into the actual (packed) HT.
//     nnStruct->hashedBuckets[i] = newUHashStructure(HT_PACKED, nPointsEstimate, nnStruct->parameterK, TRUE, mainHashA, controlHash1, modelHT);

//     // clear the model HT for the next iteration.
//     clearUHashStructure(modelHT);
//   }

//   return nnStruct;
// }


// Optimizes the nnStruct (non-agressively, i.e., without changing the
// parameters).
void optimizeLSH(PRNearNeighborStructT nnStruct){
  ASSERT(nnStruct != NULL);

  PointsListEntryT *auxList = NULL;
  for(IntT i = 0; i < nnStruct->parameterL; i++){
    optimizeUHashStructure(nnStruct->hashedBuckets[i], auxList);
  }
  FREE(auxList);
}

// Frees completely all the memory occupied by the <nnStruct>
// structure.
void freePRNearNeighborStruct(PRNearNeighborStructT nnStruct){
  if (nnStruct == NULL){
    return;
  }

  if (nnStruct->points != NULL) {
    free(nnStruct->points);
  }
  
  if (nnStruct->lshFunctions != NULL) {
    for(IntT i = 0; i < nnStruct->nHFTuples; i++){
      free(nnStruct->lshFunctions[i]);
    }
    free(nnStruct->lshFunctions);
  }
  
  if (nnStruct->precomputedHashesOfULSHs != NULL) {
    for(IntT i = 0; i < nnStruct->nHFTuples; i++){
      free(nnStruct->precomputedHashesOfULSHs[i]);
    }
    free(nnStruct->precomputedHashesOfULSHs);
  }

  freeUHashStructure(nnStruct->hashedBuckets[0], TRUE);
  for(IntT i = 1; i < nnStruct->parameterL; i++){
    freeUHashStructure(nnStruct->hashedBuckets[i], FALSE);
  }
  free(nnStruct->hashedBuckets);

  if (nnStruct->pointULSHVectors != NULL){
    for(IntT i = 0; i < nnStruct->nHFTuples; i++){
      free(nnStruct->pointULSHVectors[i]);
    }
    free(nnStruct->pointULSHVectors);
  }

  if (nnStruct->reducedPoint != NULL){
    free(nnStruct->reducedPoint);
  }

  if (nnStruct->markedPoints != NULL){
    free(nnStruct->markedPoints);
  }

  if (nnStruct->markedPointsIndeces != NULL){
    free(nnStruct->markedPointsIndeces);
  }
}

// If <reportingResult> == FALSe, no points are reported back in a
// <get> function. In particular any point that is found in the bucket
// is considered to be outside the R-ball of the query point.  If
// <reportingResult> == TRUE, then the structure behaves normally.
void setResultReporting(PRNearNeighborStructT nnStruct, BooleanT reportingResult){
  ASSERT(nnStruct != NULL);
  nnStruct->reportingResult = reportingResult;
}

// Compute the value of a hash function u=lshFunctions[gNumber] (a
// vector of <hfTuplesLength> LSH functions) in the point <point>. The
// result is stored in the vector <vectorValue>. <vectorValue> must be
// already allocated (and have space for <hfTuplesLength> Uns32T-words).
// PI-64
#if 0
inline void computeULSH(PRNearNeighborStructT nnStruct, IntT gNumber, RealT *point, UnsT *vectorValue){
  CR_ASSERT(nnStruct != NULL);
  CR_ASSERT(point != NULL);
  CR_ASSERT(vectorValue != NULL);

  for(IntT i = 0; i < nnStruct->hfTuplesLength; i++){
    RealT value = 0;
    for(IntT d = 0; d < nnStruct->dimension; d++){
      value += point[d] * nnStruct->lshFunctions[gNumber][i].a[d];
    }
  
    vectorValue[i] = (UnsT)(FLOOR((value + nnStruct->lshFunctions[gNumber][i].b) / nnStruct->parameterW) /*- MIN_INTT*/);
  }
}
#endif
inline void computeULSH(PRNearNeighborStructT nnStruct, IntT gNumber, const std::vector<uint8_t>& pointC, RealT scalingFactor, UnsT* vectorValue){
  CR_ASSERT(nnStruct != NULL);
  CR_ASSERT(point != NULL);
  CR_ASSERT(vectorValue != NULL);

  for(IntT i = 0; i < nnStruct->hfTuplesLength; i++){
    RealT value = dotProduct(&pointC[0], pointC.size(), &nnStruct->lshFunctions[gNumber][i].a[0]);
    value /= scalingFactor;
    vectorValue[i] = (UnsT)(FLOOR((value + nnStruct->lshFunctions[gNumber][i].b) / nnStruct->parameterW) /*- MIN_INTT*/);
  }
}


inline void preparePointAdding(PRNearNeighborStructT nnStruct, PUHashStructureT uhash, PPointT point){
  ASSERT(nnStruct != NULL);
  ASSERT(uhash != NULL);
  ASSERT(point != NULL);
  ASSERT(nnStruct->parameterR >= Pi_EPSILON);


#if 0
  // Pi: if 'R' is small, just leave the point as it is to avoid division by 0 problem.
  if ( nnStruct->parameterR < Pi_EPSILON /* better be the machine epsilon */ ) {
    for(IntT d = 0; d < nnStruct->dimension; d++){
      nnStruct->reducedPoint[d] = point->coordinates[d];
    }
  } else {
    for(IntT d = 0; d < nnStruct->dimension; d++){
      nnStruct->reducedPoint[d] = point->coordinates[d] / nnStruct->parameterR; // Pi: ==>inf if 'R'==0; ==> all vectors are hashed into a same bucket ==> O(dn^2), instead of O(dn^\rho\logn), query time!
    }
  }
#endif

  // Compute all ULSH functions.
  TIMEV_START(timeComputeULSH);
  for(IntT i = 0; i < nnStruct->nHFTuples; i++){
     computeULSH(nnStruct, i, point->coordinatesC, nnStruct->parameterR, nnStruct->pointULSHVectors[i]);
  }

  // Compute data for <precomputedHashesOfULSHs>.
  if (USE_SAME_UHASH_FUNCTIONS) {
    for(IntT i = 0; i < nnStruct->nHFTuples; i++){
      precomputeUHFsForULSH(uhash, nnStruct->pointULSHVectors[i], nnStruct->hfTuplesLength, nnStruct->precomputedHashesOfULSHs[i]);
    }
  }

  TIMEV_END(timeComputeULSH);
}

inline void batchAddRequest(PRNearNeighborStructT nnStruct, IntT i, IntT &firstIndex, IntT &secondIndex, PPointT point){
    // PI-64
//   UnsT *(gVector[4]);
//   if (!nnStruct->useUfunctions) {
//     // Use usual <g> functions (truly independent).
//     gVector[0] = nnStruct->pointULSHVectors[i];
//     gVector[1] = nnStruct->precomputedHashesOfULSHs[i];
//     addBucketEntry(nnStruct->hashedBuckets[firstIndex], gVector, 1, point);
//   } else {
//     // Use <u> functions (<g>s are pairs of <u> functions).
//     gVector[0] = nnStruct->pointULSHVectors[firstIndex];
//     gVector[1] = nnStruct->pointULSHVectors[secondIndex];
//     gVector[2] = nnStruct->precomputedHashesOfULSHs[firstIndex];
//     gVector[3] = nnStruct->precomputedHashesOfULSHs[secondIndex];
    
//     // compute what is the next pair of <u> functions.
//     secondIndex++;
//     if (secondIndex == nnStruct->nHFTuples) {
//       firstIndex++;
//       secondIndex = firstIndex + 1;
//     }
    
//     addBucketEntry(nnStruct->hashedBuckets[i], gVector, 2, point);
//   }
}

// Adds a new point to the LSH data structure, that is for each
// i=0..parameterL-1, the point is added to the bucket defined by
// function g_i=lshFunctions[i].
void addNewPointToPRNearNeighborStruct(PRNearNeighborStructT nnStruct, PPointT point){
  ASSERT(nnStruct != NULL);
  ASSERT(point != NULL);
  ASSERT(nnStruct->reducedPoint != NULL);
  ASSERT(!nnStruct->useUfunctions || nnStruct->pointULSHVectors != NULL);
  ASSERT(nnStruct->hashedBuckets[0]->typeHT == HT_LINKED_LIST || nnStruct->hashedBuckets[0]->typeHT == HT_STATISTICS);

  nnStruct->points[nnStruct->nPoints] = point;
  nnStruct->nPoints++;

  preparePointAdding(nnStruct, nnStruct->hashedBuckets[0], point);

  // Initialize the counters for defining the pair of <u> functions used for <g> functions.
  IntT firstUComp = 0;
  IntT secondUComp = 1;

  TIMEV_START(timeBucketIntoUH);
  for(IntT i = 0; i < nnStruct->parameterL; i++){
    if (!nnStruct->useUfunctions) {
      // Use usual <g> functions (truly independent; <g>s are precisly
      // <u>s).
      addBucketEntry(nnStruct->hashedBuckets[i], 1, nnStruct->precomputedHashesOfULSHs[i], NULL, nnStruct->nPoints - 1);
    } else {
      // Use <u> functions (<g>s are pairs of <u> functions).
      addBucketEntry(nnStruct->hashedBuckets[i], 2, nnStruct->precomputedHashesOfULSHs[firstUComp], nnStruct->precomputedHashesOfULSHs[secondUComp], nnStruct->nPoints - 1);

      // compute what is the next pair of <u> functions.
      secondUComp++;
      if (secondUComp == nnStruct->nHFTuples) {
	firstUComp++;
	secondUComp = firstUComp + 1;
      }
    }
    //batchAddRequest(nnStruct, i, firstUComp, secondUComp, point);
  }
  TIMEV_END(timeBucketIntoUH);

  // Check whether the vectors <nearPoints> & <nearPointsIndeces> is still big enough.
  if (nnStruct->nPoints > nnStruct->sizeMarkedPoints) {
    nnStruct->sizeMarkedPoints = 2 * nnStruct->nPoints;
    FAILIF(NULL == (nnStruct->markedPoints = (BooleanT*)REALLOC(nnStruct->markedPoints, nnStruct->sizeMarkedPoints * sizeof(BooleanT))));
    for(IntT i = 0; i < nnStruct->sizeMarkedPoints; i++){
      nnStruct->markedPoints[i] = FALSE;
    }
    FAILIF(NULL == (nnStruct->markedPointsIndeces = (IntT*)REALLOC(nnStruct->markedPointsIndeces, nnStruct->sizeMarkedPoints * sizeof(IntT))));
  }
}
// Returns TRUE iff |p1-p2|_2^2 <= threshold
inline BooleanT isDistanceSqrLeq(IntT dimension, uint16_t* p1data, PPointT p2, RealT threshold){
  //std::cout << "RowNumber " << p2->prop.rowNumber << std::endl;
  RealT result = l2distanceSquared(&p2->coordinatesC[0], p2->coordinatesC.size(), p1data);
  return result <= threshold;
}

#if 0
// Returns TRUE iff |p1-p2|_2^2 <= threshold
inline BooleanT isDistanceSqrLeq(IntT dimension, PPointT p1, PPointT p2, RealT threshold){
  RealT result = 0;
  nOfDistComps++;

  //TIMEV_START(timeDistanceComputation);
  for (IntT i = 0; i < dimension; i++){
    RealT temp = p1->coordinates[i] - p2->coordinates[i];
    result += SQR(temp);
    if (result > threshold){
      TIMEV_END(timeDistanceComputation);
      return 0;
    }
  }
  //TIMEV_END(timeDistanceComputation);

  //return result <= threshold;
  return 1;
}
#endif
// // Returns TRUE iff |p1-p2|_2^2 <= threshold
// inline BooleanT isDistanceSqrLeq(IntT dimension, PPointT p1, PPointT p2, RealT threshold){
//   RealT result = 0;
//   nOfDistComps++;

//   //TIMEV_START(timeDistanceComputation);
//   for (IntT i = 0; i < dimension; i++){
//     result += p1->coordinates[i] * p2->coordinates[i];
//   }
//   //TIMEV_END(timeDistanceComputation);

//   return p1->sqrLength + p2->sqrLength - 2 * result <= threshold;
// }

// Returns the list of near neighbors of the point <point> (with a
// certain success probability). Near neighbor is defined as being a
// point within distance <parameterR>. Each near neighbor from the
// data set is returned is returned with a certain probability,
// dependent on <parameterK>, <parameterL>, and <parameterT>. The
// returned points are kept in the array <result>. If result is not
// allocated, it will be allocated to at least some minimum size
// (RESULT_INIT_SIZE). If number of returned points is bigger than the
// size of <result>, then the <result> is resized (to up to twice the
// number of returned points). The return value is the number of
// points found.
// PI-64
IntT getNearNeighborsFromPRNearNeighborStruct(PRNearNeighborStructT nnStruct, PPointT query, PPointT *(&result), IntT &resultSize){
  ASSERT(nnStruct != NULL);
  ASSERT(query != NULL);
  ASSERT(nnStruct->reducedPoint != NULL);
  ASSERT(!nnStruct->useUfunctions || nnStruct->pointULSHVectors != NULL);

  PPointT point = query;

  if (result == NULL){
    resultSize = RESULT_INIT_SIZE;
    FAILIF(NULL == (result = (PPointT*)MALLOC(resultSize * sizeof(PPointT))));
  }
  
  preparePointAdding(nnStruct, nnStruct->hashedBuckets[0], point);
  boost::scoped_array<uint16_t> pointUncompressed(new uint16_t[nnStruct->dimension]);
  decompressVector(&point->coordinatesC[0], point->coordinatesC.size(), &pointUncompressed[0]);

  // PI-64
  UnsT precomputedHashesOfULSHs[nnStruct->nHFTuples][N_PRECOMPUTED_HASHES_NEEDED];
  for(IntT i = 0; i < nnStruct->nHFTuples; i++){
    for(IntT j = 0; j < N_PRECOMPUTED_HASHES_NEEDED; j++){
      precomputedHashesOfULSHs[i][j] = nnStruct->precomputedHashesOfULSHs[i][j];
    }
  }
  TIMEV_START(timeTotalBuckets);

  BooleanT oldTimingOn = timingOn;
  if (noExpensiveTiming) {
    timingOn = FALSE;
  }
  
  // Initialize the counters for defining the pair of <u> functions used for <g> functions.
  IntT firstUComp = 0;
  IntT secondUComp = 1;

  // PI-64
  IntT nNeighbors = 0;// the number of near neighbors found so far.
  IntT nMarkedPoints = 0;// the number of marked points
  for(IntT i = 0; i < nnStruct->parameterL; i++){ 
    TIMEV_START(timeGetBucket);
    GeneralizedPGBucket gbucket;
    if (!nnStruct->useUfunctions) {
      // Use usual <g> functions (truly independent; <g>s are precisly
      // <u>s).
      gbucket = getGBucket(nnStruct->hashedBuckets[i], 1, precomputedHashesOfULSHs[i], NULL);
    } else {
      // Use <u> functions (<g>s are pairs of <u> functions).
      gbucket = getGBucket(nnStruct->hashedBuckets[i], 2, precomputedHashesOfULSHs[firstUComp], precomputedHashesOfULSHs[secondUComp]);

      // compute what is the next pair of <u> functions.
      secondUComp++;
      if (secondUComp == nnStruct->nHFTuples) {
	firstUComp++;
	secondUComp = firstUComp + 1;
      }
    }
    TIMEV_END(timeGetBucket);

    PGBucketT bucket;

    TIMEV_START(timeCycleBucket);
    switch (nnStruct->hashedBuckets[i]->typeHT){
    case HT_LINKED_LIST:
      bucket = gbucket.llGBucket;
      if (bucket != NULL){
	// circle through the bucket and add to <result> the points that are near.
	PBucketEntryT bucketEntry = &(bucket->firstEntry);
	//TIMEV_START(timeCycleProc);
	while (bucketEntry != NULL){
	  //TIMEV_END(timeCycleProc);
	  //ASSERT(bucketEntry->point != NULL);
	  //TIMEV_START(timeDistanceComputation);
          // PI-64
	  IntT candidatePIndex = bucketEntry->pointIndex;
	  PPointT candidatePoint = nnStruct->points[candidatePIndex];
          if (isDistanceSqrLeq(nnStruct->dimension, &pointUncompressed[0], candidatePoint, nnStruct->parameterR2) && nnStruct->reportingResult){
	  //if (isDistanceSqrLeq(nnStruct->dimension, point, candidatePoint, nnStruct->parameterR2) && nnStruct->reportingResult){
	    //TIMEV_END(timeDistanceComputation);
	    if (nnStruct->markedPoints[candidatePIndex] == FALSE) {
	      //TIMEV_START(timeResultStoring);
	      // a new R-NN point was found (not yet in <result>).
	      if (nNeighbors >= resultSize){
		// run out of space => resize the <result> array.
		resultSize = 2 * resultSize;
		result = (PPointT*)REALLOC(result, resultSize * sizeof(PPointT));
	      }
	      result[nNeighbors] = candidatePoint;
	      nnStruct->markedPointsIndeces[nNeighbors] = candidatePIndex;
	      nnStruct->markedPoints[candidatePIndex] = TRUE; // do not include more points with the same index
	      nNeighbors++;
	      //TIMEV_END(timeResultStoring);
	    }
	  }else{
	    //TIMEV_END(timeDistanceComputation);
	  }
	  //TIMEV_START(timeCycleProc);
	  bucketEntry = bucketEntry->nextEntry;
	}
	//TIMEV_END(timeCycleProc);
      }
      break;
    case HT_STATISTICS:
      ASSERT(FALSE); // HT_STATISTICS not supported anymore

//       if (gbucket.linkGBucket != NULL && gbucket.linkGBucket->indexStart != INDEX_START_EMPTY){
// 	IntT position;
// 	PointsListEntryT *pointsList = nnStruct->hashedBuckets[i]->bucketPoints.pointsList;
// 	position = gbucket.linkGBucket->indexStart;
// 	// circle through the bucket and add to <result> the points that are near.
// 	while (position != INDEX_START_EMPTY){
// 	  PPointT candidatePoint = pointsList[position].point;
// 	  if (isDistanceSqrLeq(nnStruct->dimension, point, candidatePoint, nnStruct->parameterR2) && nnStruct->reportingResult){
// 	    if (nnStruct->nearPoints[candidatePoint->index] == FALSE) {
// 	      // a new R-NN point was found (not yet in <result>).
// 	      if (nNeighbors >= resultSize){
// 		// run out of space => resize the <result> array.
// 		resultSize = 2 * resultSize;
// 		result = (PPointT*)REALLOC(result, resultSize * sizeof(PPointT));
// 	      }
// 	      result[nNeighbors] = candidatePoint;
// 	      nNeighbors++;
// 	      nnStruct->nearPoints[candidatePoint->index] = TRUE; // do not include more points with the same index
// 	    }
// 	  }
// 	  // IntT oldP = position;
// 	  position = pointsList[position].nextPoint;
// 	  // ASSERT(position == INDEX_START_EMPTY || position == oldP + 1);
// 	}
//       }
      break;
    case HT_HYBRID_CHAINS:
      if (gbucket.hybridGBucket != NULL){
	PHybridChainEntryT hybridPoint = gbucket.hybridGBucket;
        // PI-64
	UnsT offset = 0;
	if (hybridPoint->point.bucketLength == 0){
	  // there are overflow points in this bucket.
	  offset = 0;
	  for(IntT j = 0; j < N_FIELDS_PER_INDEX_OF_OVERFLOW; j++){
              // PI-64
	    offset += ((UnsT)((hybridPoint + 1 + j)->point.bucketLength) << (j * N_BITS_FOR_BUCKET_LENGTH));
	  }
	}
	UnsT index = 0;
	BooleanT done = FALSE;
	while(!done){
	  if (index == MAX_NONOVERFLOW_POINTS_PER_BUCKET){
	    //CR_ASSERT(hybridPoint->point.bucketLength == 0);
	    index = index + offset;
	  }
          // PI-64
	  IntT candidatePIndex = (hybridPoint + index)->point.pointIndex;
	  CR_ASSERT(candidatePIndex >= 0 && candidatePIndex < nnStruct->nPoints);
	  done = (hybridPoint + index)->point.isLastPoint == 1 ? TRUE : FALSE;
	  index++;
	  if (nnStruct->markedPoints[candidatePIndex] == FALSE){
	    // mark the point first.
	    nnStruct->markedPointsIndeces[nMarkedPoints] = candidatePIndex;
	    nnStruct->markedPoints[candidatePIndex] = TRUE; // do not include more points with the same index
	    nMarkedPoints++;

	    PPointT candidatePoint = nnStruct->points[candidatePIndex];
            if (isDistanceSqrLeq(nnStruct->dimension, &pointUncompressed[0], candidatePoint, nnStruct->parameterR2) && nnStruct->reportingResult){
//	    if (isDistanceSqrLeq(nnStruct->dimension, point, candidatePoint, nnStruct->parameterR2) && nnStruct->reportingResult){
	      //if (nnStruct->markedPoints[candidatePIndex] == FALSE) {
	      // a new R-NN point was found (not yet in <result>).
	      //TIMEV_START(timeResultStoring);
	      if (nNeighbors >= resultSize){
		// run out of space => resize the <result> array.
		resultSize = 2 * resultSize;
		result = (PPointT*)REALLOC(result, resultSize * sizeof(PPointT));
	      }
	      result[nNeighbors] = candidatePoint;
	      nNeighbors++;
	      //TIMEV_END(timeResultStoring);
	      //nnStruct->markedPointsIndeces[nMarkedPoints] = candidatePIndex;
	      //nnStruct->markedPoints[candidatePIndex] = TRUE; // do not include more points with the same index
	      //nMarkedPoints++;
	      //}
	    }
	  }else{
	    // the point was already marked (& examined)
	  }
	}
      }
      break;
    default:
      ASSERT(FALSE);
    }
    TIMEV_END(timeCycleBucket);
    
  }

  timingOn = oldTimingOn;
  TIMEV_END(timeTotalBuckets);

  // we need to clear the array nnStruct->nearPoints for the next query.
  // PI-64
  for(IntT i = 0; i < nMarkedPoints; i++){
    ASSERT(nnStruct->markedPoints[nnStruct->markedPointsIndeces[i]] == TRUE);
    nnStruct->markedPoints[nnStruct->markedPointsIndeces[i]] = FALSE;
  }

  return nNeighbors;
}
