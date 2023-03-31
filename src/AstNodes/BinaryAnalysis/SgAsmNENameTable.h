class SgAsmNENameTable: public SgAsmGenericSection {
    [[using Rosebud: rosetta]]
    SgStringList names;

    [[using Rosebud: rosetta]]
    SgUnsignedList ordinals;

public:
    SgAsmNENameTable(SgAsmNEFileHeader *fhdr, rose_addr_t offset);
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
    std::vector<std::string> get_names_by_ordinal(unsigned ordinal);
};
