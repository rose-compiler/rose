#ifndef ROSE_BinaryAnalysis_FloatingPointFormat_H
#define ROSE_BinaryAnalysis_FloatingPointFormat_H

namespace rose {
namespace BinaryAnalysis {

/** Floating-point data type. */
class FloatingPointFormat {
public:
    typedef Sawyer::Container::BitVector::BitRange BitRange;

private:
    size_t totalWidth_;                                 // width of entire value, all fields
    size_t signBit_;                                    // position of the sign bit
    BitRange exponentBits_;                             // location of the exponent bits
    uint64_t exponentBias_;                             // bias for exponent (stored exponent is true expondent + bias)
    BitRange significandBits_;                          // location of the significand
    bool gradualUnderflow_;                             // subnormal values when exponent is zero
    bool normalizedSignificand_;                        // significand is normalized for non-zero exponents

public:
    /** Default constructor.
     *
     *  Constructs a floating point type whose @ref isValid method returns false. */
    FloatingPointFormat()
        : totalWidth_(0), signBit_(0), exponentBias_(0), gradualUnderflow_(true), normalizedSignificand_(true) {}

    bool operator==(const FloatingPointFormat &x) const {
        return (totalWidth_     == x.totalWidth_ &&
                signBit_        == x.signBit_ &&
                exponentBits_   == x.exponentBits_ &&
                exponentBias_   == x.exponentBias_ &&
                significandBits_== x.significandBits_ &&
                gradualUnderflow_ == x.gradualUnderflow_ &&
                normalizedSignificand_ == x.normalizedSignificand_);
    }
    
    /** Double-precision (64-bit) IEEE-754 floating-point. */
    static const FloatingPointFormat& IEEE754_double();

    /** Single-precision (32-bit) IEEE-754 floating-point. */
    static const FloatingPointFormat& IEEE754_single();

    /** Whether this format is valid.
     *
     *  Checks various properties, returning true if everything looks okay.  For instance, the total width must be large enough
     *  to include the sign, exponent, and significand fields, which must all be non-empty. In particular, this predicate
     *  returns false for default-constructed objects. */
    bool isValid() const;

    /** Property: Total width of floating-point value.
     *
     *  A valid format has a total width wide enough to include the sign bit and the exponent and significand fields.
     *
     * @{ */
    size_t width() const { return totalWidth_; }
    void width(size_t nBits) { totalWidth_ = nBits; }
    /** @} */

    /** Property: Location of sign bit.
     *
     *  Positive and negative floating point values are indicated by a sign bit.
     *
     * @{ */
    size_t signBit() const { return signBit_; }
    void signBit(size_t bitIdx) { signBit_ = bitIdx; }
    /** @} */

    /** Property: Exponent bits.
     *
     *  This property stores the location of the exponent.
     *
     * @{ */
    BitRange exponentBits() const { return exponentBits_; }
    void exponentBits(const BitRange &range) { exponentBits_ = range; }
    /** @} */

    /** Property: Exponent bias.
     *
     *  The bias of the stored exponent.  The stored exponent is the true exponent plus the bias.
     *
     * @{ */
    uint64_t exponentBias() const { return exponentBias_; }
    void exponentBias(uint64_t bias) { exponentBias_ = bias; }
    /** @} */

    /** Property: Significand bits.
     *
     *  This property stores the location of the significand.
     *
     * @{ */
    BitRange significandBits() const { return significandBits_; }
    void significandBits(const BitRange &range) { significandBits_ = range; }
    /** @} */

    /** Property: Gradual underflow for values near zero.
     *
     *  If this property is set and the exponent field is zero and the significand field is nonzero then the significand holds
     *  a denormalized value.
     *
     * @{ */
    bool gradualUnderflow() const { return gradualUnderflow_; }
    void gradualUnderflow(bool b) { gradualUnderflow_ = b; }
    /** @} */

    /** Property: Significand is normalized.
     *
     *  If true then the significand is normalized so that its leading bit is non-zero and the leading bit is not stored in the
     *  exponent field.  If @ref gradualUnderflow is set then the significand is not normalized when the exponent field is zero.
     *
     * @{ */
    bool normalizedSignificand() const { return normalizedSignificand_; }
    void normalizedSignificand(bool b) { normalizedSignificand_ = b; }
    /** @} */
};

} // namespace
} // namespace

#endif
