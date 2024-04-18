/** Pointer type. */
class SgAsmPointerType: public SgAsmScalarType {
public:
    [[using Rosebud: rosetta, traverse, not_null]]
    SgAsmType *subtype;

public:
    /** Construct new pointer type. */
    SgAsmPointerType(Rose::BinaryAnalysis::ByteOrder::Endianness, size_t nBits, SgAsmType *subtype);

    /** Make instance wrapped around a subtype. */
    static SgAsmPointerType* instance(Rose::BinaryAnalysis::ByteOrder::Endianness, size_t nBits, SgAsmType *subtype);

    virtual void check() const override;
    virtual std::string toString() const override;
};
