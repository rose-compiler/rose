#include <Rose/BinaryAnalysis/Address.h>
#include <Rose/BinaryAnalysis/ByteOrder.h>
#include <sageContainer.h>

/** Represents one entry of a segment table. */
class SgAsmElfSegmentTableEntry: public SgAsmExecutableFileFormat {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Segment types (host order). All other values are reserved. */
    enum SegmentType {
        PT_NULL         = 0,                        /**< Ignored entry. Other values of entry are undefined. */
        PT_LOAD         = 1,                        /**< Loadable by mapping file contents into memory. */
        PT_DYNAMIC      = 2,                        /**< Dynamic linking information. */
        PT_INTERP       = 3,                        /**< Segment contains NUL-terminated path name of interpreter. */
        PT_NOTE         = 4,                        /**< Auxiliary information. */
        PT_SHLIB        = 5,                        /**< Reserved w/unspecified semantics. Such a file is nonconforming. */
        PT_PHDR         = 6,                        /**< Segment contains the segment table itself (program header array) */
        PT_TLS          = 7,                        /**< Thread local storage. */

        // OS- and Processor-specific ranges
        PT_LOOS         = 0x60000000,               /**< Values reserved for OS-specific semantics */
        PT_HIOS         = 0x6fffffff,
        PT_LOPROC       = 0x70000000,               /**< Values reserved for processor-specific semantics */
        PT_HIPROC       = 0x7fffffff,

        // OS-specific values for GNU/Linux
        PT_GNU_EH_FRAME = 0x6474e550,               /**< GCC .eh_frame_hdr segment */
        PT_GNU_STACK    = 0x6474e551,               /**< Indicates stack executability */
        PT_GNU_RELRO    = 0x6474e552,               /**< Read-only after relocation */
        PT_PAX_FLAGS    = 0x65041580,               /**< Indicates PaX flag markings */

        // OS-specific values for Sun
        PT_SUNWBSS      = 0x6ffffffa,               /**< Sun Specific segment */
        PT_SUNWSTACK    = 0x6ffffffb                /**< Stack segment */
    };

    /** Segment bit flags */
    enum SegmentFlags {
        PF_NONE         = 0,                        /**< Initial value in c'tor */
        PF_RESERVED     = 0x000ffff8,               /**< Reserved bits */
        PF_XPERM        = 0x00000001,               /**< Execute permission */
        PF_WPERM        = 0x00000002,               /**< Write permission */
        PF_RPERM        = 0x00000004,               /**< Read permission */
        PF_OS_MASK      = 0x0ff00000,               /**< os-specific bits */
        PF_PROC_MASK    = 0xf0000000                /**< Processor-specific bits */
    };

#ifdef _MSC_VER
# pragma pack (1)
#endif
    /** File format of an ELF Segment header.
     *
     * Byte order of members depends on e_ident value in file header. This code comes directly from "Executable and
     * Linkable Format (ELF)", Portable Formats Specification, Version 1.1, Tool Interface Standards (TIS) and not from any
     * header file. The 64-bit structure is gleaned from the Linux elf(5) man page. Segment table entries (a.k.a., ELF
     * program headers) either describe process segments or give supplementary info which does not contribute to the
     * process image. */
    struct Elf32SegmentTableEntry_disk {
        uint32_t        p_type;                  /**< 0x00 kind of segment */
        uint32_t        p_offset;                /**< 0x04 file offset */
        uint32_t        p_vaddr;                 /**< 0x08 desired mapped address of segment */
        uint32_t        p_paddr;                 /**< 0x0c physical address where supported (unused by System V) */
        uint32_t        p_filesz;                /**< 0x20 bytes in file (may be zero or other value smaller than p_memsz) */
        uint32_t        p_memsz;                 /**< 0x24 number of bytes when mapped (may be zero) */
        uint32_t        p_flags;                 /**< 0x28 */
        uint32_t        p_align;                 /**< 0x2c alignment for file and memory (0,1=>none); power of two */
    }                                            /* 0x30 */
#if !defined(SWIG) && !defined(_MSC_VER)
    __attribute__((packed))
#endif
    ;

    struct Elf64SegmentTableEntry_disk {
        uint32_t        p_type;         /* 0x00 */
        uint32_t        p_flags;        /* 0x04 */
        uint64_t        p_offset;       /* 0x08 */
        uint64_t        p_vaddr;        /* 0x10 */
        uint64_t        p_paddr;        /* 0x18 */
        uint64_t        p_filesz;       /* 0x20 */
        uint64_t        p_memsz;        /* 0x28 */
        uint64_t        p_align;        /* 0x30 */
    }                                       /* 0x38 */
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
    /** Property: Index into table.
     *
     *  This is the index of this entry within the ELF segment table. */
    [[using Rosebud: rosetta]]
    size_t index = 0;

    /** Property: Segment type. */
    [[using Rosebud: rosetta]]
    SgAsmElfSegmentTableEntry::SegmentType type = SgAsmElfSegmentTableEntry::PT_LOAD;

    /** Property: Segment flags. */
    [[using Rosebud: rosetta]]
    SgAsmElfSegmentTableEntry::SegmentFlags flags = SgAsmElfSegmentTableEntry::PF_NONE;

    /** Property: Offset of segment in the file.
     *
     *  This is the starting byte offset of the segment within the file. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address offset = 0;

    /** Property: Virtual address.
     *
     *  This is the virtual address for the start of the segment as stored in the segment table. This is only a hint to the
     *  loader, which may map the segment to some other virtual address. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address vaddr = 0;

    /** Property: ELF paddr field.
     *
     *  See official ELF specification. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address paddr = 0;

    /** Property: Size of segment in file.
     *
     *  Size of the segment in bytes as it is stored in the file. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address filesz = 0;

    /** Property:  Size of segment in memory.
     *
     *  Size of the segment in bytes after it is loaded into virtual memory. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address memsz = 0;

    /** Property: Alignment.
     *
     *  Alignment in memory in bytes. Zero means the same thing as one, namely no alignment. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address align = 0;

    /** Property: Extra bytes.
     *
     *  These are bytes from the table entry that are not assigned any specific purpose by the ELF specification. */
    [[using Rosebud: rosetta, large]]
    SgUnsignedCharList extra;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Construct node from 32-bit file data. */
    SgAsmElfSegmentTableEntry(Rose::BinaryAnalysis::ByteOrder::Endianness sex,
                              const SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk *disk);

    /** Construct node from 64-bit file data. */
    SgAsmElfSegmentTableEntry(Rose::BinaryAnalysis::ByteOrder::Endianness sex,
                              const SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk *disk);

    /** Converts segment table entry back into disk structure.
     *
     * @{ */
    void *encode(Rose::BinaryAnalysis::ByteOrder::Endianness, SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk*) const;
    void *encode(Rose::BinaryAnalysis::ByteOrder::Endianness, SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk*) const;
    /** @} */

    /** Update this segment table entry with newer information from the section */
    void updateFromSection(SgAsmElfSection*);

    /** Print some debugging info */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

    /** Convert segment type to string. */
    static std::string toString(SgAsmElfSegmentTableEntry::SegmentType);

    /** Convert segment flags to string. */
    static std::string toString(SgAsmElfSegmentTableEntry::SegmentFlags);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    void update_from_section(SgAsmElfSection*) ROSE_DEPRECATED("use updateFromSection");
    static std::string to_string(SgAsmElfSegmentTableEntry::SegmentType) ROSE_DEPRECATED("use toString");
    static std::string to_string(SgAsmElfSegmentTableEntry::SegmentFlags) ROSE_DEPRECATED("use toString");
};
