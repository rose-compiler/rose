#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>

/** Export directory. */
class SgAsmPEExportDirectory: public SgAsmExecutableFileFormat {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#ifdef _MSC_VER
# pragma pack (1)
#endif
    /* An Export Section begins with the Export Directory */
    struct PEExportDirectory_disk {
        uint32_t res1;         /* 0x00 Reserved, must be zero */
        uint32_t timestamp;    /* 0x04 Time that export data was created */
        uint16_t vmajor;       /* 0x08 Major version number (user defined) */
        uint16_t vminor;       /* 0x0a Minor version number (user defined) */
        uint32_t name_rva;     /* 0x0c Location of name of DLL */
        uint32_t ord_base;     /* 0x10 Starting ordinal for exports in this image (usually 1) */
        uint32_t expaddr_n;    /* 0x14 Number of entries in the export address table */
        uint32_t nameptr_n;    /* 0x18 Number of entries in the name pointer table and ordinal table */
        uint32_t expaddr_rva;  /* 0x1c Location of Export Address Table */
        uint32_t nameptr_rva;  /* 0x20 Location of Export Name Pointer Table */
        uint32_t ordinals_rva; /* 0x24 Location of Ordinal Table */
    }                               /* 0x28 */
#if !defined(SWIG) && !defined(_MSC_VER)
    __attribute__((packed))
#endif
    ;
#ifdef _MSC_VER
# pragma pack ()
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Reserved aread #1.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned res1 = 0;

    /** Property: Time stamp.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    time_t timestamp = 0;

    /** Property: VMajor.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned vmajor = 0;

    /** Property: VMinor.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned vminor = 0;

    /** Property: RVA for name.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, large]]
    Rose::BinaryAnalysis::RelativeVirtualAddress name_rva;

    /** Property: Ordinal base.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned ord_base = 0;

    /** Property: Expaddr_n.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    size_t expaddr_n = 0;

    /** Property: Nameptr_n.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    size_t nameptr_n = 0;

    /** Property: Expaddr RVA.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, large]]
    Rose::BinaryAnalysis::RelativeVirtualAddress expaddr_rva;

    /** Property: Name pointer RVA.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, large]]
    Rose::BinaryAnalysis::RelativeVirtualAddress nameptr_rva;

    /** Property: Ordinals RVA.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, large]]
    Rose::BinaryAnalysis::RelativeVirtualAddress ordinals_rva;

    /** Property: Name.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmGenericString* name = NULL;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    explicit SgAsmPEExportDirectory(SgAsmPEExportSection*);
    void dump(FILE *f, const char *prefix, ssize_t idx) const;
};
