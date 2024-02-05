#include <Rose/BinaryAnalysis/AddressIntervalSet.h>
#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>

/** One import directory per library.
 *
 *  The import directory points to the Import Lookup Table and Import Address Table. ROSE no longer represents the ILT and IAT with
 *  separate IR nodes. */
class SgAsmPEImportDirectory: public SgAsmExecutableFileFormat {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#ifdef _MSC_VER
# pragma pack (1)
#endif
    struct PEImportDirectory_disk {
        uint32_t ilt_rva;               /* 0x00 Import Lookup Table RVA */
        uint32_t time;                  /* 0x04 Zero until the image is bound, then time stamp of the DLL */
        uint32_t forwarder_chain;       /* 0x08 Index of the first forwarder chain */
        uint32_t dll_name_rva;          /* 0x0c address of NUL-terminated DLL name */
        uint32_t iat_rva;               /* 0x10 Import Address Table (Thunk Table) RVA */
    }                                       /* 0x14 */
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
    /** Property: Name of library from which to import.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmGenericString* dllName = nullptr;

    /** Property: Address of the import library name.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, large]]
    Rose::BinaryAnalysis::RelativeVirtualAddress dllNameRva;

    /** Property: Bytes allocated in the file for the name.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    size_t dll_name_nalloc = 0;

    /** Property: Time.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    time_t time = 0;

    /** Property: Forwarder chain.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned forwarder_chain = 0;

    /** Property: Starting address of the ILT.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, large]]
    Rose::BinaryAnalysis::RelativeVirtualAddress ilt_rva;

    /** Property: Bytes allocated in the file for the ILT.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    size_t ilt_nalloc = 0;

    /** Property: IAT RVA.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, large]]
    Rose::BinaryAnalysis::RelativeVirtualAddress iat_rva;

    /** Property: Bytes allocated in the file for the IAT.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    size_t iat_nalloc = 0;

    /** Property: List of imports.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmPEImportItemList* imports = createAndParent<SgAsmPEImportItemList>(this);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    explicit SgAsmPEImportDirectory(SgAsmPEImportSection *isec, const std::string &dll_name="");

    /** Parse an import directory.
     *
     *  The import directory is parsed from the specified virtual address via the PE header's loader map. Return value is
     *  this directory entry on success, or the null pointer if the entry is all zero (which marks the end of the directory
     *  list). The @p isLastEntry is true if the caller thinks this should be an all-zero entry. */
    SgAsmPEImportDirectory *parse(rose_addr_t va, bool isLastEntry);

    /** Allocates space for this import directory's name, import lookup table, and import address table.
     *
     *  The items are allocated beginning at the specified relative virtual address. Items are reallocated if they are not
     *  allocated or if they are allocated in the same section to which start_rva points (the import section).  They are
     *  not reallocated if they already exist in some other section. The return value is the number of bytes allocated in
     *  the import section.  Upon return, this directory's address data members are initialized with possibly new
     *  values. */
    size_t reallocate(Rose::BinaryAnalysis::RelativeVirtualAddress starting_rva);

    /** Encode an import directory entry back into disk format */
    void *encode(SgAsmPEImportDirectory::PEImportDirectory_disk*) const;
    virtual void unparse(std::ostream&, const SgAsmPEImportSection*, size_t idx) const;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

    /** Number of bytes required for the table.
     *
     *  Returns the number of bytes required for the entire IAT or ILT (including the zero terminator) as it is currently
     *  defined in the Import Directory.  The returned size does not include space required to store any Hint/Name pairs,
     *  which are outside the ILT/IAT but pointed to by the ILT/IAT. */
    size_t iatRequiredSize() const;

    /** Find an import item in an import directory.
     *
     *  Returns the index of the specified import item in this directory, or -1 if the import item is not a child of this
     *  directory.  The hint index is checked first. */
    int findImportItem(const SgAsmPEImportItem *item, int hint=0) const;

    /** Obtains the virtual address of the Hint/Name Table.
     *
     *  The Hint/Name Table is an implicit table--the PE file format specification talks about such a table, but it is not
     *  actually defined anywhere in the PE file.  Instead, various Import Lookup Table and Import Address Table entries
     *  might point to individual Hint/Name pairs, which collectively form an implicit Hint/Name Table.  There is no
     *  requirement that the Hint/Name pairs are contiguous in the address space, and indeed they often are not.
     *  Therefore, the only way to describe the location of the Hint/Name Table is by a list of addresses.
     *
     *  This function will scan this Import Directory's import items, observe which items make references to Hint/Name
     *  pairs that have known addresses, and add those areas of virtual memory to the specified extent map.  This function
     *  returns the number of ILT entries that reference a Hint/Name pair. */
    size_t hintNameTableExtent(Rose::BinaryAnalysis::AddressIntervalSet &extent/*in,out*/) const;

private:
    void parse_ilt_iat(const Rose::BinaryAnalysis::RelativeVirtualAddress &table_start, bool assume_bound);
    void unparse_ilt_iat(std::ostream&,const Rose::BinaryAnalysis::RelativeVirtualAddress &table_start, bool assume_bound,
                         size_t nalloc) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmGenericString* get_dll_name() const ROSE_DEPRECATED("use get_dllName");
    void set_dll_name(SgAsmGenericString*) ROSE_DEPRECATED("use set_dllName");
    const Rose::BinaryAnalysis::RelativeVirtualAddress& get_dll_name_rva() const ROSE_DEPRECATED("use get_dllNameRva");
    Rose::BinaryAnalysis::RelativeVirtualAddress& get_dll_name_rva() ROSE_DEPRECATED("use get_dllNameRva");
    void set_dll_name_rva(const Rose::BinaryAnalysis::RelativeVirtualAddress&) ROSE_DEPRECATED("use set_dllNameRva");
    size_t iat_required_size() const ROSE_DEPRECATED("use iatRequiredSize");
    int find_import_item(const SgAsmPEImportItem*, int=0) const ROSE_DEPRECATED("use findImportItem");
    size_t hintname_table_extent(Rose::BinaryAnalysis::AddressIntervalSet&) const ROSE_DEPRECATED("use hintNameTableExtent");
};
