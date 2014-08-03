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
            } catch (const Disassembler::Exception&) {
                // FIXME[Robb P. Matzke 2014-07-31]: This should never happen; we should be returning an "unknown" instruction
                // instead. Unfortunately there's not currently any way to create an unknown instruction without calling the
                // disassembler.
            }
        }
        insnMap_.insert(va, retval);
    }
    return retval;
}

} // namespace
} // namespace
