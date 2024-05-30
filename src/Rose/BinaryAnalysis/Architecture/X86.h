#ifndef ROSE_BinaryAnalysis_Architecture_X86_H
#define ROSE_BinaryAnalysis_Architecture_X86_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Common base class for all x86 architectures. */
class X86: public Base {
public:
    using Ptr = X86Ptr;

protected:
    X86(const std::string &name, size_t bytesPerWord);
public:
    ~X86();

public:
    virtual const CallingConvention::Dictionary& callingConventions() const override;

    Sawyer::Container::Interval<size_t> bytesPerInstruction() const override;
    Alignment instructionAlignment() const override;
    bool terminatesBasicBlock(SgAsmInstruction*) const override;
    bool isUnknown(const SgAsmInstruction*) const override;
    bool isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t *target, rose_addr_t *ret) const override;
    bool isFunctionCallSlow(const std::vector<SgAsmInstruction*>&, rose_addr_t *target, rose_addr_t *ret) const override;
    bool isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) const override;
    Sawyer::Optional<rose_addr_t> branchTarget(SgAsmInstruction*) const override;
    AddressSet getSuccessors(SgAsmInstruction*, bool &complete) const override;
    AddressSet getSuccessors(const std::vector<SgAsmInstruction*> &basicBlock, bool &complete,
                             const MemoryMapPtr &initial_memory = MemoryMapPtr()) const override;

    Disassembler::BasePtr newInstructionDecoder() const override;
    Unparser::BasePtr newUnparser() const override;

    virtual InstructionSemantics::BaseSemantics::DispatcherPtr
    newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) const override;

    virtual std::vector<Partitioner2::FunctionPrologueMatcherPtr>
    functionPrologueMatchers(const Partitioner2::EnginePtr&) const override;

    virtual std::vector<Partitioner2::BasicBlockCallbackPtr>
    basicBlockCreationHooks(const Partitioner2::EnginePtr&) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Supporting functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
    // Create a new calling convention definition. The bitsPerWord is passed because, e.g., a 64-bit architecture can use a 32-bit
    // calling convention since x86 architectures are generally backward compatible.
    CallingConvention::Definition::Ptr cc_cdecl(size_t bitsPerWord) const;
    CallingConvention::Definition::Ptr cc_stdcall(size_t bitsPerWord) const;
    CallingConvention::Definition::Ptr cc_fastcall(size_t bitsPerWord) const;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
