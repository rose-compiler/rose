#include "sage3basic.h"

#include "FloatingPointFormat.h"

namespace rose {
namespace BinaryAnalysis {

const FloatingPointFormat&
FloatingPointFormat::IEEE754_double() {
    static FloatingPointFormat fp;
    if (!fp.isValid()) {
        fp.width(64);
        fp.signBit(63);
        fp.exponentBits(BitRange::baseSize(52, 11));
        fp.exponentBias(1023);
        fp.significandBits(BitRange::baseSize(0, 52));
        fp.gradualUnderflow(true);
        fp.normalizedSignificand(true);
        ASSERT_require(fp.isValid());
    }
    return fp;
}

const FloatingPointFormat&
FloatingPointFormat::IEEE754_single() {
    static FloatingPointFormat fp;
    if (!fp.isValid()) {
        fp.width(32);
        fp.signBit(31);
        fp.exponentBits(BitRange::baseSize(23, 8));
        fp.exponentBias(127);
        fp.significandBits(BitRange::baseSize(0, 23));
        fp.gradualUnderflow(true);
        fp.normalizedSignificand(true);
        ASSERT_require(fp.isValid());
    }
    return fp;
}

bool
FloatingPointFormat::isValid() const {
    if (exponentBits_.isEmpty() || significandBits_.isEmpty())
        return false;
    if (exponentBits_.isOverlapping(significandBits_))
        return false;
    if (exponentBits_.isOverlapping(signBit_) || significandBits_.isOverlapping(signBit_))
        return false;
    if (totalWidth_ <= signBit_ || totalWidth_ <= exponentBits_.greatest() || totalWidth_ <= significandBits_.greatest())
        return false;
    return true;
}

} // namespace
} // namespace
