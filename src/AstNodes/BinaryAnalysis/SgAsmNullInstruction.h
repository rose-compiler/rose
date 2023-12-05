class SgAsmNullInstruction: public SgAsmInstruction {
public:
    // there's only one kind of null instruction
    enum Kind { null_unknown };

public: // overrides
    virtual unsigned get_anyKind() const override;
};
