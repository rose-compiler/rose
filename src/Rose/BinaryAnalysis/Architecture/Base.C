#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>

#include <Rose/BinaryAnalysis/Architecture/Exception.h>
#include <Rose/BinaryAnalysis/CallingConvention.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/Modules.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/Unparser.h>
#include <Rose/StringUtility/Escape.h>

#include <sstream>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Base::Base(const std::string &name, size_t bytesPerWord, ByteOrder::Endianness byteOrder)
    : name_(name), bytesPerWord_(bytesPerWord), byteOrder_(byteOrder) {}

Base::~Base() {}

const std::string&
Base::name() const {
    return name_;
}

size_t
Base::bytesPerWord() const {
    return bytesPerWord_;
}

size_t
Base::bitsPerWord() const {
    return bytesPerWord() * 8;
}

ByteOrder::Endianness
Base::byteOrder() const {
    return byteOrder_;
}

bool
Base::matchesName(const std::string &name) const {
    return name_ == name;
}

bool
Base::matchesHeader(SgAsmGenericHeader*) const {
    return false;
}

InstructionSemantics::BaseSemantics::Dispatcher::Ptr
Base::newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperators::Ptr&) const {
    return {};
}

std::vector<Partitioner2::FunctionPrologueMatcher::Ptr>
Base::functionPrologueMatchers(const Partitioner2::EnginePtr&) const {
    return {};
}

} // namespace
} // namespace
} // namespace

#endif
