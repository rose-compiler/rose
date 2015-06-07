#ifndef ROSE_BinaryAnalysis_Partitioner2_InstructionProvider_H
#define ROSE_BinaryAnalysis_Partitioner2_InstructionProvider_H

#include "Disassembler.h"
#include "BaseSemantics2.h"

#include <Sawyer/Assert.h>
#include <Sawyer/Map.h>
#include <Sawyer/SharedPointer.h>

namespace rose {
namespace BinaryAnalysis {

/** Provides and caches instructions.
 *
 *  This class returns an instruction for a given address, caching the instruction so that the same instruction is returned
 *  each time the same address is specified.  If an instruction cannot be returned because it doesn't exist then a null pointer
 *  should be returned.  If an instruction cannot be disassembled at the specified address then an "unknown" instruction should
 *  be returned.
 *
 *  An instruction provider normally uses a supplied disassembler to obtain instructions that aren't in its cache.  However,
 *  the user can initialize the cache explicitly and turn off the ability to call a disassembler.  A disassembler is always
 *  required regardless of whether its used to obtain new instructions because the disassembler has the canonical information
 *  about the machine architecture: what registers are defined, which registers are the program counter and stack pointer,
 *  which instruction semantics dispatcher can be used with the instructions, etc. */
class InstructionProvider: public Sawyer::SharedObject {
public:
    typedef Sawyer::SharedPointer<InstructionProvider> Ptr;
    typedef Sawyer::Container::Map<rose_addr_t, SgAsmInstruction*> InsnMap;

private:
    Disassembler *disassembler_;
    MemoryMap memMap_;
    mutable InsnMap insnMap_;                           // this is a cache
    bool useDisassembler_;

protected:
    InstructionProvider(Disassembler *disassembler, const MemoryMap &map)
        : disassembler_(disassembler), memMap_(map), useDisassembler_(true) {
        ASSERT_not_null(disassembler);
    }

public:
    /** Static allocating Constructor.
     *
     *  The disassembler is required even if the user plans to turn off the ability to obtain instructions from the
     *  disassembler.  The memory map should be configured so that all segments that potentially contain instructions have
     *  execute permission.  Any readable/nonwritable segments will be considered to be constant for the life of the specimen.
     *  For instance, if a linking step has initialized the dynamic linking tables then those tables can be marked as readable
     *  and non-writable so that indirect jumps through the table will result in concrete execution addresses.
     *
     *  The disassembler is owned by the caller and should not be freed until after the instruction provider is destroyed.  The
     *  memory map is copied into the instruction provider. */
    static Ptr instance(Disassembler *disassembler, const MemoryMap &map) {
        return Ptr(new InstructionProvider(disassembler, map));
    }

    /** Enable or disable the disassembler.
     *
     *  When the disassembler is disabled then it is not called when a new instruction is needed, but rather a null instruction
     *  pointer is returned (and cached).
     *
     * @{ */
    bool isDisassemblerEnabled() const { return useDisassembler_; }
    void enableDisassembler(bool enable=true) { useDisassembler_ = enable; }
    void disableDisassembler() { useDisassembler_ = false; }
    /** @} */

    /** Returns the instruction at the specified virtual address, or null.
     *
     *  If the virtual address is non-executable then a null pointer is returned, otherwise either a valid instruction or an
     *  "unknown" instruction is returned.  An "unknown" instruction is used for cases where a valid instruction could not be
     *  disassembled, including the case when the first byte of a multi-byte instruction is executable but the remaining bytes
     *  are not executable. */
    SgAsmInstruction* operator[](rose_addr_t va) const;

    /** Insert an instruction into the cache.
     *
     *  This instruction provider saves a pointer to the instruction without taking ownership.  If an instruction already
     *  exists at the new instruction's address then the new instruction replaces the old instruction. */
    void insert(SgAsmInstruction*);

    /** Returns the disassembler.
     *
     *  Returns the disassembler pointer provided in the constructor.  The disassembler is not owned by this instruction
     *  provider, but must not be freed until after the instruction provider is destroyed. */
    Disassembler* disassembler() const { return disassembler_; }

    /** Returns number of cached starting addresses.
     *
     *  The number of cached starting addresses includes those addresses where an instruction exists, and those addresses where
     *  an instruction is known to not exist.
     *
     *  This is a constant-time operation. */
    size_t nCached() const { return insnMap_.size(); }

    /** Returns the register dictionary. */
    const RegisterDictionary* registerDictionary() const { return disassembler_->get_registers(); }

    /** Register used as the instruction pointer. */
    RegisterDescriptor instructionPointerRegister() const { return disassembler_->instructionPointerRegister(); }

    /** Register used as a user-mode stack pointer. */
    RegisterDescriptor stackPointerRegister() const { return disassembler_->stackPointerRegister(); }

    /** Register used as a segment to access stack memory.
     *
     *  Not all architectures have such a register, in which case a default-constructed register descriptor is returned. */
    RegisterDescriptor stackSegmentRegister() const { return disassembler_->stackSegmentRegister(); }

    /** Default memory byte order. */
    ByteOrder::Endianness defaultByteOrder() const { return disassembler_->get_sex(); }

    /** Instruction dispatcher.
     *
     *  Returns a pointer to a dispatcher used for instruction semantics.  Not all architectures support instruction semantics,
     *  in which case a null pointer is returned.  The returned dispatcher is not connected to any semantic domain, so it can
     *  only be used to call its virtual constructor to create a valid dispatcher. */
    InstructionSemantics2::BaseSemantics::DispatcherPtr dispatcher() const { return disassembler_->dispatcher(); }
};

} // namespace
} // namespace

#endif
