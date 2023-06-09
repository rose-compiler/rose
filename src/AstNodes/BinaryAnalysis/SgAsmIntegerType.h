#include <Rose/BinaryAnalysis/ByteOrder.h>

/** Integer types. */
class SgAsmIntegerType: public SgAsmScalarType {
    /** Property: whether the integral type is signed or unsigned.
     *
     *  Returns true if this is a signed type, otherwise false.
     *
     * @{ */
    [[using Rosebud: rosetta, accessors(), mutators()]]
    bool isSigned = false;

    bool get_isSigned() const;
    /** @} */

public:
    /** Constructor with specified member values.
     *
     *  Constructs a new integer type and initializes its properties according to the arguments. */
    SgAsmIntegerType(Rose::BinaryAnalysis::ByteOrder::Endianness, size_t nBits, bool isSigned);

    // Overrides documented in base class
    virtual void check() const override;
    virtual std::string toString() const override;
};
