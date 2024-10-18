#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <SgAsmExpression.h>

#include <SgAsmIntegerValueExpression.h>
#include <SgAsmType.h>
#include <Cxx_GrammarDowncast.h>

size_t
SgAsmExpression::get_nBits() const {
    SgAsmType *type = get_type();
    ASSERT_not_null2(type, "expression has no type");
    return type->get_nBits();
}

Sawyer::Optional<uint64_t>
SgAsmExpression::asUnsigned() const {
    if (const SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(this)) {
        if (ival->get_significantBits() <= 64) {
            return ival->get_absoluteValue();
        } else if (ival->get_bitVector().mostSignificantSetBit().orElse(0) < 64) {
            return ival->get_bitVector().toInteger(Sawyer::Container::Interval<size_t>::baseSize(0, 64));
        }
    }
    return Sawyer::Nothing();
}

Sawyer::Optional<int64_t>
SgAsmExpression::asSigned() const {
    if (const SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(this)) {
        if (ival->get_significantBits() <= 64) {
            return ival->get_signedValue();
        }
    }
    return Sawyer::Nothing();
}

#endif
