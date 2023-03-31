class SgAsmLESectionTable: public SgAsmGenericSection {
public:
    SgAsmLESectionTable(SgAsmLEFileHeader *fhdr, rose_addr_t offset, rose_addr_t size);
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
