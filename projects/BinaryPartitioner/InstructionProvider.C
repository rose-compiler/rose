#include "InstructionProvider.h"

namespace rose {
namespace BinaryAnalysis {

SgAsmInstruction*
InstructionProvider::operator[](rose_addr_t va) {
    SgAsmInstruction *retval = NULL;
    if (!insnMap_.getOptional(va).assignTo(retval)) {
        if (useDisassembler_ && memMap_.exists(va, MemoryMap::MM_PROT_EXEC)) {
            try {
                retval = disassembler_->disassembleOne(&memMap_, va);
            } catch (const Disassembler::Exception &e) {
                retval = disassembler_->make_unknown_instruction(e);
            }
        }
        insnMap_.insert(va, retval);
    }
    return retval;
}

} // namespace
} // namespace
