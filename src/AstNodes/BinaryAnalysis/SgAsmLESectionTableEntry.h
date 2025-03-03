#include <Rose/BinaryAnalysis/Address.h>
#include <Rose/BinaryAnalysis/ByteOrder.h>

class SgAsmLESectionTableEntry: public SgAsmExecutableFileFormat {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#ifdef _MSC_VER
# pragma pack (1)
#endif
    /* File format of a section table entry. Fields are big- or little-endian depending on file header. */
    struct LESectionTableEntry_disk {
        uint32_t    mapped_size;            /* 0x00 virtual segment size in bytes */
        uint32_t    base_addr;              /* 0x04 relocation base address */
        uint32_t    flags;                  /* 0x08 bit flags, see LESectionFlags */
        uint32_t    pagemap_index;          /* 0x0c */
        uint32_t    pagemap_nentries;       /* 0x10 number of entries in the page map */
        uint32_t    res1;                   /* 0x14 reserved */
    }                                     /* 0x18 */
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

    /* SF_BIG_BIT: The "big/default" bit, for data segments, controls the setting of the Big bit in the segment descriptor. (The
     *             Big bit, or B-bit, determines whether ESP or SP is used as the stack pointer.) For code segments, this bit
     *             controls the setting of the Default bit in the segment descriptor. (The Default bit, or D-bit, determines
     *             whether the default word size is 32-bits or 16-bits. It also affects the interpretation of the instruction
     *             stream.) */
    enum LESectionFlags {
        SF_RESERVED         = 0xffff0800,   /* Reserved bits (FIXME) */

        SF_READABLE         = 0x00000001,   /**< Read permission granted when mapped */
        SF_WRITABLE         = 0x00000002,   /**< Write permission granted when mapped */
        SF_EXECUTABLE       = 0x00000004,   /**< Execute permission granted when mapped */

        SF_RESOURCE         = 0x00000008,   /**< Section contains resource objects */
        SF_DISCARDABLE      = 0x00000010,   /**< Discardable section */
        SF_SHARED           = 0x00000020,   /**< Section is shared */
        SF_PRELOAD_PAGES    = 0x00000040,   /**< Section has preload pages */
        SF_INVALID_PAGES    = 0x00000080,   /**< Section has invalid pages */

        SF_TYPE_MASK        = 0x00000300,        /*NO_STRINGIFY*/
        SF_TYPE_NORMAL      = 0x00000000,
        SF_TYPE_ZERO        = 0x00000100,   /**< Section has zero-filled pages */
        SF_TYPE_RESIDENT    = 0x00000200,   /**< Section is resident (valid for VDDs and PDDs only) */
        SF_TYPE_RESCONT     = 0x00000300,   /**< Section is resident and contiguous */

        SF_RES_LONG_LOCK    = 0x00000400,   /**< Section is resident and "long-lockable" (VDDs and PDDs only) */
        SF_1616_ALIAS       = 0x00001000,   /**< 16:16 alias required (80x86 specific) */
        SF_BIG_BIT          = 0x00002000,   /**< Big/default bit setting (80x86 specific); see note above */
        SF_CODE_CONFORM     = 0x00004000,   /**< Section is conforming for code (80x86 specific) */
        SF_IO_PRIV          = 0x00008000    /**< Section I/O privilege level (80x86 specific; used only for 16:16 alias objects) */
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    [[using Rosebud: rosetta]]
    unsigned flags = 0;

    [[using Rosebud: rosetta]]
    unsigned pageMapIndex = 0;

    [[using Rosebud: rosetta]]
    unsigned pageMapNEntries = 0;

    [[using Rosebud: rosetta]]
    unsigned res1 = 0;

    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address mappedSize = 0;

    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address baseAddr = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmLESectionTableEntry(Rose::BinaryAnalysis::ByteOrder::Endianness sex,
                             const SgAsmLESectionTableEntry::LESectionTableEntry_disk *disk);
    void *encode(Rose::BinaryAnalysis::ByteOrder::Endianness, SgAsmLESectionTableEntry::LESectionTableEntry_disk*) const;
    virtual void dump(FILE *f, const char *prefix, ssize_t idx) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    unsigned get_pagemap_index() const ROSE_DEPRECATED("use get pageMapIndex");
    void set_pagemap_index(unsigned) ROSE_DEPRECATED("use set_pageMapIndex");
    unsigned get_pagemap_nentries() const ROSE_DEPRECATED("use get_pageMapNEntries");
    void set_pagemap_nentries(unsigned) ROSE_DEPRECATED("use set_pageMapNEntries");
    Rose::BinaryAnalysis::Address get_mapped_size() const ROSE_DEPRECATED("use get_mappedSize");
    void set_mapped_size(Rose::BinaryAnalysis::Address) ROSE_DEPRECATED("use set_mappedSize");
    Rose::BinaryAnalysis::Address get_base_addr() const ROSE_DEPRECATED("use get_baseAddr");
    void set_base_addr(Rose::BinaryAnalysis::Address) ROSE_DEPRECATED("use set_baseAddr");
};
