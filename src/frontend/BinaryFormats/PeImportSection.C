/* SgAsmPEImportSection, normally named ".idata" if it appears in the section table. Most modern PE executables don't create a
 * special section in the section table, but rather have the SgAsmPEFileHeader::PAIR_IMPORTS RVA/Size pair point to part of the
 * memory mapped from another read-only section, such as the ".text" section. */
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "Diagnostics.h"

using namespace Rose::Diagnostics;

/** Counter for import_mesg() */
size_t SgAsmPEImportSection::mesg_nprinted = 0;

bool
SgAsmPEImportSection::show_import_mesg()
{
    static const size_t max_to_print=15;
    return ++mesg_nprinted <= max_to_print;
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

/* Parse a PE Import Section.  This parses an entire PE Import Section, by recursively parsing the section's Import
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

    // Some compilers don't actually terminate the Import Directory list with an all-zero entry as specified in Microsoft's PE
    // format documenttion. Instead, they apparently assume that that loader uses the size of the import section to calculate
    // the length of the list and that it never actually reads the final entry which is normally all zero.  These compilers
    // seem to have garbage in the last entry.
    rose_addr_t nBytes = get_size();
    ASSERT_require(nBytes > 0);
    size_t nEntries = nBytes / sizeof(SgAsmPEImportDirectory::PEImportDirectory_disk);

    /* Parse each Import Directory. The list of directories is terminated with a zero-filled entry, which is not added to this
     * import section. */
    for (size_t i = 0; i < nEntries; i++) {
        /* Import directory entry */
        SgAsmPEImportDirectory *idir = new SgAsmPEImportDirectory(this);
        if (NULL==idir->parse(idir_va, i+1 == nEntries)) {
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

/* Reallocate space for the import section if necessary.  This will likely not work correctly because it moves the import
 * address tables.  Unlike ELF, which tends to have dynamic linking trampolines and tables (PLT and GOT) in well defined
 * locations, PE executables seem to have them haphazardly scattered throughout virtual memory.  That means there's no easy
 * way to fix things if we have to move the import address table.  This whole subject of Import reallocation needs much
 * improvement. */
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
            if (show_import_mesg())
                mlog[WARN] <<"SgAsmImportSection::unparse: Import Directory #" <<i <<" skipped (short write)\n";
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
    fprintf(f, "%s%-*s = %" PRIuPTR "\n", p, w, "ndirectories", p_import_directories->get_vector().size());
    for (size_t i=0; i<p_import_directories->get_vector().size(); i++)
        p_import_directories->get_vector()[i]->dump(f, p, i);

    if (variantT() == V_SgAsmPEImportSection) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

#endif
