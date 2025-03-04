/* SgAsmPEImportSection, normally named ".idata" if it appears in the section table. Most modern PE executables don't create a
 * special section in the section table, but rather have the SgAsmPEFileHeader::PAIR_IMPORTS RVA/Size pair point to part of the
 * memory mapped from another read-only section, such as the ".text" section. */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/BinaryAnalysis/Hexdump.h>
#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>
#include <Rose/Diagnostics.h>

using namespace Rose::Diagnostics;

/** Counter for import_mesg() */
size_t SgAsmPEImportSection::mesg_nprinted = 0;

bool
SgAsmPEImportSection::show_import_mesg()
{
    return showImportMessage();
}

bool
SgAsmPEImportSection::showImportMessage()
{
    static const size_t max_to_print=15;
    return ++mesg_nprinted <= max_to_print;
}

void
SgAsmPEImportSection::import_mesg_reset() {
    importMessageReset();
}

void
SgAsmPEImportSection::importMessageReset() {
    mesg_nprinted=0;
}

SgAsmPEImportSection::SgAsmPEImportSection(SgAsmPEFileHeader *fhdr)
    : SgAsmPESection(fhdr) {
    initializeProperties();

    set_synthesized(true);

    SgAsmBasicString *name = new SgAsmBasicString("PE Section Table");
    set_name(name);
    name->set_parent(this);

    set_purpose(SP_HEADER);
}

/* Parse a PE Import Section.  This parses an entire PE Import Section, by recursively parsing the section's Import
 * Directories.  An Import Section is a sequence of Import Directories terminated by a zero-filled Import Directory struct.
 * The terminating entry is not stored explicitly in the section's list of directories. */
SgAsmPEImportSection*
SgAsmPEImportSection::parse()
{
    importMessageReset();
    SgAsmPESection::parse();

    SgAsmPEFileHeader *fhdr = isSgAsmPEFileHeader(get_header());
    ROSE_ASSERT(fhdr!=NULL);

    ROSE_ASSERT(isMapped());
    Rose::BinaryAnalysis::Address idir_va = get_mappedActualVa();

    // Some compilers don't actually terminate the Import Directory list with an all-zero entry as specified in Microsoft's PE
    // format documenttion. Instead, they apparently assume that that loader uses the size of the import section to calculate
    // the length of the list and that it never actually reads the final entry which is normally all zero.  These compilers
    // seem to have garbage in the last entry.
    Rose::BinaryAnalysis::Address nBytes = get_size();
    ASSERT_require(nBytes > 0);
    size_t nEntries = nBytes / sizeof(SgAsmPEImportDirectory::PEImportDirectory_disk);

    /* Parse each Import Directory. The list of directories is terminated with a zero-filled entry, which is not added to this
     * import section. */
    for (size_t i = 0; i < nEntries; i++) {
        /* Import directory entry */
        SgAsmPEImportDirectory *idir = new SgAsmPEImportDirectory(this);
        if (NULL==idir->parse(idir_va, i+1 == nEntries)) {
            /* We've reached the zero entry. Remove this directory from the section and delete it. */
            removeImportDirectory(idir);
            SageInterface::deleteAST(idir);
            break;
        }
        idir_va += sizeof(SgAsmPEImportDirectory::PEImportDirectory_disk);
#if 1   /* FIXME: Do we really want this stuff duplicated in the AST? [RPM 2008-12-12] */
        SgAsmGenericString *name2 = new SgAsmBasicString(idir->get_dllName()->get_string());
        fhdr->addDll(new SgAsmGenericDLL(name2));
#endif
    }
    return this;
}

void
SgAsmPEImportSection::add_import_directory(SgAsmPEImportDirectory *d)
{
    addImportDirectory(d);
}

void
SgAsmPEImportSection::addImportDirectory(SgAsmPEImportDirectory *d)
{
    ROSE_ASSERT(get_importDirectories()!=NULL);
    SgAsmPEImportDirectoryPtrList &dirlist = get_importDirectories()->get_vector();

    /* Make sure it's not already on the list */
    ROSE_ASSERT(dirlist.end()==std::find(dirlist.begin(), dirlist.end(), d));

    dirlist.push_back(d);
    get_importDirectories()->set_isModified(true);
    d->set_parent(get_importDirectories());
}

void
SgAsmPEImportSection::remove_import_directory(SgAsmPEImportDirectory *d)
{
    removeImportDirectory(d);
}

void
SgAsmPEImportSection::removeImportDirectory(SgAsmPEImportDirectory *d)
{
    SgAsmPEImportDirectoryPtrList &dirlist = get_importDirectories()->get_vector();
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
    importMessageReset();
    bool reallocated = SgAsmPESection::reallocate();
    Rose::BinaryAnalysis::RelativeVirtualAddress end_rva(this->get_mappedPreferredRva(), this);
    SgAsmPEImportDirectoryPtrList &dirlist = get_importDirectories()->get_vector();

    /* Space needed for the list of import directory structs. The list is terminated with a zero entry. */
    size_t nimports = dirlist.size();
    end_rva.increment((1 + nimports) * sizeof(SgAsmPEImportDirectory::PEImportDirectory_disk));

    /* Space needed for the data of each import directory. */
    for (size_t i=0; i<nimports; i++)
        end_rva.increment(dirlist[i]->reallocate(end_rva));

    /* Adjust the section size */
    Rose::BinaryAnalysis::Address need = end_rva.boundOffset().orElse(end_rva.rva());
    if (need < get_size()) {
        if (isMapped())
            set_mappedSize(need);
        set_size(need);
        reallocated = true;
    } else if (need > get_size()) {
        get_file()->shiftExtend(this, 0, need-get_size(), SgAsmGenericFile::ADDRSP_ALL, SgAsmGenericFile::ELASTIC_HOLE);
        reallocated = true;
    }

    return reallocated;
}

size_t
SgAsmPEImportSection::reallocate_iats(Rose::BinaryAnalysis::RelativeVirtualAddress start_at)
{
    return reallocateIats(start_at);
}

size_t
SgAsmPEImportSection::reallocateIats(Rose::BinaryAnalysis::RelativeVirtualAddress start_at)
{
    Rose::BinaryAnalysis::RelativeVirtualAddress rva = start_at;
    const SgAsmPEImportDirectoryPtrList &dirs = get_importDirectories()->get_vector();
    for (SgAsmPEImportDirectoryPtrList::const_iterator di=dirs.begin(); di!=dirs.end(); ++di) {
        (*di)->set_iat_rva(rva);
        size_t need = (*di)->iatRequiredSize();
        (*di)->set_iat_nalloc(need);
        rva.increment(need);
    }
    return rva.boundOffset().orElse(rva.rva()) - start_at.boundOffset().orElse(start_at.rva());
}


/* Write the import section back to disk */
void
SgAsmPEImportSection::unparse(std::ostream &f) const
{
    importMessageReset();
#if 1 /* DEBUGGING [RPM 2010-11-09] */
    {
        uint8_t byte = 0;
        for (size_t i=0; i<get_size(); i++)
            write(f, i, 1, &byte);
    }
#endif

    unparseHoles(f);

    /* Import Directory Entries and all they point to (even in other sections) */
    for (size_t i=0; i<get_importDirectories()->get_vector().size(); i++) {
        SgAsmPEImportDirectory *idir = get_importDirectories()->get_vector()[i];
        try {
            idir->unparse(f, this, i);
        } catch(const ShortWrite&) {
            if (showImportMessage())
                mlog[WARN] <<"SgAsmImportSection::unparse: Import Directory #" <<i <<" skipped (short write)\n";
        }
    }

    /* Zero terminated */
    SgAsmPEImportDirectory::PEImportDirectory_disk zero;
    memset(&zero, 0, sizeof zero);
    write(f, get_importDirectories()->get_vector().size()*sizeof(zero), sizeof zero, &zero);
}

/* Print debugging info */
void
SgAsmPEImportSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sPEImportSection[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sPEImportSection.", prefix);
    }

    const int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));

    SgAsmPESection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %" PRIuPTR "\n", p, w, "ndirectories", p_importDirectories->get_vector().size());
    for (size_t i=0; i<p_importDirectories->get_vector().size(); i++)
        p_importDirectories->get_vector()[i]->dump(f, p, i);

    if (variantT() == V_SgAsmPEImportSection) //unless a base class
        Rose::BinaryAnalysis::hexdump(f, 0, std::string(p)+"data at ", p_data);
}

SgAsmPEImportDirectoryList*
SgAsmPEImportSection::get_import_directories() const {
    return get_importDirectories();
}

void
SgAsmPEImportSection::set_import_directories(SgAsmPEImportDirectoryList *x) {
    set_importDirectories(x);
}

#endif
