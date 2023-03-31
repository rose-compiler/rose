class SgAsmNERelocTable: public SgAsmGenericSection {
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmNERelocEntryPtrList entries;

public:
    explicit SgAsmNERelocTable(SgAsmNEFileHeader *fhdr, SgAsmNESection *section);
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
