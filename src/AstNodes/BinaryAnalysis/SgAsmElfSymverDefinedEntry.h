#include <Rose/BinaryAnalysis/ByteOrder.h>

#ifdef ROSE_IMPL
#include <SgAsmElfSymverDefinedAuxList.h>
#endif

/** One entry from an ELF symbol version definition table. */
class SgAsmElfSymverDefinedEntry: public SgAsmExecutableFileFormat {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#ifdef _MSC_VER
# pragma pack (1)
#endif
    /** Disk format. 32- and 64-bit formats are both the same. */
    struct ElfSymverDefinedEntry_disk {
        uint16_t      vd_version;                   /**< version of this struct: This field shall be set to 1 */
        uint16_t      vd_flags;                     /**< Version information flag bitmask */
        uint16_t      vd_ndx;                       /**< Version index of this entry */
        uint16_t      vd_cnt;                       /**< Number of verdaux entries @see SgAsmElfSymverDefinedAux */
        uint32_t      vd_hash;                      /**< Hash of version name */
        uint32_t      vd_aux;                       /**< Offset (in bytes) to start of array of verdaux entries */
        uint32_t      vd_next;                      /**< Offset (in bytes) to next verdef entry */
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
    /** Property: Version.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    size_t version = 0;

    /** Property: Flags.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    int flags = 0;

    /** Property: Index.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    size_t index = 0;

    /** Property: Hash.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    uint32_t hash = 0;

    /** Property: Entries.
     *
     *  List of auxilliary entries for this version definition.  Rather than storing the list here directly, we point to a
     *  node whose only purpose is to hold the list. This is due to limitations of ROSETTA. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmElfSymverDefinedAuxList* entries = createAndParent<SgAsmElfSymverDefinedAuxList>(this);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Constructor linking ojbec into the AST. */
    explicit SgAsmElfSymverDefinedEntry(SgAsmElfSymverDefinedSection *symver_defined);

    /** Initialize by parsing information from the file. */
    void parse(Rose::BinaryAnalysis::ByteOrder::Endianness, const SgAsmElfSymverDefinedEntry::ElfSymverDefinedEntry_disk*);

    /** Convert to the disk format. */
    void *encode(Rose::BinaryAnalysis::ByteOrder::Endianness, SgAsmElfSymverDefinedEntry::ElfSymverDefinedEntry_disk*) const;

    /** Print some debugging info. */
    virtual void dump(FILE *f, const char *prefix, ssize_t idx) const;
};
