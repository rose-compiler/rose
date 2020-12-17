// SgAsmAarch32Instructoin member definitions. Do not move these to src/ROSETTA/Grammar/BinaryInstruction.code (or any other
// *.code file) because then they won't be processed as C++ by IDEs.
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32
#include <sage3basic.h>

#include <Disassembler.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;

unsigned
SgAsmAarch32Instruction::get_anyKind() const {
    return p_kind;
}

bool
SgAsmAarch32Instruction::isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t *target,
                                 rose_addr_t *return_va) {
    ASSERT_not_implemented("[Robb Matzke 2020-12-22]");
}

bool
SgAsmAarch32Instruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*>&, rose_addr_t *target,
                                 rose_addr_t *return_va) {
    ASSERT_not_implemented("[Robb Matzke 2020-12-22]");
}

bool
SgAsmAarch32Instruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) {
    ASSERT_not_implemented("[Robb Matzke 2020-12-22]");
}

bool
SgAsmAarch32Instruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&) {
    ASSERT_not_implemented("[Robb Matzke 2020-12-22]");
}

AddressSet
SgAsmAarch32Instruction::getSuccessors(bool &complete) {
    ASSERT_not_implemented("[Robb Matzke 2020-12-22]");
}

bool
SgAsmAarch32Instruction::getBranchTarget(rose_addr_t *target) {
    ASSERT_not_implemented("[Robb Matzke 2020-12-22]");
}

bool
SgAsmAarch32Instruction::terminatesBasicBlock() {
    ASSERT_not_implemented("[Robb Matzke 2020-12-22]");
}

bool
SgAsmAarch32Instruction::isUnknown() const {
    return Aarch32InstructionKind::ARM_INS_INVALID == get_kind();
}

std::string
SgAsmAarch32Instruction::description() const {
    ASSERT_not_implemented("[Robb Matzke 2020-12-22]");
}

#endif
