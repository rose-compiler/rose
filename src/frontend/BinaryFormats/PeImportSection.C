/* SgAsmPEImportSection, normally named ".idata" if it appears in the section table. Most modern PE executables don't create a
 * special section in the section table, but rather have the SgAsmPEFileHeader::PAIR_IMPORTS RVA/Size pair point to part of the
 * memory mapped from another read-only section, such as the ".text" section. */
#include "sage3basic.h"
#include <stdarg.h>

/** @class SgAsmPEImportSection
 *
 *  Portable Executable Import Section.
 *
 *  Constructs an SgAsmPEImportSection that represents either a PE ".idata" section as defined by the PE Section Table, or a PE
 *  Import Table as described by the RVA/Size pairs at the end of the NT Optional Header. The ".idata" section and PE Import
 *  Table both have the same format (only important fields shown):
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
 *  The Import Lookup Table (ILT) and Import Address Table (IAT) have identical structure.  ROSE represents them as a list of
 *  SgAsmPEImportItem in the Import Directory.  The ILT and IAT are parallel arrays of 32- or 64-bit (PE32 or PE32+) entries
 *  terminated with an all-zero entry.  The terminating entry is not stored explicitly by ROSE.  The entries are identical for
 *  both ILTs and IATs.
 *
 *  @par Import Lookup Table Entry (and Import Address Table Entry)
 *  Entries for ILTs and IATs are structurally identical.  They are 32- or 64-bit vectors.  The most significant bit (31/63)
 *  indicates whether the remaining bits are an Ordinal (when set) or Hint/Name address (when clear).  Ordinals are represented
 *  by the low-order 16 bits and Hint/Name addresses are stored in the low-order 31 bits.  All other bits must be zero
 *  according to the PE specification.  Hint/Name addresses are relative virtual addresses of entries in the (implicit)
 *  Hint/Name Table. When a function is bound by the dynamic linkter, its IAT Entry within process memory is overwritten with
 *  the virtual address of the bound function.
 *
 *  @par Hint/Name Table
 *  Some Import Lookup Table (and Import Address Table) entries contain a Hint/Name Table Entry RVA.  The Hint/Name Table
 *  Entries collectively form the Hint/Name Table, but there is no requirement that the entries appear in any particular order
 *  or even that they appear contiguously in memory.  In other words, the Hint/Name Table is a conceptual object rather than a
 *  true table in the PE file.
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
 * When parsing an Import Directory, ROSE assumes that the IAT contains ordinals and/or hint/name addresses rather than bound
 * addresses.  ROSE checks that the IAT entries are compatible with the ILT entries there were already parsed and if an
 * inconsistency is detected then a warning is issued and ROSE assumes that the IAT entry is a bound value instead.  Passing
 * true as the @p assume_bound argument for the parser will cause ROSE to not issue such warnings and immediately assume that
 * all IAT entries are bound addresses.  One can therefore find the conflicting entries by looking for SgAsmImportItem objects
 * that are created with a non-zero bound address.
 *
 * The IAT is often required to be allocated at a fixed address, often the beginning of the ".rdata" section.  Increasing the
 * size of the IAT by adding more items to the import list(s) can be problematic because ROSE is unable to safely write beyond
 * the end of the original IAT.  We require the user to manually allocate space for the new IAT and tell the
 * SgAsmPEImportDirectory object the location and size of the allocated space before unparsing.  On a related note, due to ROSE
 * allocators being section-local, reallocation of an Import Section does not cause reallocation of ILTs, Hint/Name pairs, or
 * DLL names that have addresses outside the Import Section.  If these items' sizes increase, the items will be truncated when
 * written back to disk.  The reallocation happens automatically for all import-related objects that are either bound to the
 * import section or have a null RVA, so one method of getting things reallocated is to traverse the AST and null their RVAs:
 *
 * @code
 *  struct Traversal: public AstSimpleTraversal {
 *      void visit(SgNode *node) {
 *          SgAsmPEImportDirectory *idir = isSgAsmPEImportDirectory(node);
 *          SgAsmPEImportItem *import = isSgAsmPEImportItem(node);
 *          static const rose_rva_t nil(0);
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

/** Counter for import_mesg() */
size_t SgAsmPEImportSection::mesg_nprinted = 0;

/** Optionally prints an error/warning/info message regarding import tables. The messages are silenced after a certain amount
 *  are printed. Returns true if printed; false if silenced. */
bool
SgAsmPEImportSection::import_mesg(const char *fmt, ...)
{
    static const size_t max_to_print=15;

    bool printed=false;
    va_list ap;
    va_start(ap, fmt);

    if (mesg_nprinted < max_to_print) {
        vfprintf(stderr, fmt, ap);
        printed = true;
    } else if (mesg_nprinted == max_to_print) {
        fprintf(stderr, "Import message limit reached; import diagnostics are now suppressed.\n");
    }

    ++mesg_nprinted;
    va_end(ap);
    return printed;
}

void
SgAsmPEImportSection::ctor()
{
    set_synthesized(true);

    SgAsmBasicString *name = new SgAsmBasicString("PE Section Table");
    set_name(name);
    name->set_parent(this);

    set_purpose(SP_HEADER);

    p_import_directories = new SgAsmPEImportDirectoryList();
    p_import_directories->set_parent(this);
}

/** Parse a PE Import Section.  This parses an entire PE Import Section, by recursively parsing the section's Import
 * Directories.  An Import Section is a sequence of Import Directories terminated by a zero-filled Import Directory struct.
 * The terminating entry is not stored explicitly in the section's list of directories. */
SgAsmPEImportSection*
SgAsmPEImportSection::parse()
{
    import_mesg_reset();
    SgAsmPESection::parse();

    SgAsmPEFileHeader *fhdr = isSgAsmPEFileHeader(get_header());
    ROSE_ASSERT(fhdr!=NULL);

    ROSE_ASSERT(is_mapped());
    rose_addr_t idir_va = get_mapped_actual_va();

    /* Parse each Import Directory. The list of directories is terminated with a zero-filled entry, which is not added to this
     * import section. */
    for (size_t i = 0; 1; i++) {
        /* Import directory entry */
        SgAsmPEImportDirectory *idir = new SgAsmPEImportDirectory(this);
        if (NULL==idir->parse(idir_va)) {
            /* We've reached the zero entry. Remove this directory from the section and delete it. */
            remove_import_directory(idir);
            SageInterface::deleteAST(idir);
            break;
        }
        idir_va += sizeof(SgAsmPEImportDirectory::PEImportDirectory_disk);
#if 1   /* FIXME: Do we really want this stuff duplicated in the AST? [RPM 2008-12-12] */
        SgAsmGenericString *name2 = new SgAsmBasicString(idir->get_dll_name()->get_string());
        fhdr->add_dll(new SgAsmGenericDLL(name2));
#endif
    }
    return this;
}

/** Add an import directory to the end of the import directory list. */
void
SgAsmPEImportSection::add_import_directory(SgAsmPEImportDirectory *d)
{
    ROSE_ASSERT(get_import_directories()!=NULL);
    SgAsmPEImportDirectoryPtrList &dirlist = get_import_directories()->get_vector();

    /* Make sure it's not already on the list */
    ROSE_ASSERT(dirlist.end()==std::find(dirlist.begin(), dirlist.end(), d));

    dirlist.push_back(d);
    get_import_directories()->set_isModified(true);
    d->set_parent(get_import_directories());
}

/** Remove an import directory from the import directory list. Does not delete it. */
void
SgAsmPEImportSection::remove_import_directory(SgAsmPEImportDirectory *d)
{
    SgAsmPEImportDirectoryPtrList &dirlist = get_import_directories()->get_vector();
    SgAsmPEImportDirectoryPtrList::iterator found = std::find(dirlist.begin(), dirlist.end(), d);
    if (found!=dirlist.end()) {
        dirlist.erase(found);
        d->set_parent(NULL);
    }
}

/** Reallocate space for the import section if necessary.  This will likely not work correctly because it moves the import
 *  address tables.  Unlike ELF, which tends to have dynamic linking trampolines and tables (PLT and GOT) in well defined
 *  locations, PE executables seem to have them haphazardly scattered throughout virtual memory.  That means there's no easy
 *  way to fix things if we have to move the import address table.  This whole subject of Import reallocation needs much
 *  improvement. */
bool
SgAsmPEImportSection::reallocate()
{
    import_mesg_reset();
    bool reallocated = SgAsmPESection::reallocate();
    rose_rva_t end_rva(this->get_mapped_preferred_rva(), this);
    SgAsmPEImportDirectoryPtrList &dirlist = get_import_directories()->get_vector();

    /* Space needed for the list of import directory structs. The list is terminated with a zero entry. */
    size_t nimports = dirlist.size();
    end_rva.increment((1 + nimports) * sizeof(SgAsmPEImportDirectory::PEImportDirectory_disk));

    /* Space needed for the data of each import directory. */
    for (size_t i=0; i<nimports; i++)
        end_rva.increment(dirlist[i]->reallocate(end_rva));

    /* Adjust the section size */
    rose_addr_t need = end_rva.get_rel();
    if (need < get_size()) {
        if (is_mapped())
            set_mapped_size(need);
        set_size(need);
        reallocated = true;
    } else if (need > get_size()) {
        get_file()->shift_extend(this, 0, need-get_size(), SgAsmGenericFile::ADDRSP_ALL, SgAsmGenericFile::ELASTIC_HOLE);
        reallocated = true;
    }

    return reallocated;
}

/** Reallocate space for all Import Address Table.
 *
 *  This method traverses the AST beginning at this PE Import Section and assigns addresses and sizes to all Import Address
 *  Tables (IATs).  The first IAT is given the @p start_at RVA and its size is reset to what ever size is needed to store the
 *  entire table.  Each subsequent IAT is given the next available address and it's size is also updated.  The result is that
 *  all the IATs under this Import Section are given addresses and sizes that make them contiguous in memory. This method
 *  returns the total number of bytes required for all the IATs. */
size_t
SgAsmPEImportSection::reallocate_iats(rose_rva_t start_at)
{
    rose_rva_t rva = start_at;
    const SgAsmPEImportDirectoryPtrList &dirs = get_import_directories()->get_vector();
    for (SgAsmPEImportDirectoryPtrList::const_iterator di=dirs.begin(); di!=dirs.end(); ++di) {
        (*di)->set_iat_rva(rva);
        size_t need = (*di)->iat_required_size();
        (*di)->set_iat_nalloc(need);
        rva.increment(need);
    }
    return rva.get_rel() - start_at.get_rel();
}


/* Write the import section back to disk */
void
SgAsmPEImportSection::unparse(std::ostream &f) const
{
    import_mesg_reset();
#if 1 /* DEBUGGING [RPM 2010-11-09] */
    {
        uint8_t byte = 0;
        for (size_t i=0; i<get_size(); i++)
            write(f, i, 1, &byte);
    }
#endif

    unparse_holes(f);

    /* Import Directory Entries and all they point to (even in other sections) */
    for (size_t i=0; i<get_import_directories()->get_vector().size(); i++) {
        SgAsmPEImportDirectory *idir = get_import_directories()->get_vector()[i];
        try {
            idir->unparse(f, this, i);
        } catch(const ShortWrite&) {
            import_mesg("SgAsmImportSection::unparse: error: Import Directory #%zu skipped (short write)\n", i);
        }
    }

    /* Zero terminated */
    SgAsmPEImportDirectory::PEImportDirectory_disk zero;
    memset(&zero, 0, sizeof zero);
    write(f, get_import_directories()->get_vector().size()*sizeof(zero), sizeof zero, &zero);
}

/* Print debugging info */
void
SgAsmPEImportSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEImportSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEImportSection.", prefix);
    }

    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    SgAsmPESection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %zu\n", p, w, "ndirectories", p_import_directories->get_vector().size());
    for (size_t i=0; i<p_import_directories->get_vector().size(); i++)
        p_import_directories->get_vector()[i]->dump(f, p, i);

    if (variantT() == V_SgAsmPEImportSection) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}
