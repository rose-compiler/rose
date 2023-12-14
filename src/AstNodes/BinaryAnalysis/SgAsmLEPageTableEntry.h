#include <Rose/BinaryAnalysis/ByteOrder.h>

class SgAsmLEPageTableEntry: public SgAsmExecutableFileFormat {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /* The object page table provides information about a logical page in a section. A logical page may be an enumerated page, a
     * pseudo page, or an iterated page. The page table allows for efficient access to a page when a page fault occurs, while still
     * allowing the physical page to be located in the preload page, demand load page, or iterated data page sections of the
     * executable file. Entries in the page table use 1-origin indices.  This table is parallel with the Fixup Page Table (they are
     * both indexed by the logical page number). */

#ifdef _MSC_VER
# pragma pack (1)
#endif
    /* File format for a page table entry */
    struct LEPageTableEntry_disk {
        uint16_t            pageno_hi;
        unsigned char       pageno_lo;
        unsigned char       flags;
    }
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    [[using Rosebud: rosetta]]
    unsigned pageno = 0;

    [[using Rosebud: rosetta]]
    unsigned flags = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmLEPageTableEntry(Rose::BinaryAnalysis::ByteOrder::Endianness sex,
                          const SgAsmLEPageTableEntry::LEPageTableEntry_disk *disk);
    void dump(FILE*, const char *prefix, ssize_t idx) const;
    void *encode(Rose::BinaryAnalysis::ByteOrder::Endianness, SgAsmLEPageTableEntry::LEPageTableEntry_disk*) const;
};
