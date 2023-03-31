class SgAsmLEPageTable: public SgAsmGenericSection {
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmLEPageTableEntryPtrList entries;

public:
    SgAsmLEPageTable(SgAsmLEFileHeader *fhdr, rose_addr_t offset, rose_addr_t size);
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
    SgAsmLEPageTableEntry *get_page(size_t idx);
};
