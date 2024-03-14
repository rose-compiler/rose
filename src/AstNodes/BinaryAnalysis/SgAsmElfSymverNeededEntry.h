#include <Rose/BinaryAnalysis/ByteOrder.h>

#ifdef ROSE_IMPL
#include <SgAsmElfSymverNeededAuxList.h>
#include <SgAsmGenericString.h>
#endif

/** One entry of the ELF symbol version needed table. */
class SgAsmElfSymverNeededEntry: public SgAsmExecutableFileFormat {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#ifdef _MSC_VER
# pragma pack (1)
#endif

    /** Disk format. Same for 32bit and 64bit. */
    struct ElfSymverNeededEntry_disk {
        uint16_t      vn_version;                   /**< version of this struct: This field shall be set to 1 */
        uint16_t      vn_cnt;                       /**< Number of vernaux entries @see SgAsmElfSymverNeededAux */
        uint32_t      vn_file;                      /**< Offset (in bytes) to strings table to file string */
        uint32_t      vn_aux;                       /**< Offset (in bytes) to start of array of vernaux entries */
        uint32_t      vn_next;                      /**< Offset (in bytes) to next verneed entry */
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

    /** Property: File name. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmGenericString* fileName = nullptr;

    /** Property: List of entries.
     *
     *  This is a pointer to an AST node whose only purpose is to hold the list. The reason the list cannot be contained
     *  directly in this node where it's needed is due to ROSETTA limitations. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmElfSymverNeededAuxList* entries = createAndParent<SgAsmElfSymverNeededAuxList>(this);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Constructor that links object into the AST. */
    explicit SgAsmElfSymverNeededEntry(SgAsmElfSymverNeededSection*);

    /** Initialize object by parsing file. */
    void parse(Rose::BinaryAnalysis::ByteOrder::Endianness, const SgAsmElfSymverNeededEntry::ElfSymverNeededEntry_disk*);

    /** Encode object to disk representation. */
    void *encode(Rose::BinaryAnalysis::ByteOrder::Endianness, SgAsmElfSymverNeededEntry::ElfSymverNeededEntry_disk*) const;

    /** Print debugging information. */
    virtual void dump(FILE *f, const char *prefix, ssize_t idx) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmGenericString* get_file_name() const ROSE_DEPRECATED("use get_fileName");
    void set_file_name(SgAsmGenericString*) ROSE_DEPRECATED("use set_fileName");
};
