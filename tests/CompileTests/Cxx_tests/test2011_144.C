
// Example of processing the implicit variable: __PRETTY_FUNCTION__: 
//    could not trace back to SgFunctionDefinition node from SgBasicBlock

#include<assert.h>

// Need to test all possible locations of "assert()" calls.
// also need to test use of "__func__", "__FUNCTION__", "__PRETTY_FUNCTION__"
// On (9/25/2011) the support for "__PRETTY_FUNCTION__" in the EDG translations
// was fixed to set the parent of the variable built to support "__PRETTY_FUNCTION__"
// to the SgFunctionDefinition.  It was previously set to the SgVarRef and this
// was a problem when that variable was replaced as a constant folded value.
// The better solution was to associate it with the enclosing function, but
// the support for traversing the scopes to the enclosing SgFunctionDefinition 
// was not robust, so we have to fix it to make it robust. This is a test code
// that demonstrated this type of error, we need to construct more!

void foo()
   {
     for (int i=0; i < 3; i++)
        {
          assert(i >= 0);
        }
   }
