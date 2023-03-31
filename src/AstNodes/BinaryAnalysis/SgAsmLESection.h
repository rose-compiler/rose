class SgAsmLESection: public SgAsmGenericSection {
    [[using Rosebud: rosetta, traverse]]
    SgAsmLESectionTableEntry* st_entry = NULL;

public:
    explicit SgAsmLESection(SgAsmLEFileHeader*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
