#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Formatter.h>
#include <Rose/BitOps.h>

#include <sstream>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

size_t
SValue::nBits() const {
    return get_width();
}

bool
SValue::isConcrete() const {
    return is_number();
}

Sawyer::Optional<uint64_t>
SValue::toUnsigned() const {
    if (isConcrete() && nBits() <= 64) {
        return get_number();
    } else {
        return Sawyer::Nothing();
    }
}

Sawyer::Optional<int64_t>
SValue::toSigned() const {
    if (auto val = toUnsigned()) {
        uint64_t uval = BitOps::signExtend(*val, nBits());
        int64_t ival = static_cast<int64_t>(uval);

        // The above static_cast is implementation defined when uval >= 2^63, but the cast from signed to unsigned doesn't have
        // implementation defined behavior. This allows us to check that the above static cast did what we want.
        ASSERT_require2(static_cast<uint64_t>(ival) == uval, "this compiler's unsigned-to-signed static_cast is strange");
        return ival;
    } else {
        return Sawyer::Nothing();
    }
}

bool
SValue::mustEqual(const SValue::Ptr &other, const SmtSolverPtr &solver) const {
    return must_equal(other, solver);
}

bool
SValue::mayEqual(const SValue::Ptr &other, const SmtSolverPtr &solver) const {
    return may_equal(other, solver);
}

bool
SValue::isTrue() const {
    return toUnsigned().orElse(0) != 0;
}

bool
SValue::isFalse() const {
    return toUnsigned().orElse(1) == 0;
}

std::string
SValue::comment() const {
    return get_comment();
}

void
SValue::comment(const std::string &s) const {
    set_comment(s);
}

std::ostream&
operator<<(std::ostream &o, const SValue &x) {
    x.print(o);
    return o;
}

std::ostream&
operator<<(std::ostream &o, const SValue::WithFormatter &x) {
    x.print(o);
    return o;
}

void
SValue::print(std::ostream &stream) const {
    Formatter fmt;
    print(stream, fmt);
}

SValue::WithFormatter
SValue::operator+(const std::string &linePrefix) {
    static Formatter fmt;
    fmt.set_line_prefix(linePrefix);
    return with_format(fmt);
}

std::string
SValue::toString() const {
    std::ostringstream ss;
    print(ss);
    return ss.str();
}

} // namespace
} // namespace
} // namespace
} // namespace

BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::SValue);

#endif
