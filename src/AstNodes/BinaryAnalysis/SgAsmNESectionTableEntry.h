#include <Rose/BinaryAnalysis/Address.h>

class SgAsmNESectionTableEntry: public SgAsmExecutableFileFormat {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#ifdef _MSC_VER
# pragma pack (1)
#endif
    /* File format of a section table entry. All fields are little endian. */
    struct NESectionTableEntry_disk {
        uint16_t    sector;                 /* 0x00 File offset (sector size defined in hdr); zero means no file data */
        uint16_t    physical_size;          /* 0x02 Length of segment in file; zero means 64k if sector is non-zero */
        uint16_t    flags;                  /* 0x04 Segment bit flags */
        uint16_t    virtual_size;           /* 0x06 Total size of segment when mapped to memory; zero means 64k */
    }                                     /* 0x08 */
// DQ (3/7/2013): Adding support to restrict visability to SWIG.
#ifndef SWIG
#ifndef _MSC_VER
                __attribute__((packed))
#endif
#endif
                ;
#ifdef _MSC_VER
# pragma pack ()
#endif

    enum NESectionFlags {
        SF_RESERVED         = 0x0e08,       /* these bits are reserved */

        SF_TYPE_MASK        = 0x0007,       /* segment-type field */
        SF_CODE             = 0x0000,       /* code-segment type */
        SF_DATA             = 0x0001,       /* data-segment (otherwise segment is code) */
        SF_ALLOC            = 0x0002,       /* loader allocates memory */
        SF_LOAD             = 0x0004,       /* load the segment */

        SF_MOVABLE          = 0x0010,       /* segment is not fixed */
        SF_PURE             = 0x0020,       /* segment is pure, or sharable; otherwise impure or non-sharable */
        SF_PRELOAD          = 0x0040,       /* segment will be preloaded; read-only if this is a data segment */
        SF_NOT_WRITABLE     = 0x0080,       /* code segment is execute only; data segment is read-only */
        SF_RELOCINFO        = 0x0100,       /* segment has relocation records */
        SF_DISCARDABLE      = 0x1000,       /* discardable */
        SF_DISCARD          = 0xf000        /* discard priority */
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    [[using Rosebud: rosetta]]
    unsigned flags = 0;

    [[using Rosebud: rosetta]]
    unsigned sector = 0;

    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address physicalSize = 0;

    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address virtualSize = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    explicit SgAsmNESectionTableEntry(const SgAsmNESectionTableEntry::NESectionTableEntry_disk *disk);
    void *encode(SgAsmNESectionTableEntry::NESectionTableEntry_disk*) const;
    virtual void dump(FILE *f, const char *prefix, ssize_t idx) const {dump(f, prefix, idx, NULL);}
    void dump(FILE*, const char *prefix, ssize_t idx, SgAsmNEFileHeader *fhdr) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    Rose::BinaryAnalysis::Address get_physical_size() const ROSE_DEPRECATED("use get_physicalSize");
    void set_physical_size(Rose::BinaryAnalysis::Address) ROSE_DEPRECATED("use set_physicalSize");
    Rose::BinaryAnalysis::Address get_virtual_size() const ROSE_DEPRECATED("use get_virtualSize");
    void set_virtual_size(Rose::BinaryAnalysis::Address) ROSE_DEPRECATED("use set_virtualSize");
};
