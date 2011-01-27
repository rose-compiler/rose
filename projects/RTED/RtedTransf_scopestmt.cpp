#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;


void
RtedTransformation::visit_isSgScopeStatement( SgScopeStatement* const n)
{
    ROSE_ASSERT(n);

    if(   isSgWhileStmt( n)
          || isSgSwitchStatement( n)
          || isSgIfStmt( n)
          || isSgForStatement( n)
          || isSgUpcForAllStatement( n)
          || isSgDoWhileStmt( n)
                // if we have a basic block in another basic block, then it
                // won't be covered by switch, function body, etc., but it does
                // introduce a new scope, as in the following case:
                //      void foo() {
                //          int x;
                //          {
                //              float x;
                //          }
                //      }
          ||    ( isSgBasicBlock( n ) && isSgBasicBlock( n -> get_parent() ))
      )
    {
      scopes[ n ] = n;
    }
}

#endif
