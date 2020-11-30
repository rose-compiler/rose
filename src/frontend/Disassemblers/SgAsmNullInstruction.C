#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

using namespace Rose;                                   // temporary until this lives in "rose"
using namespace Rose::BinaryAnalysis;

bool
SgAsmNullInstruction::terminatesBasicBlock() {
    return true;
}

bool
SgAsmNullInstruction::isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t* /*target,inout*/,
                                         rose_addr_t*/*ret,inout*/) {
    return false;
}

bool
SgAsmNullInstruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*>&, rose_addr_t* /*target,inout*/,
                                         rose_addr_t*/*ret,inout*/) {
    return false;
}

bool
SgAsmNullInstruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) {
    return false;
}

bool
SgAsmNullInstruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&) {
    return false;
}

bool
SgAsmNullInstruction::getBranchTarget(rose_addr_t*) {
    return false;
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
