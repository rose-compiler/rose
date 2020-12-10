#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <BaseSemanticsMemoryState.h>

#include <BaseSemanticsFormatter.h>
#include <BaseSemanticsMerger.h>
#include <BaseSemanticsSValue.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
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

#endif
