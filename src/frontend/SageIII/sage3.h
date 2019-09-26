// DQ (6/25/2011): remove to isolate name qualification error
#if 1

/*
 * this includes the forward declarations of all the sage node classes
 * from the generated files (i.e. gives just the class names.)
 *
 */

#ifndef SAGE3_CLASSES_H
#define SAGE3_CLASSES_H

// DQ (1/20/2010): This allows compatability with the older STL list of SgFile pointers.
// The newer approach uses a pointer to a SgFileList IR node and this design permits
// the use of directory structure support as required for scalable code generation.
#define ROSE_USING_OLD_PROJECT_FILE_LIST_SUPPORT 0

//#include "sage3basic.h"

// DQ (12/9/2004): The name of this file has been changed to be the new location
// of many future Sage III AST manipulation functions in the future.  A namespace
// (SageInterface) is defined in sageInterface.h.
#include "sageInterface.h"

#include "AstProcessing.h"

// Markus Kowarschik: Support for preprocessors declarations and comments
#include "attachPreprocessingInfo.h"

// Lingxiao's work to add comments from all header files to the AST.
#include "attach_all_info.h"

// DQ (8/20/2005): Changed name to make sure that we don't use the old 
// header file (which has been removed).
// #include "AstFixes.h"
#include "astPostProcessing.h"


#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
//Liao, 10/9/2008, support for abstract handles for language constructs
#include "abstract_handle.h"
#include "roseAdapter.h"

#include "memory_object.h"
//#include "memory_object_impl.h"
#endif

//Liao, 2/8/2008. SAGE III node building interface
#include "sageBuilder.h"


#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
// Liao, 11/14/2008, support for annotation-based array optimization
// The headers bring some garbage which cannot be compiled
//#include "ArrayAnnot.h"
//#include "ArrayInterface.h"
//include "ArrayRewrite.h"
// Liao, 8/11/2009, support for OpenMP lowering
#include "omp_lowering.h" 
#else
// DQ (11/12/2011): This is included in the omp_lowering.h and it is needed in sageInterface.C.
// #include "astQuery.h"
#include "nodeQuery.h"
#endif


#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
// DQ (5/28/2007): Added new AST Merge API
#include "astMergeAPI.h"
#endif

// JJW 10-23-2007
// Add possibility to include Valgrind header for memcheck
#if ROSE_USE_VALGRIND
#include <valgrind/valgrind.h>
#include <valgrind/memcheck.h>
#endif

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
// TV (05/23/2011): Add headers for the AstFromString library
#include "AstFromString.h"
#include "ParserBuilder.hpp"
#endif

#endif

// DQ (6/25/2011): remove to isolate name qualification error
#endif













