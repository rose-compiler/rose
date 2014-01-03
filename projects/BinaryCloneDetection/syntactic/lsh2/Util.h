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

#ifndef UTIL_INCLUDED
#define UTIL_INCLUDED

#include "BasicDefinitions.h"

BooleanT vectorsEqual(IntT size, IntT *v1, IntT *v2);

void copyVector(IntT size, IntT *from, IntT *to);

IntT *copyOfVector(IntT size, IntT *from);

void printRealVector(char *s, IntT size, RealT *v);

void printIntVector(char *s, IntT size, IntT *v);

// PI-64
UnsT getAvailableMemory();

#endif
