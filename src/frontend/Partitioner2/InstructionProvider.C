#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"
#include "InstructionProvider.h"

namespace Rose {
namespace BinaryAnalysis {

SgAsmInstruction*
InstructionProvider::operator[](rose_addr_t va) const {
    SgAsmInstruction *insn = NULL;
    if (!insnMap_.getOptional(va).assignTo(insn)) {
        if (useDisassembler_ && memMap_->at(va).require(MemoryMap::EXECUTABLE).exists()) {
            try {
                insn = disassembler_->disassembleOne(memMap_, va);
            } catch (const Disassembler::Exception &e) {
                insn = disassembler_->makeUnknownInstruction(e);
                ASSERT_not_null(insn);
                ASSERT_require(insn->get_address()==va);
                if (0 == insn->get_size()) {
                    uint8_t byte;
                    if (1==memMap_->at(va).limit(1).require(MemoryMap::EXECUTABLE).read(&byte).size())
                        insn->set_raw_bytes(SgUnsignedCharList(1, byte));
                    ASSERT_require(insn->get_size()==1);
                }
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

void
InstructionProvider::showStatistics() const {
    std::cout <<"Rose::BinaryAnalysis::InstructionProvider statistics:\n";
    std::cout <<"  instruction map:\n";
    std::cout <<"    size = " <<insnMap_.size() <<"\n";
    std::cout <<"    number of hash buckets = " <<insnMap_.nBuckets() <<"\n";
    std::cout <<"    load factor = " <<insnMap_.loadFactor() <<"\n";
}

} // namespace
} // namespace

#endif
