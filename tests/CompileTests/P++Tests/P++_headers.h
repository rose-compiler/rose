
// Allow repeated includes of A++.h without error
#ifndef _PPP_ARRAY_CLASS_LIBRARY_H
#define _PPP_ARRAY_CLASS_LIBRARY_H

// The P++.h file calls the A++.h file (not the other way around)
// include <P++.h> 

class SerialArray_Domain_Type; // Forward declaration
//class SerialArray_Descriptor_Type; // Forward declaration
class doubleSerialArray;           // Forward declaration
class floatSerialArray;            // Forward declaration
class intSerialArray;              // Forward declaration

#define STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES 50
#ifdef MAIN_APP_PROGRAM
int    PPP_DEBUG                       = 0;
int    Array_Index_For_double_Variable = 0;
int    Array_Index_For_float_Variable  = 0;
int    Array_Index_For_int_Variable    = 0;
double Static_double_Variable [STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES];
float  Static_float_Variable  [STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES];
int    Static_int_Variable    [STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES];
#else
extern int    PPP_DEBUG;
extern int    Array_Index_For_double_Variable;
extern int    Array_Index_For_float_Variable;
extern int    Array_Index_For_int_Variable;
extern double Static_double_Variable[STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES];
extern float  Static_float_Variable [STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES];
extern int    Static_int_Variable   [STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES];
#endif

#if 0
// START: Evaluate the macros that drive the compilation
#if defined(COMPILE_APP)
#error "In P++/include/P++_headers.h: COMPILE_APP is defined"
#else
#error "In P++/include/P++_headers.h: COMPILE_APP is NOT defined"
#endif

#if defined(COMPILE_SERIAL_APP)
#error "In P++/include/P++_headers.h: COMPILE_SERIAL_APP is defined"
#else
#error "In P++/include/P++_headers.h: COMPILE_SERIAL_APP is NOT defined"
#endif

#if defined(COMPILE_PPP)
#error "In P++/include/P++_headers.h: COMPILE_PPP is defined"
#else
#error "In P++/include/P++_headers.h: COMPILE_PPP is NOT defined"
#endif

#if defined(SERIAL_APP)
#error "In P++/include/P++_headers.h: SERIAL_APP is defined"
#else
#error "In P++/include/P++_headers.h: SERIAL_APP is NOT defined"
#endif

#if defined(PPP)
#error "In P++/include/P++_headers.h: PPP is defined"
#else
#error "In P++/include/P++_headers.h: PPP is NOT defined"
#endif

// END: Evaluate the macros that drive the compilation
#endif

// We can't just test if COMPILE_PPP is defined since it 
// might be set to FALSE in which case it is still defined!
// if defined(COMPILE_PPP) || defined(COMPILE_SERIAL_APP)
// if COMPILE_PPP
#if defined(COMPILE_PPP)

// We can define the preprocessor variable on the command line of the makefile!
#ifdef _CM5_
#include "alias.h"
#endif

#if defined(USE_PADRE)
// define PARALLEL_PADRE
#include "../PADRE/src/PADRE.h"
// include "PADRE.h"
#else
extern "C"
   {
// include "../BLOCK_PARTI/bsparti.h"
// include "../../PADRE/PARTI/bsparti.h"
// We use a link to represent this file within the P++/include directory

// (DQ 3/26/2000) New versions of MPI include mpi++.h if defined(__cplusplus) == TRUE
// This is a problem on the NCSA SGI Origin 2000 machine. But we can't redefine a predefined macro!!!
// if defined(__cplusplus)
// undef __cplusplus
// endif
#include "bsparti.h"
// define __cplusplus 1
   }
#endif

// Header file for Block Parti (low level parallel support for P++)
// include "../BLOCK_PARTI/bsparti.h"
// define USE_PADRE
// Header files specific to parallel information
#include "comm_man.h"
#include "partitioning.h"

// Header files specific to the array class features of P++
// include "mdi_typedef.h"
#include "p_data_hash.h"
// include "index.h"
#include "p_domain.h"
#include "p_descriptor.h"
// include "lazy_expression.h"
#include "p_lazy_operand.h"

// include "lazy_task.h"
// include "lazy_taskset.h"
// include "lazy_statement.h"
#include "p_lazyarray.h"
#include "p_lazy_aggregate.h"
// include "optimization.h"

// include "array.h"
// include "where.h"
#else
// These are the only files the user codes require!
// include "index.h"

#if defined(USE_PADRE)
#define PARALLEL_PADRE
#include "../PADRE/src/PADRE.h"
#else
extern "C"
   {
// include "../BLOCK_PARTI/bsparti.h"
// include "../../PADRE/PARTI/bsparti.h"
// We use a link to represent this file within the P++/include directory
#include "bsparti.h"
   }
#endif

#include "comm_man.h"
#include "partitioning.h"
#include "lazy_taskset.h"
// include "optimization.h"
#include "p_data_hash.h"
// include "mdi_typedef.h"
#include "p_domain.h"
#include "p_descriptor.h"
// include "array.h"
// include "where.h"
#endif

#include "conform_array_set.h"
#include "conform_enforce.h"
//#include "parallel_io.h"

// Define macros for parallel I/O
// DEFINE scanf Parallel_Scanf

// Exit scope of initial header ifdef (this avoids errors if A++.h is included twice)
#endif  /* !defined(_PPP_ARRAY_CLASS_LIBRARY_H) */

