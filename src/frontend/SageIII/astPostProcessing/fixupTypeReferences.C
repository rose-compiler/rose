#include "sage3basic.h"
#include "fixupTypeReferences.h"

#include <rose_config.h>

using namespace std;

FixupTypeReferencesOnMemoryPool::~FixupTypeReferencesOnMemoryPool() {}

void fixupTypeReferences()
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance fixupTypeReferences ("Reset type references:");

  // printf ("Inside of fixupTypeReferences() \n");

     FixupTypeReferencesOnMemoryPool t;

     SgModifierType::traverseMemoryPoolNodes(t);

  // printf ("DONE: Inside of fixupTypeReferences() \n");
   }
