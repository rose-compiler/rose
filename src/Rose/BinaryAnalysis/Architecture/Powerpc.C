#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Powerpc.h>

#include <Rose/BinaryAnalysis/Disassembler/Powerpc.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherPowerpc.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesPowerpc.h>
#include <Rose/BinaryAnalysis/Unparser/Powerpc.h>

#include <boost/lexical_cast.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Powerpc::Powerpc(size_t bytesPerWord, ByteOrder::Endianness byteOrder)
    : Base("ppc" + boost::lexical_cast<std::string>(8*bytesPerWord) +
           (ByteOrder::ORDER_MSB == byteOrder ? "-be" : (ByteOrder::ORDER_LSB == byteOrder ? "-el" : "")),
           bytesPerWord, byteOrder) {}

Powerpc::~Powerpc() {}

Disassembler::Base::Ptr
Powerpc::newInstructionDecoder() const {
    return Disassembler::Powerpc::instance(shared_from_this());
}

Unparser::Base::Ptr
Powerpc::newUnparser() const {
    return Unparser::Powerpc::instance(shared_from_this());
}

InstructionSemantics::BaseSemantics::DispatcherPtr
Powerpc::newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr &ops) const {
    return InstructionSemantics::DispatcherPowerpc::instance(shared_from_this(), ops);
}

std::vector<Partitioner2::FunctionPrologueMatcher::Ptr>
Powerpc::functionPrologueMatchers(const Partitioner2::EnginePtr&) const {
    std::vector<Partitioner2::FunctionPrologueMatcher::Ptr> retval;
    retval.push_back(Partitioner2::ModulesPowerpc::MatchStwuPrologue::instance());
    return retval;
}

} // namespace
} // namespace
} // namespace

#endif
