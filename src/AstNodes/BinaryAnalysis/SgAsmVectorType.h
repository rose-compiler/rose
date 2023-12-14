/** Base class for vector types. */
class SgAsmVectorType: public SgAsmType {
public:
    /** Property: Number of elements in vector. */
    [[using Rosebud: rosetta, accessors(), mutators()]]
    size_t nElmts = 0;

    /** Property: Type of each vector element. */
    [[using Rosebud: rosetta, accessors(), mutators()]]
    SgAsmType* elmtType = nullptr;

public:
    /** Construct a new vector type. */
    SgAsmVectorType(size_t nElmts, SgAsmType *elmtType);

    /** Property: Number of elements in vector. */
    size_t get_nElmts() const;

    /** Property: Type of each vector element. */
    SgAsmType* get_elmtType() const;

    // Overrides documented in base class
    virtual void check() const override;
    virtual std::string toString() const override;
    virtual size_t get_nBits() const override;
};
