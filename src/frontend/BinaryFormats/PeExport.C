/* Windows PE Export Sections (SgAsmPEExportSection and related classes) */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/BinaryAnalysis/Hexdump.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>
#include <Rose/Diagnostics.h>

using namespace Rose;
using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;

/* Constructor */
SgAsmPEExportDirectory::SgAsmPEExportDirectory(SgAsmPEExportSection *section) {
    initializeProperties();

    SgAsmPEFileHeader *fhdr = isSgAsmPEFileHeader(section->get_header());
    ROSE_ASSERT(fhdr!=NULL);
    set_parent(section);

    /* Read disk-formatted export directory */
    PEExportDirectory_disk disk;
    try {
        section->readContent(fhdr->get_loaderMap(), section->get_mappedActualVa(), &disk, sizeof disk);
    } catch (const MemoryMap::NotMapped &e) {
        if (mlog[ERROR]) {
            mlog[ERROR] <<"SgAsmPEExportDirectory::ctor: export directory at va "
                        <<StringUtility::addrToString(section->get_mappedActualVa())
                        <<" contains unmapped va " <<StringUtility::addrToString(e.va) <<"\n";
            if (e.map) {
                mlog[ERROR] <<"Memory map in effect at time of error:\n";
                e.map->dump(mlog[ERROR], "    ");
            }
        }
        memset(&disk, 0, sizeof disk);
    }

    /* Convert disk-format data members to native format */
    p_res1         = ByteOrder::leToHost(disk.res1);
    p_timestamp    = ByteOrder::leToHost(disk.timestamp);
    p_vmajor       = ByteOrder::leToHost(disk.vmajor);
    p_vminor       = ByteOrder::leToHost(disk.vminor);
    p_name_rva     = ByteOrder::leToHost(disk.name_rva);       p_name_rva.bindSection(section);
    p_ord_base     = ByteOrder::leToHost(disk.ord_base);
    p_expaddr_n    = ByteOrder::leToHost(disk.expaddr_n);
    p_nameptr_n    = ByteOrder::leToHost(disk.nameptr_n);
    p_expaddr_rva  = ByteOrder::leToHost(disk.expaddr_rva);    p_expaddr_rva.bindSection(section);
    p_nameptr_rva  = ByteOrder::leToHost(disk.nameptr_rva);    p_nameptr_rva.bindSection(section);
    p_ordinals_rva = ByteOrder::leToHost(disk.ordinals_rva);   p_ordinals_rva.bindSection(section);

    /* Read the name */
    std::string name;
    try {
        name = section->readContentString(fhdr->get_loaderMap(), *p_name_rva.va());
    } catch (const MemoryMap::NotMapped &e) {
        if (mlog[WARN]) {
            mlog[WARN] <<"SgAsmPEExportDirectory::ctor: directory name at rva "
                       <<StringUtility::addrToString(p_name_rva.rva())
                       <<" contains unmapped va " <<StringUtility::addrToString(e.va) <<"\n";
            if (e.map) {
                mlog[WARN] <<"Memory map in effect at time of error:\n";
                e.map->dump(mlog[WARN], "    ");
            }
        }
        memset(&disk, 0, sizeof disk);
    }
    p_name = new SgAsmBasicString(name);
    p_name->set_parent(this);
}

/* Print debugging info */
void
SgAsmPEExportDirectory::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sPEExportDirectory[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sPEExportDirectory.", prefix);
    }
    const int w = std::max(size_t{1}, DUMP_FIELD_WIDTH - strlen(p));

    fprintf(f, "%s%-*s = \"%s\"\n",                    p, w, "name", p_name->get_string(true).c_str());
    fprintf(f, "%s%-*s = 0x%08x (%u)\n",               p, w, "res1", p_res1, p_res1);
    fprintf(f, "%s%-*s = %lu %s",                      p, w, "timestamp", (unsigned long)p_timestamp, ctime(&p_timestamp));
    fprintf(f, "%s%-*s = %u\n",                        p, w, "vmajor", p_vmajor);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "vminor", p_vminor);
    fprintf(f, "%s%-*s = %s\n",                        p, w, "name_rva", p_name_rva.toString().c_str());
    fprintf(f, "%s%-*s = %u\n",                        p, w, "ord_base", p_ord_base);
    fprintf(f, "%s%-*s = %" PRIuPTR "\n",                       p, w, "expaddr_n", p_expaddr_n);
    fprintf(f, "%s%-*s = %" PRIuPTR "\n",                       p, w, "nameptr_n", p_nameptr_n);
    fprintf(f, "%s%-*s = %s\n",                        p, w, "expaddr_rva", p_expaddr_rva.toString().c_str());
    fprintf(f, "%s%-*s = %s\n",                        p, w, "nameptr_rva", p_nameptr_rva.toString().c_str());
    fprintf(f, "%s%-*s = %s\n",                        p, w, "ordinals_rva", p_ordinals_rva.toString().c_str());
}

/* Constructor */
SgAsmPEExportEntry::SgAsmPEExportEntry(SgAsmGenericString *fname, unsigned ordinal, RelativeVirtualAddress expaddr,
                                       SgAsmGenericString *forwarder) {
    initializeProperties();

    set_name(fname);
    if (fname)
        fname->set_parent(this);

    set_ordinal(ordinal);
    set_exportRva(expaddr);
    set_forwarder(forwarder);
}

/* Print debugging info */
void
SgAsmPEExportEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sPEExportEntry[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sPEExportEntry.", prefix);
    }
    const int w = std::max(size_t{1}, DUMP_FIELD_WIDTH - strlen(p));

    fprintf(f, "%s%-*s = [ordinal raw=%u", p, w, "info", p_ordinal);
    if (auto ordinal = biasedOrdinal()) {
        fprintf(f, ", index=%u]", *ordinal);
    } else {
        fprintf(f, ", biased=%s]", ordinal.unwrapError().c_str());
    }
    fprintf(f, " rva=%s \"%s\"", p_exportRva.toString().c_str(), p_name->get_string(true).c_str());
    if (p_forwarder)
        fprintf(f, " -> \"%s\"", p_forwarder->get_string(true).c_str());
    fputc('\n', f);
}

/* Override ROSETTA to set parent */
void
SgAsmPEExportEntry::set_name(SgAsmGenericString *fname)
{
    if (p_name!=fname)
        set_isModified(true);
    p_name = fname;
    if (p_name) p_name->set_parent(this);
}
void
SgAsmPEExportEntry::set_forwarder(SgAsmGenericString *forwarder)
{
    if (p_forwarder!=forwarder)
        set_isModified(true);
    p_forwarder = forwarder;
    if (p_forwarder) p_forwarder->set_parent(this);
}

/* Constructor */
SgAsmPEExportSection::SgAsmPEExportSection(SgAsmPEFileHeader *fhdr)
    : SgAsmPESection(fhdr) {
    initializeProperties();
}

SgAsmPEExportSection*
SgAsmPEExportSection::parse()
{
    SgAsmPESection::parse();

    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);

    p_exportDirectory = new SgAsmPEExportDirectory(this);

    // Check that the p_export_dir.p_nameptr_n is not out of range.
    rose_addr_t availBytes = fhdr->get_loaderMap()->at(*get_exportDirectory()->get_nameptr_rva().va()).available().size();
    size_t availElmts = availBytes / sizeof(ExportNamePtr_disk);
    if (get_exportDirectory()->get_nameptr_n() > availElmts) {
        mlog[ERROR] <<"SgAsmPEExportSection::parse: number of entries indicated (" <<get_exportDirectory()->get_nameptr_n() <<")"
                    <<" exceeds available mapped memory (room for " <<StringUtility::plural(availElmts, "entries") <<")\n";
    }
    static const size_t maxNamePtrN = 10000;
    if (get_exportDirectory()->get_nameptr_n() > maxNamePtrN) {
        availElmts = std::min(availElmts, maxNamePtrN);
        mlog[WARN] <<"SgAsmPEExportSection::parse: number of entries indicated (" <<get_exportDirectory()->get_nameptr_n() <<")"
                   <<" is large; resetting to " <<availElmts <<"\n";
    }

    /* The export directory points to three parallel arrays:
     *   1. An array of RVAs that point to NUL-terminated names.
     *   2. An array of "ordinals" which serve as indices into the Export Address Table.
     *   3. An array of export addresses (see note below). */
    for (size_t i=0; i<std::min(get_exportDirectory()->get_nameptr_n(), availElmts); i++) {
        /* Function name RVA (nameptr)*/
        ExportNamePtr_disk nameptr_disk = 0;
        rose_addr_t nameptr_va = *get_exportDirectory()->get_nameptr_rva().va() + i*sizeof(nameptr_disk);
        bool badFunctionNameVa = false;
        try {
            readContent(fhdr->get_loaderMap(), nameptr_va, &nameptr_disk, sizeof nameptr_disk);
        } catch (const MemoryMap::NotMapped &e) {
            badFunctionNameVa = true;
            if (mlog[ERROR]) {
                mlog[ERROR] <<"SgAsmPEExportSection::parse: export name #" <<i
                            <<" at va " <<StringUtility::addrToString(nameptr_va)
                            <<" contains unmapped va " <<StringUtility::addrToString(e.va) <<"\n";
                if (e.map) {
                    mlog[ERROR] <<"Memory map in effect at time of error:\n";
                    e.map->dump(mlog[ERROR], "    ");
                }
            }
            nameptr_disk = 0;
        }
        rose_addr_t fname_rva = ByteOrder::leToHost(nameptr_disk);
        rose_addr_t fname_va = fname_rva + fhdr->get_baseVa();

        /* Function name (fname) */
        std::string s;
        try {
            s = readContentString(fhdr->get_loaderMap(), fname_va);
        } catch (const MemoryMap::NotMapped &e) {
            if (mlog[ERROR]) {
                mlog[ERROR] <<"SgAsmPEExportSection::parse: export name #" <<i
                            <<" at va " <<StringUtility::addrToString(fname_va)
                            <<" contains unmapped va " <<StringUtility::addrToString(e.va) <<"\n";
                if (e.map) {
                    mlog[ERROR] <<"Memory map in effect at time of error:\n";
                    e.map->dump(mlog[ERROR], "    ");
                }
            }
        }
        SgAsmGenericString *fname = new SgAsmBasicString(s);

        /* Ordinal (an index into the Export Address Table) */
        ExportOrdinal_disk ordinal_disk = 0;
        rose_addr_t ordinal_va = *get_exportDirectory()->get_ordinals_rva().va() + i*sizeof(ordinal_disk);
        bool badOrdinalVa = false;
        try {
            readContent(fhdr->get_loaderMap(), ordinal_va, &ordinal_disk, sizeof ordinal_disk);
        } catch (const MemoryMap::NotMapped &e) {
            badOrdinalVa = true;
            if (mlog[ERROR]) {
                mlog[ERROR] <<"SgAsmPEExportSection::parse: ordinal #" <<i
                            <<" at va " <<StringUtility::addrToString(ordinal_va)
                            <<" contains unmapped va " <<StringUtility::addrToString(e.va) <<"\n";
                if (e.map) {
                    mlog[ERROR] <<"Memory map in effect at time of error:\n";
                    e.map->dump(mlog[ERROR], "    ");
                }
            }
            ordinal_disk = 0;
        }
        const unsigned ordinal = ByteOrder::leToHost(ordinal_disk);

        /* If the function name pointer and ordinal pointer are both in unmapped memory then give up. */
        if (badFunctionNameVa && badOrdinalVa) {
            if (mlog[ERROR])
                mlog[ERROR] <<"SgAsmPEExportSection::parse: giving up after trying to read entry #" <<i <<"\n";
            break;
        }

        // Read the address from the Export Address Table. This table is indexed by ordinal.
        RelativeVirtualAddress expaddr = 0;                         // export address
        const rose_addr_t expaddr_va = *get_exportDirectory()->get_expaddr_rva().va() + ordinal * sizeof(ExportAddress_disk);
        try {
            ExportAddress_disk expaddr_disk;
            readContent(fhdr->get_loaderMap(), expaddr_va, &expaddr_disk, sizeof expaddr_disk);
            expaddr = ByteOrder::leToHost(expaddr_disk);
            expaddr.bindBestSection(fhdr);
        } catch (const MemoryMap::NotMapped &e) {
            if (mlog[ERROR]) {
                mlog[ERROR] <<"SgAsmPEExportSection::parse: export address #" <<i
                            <<" at va " <<StringUtility::addrToString(expaddr_va)
                            <<" contains unmapped va " <<StringUtility::addrToString(e.va) <<"\n";
                if (e.map) {
                    mlog[ERROR] <<"Memory map in effect at time of error:\n";
                    e.map->dump(mlog[ERROR], "    ");
                }
            }
            expaddr = 0xffffffff;                       // ordinal out of range
        }

        /* If export address is within this section then it points to a NUL-terminated forwarder name.
         * FIXME: Is this the proper precondition? [RPM 2009-08-20] */
        SgAsmGenericString *forwarder = NULL;
        if (*expaddr.va()>=get_mappedActualVa() && *expaddr.va()<get_mappedActualVa()+get_mappedSize()) {
            std::string s;
            try {
                s = readContentString(fhdr->get_loaderMap(), *expaddr.va());
            } catch (const MemoryMap::NotMapped &e) {
                if (mlog[ERROR]) {
                    mlog[ERROR] <<"SgAsmPEExportSection::parse: forwarder " <<i
                                <<" at rva " <<StringUtility::addrToString(expaddr.rva())
                                <<" contains unmapped va " <<StringUtility::addrToString(e.va) <<"\n";
                    if (e.map) {
                        mlog[ERROR] <<"Memory map in effect at time of error:\n";
                        e.map->dump(mlog[ERROR], "    ");
                    }
                }
            }
            forwarder = new SgAsmBasicString(s);
        }

        SgAsmPEExportEntry *entry = new SgAsmPEExportEntry(fname, ordinal, expaddr, forwarder);
        addEntry(entry);
    }
    return this;
}

void
SgAsmPEExportSection::add_entry(SgAsmPEExportEntry *entry) {
    addEntry(entry);
}

void
SgAsmPEExportSection::addEntry(SgAsmPEExportEntry *entry) {
    ASSERT_not_null(entry);
    ASSERT_forbid2(entry->get_parent(), "already linked into AST; remove it first");
    ASSERT_not_null(p_exports);
    ASSERT_require(p_exports->get_parent() == this);

    p_exports->set_isModified(true);
    p_exports->get_exports().push_back(entry);
    entry->set_parent(p_exports);
}

/* Print debugging info */
void
SgAsmPEExportSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sPEExportSection[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sPEExportSection.", prefix);
    }

    SgAsmPESection::dump(f, p, -1);

    if (get_exportDirectory())
        get_exportDirectory()->dump(f, p, -1);
    for (size_t i=0; i<p_exports->get_exports().size(); i++)
        p_exports->get_exports()[i]->dump(f, p, i);

    if (variantT() == V_SgAsmPEExportSection) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

const RelativeVirtualAddress&
SgAsmPEExportEntry::get_export_rva() const {
    return get_exportRva();
}

void
SgAsmPEExportEntry::set_export_rva(const RelativeVirtualAddress &x) {
    set_exportRva(x);
}

SgAsmPEExportDirectory*
SgAsmPEExportSection::get_export_dir() const {
    return get_exportDirectory();
}

void
SgAsmPEExportSection::set_export_dir(SgAsmPEExportDirectory *x) {
    set_exportDirectory(x);
}

#endif
