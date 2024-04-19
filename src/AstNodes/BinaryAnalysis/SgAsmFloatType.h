#include <Sawyer/BitVector.h>
#include <Rose/BitFlags.h>
#include <Rose/BinaryAnalysis/ByteOrder.h>

/** Floating point types. */
class SgAsmFloatType: public SgAsmScalarType {
public:
    /** Individual bit flags for this floating-point type. */
    enum Flag {
        GRADUAL_UNDERFLOW           = 0x00000001,   /**< De-normalized signifand when exponent field is clear. */
        IMPLICIT_BIT_CONVENTION     = 0x00000002    /**< Use IEEE 754 implicit bit convention for signicand. */
    };

    /** Collective bit flags for this floating-point type. */
    typedef Rose::BitFlags<Flag> Flags;

    /** Range of bits used for various purposes within the values of this type. */
    typedef Sawyer::Container::BitVector::BitRange BitRange;

public:
    [[using Rosebud: rosetta, accessors(), mutators()]]
    size_t significandOffset =(size_t)(-1);

    [[using Rosebud: rosetta, accessors(), mutators()]]
    size_t significandNBits = (size_t)(-1);

    [[using Rosebud: rosetta, accessors(), mutators()]]
    size_t signBitOffset = (size_t)(-1);

    [[using Rosebud: rosetta, accessors(), mutators()]]
    size_t exponentOffset = (size_t)(-1);

    [[using Rosebud: rosetta, accessors(), mutators()]]
    size_t exponentNBits = (size_t)(-1);

    [[using Rosebud: rosetta, accessors(), mutators()]]
    uint64_t exponentBias = 0;

    [[using Rosebud: rosetta, accessors(), mutators()]]
    unsigned flags = 0;

public:
    /** Construct a new floating-point type. */
    SgAsmFloatType(Rose::BinaryAnalysis::ByteOrder::Endianness, size_t nBits,
                   const BitRange &significandBits, const BitRange exponentBits, size_t signBit,
                   uint64_t exponentBias, Flags flags);

    /** Construct an IEEE 754 floating point type of the specified width.
     *
     * @{ */
    static SgAsmFloatType* instanceIeee32(Rose::BinaryAnalysis::ByteOrder::Endianness);
    static SgAsmFloatType* instanceIeee64(Rose::BinaryAnalysis::ByteOrder::Endianness);
    /** @} */

    /** Property: Offset to significand least significant bit. */
    BitRange significandBits() const;

    /** Property: Number of bits in the exponent. */
    BitRange exponentBits() const;

    /** Property: Offset to significand sign bit. */
    size_t signBit() const;

    /** Property: Zero-point of exponent. */
    uint64_t exponentBias() const;

    /** Property: Bit vector of all boolean properties. */
    Flags flags() const;

    /** Default IEEE 754 flags.
     *
     *  These flags are the most common types and include the implicit bit convention for the significand and the
     *  gradual underflow capability. */
    static Flags ieeeFlags();

    /** Property: Whether type has gradual underflow.
     *
     *  If the type supports gradual underflow, then when the exponent field's bits are all clear then the integer part (a
     *  single bit implied or explicit depending on @ref implicitBitConvention) of the significand is zero instead of
     *  one. */
    bool gradualUnderflow() const;

    /** Property: Whether the type follows the IEEE 754 significand implicit bit convention.
     *
     *  If true, then the leading set bit of the significand is not stored but rather an implied. If the exponent field
     *  contains any non-zero bits then the integer part of the significand is one, otherwise when the exponent field
     *  is all clear the integer part is zero. */
    bool implicitBitConvention() const;

    // Overrides documented in base class
    virtual void check() const override;
    virtual std::string toString() const override;
};
