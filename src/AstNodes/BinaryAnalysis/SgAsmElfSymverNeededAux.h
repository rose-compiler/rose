#include <Rose/BinaryAnalysis/ByteOrder.h>

#ifdef ROSE_IMPL
#include <SgAsmGenericString.h>
#endif

/** Auxiliary info for needed symbol version. */
class SgAsmElfSymverNeededAux: public SgAsmExecutableFileFormat {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#ifdef _MSC_VER
# pragma pack (1)
#endif

    /** Disk format. Same for 32bit and 64bit. */
    struct ElfSymverNeededAux_disk {
        uint32_t      vna_hash;                     /**< Hash of version name */
        uint16_t      vna_flags;                    /**< Version information flag bitmask */
        uint16_t      vna_other;                    /**< Version index of this entry (bit 15 is special) */
        uint32_t      vna_name;                     /**< Offset (in bytes) to strings table to name string */
        uint32_t      vna_next;                     /**< Offset (in bytes) to next vernaux entry */
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
    /** Property: Hash.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    uint32_t hash = 0;

    /** Property: Flags.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    int flags = 0;

    /** Property: Other.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    size_t other = 0;

    /** Property: Name. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmGenericString* name = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Constructor linking object into AST.
     *
     *  This constructor adds this auxiliary information object to the specified entry of the specified GNU Symbol Version
     *  Requirements Table. */
    SgAsmElfSymverNeededAux(SgAsmElfSymverNeededEntry *entry, SgAsmElfSymverNeededSection *symver);

    /** Initialize this auxiliary record by parsing data from the file. */
    void parse(Rose::BinaryAnalysis::ByteOrder::Endianness, const SgAsmElfSymverNeededAux::ElfSymverNeededAux_disk*);

    /** Encode this auxiliary record into a format that can be written to a file. */
    void *encode(Rose::BinaryAnalysis::ByteOrder::Endianness, SgAsmElfSymverNeededAux::ElfSymverNeededAux_disk*) const;

    /** Print debugging information.
     *
     *  Prints debugging information about this auxiliary record of an entry of the GNU Symbol Version Requirements
     *  Table. Note that this method is not normally called since @ref SgAsmElfSymverNeededEntry::dump prints the auxiliary
     *  information explicitly for a more compact listing. */
    virtual void dump(FILE *f, const char *prefix, ssize_t idx) const;
};
