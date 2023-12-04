class SgAsmNullInstruction: public SgAsmInstruction {
public:
    // there's only one kind of null instruction
    enum Kind { null_unknown };

public: // overrides
    virtual bool isFunctionReturnFast(const std::vector<SgAsmInstruction*> &insns) override;
    virtual bool isFunctionReturnSlow(const std::vector<SgAsmInstruction*> &insns) override;
    virtual Sawyer::Optional<rose_addr_t> branchTarget() override;
    virtual Rose::BinaryAnalysis::AddressSet getSuccessors(bool &complete) override;
    virtual Rose::BinaryAnalysis::AddressSet getSuccessors(const std::vector<SgAsmInstruction*>&,
                                                           bool &complete,
                                                           const Rose::BinaryAnalysis::MemoryMap::Ptr &initial_memory =
                                                           Rose::BinaryAnalysis::MemoryMap::Ptr()) override;
    virtual bool isUnknown() const override;
    virtual unsigned get_anyKind() const override;
};
