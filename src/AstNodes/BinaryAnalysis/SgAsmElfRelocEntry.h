#include <Rose/BinaryAnalysis/Address.h>
#include <Rose/BinaryAnalysis/ByteOrder.h>
#include <sageContainer.h>

/** One entry of an ELF relocation table. */
class SgAsmElfRelocEntry: public SgAsmExecutableFileFormat {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Relocation Type. In host order.  All other values are reserved. */
    enum RelocType{
        // Intel 80386 specific definitions.
        R_386_NONE         =0,  /**< No reloc */
        R_386_32           =1,  /**< Direct 32 bit  */
        R_386_PC32         =2,  /**< PC relative 32 bit */
        R_386_GOT32        =3,  /**< 32 bit GOT entry */
        R_386_PLT32        =4,  /**< 32 bit PLT address */
        R_386_COPY         =5,  /**< Copy symbol at runtime */
        R_386_GLOB_DAT     =6,  /**< Create GOT entry */
        R_386_JMP_SLOT     =7,  /**< Create PLT entry */
        R_386_RELATIVE     =8,  /**< Adjust by program base */
        R_386_GOTOFF       =9,  /**< 32 bit offset to GOT */
        R_386_GOTPC        =10, /**< 32 bit PC relative offset to GOT */
        R_386_32PLT        =11,
        R_386_TLS_TPOFF    =14, /**< Offset in static TLS block */
        R_386_TLS_IE       =15, /**< Address of GOT entry for static TLS block offset */
        R_386_TLS_GOTIE    =16, /**< GOT entry for static TLS block offset */
        R_386_TLS_LE       =17, /**< Offset relative to static TLS block */
        R_386_TLS_GD       =18, /**< Direct 32 bit for GNU version of general dynamic thread local data */
        R_386_TLS_LDM      =19, /**< Direct 32 bit for GNU version of local dynamic thread local data in LE code */
        R_386_16           =20,
        R_386_PC16         =21,
        R_386_8            =22,
        R_386_PC8          =23,
        R_386_TLS_GD_32    =24, /**< Direct 32 bit for general dynamic thread local data */
        R_386_TLS_GD_PUSH  =25, /**< Tag for pushl in GD TLS code */
        R_386_TLS_GD_CALL  =26, /**< Relocation for call to __tls_get_addr() */
        R_386_TLS_GD_POP   =27, /**< Tag for popl in GD TLS code */
        R_386_TLS_LDM_32   =28, /**< Direct 32 bit for local dynamic thread local data in LE code */
        R_386_TLS_LDM_PUSH =29, /**< Tag for pushl in LDM TLS code */
        R_386_TLS_LDM_CALL =30, /**< Relocation for call to __tls_get_addr() in LDM code */
        R_386_TLS_LDM_POP  =31, /**< Tag for popl in LDM TLS code */
        R_386_TLS_LDO_32   =32, /**< Offset relative to TLS block */
        R_386_TLS_IE_32    =33, /**< GOT entry for negated static TLS block offset */
        R_386_TLS_LE_32    =34, /**< Negated offset relative to static TLS block */
        R_386_TLS_DTPMOD32 =35, /**< ID of module containing symbol */
        R_386_TLS_DTPOFF32 =36, /**< Offset in TLS block */
        R_386_TLS_TPOFF32  =37, /**< Negated offset in static TLS block */

        // First Entry for X86-64
        R_X86_64_NONE     =100, /**<  No reloc */
        R_X86_64_64       =101, /**<  Direct 64 bit  */
        R_X86_64_PC32     =102, /**<  PC relative 32 bit signed */
        R_X86_64_GOT32    =103, /**<  32 bit GOT entry */
        R_X86_64_PLT32    =104, /**<  32 bit PLT address */
        R_X86_64_COPY     =105, /**<  Copy symbol at runtime */
        R_X86_64_GLOB_DAT =106, /**<  Create GOT entry */
        R_X86_64_JUMP_SLOT=107, /**<  Create PLT entry */
        R_X86_64_RELATIVE =108, /**<  Adjust by program base */
        R_X86_64_GOTPCREL =109, /**<  32 bit signed PC relative offset to GOT */
        R_X86_64_32       =110, /**<  Direct 32 bit zero extended */
        R_X86_64_32S      =111, /**<  Direct 32 bit sign extended */
        R_X86_64_16       =112, /**<  Direct 16 bit zero extended */
        R_X86_64_PC16     =113, /**<  16 bit sign extended pc relative */
        R_X86_64_8        =114, /**<  Direct 8 bit sign extended  */
        R_X86_64_PC8      =115, /**<  8 bit sign extended pc relative */
        R_X86_64_DTPMOD64 =116, /**<  ID of module containing symbol */
        R_X86_64_DTPOFF64 =117, /**<  Offset in module's TLS block */
        R_X86_64_TPOFF64  =118, /**<  Offset in initial TLS block */
        R_X86_64_TLSGD    =119, /**<  32 bit signed PC relative offset to two GOT entries for GD symbol */
        R_X86_64_TLSLD    =120, /**<  32 bit signed PC relative offset to two GOT entries for LD symbol */
        R_X86_64_DTPOFF32 =121, /**<  Offset in TLS block */
        R_X86_64_GOTTPOFF =122, /**<  32 bit signed PC relative offset to GOT entry for IE symbol */
        R_X86_64_TPOFF32  =123  /**<  Offset in initial TLS block */
    };

#ifdef _MSC_VER
# pragma pack (1)
#endif

    struct Elf32RelaEntry_disk {
        uint32_t        r_offset;
        uint32_t        r_info;
        uint32_t        r_addend;
    }
#if !defined(SWIG) && !defined(_MSC_VER)
    __attribute__((packed))
#endif
    ;

    struct Elf64RelaEntry_disk {
        uint64_t        r_offset;
        uint64_t        r_info;
        uint64_t        r_addend;
    }
#if !defined(SWIG) && !defined(_MSC_VER)
    __attribute__((packed))
#endif
    ;

    struct Elf32RelEntry_disk {
        uint32_t        r_offset;
        uint32_t        r_info;
    }
#if !defined(SWIG) && !defined(_MSC_VER)
    __attribute__((packed))
#endif
    ;

    struct Elf64RelEntry_disk {
        uint64_t        r_offset;
        uint64_t        r_info;
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
    /** Property: Offset.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address r_offset = 0;

    /** Property: Addend.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address r_addend = 0;

    /** Property: Sym.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    unsigned long sym = 0;

    /** Property: Type.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    SgAsmElfRelocEntry::RelocType type = R_386_NONE;

    /** Property: Value of padding bytes. */
    [[using Rosebud: rosetta, large]]
    SgUnsignedCharList extra;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Constructor that adds the new entry to the relocation table. */
    SgAsmElfRelocEntry(SgAsmElfRelocSection*);

    /** Initialize object by parsing from file.
     *
     * @{ */
    void parse(Rose::BinaryAnalysis::ByteOrder::Endianness sex, const SgAsmElfRelocEntry::Elf32RelaEntry_disk *disk);
    void parse(Rose::BinaryAnalysis::ByteOrder::Endianness sex, const SgAsmElfRelocEntry::Elf64RelaEntry_disk *disk);
    void parse(Rose::BinaryAnalysis::ByteOrder::Endianness sex, const SgAsmElfRelocEntry::Elf32RelEntry_disk *disk);
    void parse(Rose::BinaryAnalysis::ByteOrder::Endianness sex, const SgAsmElfRelocEntry::Elf64RelEntry_disk *disk);
    /** @} */

    /** Convert object to on-disk format.
     *
     * @{ */
    void *encode(Rose::BinaryAnalysis::ByteOrder::Endianness, SgAsmElfRelocEntry::Elf32RelaEntry_disk*) const;
    void *encode(Rose::BinaryAnalysis::ByteOrder::Endianness, SgAsmElfRelocEntry::Elf64RelaEntry_disk*) const;
    void *encode(Rose::BinaryAnalysis::ByteOrder::Endianness, SgAsmElfRelocEntry::Elf32RelEntry_disk*) const;
    void *encode(Rose::BinaryAnalysis::ByteOrder::Endianness, SgAsmElfRelocEntry::Elf64RelEntry_disk*) const;
    /** @} */

    /** Print debugging information.
     *
     * @{ */
    void dump(FILE *f, const char *prefix, ssize_t idx, SgAsmElfSymbolSection *symtab) const;
    virtual void dump(FILE *f, const char *prefix, ssize_t idx) const {
        dump(f, prefix, idx, NULL);
    }
    /** @} */

    /** Convert relocation to string for debugging. */
    std::string toString() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    std::string reloc_name() const ROSE_DEPRECATED("use toString");
};
