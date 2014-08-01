#ifndef ROSE_BinaryAnalysis_Partitioner2_InstructionProvider_H
#define ROSE_BinaryAnalysis_Partitioner2_InstructionProvider_H

#include "sage3basic.h"
#include "Disassembler.h"

#include <sawyer/Assert.h>
#include <sawyer/Map.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Base class for creating instructions at addresses.
 *
 *  This class returns an instruction for a given address, caching the instruction so that the same instruction is returned
 *  each time the same address is specified.
 *
 *  If an instruction cannot be returned because it doesn't exist then a null pointer should be returned.  If an instruction
 *  cannot be disassembled at the specified address then an "unknown" instruction should be returned. */
class InstructionProvider {
public:
    virtual ~InstructionProvider() {}

    /** Returns the instruction at the specified virtual address, or null. */
    virtual SgAsmInstruction* operator[](rose_addr_t va) = 0;
};

/** An instruction providor that uses a disassembler.
 *
 *  This instruction provider uses the specified disassembler and memory map to obtain instructions. */
class DisassemblerProvider: public InstructionProvider {
public:
    typedef Sawyer::Container::Map<rose_addr_t, SgAsmInstruction*> InsnMap;
protected:
    Disassembler *disassembler_;
    MemoryMap memMap_;
    InsnMap insnMap_;
    
public:

    /** Construct from disassembler and map. */
    DisassemblerProvider(Disassembler *disassembler, const MemoryMap &map)
        : disassembler_(disassembler), memMap_(map) {
        ASSERT_not_null(disassembler);
    }

    // see super class
    virtual SgAsmInstruction *operator[](rose_addr_t va)/*override*/;
};
    

} // namespace
} // namespace
} // namespace

#endif
