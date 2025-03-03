#include <Rose/BinaryAnalysis/Address.h>
#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>

#ifdef ROSE_IMPL
#include <SgAsmBasicString.h>
#endif

/** A single imported object.
 *
 *  Each of these nodes is created from a combination of the PE Import Lookup Table and the PE Import Address Table, pointers to
 *  which are stored in the Import Directory (@ref SgAsmPEImportDirectory). This node represents a single import object described by
 *  data structures referenced by a PE Import Directory.  Such a node represents data from two, possibly three, distinct data
 *  structures in the PE file: (1) An entry in the Import Lookup Table, (2) an entry in the Import Address Table, and (3) an
 *  optional Hint/Name pair in the (implicit) Hint/Name Table. */
class SgAsmPEImportItem: public SgAsmExecutableFileFormat {
public:
    /** Property: Whether ordinal number is stored.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    bool by_ordinal = true;

    /** Property: Ordinal number
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned ordinal = 0;

    /** Property: Export name table hint if not ordinal.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned hint = 0;

    /** Property: Name if not by ordinal.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmGenericString* name = createAndParent<SgAsmBasicString>(this);

    /** Property: RVA of hint/name pair.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, large]]
    Rose::BinaryAnalysis::RelativeVirtualAddress hintname_rva;

    /** Property: File bytes allocated for hint/name pair.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    size_t hintname_nalloc = 0;

    /** Property: Bound RVA from IAT if not a copy of ILT.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, large]]
    Rose::BinaryAnalysis::RelativeVirtualAddress bound_rva;

    /** Property: Whether the resolved address has been written to the memory map.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    bool iat_written = false;

public:
    explicit SgAsmPEImportItem(SgAsmPEImportItemList *parent);
    explicit SgAsmPEImportItem(SgAsmPEImportDirectory*);
    SgAsmPEImportItem(SgAsmPEImportDirectory*, const std::string &name, unsigned hint=0);
    SgAsmPEImportItem(SgAsmPEImportDirectory*, const unsigned ordinal);

    virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

    /** Bytes needed to store hint/name pair.
     *
     *  A hint/name pair consists of a two-byte, little endian, unsigned hint and a NUL-terminated ASCII string.  An
     *  optional zero byte padding appears after the string's NUL terminator if necessary to make the total size of the
     *  hint/name pair a multiple of two. */
    size_t hintNameRequiredSize() const;

    /** Virtual address of an IAT entry.
     *
     *  Returns the virtual address of the IAT slot for this import item.  This import item must be linked into the AST in
     *  order for this method to succeed. */
    Rose::BinaryAnalysis::Address get_iatEntryVa() const;

private:
    void initFromParent(SgAsmPEImportItemList *parent);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    size_t hintname_required_size() const ROSE_DEPRECATED("use hintNameRequiredSize");
    Rose::BinaryAnalysis::Address get_iat_entry_va() const ROSE_DEPRECATED("use get_iatEntryVa");

};
