#include <Rose/BinaryAnalysis/Address.h>
#include <Rose/BinaryAnalysis/ByteOrder.h>
#include <sageContainer.h>

/** Represents one entry in an ELF section table. */
class SgAsmElfSectionTableEntry: public SgAsmExecutableFileFormat {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Section types (host order). All other values are reserved. */
    enum SectionType {
        SHT_NULL        = 0,                  /**< Section header is inactive */
        SHT_PROGBITS    = 1,                  /**< Info defined by the program; format and meaning determined by prog */
        SHT_SYMTAB      = 2,                  /**< Complete symbol table */
        SHT_STRTAB      = 3,                  /**< String table */
        SHT_RELA        = 4,                  /**< Relocation entries with explicit addends (e.g., Elf32_Rela types) */
        SHT_HASH        = 5,                  /**< Symbol hash table (used by dynamic linking) */
        SHT_DYNAMIC     = 6,                  /**< Information for dynamic linking */
        SHT_NOTE        = 7,                  /**< Information that marks the file in some way */
        SHT_NOBITS      = 8,                  /**< Like SHT_PROGBITS but occupies no file space */
        SHT_REL         = 9,                  /**< Relocation entries without explicit addends (e.g., Elf32_Rel types) */
        SHT_SHLIB       = 10,                 /**< Reserved, unspecified semantics; Present only in non-conforming files */
        SHT_DYNSYM      = 11,                 /**< Minimal set of dynamic linking symbols */

        SHT_LOOS        = 0x60000000,         /**< OS specific semantics */
        SHT_GNU_verdef  = 0x6ffffffd,         /**< Symbol Version Definitions [gnu extension] .gnu.version_d */
        SHT_GNU_verneed = 0x6ffffffe,         /**< Symbol Version Requirements [gnu extension] .gnu.version_r */
        SHT_GNU_versym  = 0x6fffffff,         /**< Symbol Version Table [gnu extension] .gnu.version */
        SHT_HIOS        = 0x6fffffff,         /**< End OS specific semantics */       /*NO_STRINGIFY*/

        SHT_LOPROC      = 0x70000000,         /* Processor specific semantics */
        SHT_HIPROC      = 0x7fffffff,
        SHT_LOUSER      = 0x80000000,         /* Application specific semantics */
        SHT_HIUSER      = 0xffffffff
    };

    /** Section Flags (host order).  All other values are reserved. */
    enum SectionFlags {
        SHF_NULL=                  0,        /**< Invalid section flag (added for rose) */
        SHF_WRITE=           (1 << 0),       /**< Writable */
        SHF_ALLOC=           (1 << 1),       /**< Occupies memory during execution */
        SHF_EXECINSTR=       (1 << 2),       /**< Executable */
        SHF_MERGE=           (1 << 4),       /**< Might be merged */
        SHF_STRINGS=         (1 << 5),       /**< Contains nul-terminated strings */
        SHF_INFO_LINK=       (1 << 6),       /**< 'sh_info' contains SHT index */
        SHF_LINK_ORDER=      (1 << 7),       /**< Preserve order after combining */
        SHF_OS_NONCONFORMING=(1 << 8),       /**< Non-standard OS specific handling required */
        SHF_GROUP=           (1 << 9),       /**< Section is member of a group.  */
        SHF_TLS=             (1 << 10),      /**< Section hold thread-local data.  */
        SHF_MASKOS=          0x0ff00000,     /**< OS-specific.  */
        SHF_MASKPROC=        0xf0000000      /**< Processor-specific */
    };

    /** File format of an ELF Section header.
     *
     *  Byte order of members depends on e_ident value in file header. This code
     * comes directly from "Executable and Linkable Format (ELF)", Portable Formats Specification, Version 1.1, Tool
     * Interface Standards (TIS) and not from any header file. The 64-bit structure is gleaned from the Linux elf(5) man
     * page. */
#ifdef _MSC_VER
# pragma pack (1)
#endif
    struct Elf32SectionTableEntry_disk {
        uint32_t        sh_name;             /* 0x00 Section name; index into section header string table */
        uint32_t        sh_type;             /* 0x04 Section contents and semantics (see SectionType enum) */
        uint32_t        sh_flags;            /* 0x08 Bit flags */
        uint32_t        sh_addr;             /* 0x0c Desired mapped address */
        uint32_t        sh_offset;           /* 0x10 Section location in file unless sh_type==SHT_NOBITS */
        uint32_t        sh_size;             /* 0x14 Section size in bytes */
        uint32_t        sh_link;             /* 0x18 Section ID of another section; meaning depends on section type */
        uint32_t        sh_info;             /* 0x1c Extra info depending on section type */
        uint32_t        sh_addralign;        /* 0x20 Mapped alignment (0 and 1=>byte aligned); sh_addr must be aligned*/
        uint32_t        sh_entsize;          /* 0x24 If non-zero, size in bytes of each array member in the section */
    }                                        /* 0x28 */
#if !defined(SWIG) && !defined(_MSC_VER)
    __attribute__((packed))
#endif
    ;

    struct Elf64SectionTableEntry_disk {
        uint32_t        sh_name;             /* 0x00 see Elf32SectionTableEntry_disk */
        uint32_t        sh_type;             /* 0x04 */
        uint64_t        sh_flags;            /* 0x08 */
        uint64_t        sh_addr;             /* 0x10 */
        uint64_t        sh_offset;           /* 0x18 */
        uint64_t        sh_size;             /* 0x20 */
        uint32_t        sh_link;             /* 0x28 */
        uint32_t        sh_info;             /* 0x2c */
        uint64_t        sh_addralign;        /* 0x30 */
        uint64_t        sh_entsize;          /* 0x38 */
    }                                        /* 0x40 */
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
    /** Property: sh_name.
     *
     *  The sh_name property of an ELF section table. See official ELF specification. */
    [[using Rosebud: rosetta]]
    unsigned sh_name = 0;

    /** Property: sh_type.
     *
     *  The sh_type property of an ELF section table. See official ELF specification. */
    [[using Rosebud: rosetta]]
    SgAsmElfSectionTableEntry::SectionType sh_type = SHT_PROGBITS;

    /** Property: sh_link.
     *
     *  The sh_link property of an ELF section table. See official ELF specification. */
    [[using Rosebud: rosetta]]
    unsigned long sh_link = 0;

    /** Property: sh_info.
     *
     *  The sh_info property of an ELF section table. See official ELF specification. */
    [[using Rosebud: rosetta]]
    unsigned long sh_info = 0;

    /** Property: sh_flags.
     *
     *  The sh_flags property of an ELF section table.  See official ELF specification. */
    [[using Rosebud: rosetta]]
    uint64_t sh_flags = 0;

    /** Property: sh_addr.
     *
     *  The sh_addr property of an ELF section table. See official ELF specification. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address sh_addr= 0;

    /** Property: sh_offset.
     *
     *  The sh_offset property of an ELF section table. See official ELF specification. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address sh_offset = 0;

    /** Property: sh_size.
     *
     *  The sh_size property of an ELF section table. See official ELF specification. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address sh_size = 0;

    /** Property: sh_addralign.
     *
     *  The sh_addralign property of an ELF section table. See official ELF specification. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address sh_addralign = 0;

    /** Property: sh_entsize.
     *
     *  The sh_entsize property of an ELF section table. See official ELF specification. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address sh_entsize = 0;

    /** Property: Extra bytes not officially part of the table entry.
     *
     *  These are the extra bytes that aren't assigned any meaning by the specification. */
    [[using Rosebud: rosetta, large]]
    SgUnsignedCharList extra;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Converts 32-bit disk representation to host representation. */
    SgAsmElfSectionTableEntry(Rose::BinaryAnalysis::ByteOrder::Endianness sex,
                              const SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk *disk);

    /** Converts 64-bit disk representation to host representation. */
    SgAsmElfSectionTableEntry(Rose::BinaryAnalysis::ByteOrder::Endianness sex,
                              const SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk *disk);

    /** Encode a section table entry into the disk structure.
     *
     * @{ */
    void *encode(Rose::BinaryAnalysis::ByteOrder::Endianness sex,
                 SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk *disk) const;
    void *encode(Rose::BinaryAnalysis::ByteOrder::Endianness sex,
                 SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk *disk) const;
    /** @} */

    /** Update this section table entry with newer information from the section. */
    void updateFromSection(SgAsmElfSection*);

    /** Print some debugging info */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

    // Use Rose::stringify... function instead.
    static std::string toString(SgAsmElfSectionTableEntry::SectionType);
    static std::string toString(SgAsmElfSectionTableEntry::SectionFlags);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    void update_from_section(SgAsmElfSection*) ROSE_DEPRECATED("use updateFromSection");
    static std::string to_string(SgAsmElfSectionTableEntry::SectionType) ROSE_DEPRECATED("use toString");
    static std::string to_string(SgAsmElfSectionTableEntry::SectionFlags) ROSE_DEPRECATED("use toString");
};
