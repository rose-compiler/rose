/** A type that doesn't represent any data. */
class SgAsmVoidType: public SgAsmType {
public:
    static SgAsmVoidType* instance();

    virtual std::string toString() const override;
    virtual size_t get_nBits() const override;
};
