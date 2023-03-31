class SgAsmNEStringTable: public SgAsmGenericSection {
public:
    SgAsmNEStringTable(SgAsmNEFileHeader *fhdr, rose_addr_t offset, rose_addr_t size);
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
    std::string get_string(rose_addr_t offset);
};
