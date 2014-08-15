#include "InstructionProvider.h"

namespace rose {
namespace BinaryAnalysis {

SgAsmInstruction*
InstructionProvider::operator[](rose_addr_t va) const {
    SgAsmInstruction *insn = NULL;
    if (!insnMap_.getOptional(va).assignTo(insn)) {
        if (useDisassembler_ && memMap_.exists(va, MemoryMap::MM_PROT_EXEC)) {
            try {
                insn = disassembler_->disassembleOne(&memMap_, va);
            } catch (const Disassembler::Exception &e) {
                insn = disassembler_->make_unknown_instruction(e);
                ASSERT_not_null(insn);
                uint8_t byte;
                if (1==memMap_.read(&byte, va, 1, MemoryMap::MM_PROT_EXEC))
                    insn->set_raw_bytes(SgUnsignedCharList(1, byte));
                ASSERT_require(insn->get_address()==va);
                ASSERT_require(insn->get_size()==1);
            }
        }
        insnMap_.insert(va, insn);
    }
    return insn;
}

} // namespace
} // namespace
