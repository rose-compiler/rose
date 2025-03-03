#include <Rose/BinaryAnalysis/Address.h>

/** Section table entry. */
class SgAsmPESectionTableEntry: public SgAsmExecutableFileFormat {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#ifdef _MSC_VER
# pragma pack (1)
#endif
    /* File format of a section table entry. All fields are little endian. Sections are ordered by RVA. */
    struct PESectionTableEntry_disk {
        char     name[8];          /* NUL-padded */
        uint32_t virtual_size;     /* virtual memory size, >= physical_size and difference is zero filled */
        uint32_t rva;              /* relative virt addr wrt Image Base; multiple of section_align; dense space */
        uint32_t physical_size;    /* bytes of initialized data on disk; multiple of file_align & <= virtual_size*/
        uint32_t physical_offset;  /* location of initialized data on disk; multiple of file_align */
        uint32_t coff_line_nums;   /* file offset of COFF line number info or zero */
        uint32_t n_relocs;         /* number of relocation entries; should be zero for executables */
        uint32_t n_coff_line_nums; /* number of COFF line number entries */
        uint32_t flags;            /* PESectionFlags bits: code, data, caching, paging, shared, permissions, etc.*/
    }
#if !defined(SWIG) && !defined(_MSC_VER)
    __attribute__((packed))
#endif
    ;
#ifdef _MSC_VER
# pragma pack ()
#endif

    /* These come from the windows PE documentation and
     * http://en.wikibooks.org/wiki/X86_Disassembly/Windows_Executable_Files */
    enum PESectionFlags {
        OF_CODE             = 0x00000020,   /* section contains code */
        OF_IDATA            = 0x00000040,   /* initialized data */
        OF_UDATA            = 0x00000080,   /* uninitialized data */
        OF_INFO             = 0x00000200,   /* comments or some other type of info */
        OF_REMOVE           = 0x00000800,   /* section will not become part of image */
        OF_COMDAT           = 0x00001000,   /* section contains comdat */
        OF_NO_DEFER_SPEC_EXC= 0x00004000,   /* reset speculative exception handling bits in the TLB entires for
                                             * this section */
        OF_GPREL            = 0x00008000,   /* section content can be access relative to GP */
        OF_ALIGN_1          = 0x00100000,   /* no alignment */
        OF_ALIGN_2          = 0x00200000,   /* 2-byte alignment */
        OF_ALIGN_4          = 0x00300000,   /* 4-byte alignment */
        OF_ALIGN_8          = 0x00400000,   /* 8-byte alignment */
        OF_ALIGN_16         = 0x00500000,   /* 16-byte alignment (default if not other alignment specified) */
        OF_ALIGN_32         = 0x00600000,   /* 32-byte alignment */
        OF_ALIGN_64         = 0x00700000,   /* 64-byte alignment */
        OF_ALIGN_128        = 0x00800000,   /* 128-byte alignment */
        OF_ALIGN_256        = 0x00900000,   /* 256-byte alignment */
        OF_ALIGN_512        = 0x00a00000,   /* 512-byte alignment */
        OF_ALIGN_1k         = 0x00b00000,   /* 1024-byte alignment */
        OF_ALIGN_2k         = 0x00c00000,   /* 2048-byte alignment */
        OF_ALIGN_4k         = 0x00d00000,   /* 4096-byte alignment */
        OF_ALIGN_8k         = 0x00e00000,   /* 8192-byte alignment */
        OF_ALIGN_MASK       = 0x00f00000,   /* mask for alignment value */
        OF_NRELOC_OVFL      = 0x01000000,   /* section contains extended relocations */
        OF_DISCARDABLE      = 0x02000000,   /* can be discarded */
        OF_NO_CACHE         = 0x04000000,   /* section must not be cached */
        OF_NO_PAGING        = 0x08000000,   /* section is not pageable */
        OF_SHARED           = 0x10000000,   /* section is shared */
        OF_EXECUTABLE       = 0x20000000,   /* execute permission */
        OF_READABLE         = 0x40000000,   /* read permission */
        OF_WRITABLE         = 0x80000000    /* write permission */
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Name.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    std::string name;

    /** Property: Virtual size.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address virtual_size = 0;

    /** Property: RVA.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address rva = 0;

    /** Property: Physical size.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address physical_size = 0;

    /** Property: Physical offset.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address physical_offset = 0;

    /** Property: COFF line numbers.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned coff_line_nums = 0;

    /** Property: Number of relocations.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned n_relocs = 0;

    /** Property: Number of COFF line numbers.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned n_coff_line_nums = 0;

    /** Property: Flags.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned flags = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    explicit SgAsmPESectionTableEntry(const SgAsmPESectionTableEntry::PESectionTableEntry_disk *disk);

    /** Update this section table entry with newer information from the section */
    void updateFromSection(SgAsmPESection *section);
    void *encode(SgAsmPESectionTableEntry::PESectionTableEntry_disk*) const;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    void update_from_section(SgAsmPESection*) ROSE_DEPRECATED("use updateFromSection");
};
