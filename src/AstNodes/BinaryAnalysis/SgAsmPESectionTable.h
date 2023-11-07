/** Section table. */
class SgAsmPESectionTable: public SgAsmGenericSection {
public:
    explicit SgAsmPESectionTable(SgAsmPEFileHeader*);
    virtual SgAsmPESectionTable* parse() override;

    /** Attaches a previously unattached PE Section to the PE Section Table.
     *
     *  This method complements SgAsmPESection::init_from_section_table. This method initializes the section table from the
     *  section while init_from_section_table() initializes the section from the section table. */
    void addSection(SgAsmPESection *section);
    virtual bool reallocate() override;
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    void add_section(SgAsmPESection*) ROSE_DEPRECATED("use addSection");
};
