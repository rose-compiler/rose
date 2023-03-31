class SgAsmLERelocTable: public SgAsmGenericSection {
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmLERelocEntryPtrList entries;

public:
    SgAsmLERelocTable(SgAsmLEFileHeader *fhdr, rose_addr_t offset);
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
