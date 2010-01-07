#include "sage3basic.h"
/*
 * this includes the forward declarations of all the sage node classes
 * from the generated files (i.e. gives just the class names.)
 *
 */

#ifndef SAGE3_CLASSES_H
#define SAGE3_CLASSES_H


// DQ (7/6/2005): Added to support performance analysis of ROSE.
// This is located in ROSE/src/midend/astDiagnostics
//#include "AstPerformance.h"

// DQ (5/28/2007): Added new AST Merge API
#include "astMergeAPI.h"


// DQ (9/1/2006): It is currently an error to normalize the source file names stored 
// in the SgProject IR node to be absolute paths if they didn't originally appear 
// that way on the commandline.  We have partial support for this but it is a bug
// at the moment to use this.  However, we do now (work by Andreas) normalize the
// source file name when input to EDG so that all Sg_File_Info objects store an
// absolute path (unless modified using a #line directive, see test2004_60.C as an 
// example).  The current work is an incremental solution.
#define USE_ABSOLUTE_PATHS_IN_SOURCE_FILE_LIST 0

// JJW 10-23-2007
// Add possibility to include Valgrind header for memcheck
#if ROSE_USE_VALGRIND
#include <valgrind/valgrind.h>
#include <valgrind/memcheck.h>
#endif

#endif















