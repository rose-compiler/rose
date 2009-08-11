#include <rose.h>
#include <string>
#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;


void 
RtedTransformation::visit_isSgScopeStatement( SgNode* n) {
    SgScopeStatement* stmt = isSgScopeStatement( n);
    if( stmt && (
          isSgWhileStmt( n)
          || isSgSwitchStatement( n)
          || isSgIfStmt( n)
          || isSgForStatement( n)
          || isSgDoWhileStmt( n)
        )
    ) {
      ROSE_ASSERT( stmt);
      scopes[ stmt ] = n;
    }
}
