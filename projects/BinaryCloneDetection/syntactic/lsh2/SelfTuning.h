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

#ifndef SELFTUNING_INCLUDED
#define SELFTUNING_INCLUDED

void tuneTimeFunctions();

// PI-64
void sortQueryPointsByRadii(IntT dimension,
			    IntT nQueries, 
			    PPointT *queries, 
			    IntT nPoints, 
			    PPointT *dataSet,
			    IntT nRadii,
			    RealT *radii,
			    IntT *boundaryIndeces);

// PI-64
void determineRTCoefficients(RealT thresholdR, RealT successProbability, BooleanT useUfunctions, IntT typeHT, IntT dimension, IntT nPoints, PPointT *realData, RealT &lshPrecomp, RealT &uhashOver, RealT &distComp);

RealT estimateNCollisions(IntT nPoints, IntT dim, PPointT *dataSet, PPointT query, IntT k, IntT L, RealT R);

//RealT estimateNDistinctCollisions(IntT nPoints, IntT dim, PPointT *dataSet, PPointT query, BooleanT useUfunctions, IntT hfTuplesLength, IntT nHFTuples, RealT R);
RealT estimateNDistinctCollisions(const IntT nPoints, const IntT dim, const PPointT *dataSet, const PPointT query, const BooleanT useUfunctions, const IntT k, const IntT LorM, const RealT R);

RealT computeFunctionP(RealT w, RealT c);

IntT computeLfromKP(IntT k, RealT successProbability);

IntT computeMForULSH(IntT k, RealT successProbability);

// PI-64
RNNParametersT computeOptimalParameters(RealT R, RealT successProbability, IntT nPoints, IntT dimension, PPointT *dataSet, IntT nSampleQueries, PPointT *sampleQueries, IntT memoryUpperBound);


#endif
