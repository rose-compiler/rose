#include "sage3basic.h"
#include "InstructionProvider.h"

namespace rose {
namespace BinaryAnalysis {

SgAsmInstruction*
InstructionProvider::operator[](rose_addr_t va) const {
    SgAsmInstruction *insn = NULL;
    if (!insnMap_.getOptional(va).assignTo(insn)) {
        if (useDisassembler_ && memMap_.at(va).require(MemoryMap::EXECUTABLE).exists()) {
            try {
                insn = disassembler_->disassembleOne(&memMap_, va);
            } catch (const Disassembler::Exception &e) {
                insn = disassembler_->make_unknown_instruction(e);
                ASSERT_not_null(insn);
                uint8_t byte;
                if (1==memMap_.at(va).limit(1).require(MemoryMap::EXECUTABLE).read(&byte).size())
                    insn->set_raw_bytes(SgUnsignedCharList(1, byte));
                ASSERT_require(insn->get_address()==va);
                ASSERT_require(insn->get_size()==1);
            }
        }
        insnMap_.insert(va, insn);
    }
    return insn;
}

void
InstructionProvider::insert(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);
    insnMap_.insert(insn->get_address(), insn);
}

} // namespace
} // namespace
