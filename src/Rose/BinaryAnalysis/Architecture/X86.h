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
    Disassembler::BasePtr newInstructionDecoder() const override;
    Unparser::BasePtr newUnparser() const override;

    virtual InstructionSemantics::BaseSemantics::DispatcherPtr
    newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) const override;

    std::vector<Partitioner2::FunctionPrologueMatcherPtr>
    functionPrologueMatchers(const Partitioner2::EnginePtr&) const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
