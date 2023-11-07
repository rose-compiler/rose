/** Export entry. */
class SgAsmPEExportEntry: public SgAsmExecutableFileFormat {
    /** Property: Name.
     *
     *  See PE specification.
     *
     * @{ */
    [[using Rosebud: rosetta, traverse, mutators()]]
    SgAsmGenericString* name = nullptr;

    void set_name(SgAsmGenericString*);
    /** @} */

   /** Property: Ordinal.
     *
     *  Unbiased ordinal. This is the index into the Export Address Table. See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned ordinal = 0;

    /** Property: Export RVA.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, large]]
    rose_rva_t exportRva = 0;

    /** Property: Forwarder.
     *
     *  See PE specification.
     *
     *  @{ */
    [[using Rosebud: rosetta, traverse, mutators()]]
    SgAsmGenericString* forwarder = nullptr;

    void set_forwarder(SgAsmGenericString*);
    /** @} */

public:
    SgAsmPEExportEntry(SgAsmGenericString *name, unsigned ordinal, rose_rva_t export_rva, SgAsmGenericString *forwarder);
    void dump(FILE *f,const char *prefix,ssize_t idx) const;
    /* Accessors. Override ROSETTA because we adjust parents. */

    /** Property: Biased ordinal.
     *
     *  This is the biased ordinal, which is the unbiased ordinal plus the ordinal base from the export directory.
     *  This is a read-only value calculated by following the parent pointer to the SgAsmPEExportSection node and
     *  then down to its SgAsmPEExportDirectory. If any links are missing then an error is returned. */
    Sawyer::Result<unsigned /*ordinal*/, std::string /*reason*/> biasedOrdinal() const;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    const rose_rva_t& get_export_rva() const ROSE_DEPRECATED("use get_exportRva");
    void set_export_rva(const rose_rva_t&) ROSE_DEPRECATED("use set_exportRva");
};
