class SgAsmNESectionTable: public SgAsmGenericSection {
    [[using Rosebud: rosetta]]
    unsigned flags = 0;

    [[using Rosebud: rosetta]]
    unsigned sector = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t physical_size = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t virtual_size = 0;

public:
    explicit SgAsmNESectionTable(SgAsmNEFileHeader*);
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
