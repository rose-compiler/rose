#ifdef ROSE_IMPL
#include <SgAsmPEExportDirectory.h>
#include <SgAsmPEExportEntryList.h>
#endif

/** Export file section. */
class SgAsmPEExportSection: public SgAsmPESection {
public:
    /** Property: Export directory.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmPEExportDirectory* exportDirectory = nullptr;

    /** Property: List of export entries.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmPEExportEntryList* exports = createAndParent<SgAsmPEExportEntryList>(this);

public:
    /** The PE Export Address Table is an array of expaddr_n 4-byte RVAs.
     *
     *  If the address is not in the export section (as defined by the address and length that are indicated in the NT Optional
     *  Header) then the RVA is an actual address in code or data. Otherwise its a Forwarder RVA that names a symbol in another
     *  DLL. */
    typedef uint32_t ExportAddress_disk;

    /** The PE Export Name Pointer Table is an array of nameptr_n 4-byte RVAs pointing into the Export Name Table. The pointers are
     *  ordered lexically to allow binary searches.  An export name is defined only if the export name pointer table contains a
     *  pointer to it. */
    typedef uint32_t ExportNamePtr_disk;

    /** The PE Export Ordinal Table is an array of nameptr_n (yes) 2-byte indices into the Export Address Table biased by
     *  ord_base. In other words, the ord_base must be subtracted from the ordinals to obtain true indices into the Export Address
     *  Table. */
    typedef uint16_t ExportOrdinal_disk;

    SgAsmPEExportSection(SgAsmPEFileHeader*);
    virtual SgAsmPEExportSection *parse() override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
    void addEntry(SgAsmPEExportEntry*);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmPEExportDirectory* get_export_dir() const ROSE_DEPRECATED("use get_exportDirectory");
    void set_export_dir(SgAsmPEExportDirectory*) ROSE_DEPRECATED("use set_exportDirectory");
    void add_entry(SgAsmPEExportEntry*) ROSE_DEPRECATED("use addEntry");
};
