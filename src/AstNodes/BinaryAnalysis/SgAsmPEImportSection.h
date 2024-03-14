#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>

#ifdef ROSE_IMPL
#include <SgAsmPEImportDirectoryList.h>
#endif

/** Portable Executable Import Section.
 *
 *  Constructs an SgAsmPEImportSection that represents either a PE ".idata" section as defined by the PE Section Table, or
 *  a PE Import Table as described by the RVA/Size pairs at the end of the NT Optional Header. The ".idata" section and PE
 *  Import Table both have the same format (only important fields shown):
 *
 *  @par Import Section
 *  An Import Section consists of a list of Import Directory Entries ("Directories"), one per dynamically linked library,
 *  followed by an all-zero Directory entry that marks the end of the list.  ROSE does not explicitly store the terminating
 *  entry, and wherever "Directories" appears in the following description it does not include this null directory.
 *
 *  @par Import Directory
 *  Each directory points to (by relative virtual address (RVA)) both an Import Lookup Table (ILT) and Import Address Table
 *  (IAT).
 *
 *  @par Import Lookup Table (and Import Address Table)
 *  The Import Lookup Table (ILT) and Import Address Table (IAT) have identical structure.  ROSE represents them as a list
 *  of SgAsmPEImportItem in the Import Directory.  The ILT and IAT are parallel arrays of 32- or 64-bit (PE32 or PE32+)
 *  entries terminated with an all-zero entry.  The terminating entry is not stored explicitly by ROSE.  The entries are
 *  identical for both ILTs and IATs.
 *
 *  @par Import Lookup Table Entry (and Import Address Table Entry)
 *  Entries for ILTs and IATs are structurally identical.  They are 32- or 64-bit vectors.  The most significant bit
 *  (31/63) indicates whether the remaining bits are an Ordinal (when set) or Hint/Name address (when clear).  Ordinals are
 *  represented by the low-order 16 bits and Hint/Name addresses are stored in the low-order 31 bits.  All other bits must
 *  be zero according to the PE specification.  Hint/Name addresses are relative virtual addresses of entries in the
 *  (implicit) Hint/Name Table. When a function is bound by the dynamic linkter, its IAT Entry within process memory is
 *  overwritten with the virtual address of the bound function.
 *
 *  @par Hint/Name Table
 *  Some Import Lookup Table (and Import Address Table) entries contain a Hint/Name Table Entry RVA.  The Hint/Name Table
 *  Entries collectively form the Hint/Name Table, but there is no requirement that the entries appear in any particular
 *  order or even that they appear contiguously in memory.  In other words, the Hint/Name Table is a conceptual object
 *  rather than a true table in the PE file.
 *
 *
 * @verbatim
    +------------ Import Section -------------+                         (SgAsmPEImportSection)
    |                                         |
    |                                         |
    |  +------- Import Directory #0 ------+   |                         (SgAsmPEImportDirectory)
    |  |   1. Import Lookup Table RVA     |   |
    |  |   2. Date/time stamp             |   |
    |  |   3. Forwarder chain index       |   |
    |  |   4. Name RVA                    |   |
    |  |   5. Import Address Table RVA    |   |
    |  +----------------------------------+   |
    |                                         |
    |                                         |
    |  +------- Import Directory #1 ------+   |
    |  |   1. Import Lookup Table RVA     |--------+
    |  |   2. Date/time stamp             |   |    |
    |  |   3. Forwarder chain index       |   |    |
    |  |   4. Name RVA                    |   |    |
    |  |   5. Import Address Table RVA    |------- | -------+
    |  +----------------------------------+   |    |        |
    |                                         |    |        |
    |         . . .                           |    |        |
    |                                         |    |        |
    |  +------- Import Directory #N ------+   |    |        |
    |  |                                  |   |    |        |
    |  |   Terminating directory is       |   |    |        |
    |  |   zero filled.                   |   |    |        |
    |  |                                  |   |    |        |
    |  |                                  |   |    |        |
    |  +----------------------------------+   |    |        |
    |                                         |    |        |
    +-----------------------------------------+    |        |           (Entries of the ILT and IAT are combined into
                                                   |        |            SgAsmPEImportItem objects.)
                                                   |        |
                                                   |        |
    +----------- Import Lookup Table ---------+ <--+        +-->  +----------- Import Address Table --------+
    | #0  32/64-bit vector                    |                   | #0  32/64-bit vector or VA when bound   |
    |                                         |   These arrays    |                                         |
    | #1  32/64-bit vector                    |   are parallel    | #1  32/64-bit vector or VA when bound   |
    |                      \                  |                   |                                         |
    |     ...               \when used as     |                   |     ...                                 |
    |                        \a Hint/Name     |                   |                                         |
    | #N  32/64-bit zero      \RVA            |                   | #N  32/64-bit zero                      |
    +--------------------------\--------------+                   +-----------------------------------------+
                                \
                                 \
                                  |
    + - - - - -  Hint/Name Table  | - - - - - +           The Hint/Name Table doesn't actually
                                  v                       exist explicitly--there is no pointer
    |  +------ Hint/Name ----------------+    |           to the beginning of the table and no
       |  1. 2-byte index ENPT           |                requirement that the entries be in any
    |  |  2. NUL-terminated name         |    |           particular order, or even contiguous.
       |  3. Optional extran NUL         |
    |  +---------------------------------+    |           "ENPT" means Export Name Pointer Table,
                                                          which is a table in the linked-to
    |          . . .                          |           shared library.

    |  +------ Hint/Name ----------------+    |
       |  1. 2-byte index ENPT           |                              (SgAsmPEImportHNTEntry)
    |  |  2. NUL-terminated name         |    |
       |  3. Optional extran NUL         |
    |  +---------------------------------+    |

    + - - - - - - - - - - - - - - - - - - - - +
@endverbatim
 *
 * When parsing an Import Directory, ROSE assumes that the IAT contains ordinals and/or hint/name addresses rather than
 * bound addresses.  ROSE checks that the IAT entries are compatible with the ILT entries there were already parsed and if
 * an inconsistency is detected then a warning is issued and ROSE assumes that the IAT entry is a bound value instead.
 * Passing true as the @p assume_bound argument for the parser will cause ROSE to not issue such warnings and immediately
 * assume that all IAT entries are bound addresses.  One can therefore find the conflicting entries by looking for
 * SgAsmImportItem objects that are created with a non-zero bound address.
 *
 * The IAT is often required to be allocated at a fixed address, often the beginning of the ".rdata" section.  Increasing
 * the size of the IAT by adding more items to the import list(s) can be problematic because ROSE is unable to safely write
 * beyond the end of the original IAT.  We require the user to manually allocate space for the new IAT and tell the
 * SgAsmPEImportDirectory object the location and size of the allocated space before unparsing.  On a related note, due to
 * ROSE allocators being section-local, reallocation of an Import Section does not cause reallocation of ILTs, Hint/Name
 * pairs, or DLL names that have addresses outside the Import Section.  If these items' sizes increase, the items will be
 * truncated when written back to disk.  The reallocation happens automatically for all import-related objects that are
 * either bound to the import section or have a null RVA, so one method of getting things reallocated is to traverse the
 * AST and null their RVAs:
 *
 * @code
 *  struct Traversal: public AstSimpleTraversal {
 *      void visit(SgNode *node) {
 *          SgAsmPEImportDirectory *idir = isSgAsmPEImportDirectory(node);
 *          SgAsmPEImportItem *import = isSgAsmPEImportItem(node);
 *          static const Rose::BinaryAnalysis::RelativeVirtualAddress nil(0);
 *
 *          if (idir) {
 *              idir->set_dll_name_rva(nil);
 *              idir->set_ilt_rva(nil);
 *              idir->set_iat_rva(nil);
 *          }
 *
 *          if (import)
 *              idir->set_hintname_rva(nil);
 *     }
 *  };
 * @endcode
 *
 * @sa
 *      SgAsmPEImportDirectory
 *      SgAsmPEImportItem
 */
class SgAsmPEImportSection: public SgAsmPESection {
public:
    /** Property: List of import directories.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmPEImportDirectoryList* importDirectories = createAndParent<SgAsmPEImportDirectoryList>(this);

public:
    explicit SgAsmPEImportSection(SgAsmPEFileHeader*);
    virtual SgAsmPEImportSection *parse() override;
    virtual bool reallocate() override;
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

    /** Add an import directory to the end of the import directory list. */
    void addImportDirectory(SgAsmPEImportDirectory*);

    /** Remove an import directory from the import directory list. Does not delete it. */
    void removeImportDirectory(SgAsmPEImportDirectory*);
    static bool showImportMessage();
    static void importMessageReset();

    /** Reallocate space for all Import Address Table.
     *
     *  This method traverses the AST beginning at this PE Import Section and assigns addresses and sizes to all Import
     *  Address Tables (IATs).  The first IAT is given the @p start_at RVA and its size is reset to what ever size is
     *  needed to store the entire table.  Each subsequent IAT is given the next available address and it's size is also
     *  updated.  The result is that all the IATs under this Import Section are given addresses and sizes that make them
     *  contiguous in memory. This method returns the total number of bytes required for all the IATs. */
    size_t reallocateIats(Rose::BinaryAnalysis::RelativeVirtualAddress start_at);

private:
    static size_t mesg_nprinted; //counter for import_mesg()

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmPEImportDirectoryList* get_import_directories() const ROSE_DEPRECATED("use get_importDirectories");
    void set_import_directories(SgAsmPEImportDirectoryList*) ROSE_DEPRECATED("use set_importDirectories");
    void add_import_directory(SgAsmPEImportDirectory*) ROSE_DEPRECATED("use addImportDirectory");
    void remove_import_directory(SgAsmPEImportDirectory*) ROSE_DEPRECATED("use removeImportDirectory");
    static bool show_import_mesg() ROSE_DEPRECATED("use showImportMessage");
    static void import_mesg_reset() ROSE_DEPRECATED("use importMessageReset");
    size_t reallocate_iats(Rose::BinaryAnalysis::RelativeVirtualAddress) ROSE_DEPRECATED("use reallocateIats");
};
