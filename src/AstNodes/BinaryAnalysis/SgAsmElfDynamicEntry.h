#include <Rose/BinaryAnalysis/ByteOrder.h>
#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>

/** One entry from the dynamic linking table. */
class SgAsmElfDynamicEntry: public SgAsmExecutableFileFormat {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    enum EntryType {                /* Type    Executable SharedObj Purpose */
        DT_NULL     = 0,        /* ignored mandatory  mandatory Marks end of dynamic array */
        DT_NEEDED   = 1,        /* value   optional   optional  Name of needed library */
        DT_PLTRELSZ = 2,        /* value   optional   optional  Size of reloc entries associated with PLT */
        DT_PLTGOT   = 3,        /* pointer optional   optional  PLT and/or GOT */
        DT_HASH     = 4,        /* pointer mandatory  mandatory Symbol hash table */
        DT_STRTAB   = 5,        /* pointer mandatory  mandatory String table for symbols, lib names, etc. */
        DT_SYMTAB   = 6,        /* pointer mandatory  mandatory Symbol table */
        DT_RELA     = 7,        /* pointer mandatory  optional  Relocation table */
        DT_RELASZ   = 8,        /* value   mandatory  optional  Size of RELA relocation table */
        DT_RELAENT  = 9,        /* value   mandatory  optional  Size of each RELA table entry */
        DT_STRSZ    = 10,       /* value   mandatory  mandatory Size of string table */
        DT_SYMENT   = 11,       /* value   mandatory  mandatory Size of symbol table entry */
        DT_INIT     = 12,       /* pointer optional   optional  Initialization function */
        DT_FINI     = 13,       /* pointer optional   optional  Termination function */
        DT_SONAME   = 14,       /* value   ignored    optional  Name of shared object */
        DT_RPATH    = 15,       /* value   optional   ignored   NUL-term library search path */
        DT_SYMBOLIC = 16,       /* ignored ignored    optional  Bool determines dynamic linker symbol resolution */
        DT_REL      = 17,       /* pointer mandatory  optional  Relocation table */
        DT_RELSZ    = 18,       /* value   mandatory  optional  Size of REL relocation table */
        DT_RELENT   = 19,       /* value   mandatory  optional  Size of each REL table entry */
        DT_PLTREL   = 20,       /* value   optional   optional  Reloc type for PLT; value is DT_RELA or DT_REL */
        DT_DEBUG    = 21,       /* pointer optional   ignored   Contents are not specified at ABI level */
        DT_TEXTREL  = 22,       /* ignored optional   optional  presence => relocs to nonwritable segments OK */
        DT_JMPREL   = 23,       /* pointer optional   optional  Addr of relocation entries for PLT */
        DT_BIND_NOW = 24,       /* ignored optional   optional  Shall dynlinker do relocs before xfering control?*/
        DT_INIT_ARRAY = 25,     /* pointer optional   ?         Array with addresses of init fct */
        DT_FINI_ARRAY = 26,     /* pointer optional   ?         Array with address of fini fct */
        DT_INIT_ARRAYSZ = 27,   /* value   optional   ?         Size in bytes of DT_INIT_ARRAY */
        DT_FINI_ARRAYSZ = 28,   /* value   optional   ?         Size in bytes of DT_FINI_ARRAY */
        DT_RUNPATH  = 29,       /* ?       optional   ?         Library search path (how diff from DT_RPATH?) */
        DT_FLAGS    = 30,       /* value   optional   ?         Bit flags */
        DT_PREINIT_ARRAY = 32,  /* pointer optional   ?         Array with addrs of preinit fct (aka DT_ENCODING)*/
        DT_PREINIT_ARRAYSZ = 33,/* value   optional   ?         size in bytes of DT_PREINIT_ARRAY */
        DT_NUM      = 34,       /* ?       ?          ?         "number used"? */

        DT_GNU_PRELINKED =0x6ffffdf5,/*value ?          ?         Prelinking time stamp */
        DT_GNU_CONFLICTSZ=0x6ffffdf6,/*value ?          ?         Size of conflict section */
        DT_GNU_LIBLISTSZ=0x6ffffdf7,/*value  ?          ?         Size of library list */
        DT_CHECKSUM = 0x6ffffdf8, /* value   ?          ?         ? */
        DT_PLTPADSZ = 0x6ffffdf9, /* value   ?          ?         ? */
        DT_MOVEENT  = 0x6ffffdfa, /* value   ?          ?         ? */
        DT_MOVESZ   = 0x6ffffdfb, /* value   ?          ?         ? */
        DT_FEATURE_1= 0x6ffffdfc, /* value   ?          ?         Feature selection (DTF_*) */
        DT_POSFLAG_1= 0x6ffffdfd, /* value   ?          ?         Flag for DT_* entries affecting next entry */
        DT_SYMINSZ  = 0x6ffffdfe, /* value   ?          ?         Size of syminfo table in bytes */
        DT_SYMINENT = 0x6ffffdff, /* value   ?          ?         Size of each syminfo table entry */

        DT_GNU_HASH = 0x6ffffef5, /* pointer ?          ?         GNU-style hash table */
        DT_TLSDESC_PLT=0x6ffffef6,/* pointer ?          ?         ? */
        DT_TLSDESC_GOT=0x6ffffef7,/* pointer ?          ?         ? */
        DT_GNU_CONFLICT=0x6ffffef8,/*pointer ?          ?         Start of conflict section */
        DT_GNU_LIBLIST=0x6ffffef9,/* pointer ?          ?         Library list */
        DT_CONFIG   = 0x6ffffefa, /* pointer ?          ?         Configuration information */
        DT_DEPAUDIT = 0x6ffffefb, /* pointer ?          ?         Dependency auditing */
        DT_AUDIT    = 0x6ffffefc, /* pointer ?          ?         Object auditing */
        DT_PLTPAD   = 0x6ffffefd, /* pointer ?          ?         PLT padding */
        DT_MOVETAB  = 0x6ffffefe, /* pointer ?          ?         Move table */
        DT_SYMINFO  = 0x6ffffeff, /* pointer ?          ?         Syminfo table */

        DT_VERSYM   = 0x6ffffff0, /* pointer ?          ?         ? */
        DT_RELACOUNT= 0x6ffffff9, /* value   ?          ?         ? */
        DT_RELCOUNT = 0x6ffffffa, /* value   ?          ?         ? */
        DT_FLAGS_1  = 0x6ffffffb, /* value   ?          ?         Sun state flags */
        DT_VERDEF   = 0x6ffffffc, /* pointer ?          ?         Sun version definition table */
        DT_VERDEFNUM= 0x6ffffffd, /* value   ?          ?         Sun number of version definitions */
        DT_VERNEED  = 0x6ffffffe, /* pointer ?          ?         Sun needed versions table */
        DT_VERNEEDNUM=0x6fffffff, /* value   ?          ?         Sun number of needed versions */

        DT_AUXILIARY= 0x7ffffffd, /* pointer ?          ?         Sun shared obj to load before self */
        DT_FILTER   = 0x7fffffff  /* pointer ?          ?         Shared object ot get values from */
    };

#ifdef _MSC_VER
# pragma pack (1)
#endif

    /** Disk format. */
    struct Elf32DynamicEntry_disk {
        uint32_t            d_tag;                  /**< Entry type, one of the DT_* constants */
        uint32_t            d_val;                  /**< Tag's value */
    }
#if !defined(SWIG) && !defined(_MSC_VER)
    __attribute__((packed))
#endif
    ;

    struct Elf64DynamicEntry_disk {
        uint64_t            d_tag;                  /**< Entry type, one of the DT_* constants */
        uint64_t            d_val;                  /**< Tag's value */
    }
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
    /** Property: Tag.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    SgAsmElfDynamicEntry::EntryType d_tag = SgAsmElfDynamicEntry::DT_NULL;

    /** Property: Value.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta, large]]
    Rose::BinaryAnalysis::RelativeVirtualAddress d_val;

    /** Property: Name.
     *
     * @{ */
    [[using Rosebud: rosetta, traverse, mutators()]]
    SgAsmGenericString* name = nullptr;

    void set_name(SgAsmGenericString*);
    /** @} */

    /** Property: Padding byte values. */
    [[using Rosebud: rosetta, large]]
    SgUnsignedCharList extra;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Constructor that links the object into the AST. */
    explicit SgAsmElfDynamicEntry(SgAsmElfDynamicSection *dynsec);

    /** Initialize object by parsing the file.
     *
     * @{ */
    void parse(Rose::BinaryAnalysis::ByteOrder::Endianness, const SgAsmElfDynamicEntry::Elf32DynamicEntry_disk*);
    void parse(Rose::BinaryAnalysis::ByteOrder::Endianness, const SgAsmElfDynamicEntry::Elf64DynamicEntry_disk*);
    /** @} */

    /** Convert object to disk representation.
     *
     * @{ */
    void *encode(Rose::BinaryAnalysis::ByteOrder::Endianness, SgAsmElfDynamicEntry::Elf32DynamicEntry_disk*) const;
    void *encode(Rose::BinaryAnalysis::ByteOrder::Endianness, SgAsmElfDynamicEntry::Elf64DynamicEntry_disk*) const;
    /** @} */

    /** Print some debugging info */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

    /** Convert Dynamic Entry Tag to a string */
    static std::string toString(SgAsmElfDynamicEntry::EntryType);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    static std::string to_string(SgAsmElfDynamicEntry::EntryType) ROSE_DEPRECATED("use toString");
};
