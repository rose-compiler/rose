#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <SgAsmJvmInstruction.h>

unsigned
SgAsmJvmInstruction::get_anyKind() const {
  return static_cast<unsigned>(p_kind);
}

#endif
