// Definitions used when C/C++ frontend is not enabled.  Conditionally compiled in the Makefile
#include "sage3basic.h"
#include "fixupTypeReferences.h"

void FixupTypeReferencesOnMemoryPool::visit(SgNode*) {}
namespace EDG_ROSE_Translation {
  void clear_global_caches() {}
}

