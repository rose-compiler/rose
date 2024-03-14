#include <Rose/BinaryAnalysis/ByteOrder.h>
#include <sageContainer.h>

/** Represents a single ELF symbol.
 *
 *  Most of the properties of this node correspond directly with properties defined by the ELF specification. Their
 *  documentation is not replicated here -- refer to the specification. */
class SgAsmElfSymbol: public SgAsmGenericSymbol {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    enum ElfSymBinding {
        STB_LOCAL=0,
        STB_GLOBAL=1,
        STB_WEAK=2
    };

    enum ElfSymType {
        STT_NOTYPE      = 0,                    /**< Unspecified type */
        STT_OBJECT      = 1,                    /**< Data object */
        STT_FUNC        = 2,                    /**< Code object */
        STT_SECTION     = 3,                    /**< Associated with a section */
        STT_FILE        = 4,                    /**< Name of a file */
        STT_COMMON      = 5,                    /**< Common data object */
        STT_TLS         = 6,                    /**< Thread-local data object */
        STT_IFUNC       = 10                    /**< Indirect function. Function call w/out args results in reloc value. */
    };

#ifdef _MSC_VER
# pragma pack (1)
#endif

    /** 32-bit format of an ELF symbol. */
    struct Elf32SymbolEntry_disk {
        uint32_t      st_name;                      /**< Name offset into string table */
        uint32_t      st_value;                     /**< Value: absolute value, address, etc. depending on sym type */
        uint32_t      st_size;                      /**< Symbol size in bytes */
        unsigned char st_info;                      /**< Type and binding attributes */
        unsigned char st_res1;                      /**< Reserved; always zero */
        uint16_t      st_shndx;                     /**< Section index or special meaning */
    }
#if !defined(SWIG) && !defined(_MSC_VER)
    __attribute__((packed))
#endif
    ;

    struct Elf64SymbolEntry_disk {
        uint32_t      st_name;
        unsigned char st_info;
        unsigned char st_res1;
        uint16_t      st_shndx;
        uint64_t      st_value;
        uint64_t      st_size;
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
    /** Property: Info.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    unsigned char st_info = 0;

    /** Property: Reserved byte.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    unsigned char st_res1 = 0;

    /** Property: shndx.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    unsigned st_shndx = 0;

    /** Property: size.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    rose_addr_t st_size = 0;

    /** Property: Extra data.
     *
     *  Bytes that are not part of the symbol but which appear in the table as reserved or padding. */
    [[using Rosebud: rosetta, large]]
    SgUnsignedCharList extra;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Constructor that adds the symbol to a symbol table. */
    explicit SgAsmElfSymbol(SgAsmElfSymbolSection*);

    /** Initialize symbol by parsing a symbol table entry.
     *
     *  An ELF String Section must be supplied in order to get the symbol name. */
    void parse(Rose::BinaryAnalysis::ByteOrder::Endianness, const SgAsmElfSymbol::Elf32SymbolEntry_disk*);

    /** Initialize symbol by parsing a symbol table entry.
     *
     *  An ELF String Section must be supplied in order to get the symbol name. */
    void parse(Rose::BinaryAnalysis::ByteOrder::Endianness, const SgAsmElfSymbol::Elf64SymbolEntry_disk*);

    /** Encode a symbol into disk format.
     *
     * @{ */
    void *encode(Rose::BinaryAnalysis::ByteOrder::Endianness, SgAsmElfSymbol::Elf32SymbolEntry_disk*) const;
    void *encode(Rose::BinaryAnalysis::ByteOrder::Endianness, SgAsmElfSymbol::Elf64SymbolEntry_disk*) const;
    /** @} */

    /** Print some debugging info.
     *
     *  The 'section' is an optional section pointer for the st_shndx member.
     *
     * @{ */
    virtual void dump(FILE *f, const char *prefix, ssize_t idx) const override;
    void dump(FILE*, const char *prefix, ssize_t idx, SgAsmGenericSection*) const;
    /** @} */

    /** Returns binding as an enum constant. */
    SgAsmElfSymbol::ElfSymBinding get_elfBinding() const;

    /** Returns type as an enum constant. */
    SgAsmElfSymbol::ElfSymType get_elfType() const;

    /** Converts enum constant to string. */
    static std::string toString(SgAsmElfSymbol::ElfSymBinding);

    /** Converts enum constant to string. */
    static std::string toString(SgAsmElfSymbol::ElfSymType);

private:
    void parse_common();                            // initialization common to all parse() methods

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmElfSymbol::ElfSymBinding get_elf_binding() const ROSE_DEPRECATED("use get_elfBinding");
    SgAsmElfSymbol::ElfSymType get_elf_type() const ROSE_DEPRECATED("use get_elfType");
    static std::string to_string(SgAsmElfSymbol::ElfSymBinding) ROSE_DEPRECATED("use toString");
    static std::string to_string(SgAsmElfSymbol::ElfSymType) ROSE_DEPRECATED("use toString");
};
