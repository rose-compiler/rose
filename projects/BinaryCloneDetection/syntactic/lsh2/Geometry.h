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

#ifndef GEOMETRY_INCLUDED
#define GEOMETRY_INCLUDED
#include <vector>

/* A set of properties of a point */

struct VectorEntry {
  // CREATE TABLE vectors(row_number INTEGER PRIMARY KEY, function_id INTEGER,  index_within_function INTEGER, line INTEGER, offset INTEGER, sum_of_counts INTEGER, counts BLOB, instr_seq BLOB);
  uint64_t rowNumber;
  uint32_t functionId;
  uint32_t indexWithinFunction;
  uint64_t line;
  uint32_t offset;
  uint32_t sum_of_counts;
  uint64_t compressedCountsOffset;
  uint32_t compressedCountsLength;
  char instrSeqMD5[16];
  VectorEntry(): compressedCountsLength(0) {} // Mark entry as invalid
};

// A simple point in d-dimensional space. A point is defined by a
// vector of coordinates. 
typedef struct _PointT {
  //IntT dimension;
  IntT index; // the index of this point in the dataset list of points
  std::vector<uint8_t> coordinatesC;
  RealT sqrLength; // the square of the length of the vector
  VectorEntry prop;
  //char *filename;
  //int prop[ENUM_PPROP_LAST_NOT_USED-1]; // doesn't contain ENUM_PPROP_FILE.
  //char *oids;
} PointT  , *PPointT ;


typedef enum {
  ENUM_PPROP_FILE, ENUM_PPROP_LINE, ENUM_PPROP_OFFSET, ENUM_PPROP_NODE_KIND,
  ENUM_PPROP_NUM_NODE, ENUM_PPROP_NUM_DECL, ENUM_PPROP_NUM_STMT, ENUM_PPROP_NUM_EXPR,
  // the following is mainly for bug finding:
  ENUM_PPROP_TBID, ENUM_PPROP_TEID,
  ENUM_PPROP_nVARs, 
  ENUM_PPROP_CONTEXT_KIND, ENUM_PPROP_NEIGHBOR_KIND, ENUM_PPROP_OIDs,
  ENUM_PPROP_LAST_NOT_USED
} pprop_t;
#if 0
// A simple point in d-dimensional space. A point is defined by a
// vector of coordinates. 
typedef struct _PointT {
  //IntT dimension;
  IntT index; // the index of this point in the dataset list of points
  RealT *coordinates;
  RealT sqrLength; // the square of the length of the vector
  char *filename;
  int prop[ENUM_PPROP_LAST_NOT_USED-1]; // doesn't contain ENUM_PPROP_FILE.
  char *oids;
} PointT, *PPointT;
#endif
RealT distance(IntT dimension, PPointT p1, PPointT p2);

#endif
