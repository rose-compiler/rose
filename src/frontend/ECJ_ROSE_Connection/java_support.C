#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

// Support functions declaration of function defined in this file.
#include "java_support.h"

// DQ (10/21/2010): If Fortran is being supported then there will
// be a definition of this pointer there.  Note that we currently
// use only one pointer so that we can simplify how the JVM support 
// is used for either Fortran or Java language support.
#ifndef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
SgSourceFile* OpenFortranParser_globalFilePointer = NULL;
#endif




// Global stack of scopes
std::list<SgScopeStatement*> astJavaScopeStack;

// Global stack of expressions 
std::list<SgExpression*> astJavaExpressionStack;

// Simplifying type for the setSourcePosition() functions
// typedef std::vector<Token_t*> TokenListType;

// Global stack of IR nodes
std::list<SgNode*> astJavaNodeStack;

// Attribute spec for holding attributes
// std::list<int> astAttributeSpecStack;




bool emptyJavaStateStack()
   {
  // Use the scope stack to indicate if we have a value scope available as part of Java lanaguage processing.
     return astJavaScopeStack.empty();
   }


SgScopeStatement* getTopOfJavaScopeStack()
   {
     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     SgScopeStatement* topOfStack = astJavaScopeStack.front();

     return topOfStack;
   }

