#ifndef __ROSETTAMACROS_H__
#define __ROSETTAMACROS_H__

// DQ (11/10/2007): This is a redundant included header file.
// #include <rose_config.h>

// MK: This header files contains macro definitions to be used
// by the setDataPrototype() member function of class Terminal

// define HL_GRAMMARS if you want all rules added (experimental) to allow X/Non_X-rules to be
// added to the abstract C++ grammar.
//#define HL_GRAMMARS


// DQ (1/20/2010): This allows compatability with the older STL list of SgFile pointers.
// The newer approach uses a pointer to a SgFileList IR node and this design permits
// the use of directory structure support as required for scalable code generation.
#define ROSE_USING_OLD_PROJECT_FILE_LIST_SUPPORT 0

// MS 2015: rewrote ROSETTA to use enums instead of wrapper classes around enums.
// enums: ConstructParamEnum, BuildAccessEnum, CopyConfigEnum
enum ConstructParamEnum { 
  NO_CONSTRUCTOR_PARAMETER , 
  CONSTRUCTOR_PARAMETER    
};

enum BuildAccessEnum { 
  NO_ACCESS_FUNCTIONS,
  BUILD_ACCESS_FUNCTIONS,
  // Just like TAG_BUILD_ACCESS_FUNCTIONS except doesn't set p_isModified
  BUILD_FLAG_ACCESS_FUNCTIONS,
  BUILD_LIST_ACCESS_FUNCTIONS
};

enum CopyConfigEnum {
  // Note that CLONE_TREE is the default if nothing is specified in the setDataPrototype() member function.

  /* This skips the generation of any code to copy the 
     pointer (deep or shallow)
  */
  NO_COPY_DATA,
  /* This copies the data (if a pointer this copies the 
     pointer, else calls the operator= for any object)
  */
  COPY_DATA,
  /* This copies the data by building a new object using 
     the copy constructor
  */
  CLONE_PTR,
  /* This builds a new object dependent on the use of the 
     SgCopyHelp input object (deep or shallow) using the object's copy member function
  */
  CLONE_TREE    
};

class TraversalFlag { // Wrapper for extra argument type checking
  bool value;
  public:
  explicit TraversalFlag(bool value): value(value) {}
  bool getValue() const {return value;}
  bool operator==(const TraversalFlag& o) const {return value == o.value;}
  bool operator!=(const TraversalFlag& o) const {return value != o.value;}
  TraversalFlag operator||(const TraversalFlag& o) const {return TraversalFlag(value || o.value);} // For TYPE_TRAVERSAL
};

class DeleteFlag { // Wrapper for extra argument type checking
  bool value;
  public:
  explicit DeleteFlag(bool value): value(value) {}
  bool getValue() const {return value;}
  bool operator==(const DeleteFlag& o) const {return value == o.value;}
  bool operator!=(const DeleteFlag& o) const {return value != o.value;}
};

// DQ (7/18/2004): Comment out so that we can link when it is 
// turned on to the definition of DEF2TYPE_TRAVERSAL
// #define TYPE_TRAVERSAL true // traversal within types

#if 1
// DQ (7/18/2004): Turn this on by default so that nested traversals 
// on types are possible this does not change the default behavior.
#define TYPE_TRAVERSAL DEF_TRAVERSAL // traversal within types

// DEF2TYPE_TRAVERSAL must be defined as false in any release of ROSE!

// This is the usual setting (traversal excludes types).  Users cannot apply transformations
// to SgType objects since these objects are shared within the AST.
#define DEF2TYPE_TRAVERSAL NO_TRAVERSAL // traversal (paths) from default traversal to type traversal
#else
// Debugging setting (helpful under specialized circumstances, but sometimes problematic).
// This is a setting that permits us to traverse the types, but it can cause problems
// (the ROSE/TESTS/CompileTests/OvertureCode/overtureBug_01.C fails to attach comments
// at the correct location if the types are traversed (not clear if this is important to debug).
// This setting is reserved for internal debugging only, misleading resuts can occure in the generation
// of PDF output of the AST.
#define DEF2TYPE_TRAVERSAL DEF_TRAVERSAL; // traversal (paths) from default traversal to type traversal

// DQ (7/18/2004): link this definition to that of DEF2TYPE_TRAVERSAL
#define TYPE_TRAVERSAL NO_TRAVERSAL // traversal within types
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

#ifdef _MSC_VER
    // DQ (11/28/2009): This is a problem for MSVC ("warning C4273: 'abort' : inconsistent dll linkage").
    //#define __builtin_constant_p(exp) (0)
    #define ROSE_ASSERT assert
#elif defined(__APPLE__) && defined(__MACH__)
    // Pei-Hung (06/16/2015) Sawyer is turned off for Mac OSX
    #define ROSE_ASSERT assert
    #define ROSE_ABORT  abort
#elif defined(ROSE_ASSERTION_BEHAVIOR)
    // ROSE_ASSERT should use Sawyer ASSERT macros which support various termination behaviors that are configurable at compile
    // time and can be overridden at runtime by the command-line or the rose::Diagnostics API.  They also produce nicer output.
#ifdef __GNUC__
    // Pei-Hung (6/16/2015): Using Sawyer ASSERT will consume more than 4GB memory when building ROSE with 32-bit GCC in version 4.2.4
    // If building ROSE with GCC older than version 4.4, turn off support for Sawyer assert.
  #include <features.h>
  #if __GNUC_PREREQ(4,4)
      #define ROSE_ASSERT ASSERT_require
      #define ROSE_ABORT  abort
  #else
      #define ROSE_ASSERT assert
      #define ROSE_ABORT  abort
  #endif
#endif
#else
    #define ROSE_ASSERT assert
    #define ROSE_ABORT  abort
#endif

// JJW (8/26/2008): Removing these
// #define TRUE  1
// #define FALSE 0

#define MAX_NUMBER_OF_TERMINALS    50
#define MAX_NUMBER_OF_NONTERMINALS 50

// Support for debugging a bug in Insure++
#define INSURE_BUG false

#include <vector>
// #ifndef STL_LIST_IS_BROKEN
// #include STL_LIST_HEADER_FILE
// #endif

// using namespace std;

extern const TraversalFlag DEF_TRAVERSAL; // default traversal
extern const TraversalFlag NO_TRAVERSAL; // no traversal

// AJ (10/26/2004)
extern const DeleteFlag NO_DELETE;
// DQ (12/3/2004): Avoid calling delete
extern const DeleteFlag DEF_DELETE;
// DeleteFlag DEF_DELETE(false);

#endif
