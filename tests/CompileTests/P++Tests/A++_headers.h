

// Allow repeated includes of A++.h without error
#ifndef _APP_ARRAY_CLASS_LIBRARY_H
#define _APP_ARRAY_CLASS_LIBRARY_H

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will
// build the library (factor of 5-10).
#ifdef GNU
#pragma interface
#endif

#include <stdio.h> 
#include <stdlib.h>

// We need these so we can make the function call that will sync the I/O!
#if !USE_SAGE_PREPROCESSOR
#include <iostream.h>
#include <fstream.h>
#endif

#include <math.h>
#include <time.h>
#include <limits.h>

// We need this for the getcwd and chdir UNIX command
#include <unistd.h>

#ifdef USE_STRING_SPECIFIC_CODE
#include <string.h>
#endif

#define TRUE  1
#define FALSE 0

// undefine USE_PPP if it was set to 0 in config.h. We can't do this at the bottom 
// of accconfig.h because undef will be changed to define in the config.h file
#if !USE_PPP
#undef USE_PPP
#endif

// Include TAU Perfromance Tools
#if defined(USE_TAU)
#include "../../TAU_Performance_Tools/Profile/Profiler.h"
#endif

#if defined(USE_EXPRESSION_TEMPLATES)
// Use the restrict keyword with the expression template version
#define APP_RESTRICT_MACRO restrict
#define LOCATE_EXPRESSION_TEMPLATES_IN_SOURCE_FILES
#else
#define APP_RESTRICT_MACRO 
#endif

#define APP_STATIC_NUMBER_OF_MAPS 100

// d e fine MEMORY_LEAK_TEST

// Turn ON the assert macro (ON is the default so this is not required) and turn it OFF with NDEBUG
// When the user uses P++ we want to avoid forcing the use of NDEBUG
// define NDEBUG

// The new makefile system can optional pass -DINTERNALDEBUG or not!
// if defined(NDEBUG) || (!defined(COMPILE_SERIAL_APP) && !defined(COMPILE_PPP))
#if defined(INTERNALDEBUG)
#define COMPILE_DEBUG_STATEMENTS TRUE
#define EXTRA_ERROR_CHECKING     TRUE

// (bugfix: 7/9/2000) If using INTERNALDEBUG then internally use bounds checking for all scalar indexing.
#define BOUNDS_CHECK
#else
// Bugfix (12/19/96) Allow the user to set NDEBUG or not
#define APP_NDEBUG
#define COMPILE_DEBUG_STATEMENTS FALSE
#define EXTRA_ERROR_CHECKING     FALSE
#endif

#if defined(USE_PTHREADS)
// Turn on the use of Pthreads in P++
#include <pthread.h>
#endif

#if defined(USE_PURIFY)
// Turn on the use of PURIFY in P++
#include <purify.h>
#endif

#include <assert.h>

// Prototypes for functions defined in array.C for which prototypes are required before array.h is processed.
void APP_ABORT();
void APP_Exit();
void APP_Assertion_Support ( char* Source_File_With_Error, unsigned Line_Number_In_File );

// Prototype for parallel printf support
int parallelPrintf ( const char *__builtin_va_alist , ... );

#if INTERNALDEBUG
// Make this available with internal debugging (later make it anoption in the configure script)
#define USE_PARALLEL_PRINTF
#endif

#if defined(APP_NDEBUG) || defined(NDEBUG)
// Define a version of assert for A++/P++ (case of NDEBUG defined)
// define APP_ASSERT(f) NULL
#define APP_ASSERT(f) assert(f)
#else
// Define a version of assert for A++/P++ (case of NDEBUG NOT defined)
// A++/P++ requires it's own version of assert since special termination
// procedures are required for the parallel environment.
#define APP_ASSERT(f) \
     if(f)            \
          (void)NULL;       \
     else             \
          APP_Assertion_Support (__FILE__,__LINE__)
#endif

// if defined(SOLARIS)
#if !defined(HPPA) && !defined(CRAY)
// This does not work on the HP-UX machines
#define AUTO_INITIALIZE_APPLICATION_PATH_NAME
#endif

#define DEFERED_EVALUATION TRUE 

// I like to define a Boolean type even if is is not in the current version of C++
// Unfortunately this conflicts with the X Window system
#if BOOL_IS_BROKEN
typedef int bool;
#endif

// Can we use valloc with the GNU g++ compiler on the SUN?
#if defined(sun) && !defined(GNU)
// valloc aligns allocated memory on page boundaries
#define APP_MALLOC(size) valloc(size)
#else
// standard malloc used
#define APP_MALLOC(size) malloc(size)
#endif

// We could use any unique values for these constants
// but we need to use the macro since otherwise the switch statements won't work!
// I don't currently know any way around this issue in C++!
#define UNINITIALIZED_VARIABLE -742 
#define NEWLY_ALLOCATED_MEMORY 101 
#define MEMORY_FROM_LHS        102 
#define MEMORY_FROM_RHS        103 
#define MEMORY_FROM_OPERAND    104 

// There are different types of copies:
//    Shallow Copy
//        Don't really copy anything (just make a new reference to the existing data)
//        This is NOT the default!
//    Deep Copy
//        Deep Collapsed Copy
//             Current functionality of the Deep Copy option.  This IS the default!
//        Deep Aligned Copy
//             Build copy to be same size as original array (might be inefficient)
//        Deep Collapsed and Aligned Copy
//             This requires additional support from PADRE (table based distributions)
// The ordering of this is a little off (we should fix that later)
#define SHALLOWCOPY             200
#define DEEPCOPY                201
#define DEEPCOLAPSEDCOPY        202
#define DEEPALIGNEDCOPY         203
#define DEEPCOLAPSEDALIGNEDCOPY 204

#define UNKNOWN_OPERATION       -1 

// Magic numbers (nothing really magic about them)
#define SINGLE_PROCESSOR         5000
#define LEFT_PROCESSOR           5001
#define MIDDLE_PROCESSOR         5002
#define RIGHT_PROCESSOR          5003
#define NOT_PRESENT_ON_PROCESSOR 5004

#define NON_DISTRIBUTED_AXIS           6000
#define FULL_VSG_MODEL                 6001
#define OVERLAPPING_BOUNDARY_VSG_MODEL 6002
#define UNDEFINED_VSG_MODEL            6003

#define MAX_PROCESSORS 1024

// Values used to define the element type for internal diagnostics
#define APP_UNDEFINED_ELEMENT_TYPE 7000
#define APP_INT_ELEMENT_TYPE       7001
#define APP_FLOAT_ELEMENT_TYPE     7002
#define APP_DOUBLE_ELEMENT_TYPE    7003

#define DEFAULT_CLASS_ALLOCATION_POOL_SIZE 100

// enum On_Off_Type { On , ON , on , Off , OFF , off };

enum Memory_Source_Type { Uninitialized_Source   = UNINITIALIZED_VARIABLE , 
                          Newly_Allocated_Memory = NEWLY_ALLOCATED_MEMORY , 
                          Memory_From_Lhs        = MEMORY_FROM_LHS , 
                          Memory_From_Rhs        = MEMORY_FROM_RHS ,
                          Memory_From_Operand    = MEMORY_FROM_OPERAND };

template<class T, int Template_Dimension> class Array_Descriptor_Type;       // Forward declaration
class doubleArray;                 // Forward declaration
class floatArray;                  // Forward declaration
class intArray;                    // Forward declaration

template<class T, int Template_Dimension> class SerialArray_Descriptor_Type;       // Forward declaration
class doubleSerialArray;           // Forward declaration
class floatSerialArray;            // Forward declaration
class intSerialArray;              // Forward declaration

class Where_Statement_Support;
class Expression_Tree_Node_Type;

#undef INLINE_FUNCTIONS

// Bug fix (8/11/94)
// Use absolute indexing rather than relative indexing
// We redefine the behavior of the A++/P++ Indexing so that views have a base
// associated with the base of the index used to build the view!  Previously
// A++/P++ defined the base of a view to be a fixed default value (usually ZERO)
// Previously the idexing was consistant with FORTRAN 90 but FORTRAN 90 was 
// locked into a bad choise because it had to be consistant with FORTRAN 77.
// This turned out to be a bad idea and now it is finally changed.  However this 
// HPF_INDEXING preprocessor macro can be used to switch between the two.
// define HPF_INDEXING TRUE
#define HPF_INDEXING FALSE

// The Sage++ tool is being used to build an A++ preprocessor which would allow
// us to address issues of efficency important for imporved performance (like FORTRAN or C).
#if USE_SAGE_PREPROCESSOR
// Need to turn these off since Sage++ can't process the code that these represent!
#define COMPILE_AGGREGATE_OPERATOR_OPTIMIZATIONS    FALSE
#define COMPILE_DEFERRED_DISPLAY_AND_VIEW_FUNCTIONS FALSE
#else
// The following should be false for use with the new Solaris C++ compiler!
// Also the RS6000 C++ compiler works only if these are set to FALSE.
// Otherwise the new Solaris compiler will segment fault in the lazy_statement.C file!
#define COMPILE_AGGREGATE_OPERATOR_OPTIMIZATIONS FALSE
// The following should be false for use with the new Solaris C++ compiler!
// the new compiler has a bug in the use of pointers to member functions!
#define COMPILE_DEFERRED_DISPLAY_AND_VIEW_FUNCTIONS FALSE
#endif

#undef COMPILE_DEFERRED_DISPLAY_AND_VIEW_FUNCTIONS
#define COMPILE_DEFERRED_DISPLAY_AND_VIEW_FUNCTIONS 0
 
// Used for the new operators in each class! Can't be a constant since then
// the constant could not be evaluated in the header files where static 
// arrays need to be declared in each class!
// define MAX_NUMBER_OF_MEMORY_BLOCKS 10000
// decreased in size to test problem with MSE error from purify
#define MAX_NUMBER_OF_MEMORY_BLOCKS 1000

// This is the values used in the getReferenceCountBase() member function
// for each of the A++/P++ objects.  It represents the value of the
// referenceCount of the a single reference to an object.  We will
// change this to the value = 1 instead of the current value = 0
// this will allow the rawDataReferenceCountArray to be initialized to
// zero when the cooresponding Array_ID (and it's array object)
// does not exist.  Then a value of one for those which do exist will
// indicate array objects in use.  These are counted by the A++/P++ diagnostics
// and can provide a mechanism for counting the number of arrays in use.
// Presently the value of ZERO inticates the initialization value AND
// the value for a simgle reference (a minor design error early on).
// Changed value (6/12/2000)
// define APP_REFERENCE_COUNT_BASE 0
#define APP_REFERENCE_COUNT_BASE 1

#include <dimension_macros.h>

// APP_DEBUG is a variable used to selectively turn debugging output ON and OFF at runtime 
// The used can turn it on an off within his application -- but the output is only meaingful 
// to the author (I suspect)
#ifdef MAIN_APP_PROGRAM
int APP_DEBUG = 0;

#ifdef GNU
// This allows the variables (virtual function tables especially) to be stored
// once rather than each time in every object file.

// Serial_A++ specific header files
// pragma implementation "A++.h"
// pragma implementation "A++_headers.h"
// pragma implementation "domain.h"
// pragma implementation "descriptor.h"
// pragma implementation "data_hash.h"
#pragma implementation "index.h"
#ifdef PXX_ENABLE_MP_INTERFACE_MPI
#pragma implementation "indirect_addressing.h"
#endif
#pragma implementation "inline_func.h"
#pragma implementation "inline_support.h"
#pragma implementation "lazy_aggregate.h"
// pragma implementation "lazy_expression.h"
// pragma implementation "lazy_operand.h"
// pragma implementation "lazy_statement.h"
#pragma implementation "lazy_task.h"
#pragma implementation "lazy_taskset.h"
// pragma implementation "lazyarray.h"
#pragma implementation "machine.h"
#pragma implementation "mdi_typedef.h"
#pragma implementation "optimization.h"
#pragma implementation "partitioning.h"
#pragma implementation "where.h"

// P++ specific header files
#pragma implementation "P++.h"
#pragma implementation "P++_headers.h"
#pragma implementation "arraylist.h"
#pragma implementation "comm_man.h"
#pragma implementation "conform_array_set.h"
#pragma implementation "conform_enforce.h"
#pragma implementation "parallel_io.h"
#pragma implementation "partlist.h"

#endif

#else
extern int APP_DEBUG;
#endif

// These are defined only when building the A++/P++ source code (not user applications)
#if defined(COMPILE_SERIAL_APP) || defined(COMPILE_PPP)

// The path to PADRE is defined in P++_headers.h
// if defined(USE_PADRE)
// include "../PADRE/src/PADRE.h"
// endif

#define INLINE_APP1_FUNCTIONS
// These are the include files that the A++/P++ source code requires!

// We can define the preprocessor variable on the command line of the makefile!
#ifdef _CM5_
#include "alias.h"
#endif

#include "mdi_typedef.h"
#include "data_hash.h"
#include "index.h"
#ifdef PXX_ENABLE_MP_INTERFACE_MPI
#include "indirect_addressing.h"
#endif
#include "domain.h"
#include "descriptor.h"
#include "lazy_expression.h"
#include "lazy_operand.h"

#include "lazy_task.h"
#include "lazy_taskset.h"
#include "lazy_statement.h"
#include "lazyarray.h"
#include "lazy_aggregate.h"
#include "optimization.h"

#else

// These are the only include files the user codes require!
// include "../BLOCK_PARTI/bsparti.h"
extern "C"
   {
// include "../BLOCK_PARTI/bsparti.h"
// include "../PADRE/PARTI/bsparti.h"
// include <../PADRE/PARTI/bsparti.h>
#include "bsparti.h"
   }

#include "mdi_typedef.h"
#include "data_hash.h"
#include "index.h"
#ifdef PXX_ENABLE_MP_INTERFACE_MPI
#include "indirect_addressing.h"
#endif
#include "domain.h"
#include "descriptor.h"
#include "lazy_taskset.h"
#include "optimization.h"
#endif

// Exit scope of initial header ifdef (this avoids errors if A++.h is included twice)
#endif  /* !defined(_APP_ARRAY_CLASS_LIBRARY_H) */


