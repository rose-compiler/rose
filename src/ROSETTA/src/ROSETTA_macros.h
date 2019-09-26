#ifndef __ROSETTAMACROS_H__
#define __ROSETTAMACROS_H__

// MK: These header files contains macro definitions to be used
// by the setDataPrototype() member function of class AstNodeClass

// define HL_GRAMMARS if you want all rules added (experimental) to allow X/Non_X-rules to be
// added to the abstract C++ grammar.
//#define HL_GRAMMARS


// DQ (1/20/2010): This allows compatability with the older STL list of SgFile pointers.
// The newer approach uses a pointer to a SgFileList IR node and this design permits
// the use of directory structure support as required for scalable code generation.
#define ROSE_USING_OLD_PROJECT_FILE_LIST_SUPPORT 0

// ifndef USE_ROSE
#ifndef ROSE_H
// We don't want the EDG front-end to see this (since it supports bool properly)
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
#elif defined(__sun)
    // PP (05/16/19)
    #define ROSE_ASSERT assert
    #define ROSE_ABORT  abort
#elif defined(ROSE_ASSERTION_BEHAVIOR)
    // ROSE_ASSERT should use Sawyer ASSERT macros which support various termination behaviors that are configurable at compile
    // time and can be overridden at runtime by the command-line or the Rose::Diagnostics API.  They also produce nicer output.
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

#define MAX_NUMBER_OF_TERMINALS    50
#define MAX_NUMBER_OF_NONTERMINALS 50

// Support for debugging a bug in Insure++
#define INSURE_BUG false

// Macro used to define terminals we want to object to be built on the heap so that
// it will not go out of scope, but we want a reference to the object instead of
// a pointer to the object to preserve the interface with existing code!
#define NEW_TERMINAL_MACRO(TERMINAL_NAME,TERMINAL_NAME_STRING,TAG_NAME_STRING)                \
     AstNodeClass & TERMINAL_NAME = terminalConstructor (TERMINAL_NAME_STRING, *this,             \
                                                     TERMINAL_NAME_STRING, TAG_NAME_STRING ); \
     ROSE_ASSERT (TERMINAL_NAME.associatedGrammar != NULL);


// A new nonterminal should not be born a parent of any child
#define NEW_NONTERMINAL_MACRO(NONTERMINAL_NAME, NONTERMINAL_EXPRESSION, NONTERMINAL_NAME_STRING, NONTERMINAL_TAG_STRING, NONTERMINAL_CAN_HAVE_INSTANCES) \
     AstNodeClass & NONTERMINAL_NAME = nonTerminalConstructor ( NONTERMINAL_NAME_STRING, *this, NONTERMINAL_NAME_STRING, NONTERMINAL_TAG_STRING, (SubclassListBuilder() | NONTERMINAL_EXPRESSION), NONTERMINAL_CAN_HAVE_INSTANCES ); \
     ROSE_ASSERT (NONTERMINAL_NAME.associatedGrammar != NULL);
     // /*printf ("%s ---> ",NONTERMINAL_NAME_STRING);*/ NONTERMINAL_NAME.show(); //printf ("\n"); //MS

#endif
