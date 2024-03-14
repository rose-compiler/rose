#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>
#include <Sawyer/Result.h>

/** Export entry. */
class SgAsmPEExportEntry: public SgAsmExecutableFileFormat {
public:
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
    Rose::BinaryAnalysis::RelativeVirtualAddress exportRva;

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
    SgAsmPEExportEntry(SgAsmGenericString *name, unsigned ordinal, Rose::BinaryAnalysis::RelativeVirtualAddress export_rva,
                       SgAsmGenericString *forwarder);
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
    const Rose::BinaryAnalysis::RelativeVirtualAddress& get_export_rva() const ROSE_DEPRECATED("use get_exportRva");
    void set_export_rva(const Rose::BinaryAnalysis::RelativeVirtualAddress&) ROSE_DEPRECATED("use set_exportRva");
};
