#ifndef ROSE_BinaryAnalysis_Architecture_Motorola_H
#define ROSE_BinaryAnalysis_Architecture_Motorola_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Base class for Motorola processors. */
class Motorola: public Base {
public:
    using Ptr = MotorolaPtr;

protected:
    explicit Motorola(const std::string &name);
public:
    ~Motorola();

public:
    Unparser::BasePtr newUnparser() const override;

    virtual InstructionSemantics::BaseSemantics::DispatcherPtr
    newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) const override;

    std::vector<Partitioner2::FunctionPrologueMatcherPtr>
    functionPrologueMatchers(const Partitioner2::EnginePtr&) const override;

    virtual std::vector<Partitioner2::BasicBlockCallbackPtr>
    basicBlockCreationHooks(const Partitioner2::EnginePtr&) const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
