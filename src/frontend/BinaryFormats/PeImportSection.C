/* SgAsmPEImportSection, normally named ".idata" if it appears in the section table. Most modern PE executables don't create a
 * special section in the section table, but rather have the SgAsmPEFileHeader::PAIR_IMPORTS RVA/Size pair point to part of the
 * memory mapped from another read-only section, such as the ".text" section. */
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdarg.h>

/** Optionally prints an error/warning/info message regarding import tables. The messages are silenced after a certain amount
 *  are printed. Returns true if printed; false if silenced. */
bool
SgAsmPEImportSection::import_mesg(const char *fmt, ...)
{
    static size_t nprinted=0;
    static const size_t max_to_print=15;

    bool printed=false;
    va_list ap;
    va_start(ap, fmt);
    
    if (nprinted++ < max_to_print) {
        vfprintf(stderr, fmt, ap);
        if (nprinted==max_to_print)
            fprintf(stderr, "Subsequent import messages will be suppressed.\n");
        printed = true;
    }
    
    va_end(ap);
    return printed;
}

/** Constructor for PE import data. Constructs an SgAsmPEImportSection that represents either a PE ".idata" section as defined
 *  by the PE Section Table, or a PE Import Table as described by the RVA/Size pairs at the end of the NT Optional Header. The
 *  ".idata" section and PE Import Table both have the same format, which is generally:
 * 
 * @code
 * +-------------------------------+  Starts at address zero of the .idata
 * | Import Directory Table:       |  section or PE Import Table. Each Directory
 * |   Import Directory Entry #0   |  represents one library and all  its
 * |   Import Directory Entry #1   |  associated symbols.
 * |   ...                         |
 * |   Zero-filled Directory Entry |
 * +-------------------------------+
 *
 * +-------------------------------+  One table per dynamic library, starting
 * | Import Lookup Table (ILT)     |  at arbitrary RVA specified in the
 * |   ILT Entry #0                |  Directory Table.
 * |   ILT Entry #1                |
 * |   Zero-filled ILTEntry        |
 * +-------------------------------+
 *
 * +-------------------------------+  There is no starting RVA for this table.
 * | Hint-Name Table               |  Rather, ILT Entries each contain an RVA
 * |                               |  to an entry in the Hint-Name Table.
 * +-------------------------------+
 * @endcode
 */
void
SgAsmPEImportSection::ctor()
{
    set_synthesized(true);
    set_name(new SgAsmBasicString("PE Section Table"));
    set_purpose(SP_HEADER);

    p_import_directories = new SgAsmPEImportDirectoryList();
    p_import_directories->set_parent(this);
}

SgAsmPEImportSection*
SgAsmPEImportSection::parse()
{
    SgAsmPESection::parse();

    SgAsmPEFileHeader *fhdr = isSgAsmPEFileHeader(get_header());
    ROSE_ASSERT(fhdr!=NULL);

    ROSE_ASSERT(is_mapped());
    rose_addr_t idir_va = get_mapped_actual_va();

    /* Parse each directory entry and its import lookup table and import address table. The list of directories is terminated
     * with a zero-filled entry, which is not added to this import section. */
    for (size_t i = 0; 1; i++) {
        /* Import directory entry */
        SgAsmPEImportDirectory *idir = new SgAsmPEImportDirectory(this);
        if (NULL==idir->parse(idir_va)) {
            /* We've reached the zero entry. Remove this directory from the section and delete it. */
            remove_import_directory(idir);
            delete idir;
            break;
        }
        idir_va += sizeof(SgAsmPEImportDirectory::PEImportDirectory_disk);

        /* DLL name */
        rose_rva_t rva = idir->get_dll_name_rva();
        if (rva.get_section()!=this) {
            rose_addr_t start_rva = get_mapped_actual_va() - get_base_va();
            import_mesg("SgAsmPEImportSection::ctor: warning: Name RVA is outside PE Import Table\n"
                        "        Import Directory Entry #%zu\n"
                        "        Name RVA is %s\n"
                        "        PE Import Table mapped from 0x%08"PRIx64" to 0x%08"PRIx64"\n",
                        i,
                        rva.to_string().c_str(),
                        start_rva, start_rva+get_mapped_size());
        }

        /* Import Lookup Table */
        SgAsmPEImportLookupTable *ilt = new SgAsmPEImportLookupTable(idir, SgAsmPEImportLookupTable::ILT_LOOKUP_TABLE);
        ilt->parse(idir->get_ilt_rva(), i);

        /* Import Address Table */
        SgAsmPEImportLookupTable *iat = new SgAsmPEImportLookupTable(idir, SgAsmPEImportLookupTable::ILT_ADDRESS_TABLE);
        iat->parse(idir->get_iat_rva(), i);

        /* Create the GenericDLL for this library */
        SgAsmGenericDLL *dll = new SgAsmGenericDLL(idir->get_dll_name());
        for (size_t j=0; j<ilt->get_entries()->get_vector().size(); j++) {
            SgAsmPEImportILTEntry *e = ilt->get_entries()->get_vector()[j];
            SgAsmPEImportHNTEntry *hn = e->get_hnt_entry();
            if (hn!=NULL)
                dll->add_symbol(hn->get_name()->c_str());
        }
        fhdr->add_dll(dll);
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

/** Reallocate space for the import section if necessary. */
bool
SgAsmPEImportSection::reallocate()
{
    bool reallocated = SgAsmPESection::reallocate();
    rose_addr_t need = 0;

    /* Space needed for the import directories. The list is terminated with a zero entry. */
    size_t nimports = get_import_directories()->get_vector().size();
    need += (1 + nimports) * sizeof(SgAsmPEImportDirectory::PEImportDirectory_disk);

    /* Adjust the section size */
    if (need < get_size()) {
        if (is_mapped()) {
            ROSE_ASSERT(get_mapped_size()==get_size());
            set_mapped_size(need);
        }
        set_size(need);
        reallocated = true;
    } else if (need > get_size()) {
        get_file()->shift_extend(this, 0, need-get_size(), SgAsmGenericFile::ADDRSP_ALL, SgAsmGenericFile::ELASTIC_HOLE);
        reallocated = true;
    }

    return reallocated;
}

/* Write the import section back to disk */
void
SgAsmPEImportSection::unparse(std::ostream &f) const
{
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
