class SgAsmNullInstruction: public SgAsmInstruction {
public:
    // there's only one kind of null instruction
    enum Kind { null_unknown };

public: // overrides
    virtual bool isUnknown() const override;
    virtual unsigned get_anyKind() const override;
};
