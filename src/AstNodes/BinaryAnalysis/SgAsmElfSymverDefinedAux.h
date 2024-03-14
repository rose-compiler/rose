#include <Rose/BinaryAnalysis/ByteOrder.h>

#ifdef ROSE_IMPL
#include <SgAsmGenericString.h>
#endif

/** Auxiliary data for an ELF Symbol Version. */
class SgAsmElfSymverDefinedAux: public SgAsmExecutableFileFormat {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#ifdef _MSC_VER
# pragma pack (1)
#endif

    /** Disk format. The format is the same for 32bit and 64bit. */
    struct ElfSymverDefinedAux_disk {
        uint32_t      vda_name;                     /**< Offset (in bytes) to strings table to name string */
        uint32_t      vda_next;                     /**< Offset (in bytes) to next verdaux entry */
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
    /** Property: Name. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmGenericString* name = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Constructor linking object into AST.
     *
     *  This constructor links this new Aux into the specified Entry of the specified Symbol Version Definition Table. */
    explicit SgAsmElfSymverDefinedAux(SgAsmElfSymverDefinedEntry *symver_def_entry, SgAsmElfSymverDefinedSection *symver_def_sec);

    /** Initialize this object with data parsed from a file. */
    void parse(Rose::BinaryAnalysis::ByteOrder::Endianness, const SgAsmElfSymverDefinedAux::ElfSymverDefinedAux_disk*);

    /** Convert this object into the disk format record to be written back to the Symbol Version Definition Table. */
    void *encode(Rose::BinaryAnalysis::ByteOrder::Endianness, SgAsmElfSymverDefinedAux::ElfSymverDefinedAux_disk*) const;

    /** Print debugging information.
     *
     *  Shows information about the specified auxiliary data for an entry in the Symbol Version Definition Table. Note that
     *  in order to have a more compact output, @ref SgAsmElfSymverDefinedEntry::dump prints the @ref
     *  SgAsmElfSymverDefinedAux objects explicitly rather than calling this method. */
    virtual void dump(FILE *f, const char *prefix, ssize_t idx) const;
};
