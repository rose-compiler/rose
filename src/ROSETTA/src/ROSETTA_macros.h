#ifndef __ROSETTAMACROS_H__
#define __ROSETTAMACROS_H__

// DQ (11/10/2007): This is a redundant included header file.
// #include <rose_config.h>

// MK: This header files contains macro definitions to be used
// by the setDataPrototype() member function of class Terminal

// define HL_GRAMMARS if you want all rules added (experimental) to allow X/Non_X-rules to be
// added to the abstract C++ grammar.
//#define HL_GRAMMARS

//QY 11/9/04 added capabilities for building multiple constructors for statements including
//ExpressionRoot (and others) as data members
typedef enum { NO_CONSTRUCTOR_PARAMETER       = 0, 
               INDIRECT_CONSTRUCTOR_PARAMETER = 1,  // member wrapped inside ExpressionRoot
               WRAP_CONSTRUCTOR_PARAMETER     = 2,  // the wrapper member with type SgExpressionRoot
               CONSTRUCTOR_PARAMETER          = 3} 
ConstructParamEnum;

typedef enum { NO_ACCESS_FUNCTIONS, BUILD_ACCESS_FUNCTIONS, BUILD_LIST_ACCESS_FUNCTIONS,
               BUILD_INDIRECT_ACCESS_FUNCTIONS, BUILD_WRAP_ACCESS_FUNCTIONS}
 BuildAccessEnum;


typedef enum
   {
  // Note that CLONE_TREE is the default is nothing is specified in the setDataPrototype() member function.
     NO_COPY_DATA, // This skips the generation of any code to copy the 
                   // pointer (deep or shallow)
     COPY_DATA,    // This copies the data (if a pointer this copies the 
                   // pointer, else calls the operator= for any object)
     CLONE_PTR,    // This copies the data by building a new object using 
                   // the copy constructor
     CLONE_TREE    // This builds a new object dependent on the use of the 
                   // SgCopyHelp input object (deep or shallow) using the object's copy member function
   } CopyConfigEnum; 

#define DEF_TRAVERSAL TRUE // default traversal
#define NO_TRAVERSAL FALSE // no traversal

// AJ (10/26/2004)
#define NO_DELETE FALSE
// DQ (12/3/2004): Avoid calling delete
#define DEF_DELETE TRUE
// #define DEF_DELETE FALSE

// DQ (7/18/2004): Comment out so that we can link when it is 
// turned on to the definition of DEF2TYPE_TRAVERSAL
// #define TYPE_TRAVERSAL TRUE // traversal within types

#if 1
// DQ (7/18/2004): Turn this on by default so that nested traversals 
// on types are possible this does not change the default behavior.
#define TYPE_TRAVERSAL TRUE // traversal within types

// DEF2TYPE_TRAVERSAL must be defined as FALSE in any release of ROSE!

// This is the usual setting (traversal excludes types).  Users cannot apply transformations
// to SgType objects since these objects are shared within the AST.
   #define DEF2TYPE_TRAVERSAL FALSE // traversal (paths) from default traversal to type traversal
#else
// Debugging setting (helpful under specialized circumstances, but sometimes problematic).
// This is a setting that permits us to traverse the types, but it can cause problems
// (the ROSE/TESTS/CompileTests/OvertureCode/overtureBug_01.C fails to attach comments
// at the correct location if the types are traversed (not clear if this is important to debug).
// This setting is reserved for internal debugging only, misleading resuts can occure in the generation
// of PDF output of the AST.
   #define DEF2TYPE_TRAVERSAL TRUE // traversal (paths) from default traversal to type traversal

// DQ (7/18/2004): link this definition to that of DEF2TYPE_TRAVERSAL
   #define TYPE_TRAVERSAL FALSE // traversal within types
#endif

// ifndef USE_ROSE
#ifndef ROSE_H
// We don't want the EDG front-end to see this (since it support bool properly)
// typedef int Boolean;
#endif

// added preprocessor code #ifndef .... BP : 10/16/2001
// We always want to use explicit template instantiation
#ifndef HAVE_EXPLICIT_TEMPLATE_INSTANTIATION   
#define HAVE_EXPLICIT_TEMPLATE_INSTANTIATION
#endif

#define ROSE_ASSERT assert
#define ROSE_ABORT  abort

#define TRUE  1
#define FALSE 0

#define MAX_NUMBER_OF_TERMINALS    50
#define MAX_NUMBER_OF_NONTERMINALS 50

// Support for debugging a bug in Insure++
#define INSURE_BUG FALSE

#include <list>
// #ifndef STL_LIST_IS_BROKEN
// #include STL_LIST_HEADER_FILE
// #endif

// using namespace std;

#endif
