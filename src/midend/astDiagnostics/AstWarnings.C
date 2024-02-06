

// DQ (3/6/2003): added from AstProcessing.h to avoid referencing
// the traversal classes in AstFixes.h before they are defined.
#include "sage3basic.h"
#include "roseInternal.h"

#include <sstream>
#include "AstWarnings.h"

//  NodeStatistics Constructors/Destructors
AstWarnings::AstWarnings()
   {
   }

AstWarnings::~AstWarnings()
   { 
   }

void
AstWarnings::visit ( SgNode* )
   {
   }

