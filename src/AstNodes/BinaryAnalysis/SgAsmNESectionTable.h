class SgAsmNESectionTable: public SgAsmGenericSection {
public:
    [[using Rosebud: rosetta]]
    unsigned flags = 0;

    [[using Rosebud: rosetta]]
    unsigned sector = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t physicalSize = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t virtualSize = 0;

public:
    explicit SgAsmNESectionTable(SgAsmNEFileHeader*);
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    rose_addr_t get_physical_size() const ROSE_DEPRECATED("use get_physicalSize");
    void set_physical_size(rose_addr_t) ROSE_DEPRECATED("use set_physicalSize");
    rose_addr_t get_virtual_size() const ROSE_DEPRECATED("use get_virtualSize");
    void set_virtual_size(rose_addr_t) ROSE_DEPRECATED("use set_virtualSize");
};
