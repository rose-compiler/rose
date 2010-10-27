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
    rose_addr_t idir_rva = get_mapped_actual_va() - fhdr->get_base_va();

    for (size_t i = 0; 1; i++) {
        /* Read idata directory entries. The list is terminated with a zero-filled entry whose idx will be negative */
        SgAsmPEImportDirectory *idir = new SgAsmPEImportDirectory(this, i, &idir_rva);
        if (idir->get_idx()<0) {
            delete idir;
            break;
        }

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
        SgAsmPEImportLookupTable *ilt = new SgAsmPEImportLookupTable(this, idir->get_ilt_rva(), i, false);
        idir->set_ilt(ilt);
        ilt->set_parent(idir);

        /* Import Address Table (same class as the Import Lookup Table) */
        SgAsmPEImportLookupTable *iat = new SgAsmPEImportLookupTable(this, idir->get_iat_rva(), i, true);
        idir->set_iat(iat);
        iat->set_parent(idir);

        add_import_directory(idir);

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

void
SgAsmPEImportSection::add_import_directory(SgAsmPEImportDirectory *d)
{
    ROSE_ASSERT(p_import_directories!=NULL);
    p_import_directories->set_isModified(true);
    p_import_directories->get_vector().push_back(d);
    d->set_parent(this);
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
            idir->unparse(f, this);
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
