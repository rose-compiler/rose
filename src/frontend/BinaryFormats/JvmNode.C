/* SgAsmJvmNode base class functions.  */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

// This function shouldn't be reached (ROSETTA won't allow pure virtual functions)
void SgAsmJvmNode::dump(FILE* f, const char* prefix, ssize_t idx) const
{
  fprintf(f, "SgAsmJvmNode::dump: should be a pure virtual function\n");
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
