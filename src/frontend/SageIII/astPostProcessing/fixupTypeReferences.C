#include "sage3basic.h"
#include "fixupTypeReferences.h"

#include <rose_config.h>

using namespace std;

FixupTypeReferencesOnMemoryPool::~FixupTypeReferencesOnMemoryPool() {}

#ifdef ROSE_USE_CLANG_FRONTEND
// This function's implementation is moved to the edgRose.C file so that it
// can easily reference EDG types and the EDG/ROSE type map in its implementation.

void
FixupTypeReferencesOnMemoryPool::visit ( SgNode* node )
   {
     ROSE_ASSERT(node != NULL);
   }
#endif

void fixupTypeReferences()
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance fixupTypeReferences ("Reset type references:");

  // printf ("Inside of fixupTypeReferences() \n");

     FixupTypeReferencesOnMemoryPool t;

     SgModifierType::traverseMemoryPoolNodes(t);

  // printf ("DONE: Inside of fixupTypeReferences() \n");
   }
