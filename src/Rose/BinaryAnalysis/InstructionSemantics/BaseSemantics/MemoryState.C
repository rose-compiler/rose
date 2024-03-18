#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MemoryState::WithFormatter
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MemoryState::WithFormatter::~WithFormatter() {}

MemoryState::WithFormatter::WithFormatter(const MemoryState::Ptr &obj, Formatter &fmt)
    : obj(obj), fmt(fmt) {}

void
MemoryState::WithFormatter::print(std::ostream &stream) const {
    obj->print(stream, fmt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MemoryState
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MemoryState::MemoryState()
    : byteOrder_(ByteOrder::ORDER_UNSPECIFIED), byteRestricted_(true) {}

MemoryState::MemoryState(const SValue::Ptr &addrProtoval, const SValue::Ptr &valProtoval)
    : addrProtoval_(addrProtoval), valProtoval_(valProtoval), byteOrder_(ByteOrder::ORDER_UNSPECIFIED), byteRestricted_(true) {
    ASSERT_not_null(addrProtoval);
    ASSERT_not_null(valProtoval);
}

MemoryState::MemoryState(const MemoryState::Ptr &other)
    : addrProtoval_(other->addrProtoval_), valProtoval_(other->valProtoval_), byteOrder_(ByteOrder::ORDER_UNSPECIFIED),
      merger_(other->merger_), byteRestricted_(other->byteRestricted_) {}

MemoryState::~MemoryState() {}

MemoryState::Ptr
MemoryState::promote(const MemoryState::Ptr &x) {
    ASSERT_not_null(x);
    return x;
}

Merger::Ptr
MemoryState::merger() const {
    return merger_;
}

void
MemoryState::merger(const Merger::Ptr &m) {
    merger_ = m;
}

SValue::Ptr
MemoryState::get_addr_protoval() const {
    return addrProtoval_;
}

SValue::Ptr
MemoryState::get_val_protoval() const {
    return valProtoval_;
}

bool
MemoryState::byteRestricted() const {
    return byteRestricted_;
}

void
MemoryState::byteRestricted(bool b) {
    byteRestricted_ = b;
}

ByteOrder::Endianness
MemoryState::get_byteOrder() const {
    return byteOrder_;
}

void
MemoryState::set_byteOrder(ByteOrder::Endianness bo) {
    byteOrder_ = bo;
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

MemoryState::WithFormatter
MemoryState::with_format(Formatter &fmt) {
    return WithFormatter(shared_from_this(), fmt);
}

MemoryState::WithFormatter
MemoryState::operator+(Formatter &fmt) {
    return with_format(fmt);
}
    
} // namespace
} // namespace
} // namespace
} // namespace

BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::MemoryState);

#endif
