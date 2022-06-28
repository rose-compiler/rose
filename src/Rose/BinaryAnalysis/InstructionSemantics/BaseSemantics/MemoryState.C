#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryState.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Exception.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Formatter.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Merger.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

std::ostream&
operator<<(std::ostream &o, const MemoryState &x) {
    x.print(o);
    return o;
}

std::ostream&
operator<<(std::ostream &o, const MemoryState::WithFormatter &x) {
    x.print(o);
    return o;
}

MemoryState::MemoryState()
    : byteOrder_(ByteOrder::ORDER_UNSPECIFIED), byteRestricted_(true) {}

MemoryState::MemoryState(const SValuePtr &addrProtoval, const SValuePtr &valProtoval)
    : addrProtoval_(addrProtoval), valProtoval_(valProtoval), byteOrder_(ByteOrder::ORDER_UNSPECIFIED), byteRestricted_(true) {
    ASSERT_not_null(addrProtoval);
    ASSERT_not_null(valProtoval);
}

MemoryState::MemoryState(const MemoryStatePtr &other)
    : addrProtoval_(other->addrProtoval_), valProtoval_(other->valProtoval_), byteOrder_(ByteOrder::ORDER_UNSPECIFIED),
      merger_(other->merger_), byteRestricted_(other->byteRestricted_) {}

MemoryState::~MemoryState() {}

MergerPtr
MemoryState::merger() const {
    return merger_;
}

void
MemoryState::merger(const MergerPtr &m) {
    merger_ = m;
}

void
MemoryState::print(std::ostream &stream, const std::string prefix) const {
    Formatter fmt;
    fmt.set_line_prefix(prefix);
    print(stream, fmt);
}

MemoryState::WithFormatter
MemoryState::operator+(const std::string &linePrefix) {
    static Formatter fmt;
    fmt.set_line_prefix(linePrefix);
    return with_format(fmt);
}
    
} // namespace
} // namespace
} // namespace
} // namespace

BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::MemoryState);

#endif
