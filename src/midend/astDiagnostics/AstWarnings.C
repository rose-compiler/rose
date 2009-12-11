// tps (12/09/2009) : Playing with precompiled headers in Windows. Requires rose.h as the first line in source files.
#ifdef _MSC_VER
// seems to cause problems under Linux
#include "rose.h"
#endif

// DQ (3/6/2003): added from AstProcessing.h to avoid referencing
// the traversal classes in AstFixes.h before they are defined.
#include "sage3.h"
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
AstWarnings::visit ( SgNode* node )
   {
   }

