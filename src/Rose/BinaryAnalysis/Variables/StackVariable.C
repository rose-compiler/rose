#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Variables/StackVariable.h>

#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/StringUtility/Escape.h>

#include <stringify.h>

namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {
namespace Variables {

StackVariable::StackVariable() {}

StackVariable::StackVariable(const P2::FunctionPtr &function, int64_t stackOffset, Address maxSizeBytes, Purpose purpose,
                             const std::vector<InstructionAccess> &definingInstructionVas, const std::string &name)
    : BaseVariable(maxSizeBytes, definingInstructionVas, name), function_(function), stackOffset_(stackOffset),
      purpose_(purpose) {}

StackVariable::StackVariable(const StackVariable&) = default;
StackVariable::~StackVariable() {}

P2::Function::Ptr
StackVariable::function() const {
    return function_;
}

void
StackVariable::function(const P2::Function::Ptr &f) {
    ASSERT_not_null(f);
    function_ = f;
}

int64_t
StackVariable::stackOffset() const {
    return stackOffset_;
}

void
StackVariable::stackOffset(int64_t offset) {
    stackOffset_ = offset;
}

StackVariable::Purpose
StackVariable::purpose() const {
    return purpose_;
}

void
StackVariable::purpose(Purpose p) {
    purpose_ = p;
}

const std::string&
StackVariable::setDefaultName() {
    int64_t offset = stackOffset();
    std::string s;

    // Depending on the architecture, local variables could be at negative or positive stack offsets, and arguments stored
    // on the stack are probably have the other sign. We'd like to be able to distinguish the two with different names.
    if (offset < 0)
        offset = -offset;

    static const char* const consonants = "bhjkclmndpqrfstvgwxz";
    static const char* const vowels = "aeiou";
    static int64_t nConsonants = strlen(consonants);
    static int64_t nVowels = strlen(vowels);

    for (size_t i=0; offset > 0; ++i) {
        if (i % 2 == 0) {
            s += consonants[offset % nConsonants];
            offset /= nConsonants;
        } else {
            s += vowels[offset % nVowels];
            offset /= nVowels;
        }
    }

    // Negative stack offsets are more common, so add "y" to the uncommon cases.
    if (stackOffset() >= 0)
        s += "y";

    name(s);
    return name();
}

bool
StackVariable::operator==(const StackVariable &other) const {
    if (!function_ || !other.function_) {
        return function_ == other.function_;
    } else {
        return function_->address() == other.function_->address() &&
            stackOffset() == other.stackOffset() &&
            maxSizeBytes() == other.maxSizeBytes();
    }
}

bool
StackVariable::operator!=(const StackVariable &other) const {
    return !(*this == other);
}

OffsetInterval
StackVariable::interval() const {
    // We need to watch for overflows.  The return type, OffsetInterval, has int64_t least and greatest values. The stack
    // offset is also int64_t. The maximum size in bytes however is uint64_t (i.e., Address).  We may need to reduce the
    // maximum size in order to fit it into the interval return value.
    int64_t least = stackOffset_;
    int64_t maxSizeSigned = maxSizeBytes() > boost::numeric_cast<uint64_t>(boost::integer_traits<int64_t>::const_max)
                            ? boost::integer_traits<int64_t>::const_max
                            : boost::numeric_cast<int64_t>(maxSizeBytes());
    if (least >= 0) {
        int64_t headroom = boost::integer_traits<int64_t>::const_max - maxSizeSigned;
        if (least > headroom) {
            // overflow would occur, so we must reduce the maxSizeS appropriately
            return OffsetInterval::hull(least, boost::integer_traits<int64_t>::max());
        } else {
            return OffsetInterval::baseSize(least, maxSizeSigned);
        }
    } else {
        return OffsetInterval::baseSize(least, maxSizeSigned);
    }
}

// class method
StackVariable::Boundary&
StackVariable::insertBoundary(Boundaries &boundaries /*in,out*/, const int64_t stackOffset, const InstructionAccess &definer) {
    for (size_t i = 0; i < boundaries.size(); ++i) {
        if (boundaries[i].stackOffset == stackOffset) {

            for (InstructionAccess &ia: boundaries[i].definingInsns) {
                if (definer.address().isEqual(ia.address())) {
                    ia.access().set(definer.access());
                    return boundaries[i];
                }
            }
            boundaries[i].definingInsns.push_back(definer);
            return boundaries[i];
        }
    }

    boundaries.push_back(Boundary());
    boundaries.back().stackOffset = stackOffset;
    boundaries.back().definingInsns.push_back(definer);
    return boundaries.back();
}

// class method
StackVariable::Boundary&
StackVariable::insertBoundaryImplied(Boundaries &boundaries /*in,out*/, const int64_t stackOffset) {
    return insertBoundary(boundaries, stackOffset, InstructionAccess(AccessFlags()));
}

// class method
void
StackVariable::printBoundary(std::ostream &out, const Boundary &boundary, const std::string &prefix) {
    switch (boundary.purpose) {
        case Purpose::RETURN_ADDRESS:
            out <<prefix <<"return address ";
            break;
        case Purpose::FRAME_POINTER:
            out <<prefix <<"frame pointer ";
            break;
        case Purpose::STACK_POINTER:
            out <<prefix <<"stack pointer";
            break;
        case Purpose::SPILL_AREA:
            out <<prefix <<"spill area ";
            break;
        case Purpose::NORMAL:
            out <<prefix <<"variable ";
            break;
        case Purpose::UNKNOWN:
            out <<prefix <<"unknown purpose ";
            break;
        case Purpose::OTHER:
            out <<prefix <<"other data area ";
            break;
    }

    out <<"at function stack offset " <<boundary.stackOffset <<"\n";

    for (const InstructionAccess &ia: boundary.definingInsns)
        out <<prefix <<"  " <<ia <<"\n";
}

void
StackVariable::print(std::ostream &out) const {
    out <<"local-variable";
    if (!name().empty())
        out <<" \"" <<StringUtility::cEscape(name()) <<"\"";
    out <<" (loc=sp" <<offsetStr(stackOffset()) <<", size=" <<sizeStr(maxSizeBytes())
        <<", type=" <<stringify::Rose::BinaryAnalysis::Variables::StackVariable::Purpose((int64_t)purpose_)
        <<")";
}

std::string
StackVariable::toString() const {
    std::ostringstream ss;
    print(ss);
    return ss.str();
}

std::ostream&
operator<<(std::ostream &out, const Rose::BinaryAnalysis::Variables::StackVariable &x) {
    x.print(out);
    return out;
}

} // namespace
} // namespace
} // namespace

#endif
