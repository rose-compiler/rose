#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

using namespace Rose;                                   // temporary until this lives in "rose"
using namespace Rose::BinaryAnalysis;

Sawyer::Optional<rose_addr_t>
SgAsmNullInstruction::branchTarget() {
    return Sawyer::Nothing();
}

Rose::BinaryAnalysis::AddressSet
SgAsmNullInstruction::getSuccessors(bool &complete) {
    complete = false;
    return AddressSet();
}

Rose::BinaryAnalysis::AddressSet
SgAsmNullInstruction::getSuccessors(const std::vector<SgAsmInstruction*>&, bool &complete, const MemoryMap::Ptr&) {
    complete = false;
    return AddressSet();
}

bool
SgAsmNullInstruction::isUnknown() const {
    return true;
}

unsigned
SgAsmNullInstruction::get_anyKind() const {
    return (unsigned)null_unknown;
}

#endif
