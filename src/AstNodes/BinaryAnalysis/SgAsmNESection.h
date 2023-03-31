class SgAsmNESection: public SgAsmGenericSection {
    [[using Rosebud: rosetta, traverse]]
    SgAsmNESectionTableEntry* st_entry = nullptr;

    [[using Rosebud: rosetta, traverse]]
    SgAsmNERelocTable* reloc_table = nullptr;

public:
    explicit SgAsmNESection(SgAsmNEFileHeader *fhdr);
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
