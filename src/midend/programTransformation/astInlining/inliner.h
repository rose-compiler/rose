#ifndef INLINER_H
#define INLINER_H

// DQ (3/12/2006): This is now not required, the file name is 
// changed to rose_config.h and is included directly by rose.h.
// #include "config.h"


#include "rewrite.h"
#include "replaceExpressionWithStatement.h"
#include "inlinerSupport.h"

//! Main inliner code.  Accepts a function call as a parameter, and inlines
//! only that single function call.  Returns true if it succeeded, and false
//! otherwise.  The function call must be to a named function, static member
//! function, or non-virtual non-static member function, and the function
//! must be known (not through a function pointer or member function
//! pointer).  Also, the body of the function must already be visible.
//! Recursive procedures are handled properly (when allowRecursion is set), by
//! inlining one copy of the procedure into itself.  Any other restrictions on
//! what can be inlined are bugs in the inliner code.
ROSE_DLL_API bool doInline(SgFunctionCallExp* funcall, bool allowRecursion = false);


// a namespace
namespace Inliner {
  // if set to true, ignore function calls within headers. Default is false. 
  extern bool skipHeaders;   
  extern bool verbose; // if set to true, generate debugging information   
}

#endif // INLINER_H
