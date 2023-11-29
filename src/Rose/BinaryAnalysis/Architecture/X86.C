#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/X86.h>

#include <Rose/BinaryAnalysis/Disassembler/X86.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/Partitioner2/EngineBinary.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesX86.h>
#include <Rose/BinaryAnalysis/Unparser/X86.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

X86::X86(const std::string &name, size_t bytesPerWord)
    : Base(name, bytesPerWord, ByteOrder::ORDER_LSB) {}

X86::~X86() {}

Disassembler::Base::Ptr
X86::newInstructionDecoder() const {
    return Disassembler::X86::instance(shared_from_this());
}

Unparser::Base::Ptr
X86::newUnparser() const {
    return Unparser::X86::instance(shared_from_this());
}

InstructionSemantics::BaseSemantics::DispatcherPtr
X86::newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr &ops) const {
    return InstructionSemantics::DispatcherX86::instance(shared_from_this(), ops);
}

std::vector<Partitioner2::FunctionPrologueMatcher::Ptr>
X86::functionPrologueMatchers(const Partitioner2::Engine::Ptr &engine) const {
    std::vector<Partitioner2::FunctionPrologueMatcher::Ptr> retval;
    retval.push_back(Partitioner2::ModulesX86::MatchHotPatchPrologue::instance());
    retval.push_back(Partitioner2::ModulesX86::MatchStandardPrologue::instance());
    retval.push_back(Partitioner2::ModulesX86::MatchAbbreviatedPrologue::instance());
    retval.push_back(Partitioner2::ModulesX86::MatchEnterPrologue::instance());
    if (engine->settings().partitioner.findingThunks) {
        if (auto engineBinary = engine.dynamicCast<Partitioner2::EngineBinary>())
            retval.push_back(Partitioner2::Modules::MatchThunk::instance(engineBinary->functionMatcherThunks()));
    }
    retval.push_back(Partitioner2::ModulesX86::MatchRetPadPush::instance());
    return retval;
}

std::vector<Partitioner2::BasicBlockCallback::Ptr>
X86::basicBlockCreationHooks(const Partitioner2::Engine::Ptr&) const {
    std::vector<Partitioner2::BasicBlockCallback::Ptr> retval;
    retval.push_back(Partitioner2::ModulesX86::FunctionReturnDetector::instance());
    retval.push_back(Partitioner2::ModulesX86::SwitchSuccessors::instance());
    return retval;
}

} // namespace
} // namespace
} // namespace

#endif
