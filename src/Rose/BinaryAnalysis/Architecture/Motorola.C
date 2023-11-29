#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Motorola.h>

#include <Rose/BinaryAnalysis/Disassembler/M68k.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherM68k.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesM68k.h>
#include <Rose/BinaryAnalysis/Unparser/M68k.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Motorola::Motorola(const std::string &name)
    : Base(name, 4, ByteOrder::ORDER_MSB) {}

Motorola::~Motorola() {}

Unparser::Base::Ptr
Motorola::newUnparser() const {
    return Unparser::M68k::instance(shared_from_this());
}

InstructionSemantics::BaseSemantics::DispatcherPtr
Motorola::newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr &ops) const {
    return InstructionSemantics::DispatcherM68k::instance(shared_from_this(), ops);
}

std::vector<Partitioner2::FunctionPrologueMatcher::Ptr>
Motorola::functionPrologueMatchers(const Partitioner2::EnginePtr&) const {
    std::vector<Partitioner2::FunctionPrologueMatcher::Ptr> retval;
    retval.push_back(Partitioner2::ModulesM68k::MatchLink::instance());
    return retval;
}

std::vector<Partitioner2::BasicBlockCallback::Ptr>
Motorola::basicBlockCreationHooks(const Partitioner2::EnginePtr&) const {
    std::vector<Partitioner2::BasicBlockCallback::Ptr> retval;
    retval.push_back(Partitioner2::ModulesM68k::SwitchSuccessors::instance());
    return retval;
}

} // namespace
} // namespace
} // namespace

#endif
